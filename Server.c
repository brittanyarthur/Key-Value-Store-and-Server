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
	int port = 30001;
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
   int sock_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); //socket == file descriptor, thus: mysocket
   if(sock_fd == -1)
   {
   	  printf("Could not create socket.");
        return -1;
   }
   
   mysocket.sin_addr.s_addr = inet_addr("127.0.0.1"); //socket binds to localhost
   mysocket.sin_family = AF_INET;
   mysocket.sin_port = htons( port ); //connect it to the port

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
   char* message = "Today is BRIGHT.";
   sockaddr_in address;
   address.sin_family = AF_INET; //http://stackoverflow.com/questions/1593946/what-is-af-inet-and-why-do-i-need-it
   address.sin_addr.s_addr = inet_addr("127.0.0.1");
   address.sin_port = htons(30000);
   while (1){
      if(sendto(sock_fd, message, sizeof(message), 0, (struct sockaddr*)&address, sizeof(sockaddr_in)) < 0)
      {
          printf("Send failed\n");
      }
      printf("Data Sent\n");
   }
}

