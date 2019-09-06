#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#define SLAVE_NUMBER    1
#define FILE_QTY        1
#define BUFF_SIZE       200
#define SEM_APP_NAME    "/sem_app"
#define SEM_SLAVE_NAME  "/sem_slave"

int main(int argc, char *argv[]) {
    int i;
    int slave[SLAVE_NUMBER];
    int fd = shm_open("/memory", O_CREAT | O_RDWR, 0777);
    if (fd < 0){
        perror("Error en el open");
        return 1;
    }
    if (ftruncate(fd, BUFF_SIZE) != 0){
        perror("Error en el ftruncate: ");
        return 1;
    }
    void * var = mmap(NULL, BUFF_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (var == MAP_FAILED){
        perror("Error en el mmap");
        return 1;
    }
    char * buf = (char *) var;
    *buf = 0;
    
    sem_t * sem_app = sem_open(SEM_APP_NAME, O_CREAT, 0777, 1);
    if (sem_app == SEM_FAILED){
        perror("Error en el sem_open");
        return 1;
    }

    sem_t * sem_slave = sem_open(SEM_SLAVE_NAME, O_CREAT, 0777, 0);
    if (sem_slave == SEM_FAILED){
        perror("Error en el sem_open");
        return 1;
    }

    for (i = 0; i < SLAVE_NUMBER; i++) {
    	slave[i] = fork();
    	if (slave[i] < 0){
			perror("Error de fork");    		
			return 1;
    	} else if (slave[i] == 0){
            char *args[1] = {NULL};
    		execv("slave", args);
	        perror("Error de execv de app");
	        return 1;
    	} else {
    		// Padre
    	}
    }

    int proccesed_files = 0;

    while (proccesed_files < FILE_QTY + 1){
        sem_wait(sem_app);
        // Mandar archivo
        sem_post(sem_slave);
        proccesed_files++;
    }

    for (i = 0; i < SLAVE_NUMBER; i++){
        waitpid(slave[i], NULL, 0);
    }

    close(fd);
    sem_close(sem_app);
    sem_close(sem_slave);
    sem_unlink(SEM_APP_NAME);
    sem_unlink(SEM_SLAVE_NAME);
    return 0;
}