#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define STDIN			0
#define STDOUT			1
#define READ_PIPE		0
#define WRITE_PIPE		1
#define N 				2
int main(int argc, char *argv[]) {
    int read_pipe[2], write_pipe[2]; // pipe[0] es de lectura, pipe[1] es de escritura

    if (pipe(read_pipe) == -1){
    	printf("Error en la creacion del pipe\n");
    	return 1;
    }
    if (pipe(write_pipe) == -1){
    	printf("Error en la creacion del pipe\n");
    	return 1;
    }
    int ret[N];
    int i;
    for (i = 0; i < N; i++){
		ret[i] = fork();
		if (ret[i] < 0){
			printf("Error en la creacion de fork\n");
			return 1;
		} else if (ret[i] == 0){
			close(STDOUT);
			dup2(write_pipe[WRITE_PIPE], STDOUT);
			close(STDIN);
			dup2(read_pipe[READ_PIPE], STDIN);
	        char *args[] = {NULL};
			execv("c", args);
	        printf("Error de ejecucion\n");
	        return 1;
		}
	}
	char buf[50];
	i = 0;
	do {
		sprintf(buf, "exit%d\n", i % 10);
		write(read_pipe[WRITE_PIPE], buf, strlen(buf));
		int index = read(write_pipe[READ_PIPE], buf, 49);
		buf[index] = 0;
		printf("En pipes.c:#%s#", buf);
		i++;
	//} while (strcmp(buf, "exit\n") != 0);
	} while (i < N * 10);
	
	for (i = 0; i < N; i++)
		waitpid(ret[i], NULL, 0);
    close(read_pipe[WRITE_PIPE]);
    close(read_pipe[READ_PIPE]);
    close(write_pipe[WRITE_PIPE]);
    close(write_pipe[READ_PIPE]);
    
    return 0;
}