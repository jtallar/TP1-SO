#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <string.h>

#define STDIN			0
#define STDOUT			1

#define SEM_APP_NAME            "/sem_app"
#define SEM_SLAVE_NAME          "/sem_slave"
#define SEM_APP_TO_SLAVE_NAME   "/sem_app_slave"
#define SEM_SLAVE_TO_APP_NAME   "/sem_slave_app"

#define CHUNK_SIZE      65
#define CMD_LENGTH      170

int main(int argc, char *argv[]) {
    int salir = 1;
    sem_t * sem_slave_to_app = sem_open(SEM_SLAVE_TO_APP_NAME, 0);
    if (sem_slave_to_app == SEM_FAILED){
        perror("Error en el sem_open de slave");
        return 1;
    }
    char cmd_buf[CMD_LENGTH];
    
    /* Ciclo de manejo de archivos */
    while(salir) {
        /* Lectura de nombre de archivo */
        char * name = NULL;
        size_t namecap = 0;
        ssize_t length;
        length = getline(&name, &namecap, stdin);
        if (length <= 0) {
            perror("Error en el getline de slave");
            free(name);
            return 1;
        }
        if (name[0] != '?') {
            name[--length] = '\0';
    
            /* Ejecucion de minisat con parseo de salida */
            sprintf(cmd_buf, "minisat %s | awk '{if(/Number of clauses/){print $5} if(/Number of variables/){print $5} if(/CPU time/) {print $4} if(/SAT/) {print $1}}'", name);
            FILE *fp = popen(cmd_buf,"r");
            if (fp == NULL) {
                perror("Error en el popen de slave");
                return 1;
            }

            /* Armado de string de informacion del procesamiento */
            char * out = malloc(CHUNK_SIZE);
            char * beg = out;
            out = stpcpy(out, name);
            *out = '\n'; 
            out++;
            namecap = CHUNK_SIZE;
            while ((length = getline(&out, &namecap, fp)) != -1) {
                out += length;
            }
            sprintf(out, "%d\n", getpid());

            /* Envio de informacion */
            sem_wait(sem_slave_to_app);
            write(STDOUT, beg, strlen(beg));
            sem_post(sem_slave_to_app);

            free(beg);
            pclose(fp);
        } else{
            salir = 0;
        }
        free(name);
    }

    /* Cierre y salida */
    sem_close(sem_slave_to_app);
    return 0;
}