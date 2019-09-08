#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#include <string.h>

#define STDIN			0
#define STDOUT			1
#define SLAVE_NUMBER    1
#define BUFF_SIZE       200
#define SEM_APP_NAME    "/sem_app"
#define SEM_SLAVE_NAME  "/sem_slave"
#define SEM_APP_TO_SLAVE_NAME   "/sem_app_slave"
#define SEM_SLAVE_TO_APP_NAME   "/sem_slave_app"
#define READ_PIPE_END	0
#define WRITE_PIPE_END	1

static char new_line[] = {'\n'};

int main(int argc, char *argv[]) {
    int i;
    int slave[SLAVE_NUMBER];

    if (argc < 2) {
        printf("Error en el ingreso de args");
        return 1;
    }

    /* Configuracion Memoria compartida */
    int fd = shm_open("/memory", O_CREAT | O_RDWR, 0666);
    if (fd < 0){
        perror("Error en el open");
        return 1;
    }
    if (ftruncate(fd, BUFF_SIZE) != 0){
        perror("Error en el ftruncate");
        return 1;
    }
    void * var = mmap(NULL, BUFF_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (var == MAP_FAILED){
        perror("Error en el mmap");
        return 1;
    }
    char * buf = (char *) var;
    *buf = 0;
    
    /* Configuracion Semaforos */
    sem_t * sem_app = sem_open(SEM_APP_NAME, O_CREAT, 0666, SLAVE_NUMBER);
    if (sem_app == SEM_FAILED){
        perror("Error en el sem_open");
        return 1;
    }

    sem_t * sem_slave = sem_open(SEM_SLAVE_NAME, O_CREAT, 0666, 0);
    if (sem_slave == SEM_FAILED){
        perror("Error en el sem_open");
        return 1;
    }

    sem_t * sem_app_to_slave = sem_open(SEM_APP_TO_SLAVE_NAME, O_CREAT, 0666, 0);
    if (sem_app_to_slave == SEM_FAILED){
        perror("Error en el sem_open");
        return 1;
    }

    sem_t * sem_slave_to_app = sem_open(SEM_SLAVE_TO_APP_NAME, O_CREAT, 0666, 1);
    if (sem_slave_to_app == SEM_FAILED){
        perror("Error en el sem_open");
        return 1;
    }

    /* Configuracion de pipes App - Slaves */
    int app_to_slave_fd[2], slave_to_app_fd[2];
    if (pipe(app_to_slave_fd) == -1) {
        perror("Error en el pipe");
        return 1;
    }
    if (pipe(slave_to_app_fd) == -1) {
        perror("Error en el pipe");
        return 1;
    }

    /* Creacion de Esclavos */
    for (i = 0; i < SLAVE_NUMBER; i++) {
    	slave[i] = fork();
    	if (slave[i] < 0){
			perror("Error de fork");    		
			return 1;
    	} else if (slave[i] == 0){
            // close(STDOUT);
            // dup2(slave_to_app_fd[WRITE_PIPE_END], STDOUT);
            close(STDIN);
            dup2(app_to_slave_fd[READ_PIPE_END], STDIN);
            char *args[] = {NULL};
    		execv("slave", args);
	        perror("Error de execv de app");
	        return 1;
    	} else {
    		// Padre
    	}
    }

    /* Reparto de archivos */
    for (i = 1; i < argc; i++){
        sem_wait(sem_app);
        // Recibir archivo
        // Mandar archivo argv[i]
        write(app_to_slave_fd[WRITE_PIPE_END], argv[i], strlen(argv[i]));
        // write(app_to_slave_fd[WRITE_PIPE_END], "Files/hole6.cnf", strlen("Files/hole6.cnf"));
        write(app_to_slave_fd[WRITE_PIPE_END], new_line, 1);
        sem_post(sem_app_to_slave);
        sem_post(sem_slave);
    }

    /* Espera de finalizacion de esclavos */
    for (i = 0; i < SLAVE_NUMBER; i++){
        waitpid(slave[i], NULL, 0);
    }

    /* Cierre y salida */
    close(fd);
    sem_close(sem_app);
    sem_close(sem_slave);
    sem_unlink(SEM_APP_NAME);
    sem_unlink(SEM_SLAVE_NAME);
    return 0;
}