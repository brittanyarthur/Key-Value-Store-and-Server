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

int OpenSocket(int port);
int RecieveData(int sock_fd);

int main( int argc, char * argv[] )
{
   int port = 631;//30000;
   printf( "creating socket on port %d\n", port );
   int sock_fd = OpenSocket(port); 
   if(sock_fd != -1){
      RecieveData(sock_fd);
   }
}

int OpenSocket(int port)
{   
   struct sockaddr_in remote_server; 
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
   remote_server.sin_addr.s_addr = inet_addr("127.0.0.1"); //get data from this server
   remote_server.sin_family = AF_INET;
   remote_server.sin_port = htons( (unsigned short) port ); //connect it to the port

   if (connect( sock_fd, (struct sockaddr*) &remote_server, sizeof(remote_server) ) < 0 )
   {
         printf( "Failed to connect to remote server.\n" );
         return -1;
   }
   //what condition to break loop?
   //???close(remote_server); how to close?
   
   return sock_fd;
}

int RecieveData(int sock_fd){
   //recieve data
   while ( 1 )
   {
      unsigned char reply_buffer[256];
      if(recv(sock_fd, reply_buffer, 256, 0) < 0)
      {
         printf("Failed to recieve message.\n");
      }else{
         printf("Data recieved is: %s\n",reply_buffer);
      }

      sleep(1);
   }
}
