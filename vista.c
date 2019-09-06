#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#define M   100000

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
    int * aux = (int *) var; // SI hago aux++, se mueve sizeof(int)

    sem_t * sem = sem_open("/semaphore", 0);
    if (sem == SEM_FAILED){
        perror("Error en el sem_open");
        return 1;
    }

    sem_wait(sem);
    int i;
    for (i = 0; i < M; i++) {
    	(*aux)++;
    }
    sem_post(sem);
    sem_close(sem);
    close(fd);
    return 0;
}