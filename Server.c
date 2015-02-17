//Server.c
#include <stdio.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
 

int OpenSocket(int port);
int SendData(int sock_fd);

typedef struct sockaddr_in sockaddr_in;

int main(int argc , char *argv[])
{
    // create socket
	int port = 30000;
	printf( "creating socket on port %d\n", port );
    int sock_fd = OpenSocket(port); 
    if(sock_fd != -1){
       printf("Connected\n");
       //Send some data
       SendData(sock_fd);
    }
     
    return 0;
}

int OpenSocket(int port)
{   
   struct sockaddr_in mysocket; 
   //create socket
   //INFO: SOCK_DGRAM is used for UDP packets, SOCK_STREAM for TCP.
   //INFO: AF_INET refers to addresses from the internet, IP addresses specifically. PF_INET refers to anything in the protocol, usually sockets/ports.
   //...Passing PF_NET ensures that the connection works right. AF = Address Family. PF = Protocol Family.
   //http://stackoverflow.com/questions/1593946/what-is-af-inet-and-why-do-i-need-it
   //int sock_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
   int sock_fd = socket( PF_INET, SOCK_STREAM, 0); //socket == file descriptor, thus: mysocket
   if(sock_fd == -1)
   {
   	  printf("Could not create socket.");
        return -1;
   }
   
   mysocket.sin_addr.s_addr = inet_addr("127.0.0.1"); //socket binds to localhost
   mysocket.sin_family = AF_INET;
   mysocket.sin_port = htons( port ); //connect it to the port
   /* WILL THIS FIX ANYTHING Set all bits of the padding field to 0 */
   memset(mysocket.sin_zero, '\0', sizeof mysocket.sin_zero);  

   //bind this remote server socket to port
   if (bind( sock_fd, (struct sockaddr*) &mysocket, sizeof(mysocket) ) < 0 )
   {
      printf( "Failed to bind socket.\n" );
      return -1;
   }

   return sock_fd;
}

int SendData(int sock_fd)
{
   sockaddr_in address;
   address.sin_family = AF_INET; //http://stackoverflow.com/questions/1593946/what-is-af-inet-and-why-do-i-need-it
   address.sin_addr.s_addr = inet_addr("127.0.0.1");
   address.sin_port = htons(631);

    while(1){
       //Listen for incoming connections. A max of 1000 connections can happen.
       //Do we want to handle multiple connections another way? Such as by forking?
       if(listen(sock_fd,1000) < 0){
       	   printf("Error\n");
       }

       //Accept a new connection on a socket
       struct sockaddr_storage serverStorage;
       //http://www.linuxquestions.org/questions/programming-9/sockaddr_in-and-sockaddr_un-difference-629184/
       struct sockaddr_in newclient; //accept creates a new socket
       int size = sizeof newclient;
       int newSocket = accept(sock_fd, (struct sockaddr *) &newclient, &size);

       //Finally, a message can be sent!
       char buffer[256];
       strcpy(buffer,"Today is BRIGHT.\n");
       send(newSocket,buffer,sizeof(buffer),0);		
    }
}

