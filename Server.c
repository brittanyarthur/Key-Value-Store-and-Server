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
int SendData(int sock_fd, int newSocket, int data_recieved);

typedef struct sockaddr_in sockaddr_in;

int main(int argc , char *argv[])
{
  // create socket
	int port = 10732;
	printf( "creating socket on port %d\n", port );
    int sock_fd = OpenSocket(port); //bind
    if(sock_fd != -1){
       printf("Connected\n");
       //Accept the incoming connection
       AcceptConnections(sock_fd);
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
    printf("about to recieve data\n");
    //get the incoming message from the client.
    char reply_buffer[256];
    //clear buffer before writing to it
    memset(reply_buffer, 0, sizeof(reply_buffer)); //<<<< We may need to do this in other places !
    // recv() will block until there is some data to read.
    if(recv(newSocket, reply_buffer, 256, 0) < 0)
    {
        printf("Failed to recieve message.\n");
        return -1;
    }else{
       printf("Data recieved from client is: %s\n",reply_buffer);
       if(strcmp(reply_buffer, "no\n") == 0){
           return 1; // 1 maps to no
       } else if(strcmp(reply_buffer, "yes\n") == 0){
           return 2; // 2 maps to yes
       } else if(strcmp(reply_buffer, "quit\n") == 0){
           printf("EXITING NOW\n");
           close(newSocket);
       }
       return 0; // 0 maps to other
    }
}

int AcceptConnections(int sock_fd){
    //Accept a new connection on a socket
    while(1){
      //the output of sockedID_fd1 becomes the input for sockedID_fd0
      //The first integer in the array (element 0) is set up and opened for reading, 
      //while the second integer (element 1) is set up and opened for writing. 
      printf("about to fork 2 processes - child and parent. \n");
      struct sockaddr_in newclient; //accept creates a new socket
      socklen_t size = sizeof newclient;
      int newSocket = 0;
      //waiting to accept a connection
      //Listen for an incoming connection
      if(ListenIncomingConnection(sock_fd) == -1) printf("Error while listening\n");
      newSocket = accept(sock_fd, (struct sockaddr *) &newclient, &size);
      int pid = fork();
      if(pid == 0) { //child process
         printf("in child!!!\n");
         while(1){ // loop for an ongoing conversation with the client
            //is it safe to return from a child process like this or is any cleanup needed?
            // 0 maps to other, 1 maps to no, 2 maps to yes 
            int data_recieved = RecieveData(newSocket); //we can change the return value to a char* but then we would have to allocate memory
            if(data_recieved == -1){
                printf("Error recieving data.\n"); 
                return -1;
            }
            //Send some data
            SendData(sock_fd, newSocket, data_recieved);
         }
         return 0;
      }else{
         //wait(&pid);
         printf("in parent!!!\n");
      }
    }
}

int SendData(int sock_fd, int newSocket, int data_recieved)
{
    //Finally, a message can be sent!
    char buffer[256];
    if(data_recieved == 1){
       strcpy(buffer,"It must be a study day for you then!"); 
    }else if(data_recieved == 2){
       strcpy(buffer,"Today is BRIGHT."); 
    }else{
       strcpy(buffer,"Nooo you have to answer the question!!"); 
    }
    if(send(newSocket,buffer,sizeof(buffer),0) < 0){
    	printf("Error sending message\n");
    	return -1;
    }
    printf("Message Successfully Sent.\n");	
    return 0;
}

