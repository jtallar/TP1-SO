#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>

// probanding si comitea

#define SEM_APP_NAME    "/sem_app"
#define SEM_SLAVE_NAME  "/sem_slave"
#define SEM_APP_TO_SLAVE_NAME   "/sem_app_slave"
#define SEM_SLAVE_TO_APP_NAME   "/sem_slave_app"

#define CHUNK_SIZE      65

int main(int argc, char *argv[]) {
    // /* Apertura de Memoria Compartida */
    // int fd = shm_open("/memory", O_RDWR, 0);
    // if (fd < 0){
    //     perror("Error en el shm_open de slave");
    //     return 1;
    // }
    // void * var = mmap(NULL, sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    // if (var == MAP_FAILED){
    //     perror("Erorr en el mmap de slave");
    //     return 1;
    // }
    // char * buf = (char *) var;

    /* Apertura de Semaforos */
    // sem_t * sem_app = sem_open(SEM_APP_NAME, 0);
    // if (sem_app == SEM_FAILED){
    //     perror("Error en el sem_open de slave");
    //     return 1;
    // }

    // sem_t * sem_slave = sem_open(SEM_SLAVE_NAME, 0);
    // if (sem_slave == SEM_FAILED){
    //     perror("Error en el sem_open de slave");
    //     return 1;
    // }

    // sem_t * sem_app_to_slave = sem_open(SEM_APP_TO_SLAVE_NAME, 0);
    // if (sem_app_to_slave == SEM_FAILED){
    //     perror("Error en el sem_open de slave");
    //     return 1;
    // }

    sem_t * sem_slave_to_app = sem_open(SEM_SLAVE_TO_APP_NAME, 0);
    if (sem_slave_to_app == SEM_FAILED){
        perror("Error en el sem_open de slave");
        return 1;
    }
    /* Ciclo de manejo de archivos */
    while(1) {
        //sem_wait(sem_slave);
        
       // sem_wait(sem_app_to_slave);

        char * name = NULL;
        size_t namecap = 0;
        ssize_t length;
        length = getline(&name, &namecap, stdin);
        if (length <= 0) {
            perror("Error en el getline de slave");
            return 1;
        }
        if (name[0] == '?') {
            printf("\nSaliendo de slave %d\n", getpid());
            return 0;
        }
        name[--length] = '\0';

        // sem_post(sem_app_to_slave);

        // printf("\n#PID: %d\tFILE: %s#\n\n", getpid(), name);

        int ret = fork();
        if (ret < 0){
            perror("Error en la creacion de fork de slave");
            return 1;
        } else if (ret == 0){
            // char *args[] = {"minisat", name, NULL};
            // execv("/usr/bin/minisat", args);
            // perror("Error de execv de slave");	      de slave   
            // return 1;
            sem_wait(sem_slave_to_app);
            printf("%s\n%d\n%d\n%s\n%f\n%d\n", name, 145, 221, "UNSAT", 0.005, getppid());
            sem_post(sem_slave_to_app);
            return 0;
        } else {
            wait(NULL); // Espera a que termine el hijo
        }
        // sem_post(sem_app);
        free(name);
    }

    /* Cierre y salida */
    // sem_close(sem_app);
    // sem_close(sem_slave);
    // sem_close(sem_app_to_slave);
    sem_close(sem_slave_to_app);
    // close(fd); 
    return 0;
}