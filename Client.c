//Clientc.c 

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
   int port =  30000;
   printf( "creating socket on port %d\n", port );
   int sock_fd = OpenSocket(port); 
   printf("Socket has been opened\n");
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

   // Set IP address
   remote_server.sin_addr.s_addr = inet_addr("127.0.0.1"); 
   // Address family = Internet 
   remote_server.sin_family = AF_INET;
   // Set port number. htons function creates proper bigendian byte order for networking.
   remote_server.sin_port = htons( (unsigned short) port ); 

   if (connect( sock_fd, (struct sockaddr*) &remote_server, sizeof(remote_server) ) < 0 )
   {
         printf( "Failed to connect to remote server.\n" );
         return -1;
   }

   return sock_fd;
}

int RecieveData(int sock_fd){
   unsigned char reply_buffer[256];

   // recv() will block until there is some data to read.
   if(recv(sock_fd, reply_buffer, 256, 0) < 0)
   {
       printf("Failed to recieve message.\n");
   }else{
       printf("Data recieved is: %s\n",reply_buffer);
   }
}
