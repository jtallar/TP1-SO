# TP1-SO
Para compilar y ejecutar:
		apt-get install minisat
		gcc -Wall app.c -o app -lrt -pthread
		gcc -Wall slave.c -o slave -lrt -pthread
		gcc -Wall vista.c -o vista -lrt -pthread
		./app Prueba/* | ./vista

Para Valgrind:
		valgrind ./app

Si complico con gcc -g, me genera mas info en el ejecutable que permite que valgrind tenga mas precision.

Valgrind solo encuentra errores del path ejecutando en este momento, no analiza todos los caminos.
Cppcheck sí los encuentra.

PVS-studio larga la salida en report.tasks
Con el codigo V... buscas ese error --> Para ver ejemplos y eso

strace 	para detectar Busy Waiting --> Ver si se llama repetitivamente una syscall
	para detectar donde estan los errores al llamar a una syscall
	--> Ver como correrlo en Google

How to make a makefile: http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

