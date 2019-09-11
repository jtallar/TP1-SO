#include <stdio.h>
#include <stdlib.h>
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
#define SLAVE_NUMBER    3
#define INIT_FILES      3
#define CHUNK_LINES     6
#define CHUNK_SIZE      65
#define SEM_APP_NAME    "/sem_app"
#define SEM_SLAVE_NAME  "/sem_slave"
#define SEM_APP_TO_SLAVE_NAME   "/sem_app_slave"
#define SEM_SLAVE_TO_APP_NAME   "/sem_slave_app"
#define READ_PIPE_END	0
#define WRITE_PIPE_END	1
#define EXIT_CAR        '?'

static char new_line[] = {'\n'};
static char exit_line[] = {EXIT_CAR};

int get_slave_index(int * slaves, int pid);

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
    if (ftruncate(fd, CHUNK_SIZE * argc) != 0){
        perror("Error en el ftruncate");
        return 1;
    }
    void * var = mmap(NULL, CHUNK_SIZE * argc, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
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

    /* Configuracion de pipes App --> Slaves */
    int app_to_slave_fd[SLAVE_NUMBER][2];
    for (i = 0; i < SLAVE_NUMBER; i++) {
        if (pipe(app_to_slave_fd[i]) == -1){
            perror("Error en el pipe");
            return 1;
        }
    }
    int slave_to_app_fd[2];
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
            close(STDOUT);
            dup2(slave_to_app_fd[WRITE_PIPE_END], STDOUT);
            close(STDIN);
            dup2(app_to_slave_fd[i][READ_PIPE_END], STDIN);
            char *args[] = {NULL};
    		execv("slave", args);
	        perror("Error de execv de app");
	        return 1;
    	} else {
    		// Padre
    	}
    }

    /* Reparto inicial de archivos */
    int arg_index = 1, recieved_files = 0;
    int j = 0;
    for (i = 0; i < SLAVE_NUMBER; i++) {
        for (j = 0; j < INIT_FILES; j++) {
            // printf("\n$APP File %s to Slave PID: %d$\n", argv[arg_index], slave[i]);
            write(app_to_slave_fd[i][WRITE_PIPE_END], argv[arg_index], strlen(argv[arg_index]));
            write(app_to_slave_fd[i][WRITE_PIPE_END], new_line, 1);
            arg_index++;
        }
    }

    /* Recepcion y reparto de archivos */
    FILE * stream = fdopen(slave_to_app_fd[READ_PIPE_END], "r");
    int line_count = 0;
    while (arg_index < argc || recieved_files < arg_index - 1) {
        char * line = NULL;
        size_t namecap = 0;
        ssize_t length;
        length = getline(&line, &namecap, stream);
        if (length < 0) {
            perror("Error en el getline");
            return 1;
        }
        // Mandar a Buffer
        printf("APP: %s", line);
        line_count++;
        if (line_count == CHUNK_LINES) {
            recieved_files++;
            printf("\narg_index = %d\trecieved_files = %d\n", arg_index, recieved_files);
            line_count = 0;
            if (arg_index < argc) {
                int pipe_index = get_slave_index(slave, atoi(line));
                if (pipe_index < 0)
                    return 1;
                printf("\n$APP File %s to Slave PID: %d de indice %d$\n", argv[arg_index], atoi(line), pipe_index);
                write(app_to_slave_fd[pipe_index][WRITE_PIPE_END], argv[arg_index], strlen(argv[arg_index]));
                write(app_to_slave_fd[pipe_index][WRITE_PIPE_END], new_line, 1);
                arg_index++;
            }
        }
        free(line);
    }

    /* Avisarles a los slaves que terminen */
    for (i = 0; i < SLAVE_NUMBER; i++) {
        write(app_to_slave_fd[i][WRITE_PIPE_END], exit_line, 1);
        write(app_to_slave_fd[i][WRITE_PIPE_END], new_line, 1);
    }

    printf("\n\n*WAITING*\n\n");

    /* Reparto de archivos */
    // for (i = 1; i < argc; i++){
    //     sem_wait(sem_app);
    //     // Recibir archivo
    //     // Mandar archivo argv[i]
    //     write(app_to_slave_fd[WRITE_PIPE_END], argv[i], strlen(argv[i]));
    //     // write(app_to_slave_fd[WRITE_PIPE_END], "Files/hole6.cnf", strlen("Files/hole6.cnf"));
    //     write(app_to_slave_fd[WRITE_PIPE_END], new_line, 1);
    //     sem_post(sem_app_to_slave);
    //     sem_post(sem_slave);
    // }

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

int get_slave_index(int * slaves, int pid) {
    int i = 0;
    for (i = 0; i < SLAVE_NUMBER; i++)
        if (slaves[i] == pid)
            return i;
    return -1;
}