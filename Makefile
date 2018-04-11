# build an executable named myprog from myprog.c
all: backdoor.c 
	gcc -pthread -o normal_web_server backdoor.c