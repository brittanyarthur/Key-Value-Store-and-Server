//Clientc.c
/*
To Run: gcc Clientc.c -o client
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

int OpenSocket();

typedef struct Socket{
   int port;
}Socket;

int main( int argc, char * argv[] )
{
   int port = 30001;
   printf( "creating socket on port %d\n", port );
   Socket* socket = malloc(sizeof(struct Socket));
   free(socket);

      
}

sockaddr_in OpenSocket(int port, Socket* sock)
{   
   struct sockaddr_in server;
   int open_result = socket( AF_INET, SOCK_DGRAM, IPPROTO_TCP );
   if(open_result == -1)
   {
   	  printf("Could not create socket.");
   }
   return 3;
}
