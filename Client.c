//Clientc.c

#include <stdlib.h> //free()
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h> //sleep
#include <ctype.h> //tolower()

int OpenSocket(int remote_port, const char* remote_IP);
int RecieveData(int sock_fd);
int WriteData(int sock_fd);
char* do_insert();
char* do_delete();
char* do_lookup();
char* do_init();
int parse_command_response(char* response);
char* BuildPacket(char* cmd, char* name, char* length, char* size, char* key, char* value);

int main( int argc, char * argv[] )
{
   (void)argc;
   (void)argv;
   int remote_port =  10732;
   const char* remote_IP = "127.0.0.1";
   int sock_fd = OpenSocket(remote_port, remote_IP); 
   printf("Socket has been opened\n");
   int write_result = 0;
   int recieve_result = 0;
   while(1){
      if(sock_fd != -1){
         write_result = WriteData(sock_fd);
      }else{
         printf("Error: file descriptor error");
      }
      if(write_result != -1){
         recieve_result = RecieveData(sock_fd);
      }
      if(recieve_result == -1){
         printf("An Error has occured.\n");
      }
   }
   return 1;
}

int OpenSocket(int port, const char* remote_IP)
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
   remote_server.sin_addr.s_addr = inet_addr(remote_IP); 
   // Address family = Internet 
   remote_server.sin_family = AF_INET;
   // Set port number. htons function creates proper bigendian byte order for networking.
   remote_server.sin_port = htons( (unsigned short) port ); 

   if (connect(sock_fd, (struct sockaddr*) &remote_server, sizeof(remote_server)) < 0 )
   {
         printf( "Failed to connect to remote server.\n" );
         return -1;
   }

   return sock_fd;
}

int WriteData(int sock_fd){
   char* write_buffer;
   printf("\n[I]nsert, [D]elete, [L]ookup, [S]etup? : ");
   char command_buffer[256];
   memset(command_buffer,'\0',strlen(command_buffer));
   fgets(command_buffer, sizeof(command_buffer), stdin);
   if(strcmp(command_buffer, "quit\n") == 0){
      printf("EXITING NOW\n");
      close(sock_fd);
   }
   switch(parse_command_response(command_buffer)){
      case 0:
         write_buffer = do_insert(); break;
      case 1:
         write_buffer = do_delete(); break;
      case 2:
         write_buffer = do_lookup(); break;
      case 3:
         write_buffer = do_init(); break;
      case -1:
         printf("Invalid command\n");
         return 0;

   }
   printf("writing to server: %s\n", write_buffer);
   int result_status = write(sock_fd, write_buffer, strlen(write_buffer));
   /*
   BuildPacket(buffer, "", "", "","","");
   int result_status = write(sock_fd, buffer, strlen(buffer));
   */
   if(result_status < 0){
      printf("An error occured sending data from the client to the server.\n");
      return -1;
   }
   return 0;
}

int parse_command_response(char* command_response){
   if(strcmp(command_response, "I\n") == 0){
      printf("User said insert\n");
      return 0;
   }else if(strcmp(command_response, "D\n") == 0){
      printf("User said delete\n");
      return 1;
   }else if(strcmp(command_response, "L\n") == 0){
      printf("User said lookup\n");
      return 2;
   }else if(strcmp(command_response, "S\n") == 0){
      printf("User said setup\n");
      return 3;
   }
   return -1;
}

char* do_insert(){
   printf("\nKey: ");
   char key_buffer[256];
   memset(key_buffer,'\0',strlen(key_buffer));
   fgets(key_buffer, sizeof(key_buffer), stdin);
   printf("\nValue: ");
   char value_buffer[256];
   memset(value_buffer,'\0',strlen(value_buffer));
   fgets(value_buffer, sizeof(value_buffer), stdin);

   //send to brit function
   printf("Insert: %s , %s \n",key_buffer,value_buffer);
   return BuildPacket("insert", "", "", "", key_buffer, value_buffer);
}

char* do_delete(){
   printf("\nKey: ");
   char key_buffer[256];
   memset(key_buffer,'\0',strlen(key_buffer));
   fgets(key_buffer, sizeof(key_buffer), stdin);

   //send to brit function
   printf("Delete: %s \n",key_buffer);
   return BuildPacket("delete", "", "", "", key_buffer, "");
}

char* do_lookup(){
   printf("\nKey: ");
   char key_buffer[256];
   memset(key_buffer,'\0',strlen(key_buffer));
   fgets(key_buffer, sizeof(key_buffer), stdin);

   //send to brit function
   printf("Lookup: %s \n",key_buffer);
   return BuildPacket("lookup", "", "", "", key_buffer, "");
}

char* do_init(){
   printf("\nTable Name: ");
   char name_buffer[256];
   memset(name_buffer,'\0',strlen(name_buffer));
   fgets(name_buffer, sizeof(name_buffer), stdin);
   printf("\nTable Length: ");
   char length_buffer[256];
   memset(length_buffer,'\0',strlen(length_buffer));
   fgets(length_buffer, sizeof(length_buffer), stdin);
   printf("\nTable Size: ");
   char size_buffer[256];
   memset(size_buffer,'\0',strlen(size_buffer));
   fgets(size_buffer, sizeof(size_buffer), stdin);

   //send to brit function

   printf("Init: %s , %s, %s \n",name_buffer, length_buffer, size_buffer);
   return BuildPacket("init", name_buffer, length_buffer, size_buffer, "", "");

}

int RecieveData(int sock_fd){
   unsigned char reply_buffer[256];
   // recv() will block until there is some data to read.
   if(recv(sock_fd, reply_buffer, 256, 0) < 0)
   {
       printf("Failed to recieve message.\n");
   }else{
       printf("Data recieved from server is: %s\n",reply_buffer);
   }
   return 1;
}

char* BuildPacket(char* cmd, char* name, char* length, char* size, char* key, char* value){
   *cmd = tolower(*cmd);
   cmd[strlen(cmd)-1] = '\0'; //had to strip off newline character
   if(strcmp(cmd, "init") == 0){
      printf("found init\n");
      //confirm that only the needed arguments are given
      if(name != NULL && length!= NULL && size != NULL && strcmp(key, "") == 0 && strcmp(value, "") == 0){
         printf("Correct\n");
         //confirm that length and size can both be converted to ints
         char* formatter = "<cmd></cmd><name></name><length></length><size></size>";
         int cmdsize = strlen(name) + strlen(length) + strlen(size) + strlen(formatter) + 4 + 1;
         char* init_cmd = malloc(cmdsize);
         snprintf(init_cmd, cmdsize, "<cmd>%s</cmd><name>%s</name><length>%s</length><size>%s</size>", cmd, name, length, size);
         printf("init_cmd is %s\n", init_cmd);
         return init_cmd;
      }else{
         printf("ERROR: Wrong INIT arguments given\n");
      }
    }
    return "";
}




