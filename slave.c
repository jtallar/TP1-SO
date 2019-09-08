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
#define SEM_APP_TO_SLAVE_NAME   "/sem_app_slave"
#define SEM_SLAVE_TO_APP_NAME   "/sem_slave_app"

#define MAX_LENGTH  50

int main(int argc, char *argv[]) {
    /* Apertura de Memoria Compartida */
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

    /* Apertura de Semaforos */
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

    sem_t * sem_app_to_slave = sem_open(SEM_APP_TO_SLAVE_NAME, 0);
    if (sem_app_to_slave == SEM_FAILED){
        perror("Error en el sem_open");
        return 1;
    }

    sem_t * sem_slave_to_app = sem_open(SEM_SLAVE_TO_APP_NAME, 0);
    if (sem_slave_to_app == SEM_FAILED){
        perror("Error en el sem_open");
        return 1;
    }

    /* Ciclo de manejo de archivos */
    while(1) {
        sem_wait(sem_slave);
        
        char * name = NULL;
        size_t namecap = 0;
        ssize_t length;
        sem_wait(sem_app_to_slave);
        length = getline(&name, &namecap, stdin);
        if (length < 0) {
            perror("Error en el getline");
            return 1;
        }
        name[--length] = '\0';
        sem_post(sem_app_to_slave);

        int ret = fork();
        if (ret < 0){
            printf("Error en la creacion de fork\n");    		
            return 1;
        } else if (ret == 0){
            char *args[] = {"minisat", name, NULL};
            execv("/usr/bin/minisat", args);
            perror("Error de execv de slave");	        
            return 1;
        } else {
            wait(NULL); // Espera a que termine el hijo
        }
        sem_post(sem_app);
    }

    /* Cierre y salida */
    sem_close(sem_app);
    sem_close(sem_slave);
    close(fd);
    return 0;
}