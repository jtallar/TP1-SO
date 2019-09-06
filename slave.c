#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>


#define SEM_APP_NAME    "/sem_app"
#define SEM_SLAVE_NAME  "/sem_slave"

int main(int argc, char *argv[]) {
    int fd = shm_open("/memory", O_RDWR, 0);
    if (fd < 0){
        perror("Error en el open");
        return 1;
    }
    void * var = mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (var == MAP_FAILED){
        perror("Erorr en el mmap");
        return 1;
    }
    char * buf = (char *) var;

    sem_t * sem_app = sem_open(SEM_APP_NAME, 0);
    if (sem_app == SEM_FAILED){
        perror("Error en el sem_open");
        return 1;
    }

    sem_t * sem_slave = sem_open(SEM_SLAVE_NAME, 0);
    if (sem_slave == SEM_FAILED){
        perror("Error en el sem_open");
        return 1;
    }

    sem_wait(sem_slave);
    int ret = fork();
    if (ret < 0){
		printf("Error en la creacion de fork\n");    		
		return 1;
	} else if (ret == 0){
        static char *args[] = {"minisat", "Files/hole6.cnf", NULL};
    	execv("/usr/bin/minisat", args);
        perror("Error de execv de slave");	        
        return 1;
    } else {
        wait(NULL); // Espera a que termine el hijo
    }
    sem_post(sem_app);
    
    sem_close(sem_app);
    sem_close(sem_slave);
    close(fd);
    return 0;
}