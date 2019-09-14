# TP1-SO
tp1: app.c slave.c vista.c
	gcc -Wall -g app.c -o app -lrt -pthread
	gcc -Wall -g slave.c -o slave -lrt -pthread
	gcc -Wall -g vista.c -o vista -lrt -pthread

