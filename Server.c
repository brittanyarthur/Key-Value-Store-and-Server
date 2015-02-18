//Server.c 
#include <stdio.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
 

int OpenSocket(int port);
int ListenIncomingConnection(int sock_fd);
int AcceptConnection(int sock_fd);
int RecieveData(int newSocket);
int SendData(int sock_fd, int newSocket);

typedef struct sockaddr_in sockaddr_in;

int main(int argc , char *argv[])
{
  // create socket
	int port = 30000;
	printf( "creating socket on port %d\n", port );
    int sock_fd = OpenSocket(port); //bind
    if(sock_fd != -1){
       printf("Connected\n");
       //Listen for an incoming connection
       if(ListenIncomingConnection(sock_fd) == -1) return 1;
       //Accept the incoming connection
       int newSocket = AcceptConnection(sock_fd);
       if(newSocket==-1) return 1;
       if(RecieveData(newSocket) == -1) return 1;
       //Send some data
       SendData(sock_fd, newSocket);
    }
     
    return 0;
}

int OpenSocket(int port)
{   
   struct sockaddr_in mysocket; 
   //create socket
   int sock_fd = socket( PF_INET, SOCK_STREAM, 0); 
   if(sock_fd == -1)
   {
   	  printf("Could not create socket.\n");
        return -1;
   }

   //socket binds to localhost
   mysocket.sin_addr.s_addr = inet_addr("127.0.0.1"); 
   mysocket.sin_family = AF_INET;
   //connect socket to the port
   mysocket.sin_port = htons( port ); 

   //bind this remote server socket to port
   if (bind( sock_fd, (struct sockaddr*) &mysocket, sizeof(mysocket) ) < 0 )
   {
      printf( "Failed to bind socket.\n" );
      return -1;
   }

   return sock_fd;
}

int ListenIncomingConnection(int sock_fd){
   //Listen for incoming connections. A max of 1000 connections can happen.
   //Do we want to handle multiple connections another way? Such as by forking?
   if(listen(sock_fd,1000) < 0){
       printf("Error\n");
       return -1;
    }
    printf("Listening\n");
    return 0;
}

int RecieveData(int newSocket){
    //get the incoming message from the client.
    unsigned char reply_buffer[256];
    //clear buffer before writing to it
    memset(reply_buffer, 0, sizeof(reply_buffer));
    // recv() will block until there is some data to read.
    if(recv(newSocket, reply_buffer, 256, 0) < 0)
    {
        printf("Failed to recieve message.\n");
        return -1;
    }else{
       printf("Data recieved is: %s\n",reply_buffer);
       return 0;
    }
}

int AcceptConnection(int sock_fd){
    //Accept a new connection on a socket
    //http://www.linuxquestions.org/questions/programming-9/sockaddr_in-and-sockaddr_un-difference-629184/
    struct sockaddr_in newclient; //accept creates a new socket
    socklen_t size = sizeof newclient;
    int newSocket = accept(sock_fd, (struct sockaddr *) &newclient, &size);
    if(newSocket < 0){
      printf("Connection cannot be accepted\n");
      return -1;
    }
    printf("Connection Accepted.\n");

    return newSocket;
}

int SendData(int sock_fd, int newSocket)
{
    //Finally, a message can be sent!
    char buffer[256];
    strcpy(buffer,"Today is BRIGHT.");
    if(send(newSocket,buffer,sizeof(buffer),0) < 0){
    	printf("Error sending message\n");
    	return -1;
    }
    printf("Message Successfully Sent.\n");	
    return 0;
}

