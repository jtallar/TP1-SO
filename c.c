#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define STDIN           0
#define STDOUT          1
#define END_CAR         '/'

void read_message  (char * buf, int size) {
    char c;
    int i;
    while ((c = getchar()) != END_CAR && c != EOF && c != '\n' && i < size - 1){
        buf[i++] = c; 
    }
    buf[i] = '\0';
}

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);   // Set stdout stream as unbuffered
                                        // Information appears on destination as soon as written
    char buf[50];
    do {
        fgets(buf, 50, stdin); // Lee hasta \n o EOF y agrega \0
        // read_message(buf, 50);
        printf("%d/%s", getpid(), buf);
    } while (strcmp(buf, "exit9") != 0);

    return 0;
}