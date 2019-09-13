#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#include <string.h>

#define CHUNK_SIZE      65
#define FILE_NUMBER     28
#define SEM_BUFFER_NAME      "/sem_memory"
#define EXIT_CAR        '?'

const char *headers[] = {"Nombre del archivo: ", "Cantidad de variables: ", "Cantidad de clausulas: ", "Tiempo de procesamiento: ", "Resultado: ", "ID del slave: "};

int main(int argc, char *argv[]) {
    /* Obtencion del codigo de la shared memory */
    char buf_name[10] = {0};
    char * app_pid = NULL;
    size_t namecap = 0;
    ssize_t length;
    length = getline(&app_pid, &namecap, stdin);
    if (length <= 0) {
        perror("Error en el getline de vista");
        return 1;
    }
    app_pid[--length] = '\0';
    sprintf(buf_name, "/mem-%s", app_pid);

    /* Apertura de la shared memory */
    int fd = shm_open(buf_name, O_RDWR, 0);
    if (fd < 0){
        perror("Error en el shm_open de vista");
        return 1;
    }
    void * var = mmap(NULL, FILE_NUMBER * CHUNK_SIZE, PROT_READ, MAP_SHARED, fd, 0);
    if (var == MAP_FAILED){
        perror("Erorr en el mmap de vista");
        return 1;
    }
    char * buffer = (char *) var;

    /* Apertura del semaforo */
    sem_t * sem_buffer = sem_open(SEM_BUFFER_NAME, 0);
    if (sem_buffer == SEM_FAILED){
        perror("Error en el sem_open de vista");
        return 1;
    }
    
    /* Lectura de buffer */
    int index = 0, header_index = 0;
    do {
        sem_wait(sem_buffer);
        if (buffer[index] != EXIT_CAR){
            printf("%s", headers[header_index % 6]);
            header_index++;
            while (buffer[index] != '\n' && buffer[index] != EXIT_CAR)
                putchar(buffer[index++]);
            putchar('\n');
        }
    } while (buffer[index++] != EXIT_CAR);

    /* Cierre de archivos y salida */
    sem_close(sem_buffer);
    close(fd);
    return 0;
}