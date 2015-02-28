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
char* do_quit();
//char* BuildPacket(char* cmd, char* name, char* length, char* size, char* key, char* value);
char* table_name = NULL;


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
         if(write_result == 1){
            printf("We hope you enjoyed using the hashtable.\n");
            close(sock_fd);
            return 0;
         }
      }else{
         printf("Error: file descriptor error\n");
         exit(EXIT_FAILURE);
      }
      if(write_result != -1){
         recieve_result = RecieveData(sock_fd);
      }
      if(recieve_result == -1){
         printf("An Error has occured.\n");
         exit(EXIT_FAILURE);
      }
   }
   return 0;
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
   // inet_addr interprets character string and returns binary rep of internet address
   remote_server.sin_addr.s_addr = inet_addr(remote_IP);
   // Address family = Internet
   remote_server.sin_family = AF_INET;
   // Set port number. htons function creates proper bigendian byte order for networking.
   remote_server.sin_port = htons(port);

   //make pointer to socket info
   struct sockaddr_in* info_ptr = &remote_server;
   //convert type
   struct sockaddr* address_info = (struct sockaddr*)info_ptr;

   if (connect(sock_fd, address_info, sizeof(remote_server)) < 0 )
   {
         printf( "Failed to connect to remote server.\n" );
         return -1;
   }

   return sock_fd;
}

//return code 1: quit. -1: error. 0: success.
int WriteData(int sock_fd){
   //Ask user for action
   int no_response = 1;
   int quit = 1;
   char* write_buffer;
   char* command_buffer;
   char com;

   while(no_response){ //loop until there is a response

      printf("\n****TABLE: %s****", table_name);
      printf("\n[I]nsert: add a key/value pair to keystore.\n[D]elete: delete a key/value pair from keystore.\n[L]ookup: lookup value in keystore.\n[S]etup: configure keystore.\n[Q]uit: quit \n");

      command_buffer = calloc(5, sizeof(char));
      fgets(command_buffer, 5, stdin);
      strtok(command_buffer, "\n"); //strip trailing newline

      com = tolower(command_buffer[0]); //get the only character

      if(com == 'q'){
         printf("EXITING NOW\n");
         close(sock_fd);
         return 1;
      }

      //Parse user action decision
      no_response = 0;
      if(table_name == NULL){
         printf("You've gotsta set up your hashtable!\n");
         write_buffer = do_init();
      }else{

         switch(com){
            case 'i':
               write_buffer = do_insert(); break;

            case 'd':
               write_buffer = do_delete(); break;

            case 'l':
               write_buffer = do_lookup(); break;

            case 's':
               write_buffer = do_init(); break;

            default:
               printf("Invalid command. Please try again.\n");
               no_response = 1;
               break;
         }


      }
    }

   if(quit == 0){ //User would like to quit out of server
         return 1;
      }

   //Send formed packet to server
   //printf("writing to server: %s\n", write_buffer);
   int result_status = write(sock_fd, write_buffer, strlen(write_buffer));
   if(result_status < 0){
      printf("An error occured sending data from the client to the server.\n");
      return -1;
   }

   return 0;
}



char* do_quit(){
   char* packet = calloc(sizeof(char), 4 + 1);
   strcpy(packet, "quit");
   return packet;
}

char* do_insert(){
   printf("\nKey: ");
   char* key_buffer = calloc(256, sizeof(char));
   fgets(key_buffer, 256, stdin);
   strtok(key_buffer, "\n"); //strip trailing newline
   printf("\nValue: ");
   char* value_buffer = calloc(256, sizeof(char));
   fgets(value_buffer, 256, stdin);
   strtok(value_buffer, "\n"); //strip trailing newline

   char* formatter = "<cmd>insert</cmd><name></name><length>NONE</length><size>NONE</size><key></key><value></value>";
   int cmdsize = strlen(key_buffer) + strlen(value_buffer) + strlen(formatter) + strlen(table_name) + 1;
   char* packet = calloc(cmdsize, sizeof(char));
   sprintf(packet, "<cmd>insert</cmd><name>%s</name><length>NONE</length><size>NONE</size><key>%s</key><value>%s</value>",table_name, key_buffer, value_buffer);

   //printf("packet is %s\n", packet);
   return packet;
}

char* do_delete(){
   printf("\nKey: ");
   char* key_buffer = calloc(256, sizeof(char));
   fgets(key_buffer, 256, stdin);
   strtok(key_buffer, "\n"); //strip trailing newline

   char* formatter = "<cmd>delete</cmd><name></name><length>NONE</length><size>NONE</size><key></key><value>NONE</value>";
   int cmdsize = strlen(key_buffer) + strlen(formatter) + strlen(table_name) + 1;

   char* packet = calloc(cmdsize, sizeof(char));
   sprintf(packet, "<cmd>delete</cmd><name>%s</name><length>NONE</length><size>NONE</size><key>%s</key><value>NONE</value>", table_name, key_buffer);
   //printf("packet is %s\n", packet);
   return packet;
}

char* do_lookup(){
   printf("\nKey: ");
   char* key_buffer = calloc(256, sizeof(char));
   fgets(key_buffer,256, stdin);
   strtok(key_buffer, "\n"); //strip trailing newline

   char* formatter = "<cmd>lookup</cmd><name></name><length>NONE</length><size>NONE</size><key></key><value>NONE</value>";
   int cmdsize = strlen(key_buffer) + strlen(formatter) + strlen(table_name) + 1;
   //int character_additions = strlen("lookup") + 1; //unused
   char* packet = calloc(cmdsize, sizeof(char));
   sprintf(packet, "<cmd>lookup</cmd><name>%s</name><length>NONE</length><size>NONE</size><key>%s</key><value>NONE</value>", table_name, key_buffer);

   //printf("packet is %s\n", packet);
   return packet;
}

char* do_init(){
   printf("\nTable Name: ");
   char* name_buffer = calloc(256, sizeof(char));


   fgets(name_buffer, 256, stdin);
   strtok(name_buffer, "\n"); //strip trailing newline



   //this assumes table is on client side, will work for assignment but is wrong
   if(access(name_buffer, W_OK) != -1){
   //file exists
   printf("Found pre-existing table, CONNECTING\n");
   char* formatter = "<cmd>init</cmd><name></name><length>NONE</length><size>NONE</size><key>NONE</key><value>NONE</value>";
   int cmdsize = strlen(name_buffer) + strlen(formatter) + 1;
   char* packet = calloc(cmdsize, sizeof(char));
   sprintf(packet, "<cmd>init</cmd><name>%s</name><length>NONE</length><size>NONE</size><key>NONE</key><value>NONE</value>", name_buffer);
   //printf("packet is %s\n", packet);
   return packet;

   }else{
   //file doesn't exist
   printf("CREATING NEW TABLE\n");
   printf("\nLength of entry: "); //entry
   char* length_buffer = calloc(256, sizeof(char));
   fgets(length_buffer, 256, stdin);
   strtok(length_buffer, "\n"); //strip trailing newline
   printf("\nNumber of entries: "); //num entries
   char* size_buffer = calloc(256, sizeof(char));
   fgets(size_buffer, 256, stdin);
   strtok(size_buffer, "\n"); //strip trailing newline

   char* formatter = "<cmd>init</cmd><name></name><length></length><size></size><key>NONE</key><value>NONE</value>";
   int cmdsize = strlen(name_buffer) + strlen(length_buffer) + strlen(size_buffer) + strlen(formatter) + 1;
   char* packet = calloc(cmdsize, sizeof(char));
   sprintf(packet, "<cmd>init</cmd><name>%s</name><length>%s</length><size>%s</size><key>NONE</key><value>NONE</value>", name_buffer, length_buffer, size_buffer);
   //printf("packet is %s\n", packet);
   return packet;
   }
}

int RecieveData(int sock_fd){
   char* reply_buffer = calloc(sizeof(char), 256);
   // recv() will block until there is some data to read.
   if(recv(sock_fd, reply_buffer, 256, 0) < 0)
   {
       printf("Failed to recieve message.\n");
   }else{
       printf("Data recieved from server is: %s\n",reply_buffer);
   }
   return 1;
}


/*
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
*/



