# TP1-SO
Para compilar y ejecutar:
		apt-get install minisat
		gcc -Wall app.c -o app -lrt -pthread
		gcc -Wall slave.c -o slave -lrt -pthread
		gcc -Wall vista.c -o vista -lrt -pthread
		./app
