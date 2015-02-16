//Clientc.c
/*
To Run: gcc Client.c -o client

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
*/
#include <stdlib.h> //free()
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr

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

int OpenSocket(int port)
{   
   struct sockaddr_in mysocket;
   //create socket
   int open_result = socket( AF_INET, SOCK_DGRAM, IPPROTO_TCP );
   if(open_result == -1)
   {
   	  printf("Could not create socket.");
   }

   //bind socket to port
   mysocket.sin_addr.s_addr = inet_addr("127.0.0.1"); //socket binds to localhost
   mysocket.sin_family = AF_INET;
   mysocket.sin_port = htons( port ); //connect it to the port

   if ( bind( open_result, (struct sockaddr*) &mysocket, sizeof(mysocket) ) < 0 )
         {
            printf( "failed to bind socket\n" );
            return -1;
         }

   return 1;
}
