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
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h> //sleep

int OpenSocket();

int main( int argc, char * argv[] )
{
   int port = 30001;
   printf( "creating socket on port %d\n", port );
   OpenSocket(port); 
}

int OpenSocket(int port)
{   
   struct sockaddr_in mysocket; 
   //create socket
   //INFO: SOCK_DGRAM is used for UDP packets, SOCK_STREAM for TCP.
   //INFO: AF_INET refers to addresses from the internet, IP addresses specifically. PF_INET refers to anything in the protocol, usually sockets/ports.
   //...Passing PF_NET ensures that the connection works right. AF = Address Family. PF = Protocol Family.
   //http://stackoverflow.com/questions/1593946/what-is-af-inet-and-why-do-i-need-it
   int sock_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); //socket == file descriptor, thus: mysocket
   if(sock_fd == -1)
   {
   	  printf("Could not create socket.");
        return -1;
   }

   //bind socket to port
   mysocket.sin_addr.s_addr = inet_addr("127.0.0.1"); //socket binds to localhost
   mysocket.sin_family = AF_INET;
   mysocket.sin_port = htons( port ); //connect it to the port

   if (bind( sock_fd, (struct sockaddr*) &mysocket, sizeof(mysocket) ) < 0 )
   {
         printf( "Failed to bind socket.\n" );
         return -1;
   }

   //recieve data
   while ( 1 )
   {
      unsigned char reply_buffer[256];
      if(recv(sock_fd, reply_buffer, 256, 0) < 0)
      {
         printf("Failed to recieve message.\n");
      }
      printf("Data recieved is: %s\n",reply_buffer);

      sleep(1);
   }
   //what condition to break loop?
   //???close(mysocket); how to close?

   return 1;
}
