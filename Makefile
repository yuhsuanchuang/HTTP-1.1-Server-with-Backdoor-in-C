# build an executable named myprog from myprog.c
all: part3.c 
	gcc -pthread -o normal_web_server part3.c
