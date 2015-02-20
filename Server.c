//Server.c 
#include <stdio.h>
#include <string.h>     //strlen
#include <sys/socket.h>
#include <arpa/inet.h>  //inet_addr
#include <unistd.h>     //fork()
#include <sys/wait.h>   //wait()

int OpenSocket(int port);
int ListenIncomingConnection(int sock_fd);
int AcceptConnections(int sock_fd);
int RecieveData(int newSocket);
int SendData(int sock_fd, int newSocket);

typedef struct sockaddr_in sockaddr_in;

int main(int argc , char *argv[])
{
  // create socket
	int port = 10732;
	printf( "creating socket on port %d\n", port );
    int sock_fd = OpenSocket(port); //bind
    if(sock_fd != -1){
       printf("Connected\n");
       //Listen for an incoming connection
       if(ListenIncomingConnection(sock_fd) == -1) return 1;
       //Accept the incoming connection
       int newSocket = AcceptConnections(sock_fd);
       if(newSocket==0) return 0; //this is the child process returning 0
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
    memset(reply_buffer, 0, sizeof(reply_buffer)); //<<<< We may need to do this in other places !
    // recv() will block until there is some data to read.
    if(recv(newSocket, reply_buffer, 256, 0) < 0)
    {
        printf("Failed to recieve message.\n");
        return -1;
    }else{
       printf("Data recieved from client is: %s\n",reply_buffer);
       return 0;
    }
}

int AcceptConnections(int sock_fd){
    //Accept a new connection on a socket
    //http://www.linuxquestions.org/questions/programming-9/sockaddr_in-and-sockaddr_un-difference-629184/
    while(1){
      int sockedID_fd[2];
      //the output of sockedID_fd1 becomes the input for sockedID_fd0
      pipe(sockedID_fd);
      printf("about to fork 2 processes - child and parent. \n");
      int pid = fork();
      struct sockaddr_in newclient; //accept creates a new socket
      socklen_t size = sizeof newclient;
      int newSocket = 0;
      if(pid == 0) { //child process
         printf("in child!!!\n");
         newSocket = accept(sock_fd, (struct sockaddr *) &newclient, &size);
         printf("new Socket in child is: %d\n",newSocket);
         write(sockedID_fd[1], &newSocket,sizeof(newSocket));
         //is it safe to return from a child process like this or is any cleanup needed?
        return 0;
      }else{
         wait(&pid);
         printf("in parent!!!\n");
         int newSocket_fromchildprocess = 0;
         read(sockedID_fd[0], &newSocket_fromchildprocess, sizeof(newSocket_fromchildprocess));
         printf("new Socket in parent is: %d\n",newSocket_fromchildprocess);
         if(newSocket_fromchildprocess < 0){
            printf("Connection cannot be accepted\n");
            return -1; 
         }  
         printf("Connection Accepted.\n");
         return newSocket;
      }
    }
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
    close(sock_fd);
    close(newSocket);
    return 0;
}

