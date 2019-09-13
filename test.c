#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#include <string.h>

#define CMD_LENGTH      170
#define FILE_NUMBER     28
#define SEM_BUFFER_NAME      "/sem_memory"
#define EXIT_CAR        '?'

const char *headers[] = {"Variables: ", "Clausulas: ", "Tiempo: ", "Resultado: "};

int main(int argc, char *argv[]) {
    char cmd_buf[CMD_LENGTH];
    char name[] = "Prueba/hole6.cnf";
    sprintf(cmd_buf, "minisat %s | awk '{if(/Number of clauses/){print $5} if(/Number of variables/){print $5} if(/CPU time/) {print $4} if(/SAT/) {print $1}}'", name);
    FILE *fp = popen(cmd_buf,"r");
    int i = 0;
    while ( fgets( cmd_buf, BUFSIZ, fp ) != NULL ) {
        printf("%s->%s", headers[i++], cmd_buf);
    }
    pclose(fp);
    return 0;
}