#include <stdio.h>
#include <sys/types.h>	
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#define ISspace(x) isspace((int)(x))
int server_sock = -1;

/**********************************************************************/
//receive message from client 
/**********************************************************************/
int get_line(int sock, char *buf)
{
  int i = 0;
  char c = '\0';
  int n;
  while (c != '\n')
  {
    n = recv(sock, &c, 1, 0);
    if (n > 0)
    {
      if (c == '\r')
      {
        n = recv(sock, &c, 1, MSG_PEEK);
        if ((n > 0) && (c == '\n'))
          recv(sock, &c, 1, 0);
        else
          c = '\n';
      }
      buf[i] = c;
      i++;
    }
    else
      c = '\n';
  }
  buf[i] = '\0';
  printf("buf = %s\n", buf);
  return(i);
}
/**********************************************************************/
//send http 404
/**********************************************************************/
void not_found(int client)
{
  printf("not_found\n");
  char buff[1024];

  sprintf(buff, "HTTP/1.1 404 NOT FOUND\r\n");
  send(client, buff, strlen(buff), 0);
  
  sprintf(buff, "\r\n");
  send(client, buff, strlen(buff), 0);
}
/**********************************************************************/
//execute command
/**********************************************************************/
void serve_exec(int client, char *command)
{
	
	FILE *resource = NULL;
 	int numchars = 1;
 	char buff[1024];
 	int c;
  
 	c = system(command);
 	printf("%d\n", c);
 	if(c == 0)
 	{
    strcat(command, " > output.txt");
    system(command);
 		resource = fopen("output.txt", "r");

		strcpy(buff, "HTTP/1.1 200 OK\r\n");//f
		send(client, buff, strlen(buff), 0);
   
    sprintf(buff, "\r\n");
    send(client, buff, strlen(buff), 0);
   
	 	if (resource == NULL)
    {
	 		not_found(client);
    }
	 	else
	 	{
		  fgets(buff, sizeof(buff), resource);
		  while (!feof(resource))
		  {
		  	sprintf(buff, "%s", buff);
		  	send(client, buff, strlen(buff), 0);
		    fgets(buff, sizeof(buff), resource);
		  }
	 	}
   
	 	fclose(resource);
 	}
 	else
 	{
  	not_found(client);
 	}
}

/**********************************************************************/
// get url and parse it
/**********************************************************************/
void *accept_request(void *client_sock)
{
  printf("pthread create\n");
  char buf[1024];
  int *client_s = (int*)client_sock;
  int client = *client_s;
  int numchars=1;
  char method[255];
  char url[1024];
  char path[512];
  char command[1024];
  size_t i, j;

  numchars = get_line(client, buf);
  
  if(numchars > 0)
  {
    i = 0; j = 0;
    while (!ISspace(buf[j]) && (i < sizeof(method) - 1))
    {
      method[i] = buf[j];
      i++; j++;
    }
    method[i] = '\0';
    printf("method = %s\n", method);

    i = 0;

    while (ISspace(buf[j]) && (j < strlen(buf)))
      j++;
    while (!ISspace(buf[j]) && (j < strlen(buf)))
    {
      url[i] = buf[j];
      i++; j++;
    }
    url[i] = '\0';
    printf("url = %s\n", url);

    if (strcasecmp(method, "GET") == 0)
    {
      printf("GET!!!!!!!!!!!\n");
      i = 0;
      j = 1;
      while(url[j] != '/' && url[j] != '\0')  ////////exec
      {
        path[i] = url[j];
        i++; j++;
      }
      path[i] = '\0';
      printf("path = %s\n", path);

      i = 0;
      if(url[j] == '/') /////command
      {
        j++;
        while(url[j] != '\0')
        {
          command[i] = url[j];
          i++; j++;
        }
        command[i] = '\0';
      }
      
      i = 0;
      while(command[i+2] != '\0')
      {
        if(command[i] == '%' && command[i+1] == '2' && command[i+2] == '0')
        {
          command[i] = ' ';
          j = i+3;
          command[j-2] = command[j];
          while(command[j] != '\0')
          {
            j++;
            command[j-2] = command[j];
          }
          command[j] = '\0';
          command[j-1] = '\0';
          command[j-2] = '\0';
        }
        i++;
      }
      
      printf("------------------------- %s from %d -------------------------\n", command, client);

      if(strcasecmp(path, "exec") != 0 || command[0] == '\0')
      {
        numchars=1;
        not_found(client);
        ////404
      }
      else
      {
        numchars=1;
        serve_exec(client, command);
      }
    }
    else
    {
      numchars=1;
      not_found(client);
        ////404
    }
  }
  else
  {
    numchars=1;
    not_found(client);
      ////404
  }
  printf("close!!\n");
  shutdown(client, 0); // close recieve
  close(client);
  pthread_exit(NULL);
}

/**********************************************************************/
// show error
/**********************************************************************/

void error_die(const char *sc)
{
  perror(sc);
  exit(1);
}

int startup(u_short *port)
{
  int httpd = 0;
  struct sockaddr_in name;

  httpd = socket(PF_INET, SOCK_STREAM, 0);
  if (httpd == -1)
    error_die("socket");

  memset(&name, 0, sizeof(name));
  name.sin_family = AF_INET;
  name.sin_port = htons(*port);
  name.sin_addr.s_addr = htonl(INADDR_ANY);
 ////////////////////////////////////////////////////////////////////////////////add setsockopt SO_REUSEADDR
  if (setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
    error_die("setsockopt(SO_REUSEADDR) failed");

  if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0)
    error_die("bind");

  if (*port == 0)  /* if dynamically allocating a port */
  {
    int namelen = sizeof(name);
    if (getsockname(httpd, (struct sockaddr *)&name, (socklen_t *)&namelen) == -1)
      error_die("getsockname");
    *port = ntohs(name.sin_port);
  }

  if (listen(httpd, 5) < 0)
    error_die("listen");

  return(httpd);
}

int main(int argc, char *argv[])
{
  u_short port = 8888;
  port =  atoi(argv[1]);
  int client_sock = -1;
  struct sockaddr_in client_name;
  int client_name_len = sizeof(client_name);
  pthread_t newthread;
  server_sock = startup(&port);
  printf("httpd running on port %d, server sock = %d\n", port, server_sock);

  while (1)
  {
    /* Connection request on original socket. */
    client_sock = accept(server_sock,
                 (struct sockaddr *)&client_name,
                 (socklen_t *)&client_name_len);
    if (client_sock == -1)
      error_die("accept");
    printf("accept client_sock = %d\n", client_sock);
    if (pthread_create(&newthread , NULL, accept_request, &client_sock) != 0)
      perror("pthread_create");

  }
  close(server_sock);

  return(0);
}
