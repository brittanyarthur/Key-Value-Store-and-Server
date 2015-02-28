//Server.c
#include <stdio.h>
#include <string.h>     //strlen
#include <sys/socket.h>
#include <stdlib.h>
#include <assert.h>
#include <arpa/inet.h>  //inet_addr
#include <unistd.h>     //fork(), access(2)
#include <sys/wait.h>   //wait()
#include "kvs2.h"

int OpenSocket(int port);
int ListenIncomingConnection(int sock_fd);
int AcceptConnections(int sock_fd);
char* RecieveData(int newSocket);
int SendData(int sock_fd, int newSocket, char* data_recieved);
char* parse_client_data(char* reply_buffer);
char* do_init(char* name, char* length, char* size);
char* do_insert(char* name, char* key, char* value);
char* do_lookup(char* name, char* key);
char* do_delete(char* name, char* key);

#define QUIT 1

typedef struct sockaddr_in sockaddr_in;

typedef enum {IN_USE, FREE} mutex_type;
mutex_type mutex;

int main(int argc , char *argv[]) {
	(void)argc;
	(void)argv;

	mutex = FREE;

	// create socket
	int port = 10732;
	printf( "creating socket on port %d\n", port );
	int sock_fd = OpenSocket(port); //bind
	if(sock_fd != -1) {
		printf("Connected\n");
		//Accept the incoming connection
		AcceptConnections(sock_fd);
	}

	return 0;
}

int OpenSocket(int port) {
	struct sockaddr_in socketinfo; //https://msdn.microsoft.com/en-us/library/aa917469.aspx
	//create socket
	int sock_fd = socket( PF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1) {
		printf("Could not create socket.\n");
		return -1;
	}

	//socket binds to localhost //address in network byte order
	socketinfo.sin_addr.s_addr = inet_addr("127.0.0.1");

	//in internet family
	socketinfo.sin_family = AF_INET;

	//connect socket to the port
	socketinfo.sin_port = htons(port);

	//make pointer to socket info
	struct sockaddr_in* info_ptr = &socketinfo;

	//convert type
	struct sockaddr* address_info = (struct sockaddr*)info_ptr;

	//bind this remote server socket to port
	if (bind( sock_fd, address_info, sizeof(socketinfo) ) < 0 ) {
		printf( "Failed to bind socket.\n" );
		return -1;
	}

	return sock_fd;
}

int ListenIncomingConnection(int sock_fd) {
	//Listen for incoming connections. A max of 1 connection can happen.
	if(listen(sock_fd,1) < 0) {
		printf("Error\n");
		return -1;
	}
	printf("Listening\n");
	return 0;
}

char* RecieveData(int newSocket) {
	printf("about to recieve data\n");
	//get the incoming message from the client.
	char* reply_buffer = calloc(256, sizeof(char));
	assert(reply_buffer != NULL);

	// recv() will block until there is some data to read.
	if(recv(newSocket, reply_buffer, 256, 0) < 0) {
		printf("Failed to recieve message.\n");
		return "-1";
	} else {
		printf("Data recieved from client is: %s\n",reply_buffer);
		if(strcmp(reply_buffer, "quit") == 0) {
			printf("EXITING NOW\n");
			close(newSocket);
			return "quit";
		}
		if(!strcmp(reply_buffer, "")) {
			printf("Exit from killed client.\n");
			return "quit";
		}

		return reply_buffer; // 0 maps to otherĿ
	}
}

char* parse_client_data(char* reply_buffer) {
	char* command = calloc(sizeof(char), 100);
	char* name = calloc(sizeof(char), 100);
	char* length = calloc(sizeof(char), 100);
	char* size = calloc(sizeof(char), 100);
	char* key = calloc(sizeof(char), 100);
	char* value = calloc(sizeof(char), 100);
	assert(command != NULL);
	assert(name != NULL);
	assert(length != NULL);
	assert(size != NULL);
	assert(key != NULL);
	assert(value != NULL);

	sscanf(reply_buffer, "<cmd>%[^<]</cmd><name>%[^<]</name><length>%[^<]</length><size>%[^<]</size><key>%[^<]</key><value>%[^<]</value>",
		   command, name, length,size, key, value);
	printf("command=\"%s\"\nname=\"%s\"\nlength=\"%s\"\nsize=\"%s\"\nkey=\"%s\"\nvalue=\"%s\"\n", command, name, length, size, key, value);

	if(!strcmp(command, "init")) {
		return do_init(name, length, size);
	} else if(!strcmp(command, "insert")) {
		return do_insert(name, key, value);
	} else if(!strcmp(command, "delete")) {
		return do_delete(name, key);
	} else if(!strcmp(command, "lookup")) {
		return do_lookup(name, key);
	}
	return "513 LUL WUT?"; //from assignment spec
}

char* do_init(char* name, char* length, char* size) {
	(void)name;
	(void)length;
	(void)size;
	return "[201] INIT SUCCESS";
}

char* do_insert(char* name, char* key, char* value) {
   if(mutex == IN_USE) return "[400] IN USE";
   mutex = IN_USE;

	printf("inserting %s, with %s\n",key,value);
	FILE* my_data = initialize(name);
	int value_size = (strlen(value) + 1)*sizeof(char);
	insert(my_data, key, value, value_size);
	char result[max_value_size];
	int length;
	int * len = &length;
	fetch(my_data, result, key, len);
	fclose(my_data);
	if(!strcmp(result, value)) {
		//insert success
		mutex = FREE;
		return "[201] INSERT_SUCCESS";
	}
	//insert failure
	mutex = FREE;
	return "[400] INSERT FAILURE";
}

char* do_lookup(char* name, char* key) {
   if(mutex == IN_USE) return "[400] IN USE";
   mutex = IN_USE;

	printf("look up %s\n",key);
	FILE* my_data = initialize(name);
	char* result = calloc(max_value_size, sizeof(char));
	assert(result != NULL);
	int length;
	int* len = &length;
	if(fetch(my_data, result, key, len) == -1) {
      mutex = FREE;
		return "[404] KEY NOT FOUND";
   }
	printf("[200] FOUND: %s\n", result);
	fclose(my_data);
	mutex = FREE;
	return result;
}

char* do_delete(char* name, char* key) {
   if(mutex == IN_USE) return "[400] IN USE";
   mutex = IN_USE;

	FILE* my_data = initialize(name);
	delete(my_data, key);
	fclose(my_data);
	 mutex = FREE;
	return "[201] DELETE SUCCESS";
}

//init:      init the semophore  http://man7.org/linux/man-pages/man3/sem_init.3.html
//wait:      lock by decrement   http://man7.org/linux/man-pages/man3/sem_wait.3.html
//post:      unlock by increment http://man7.org/linux/man-pages/man3/sem_post.3.html
//get value: http://man7.org/linux/man-pages/man3/sem_getvalue.3.html

int AcceptConnections(int sock_fd) {

	while(1) {
		//Listen for an incoming connection
		if(ListenIncomingConnection(sock_fd) == -1) printf("Error while listening\n");

		printf("about to fork 2 processes - child and parent. \n");
		struct sockaddr_in newclient; //accept creates a new socket
		socklen_t size = sizeof newclient;
		int newSocket = 0;
		//waiting to accept a connection
		newSocket = accept(sock_fd, (struct sockaddr *) &newclient, &size);
		int pid = fork();
		if(pid == 0) { //child process
			printf("in child!!!\n");
			// loop for an ongoing conversation with the client
			while(1) {
				// 0 maps to other, 1 maps to no, 2 maps to yes
				char* data_recieved = RecieveData(newSocket); //we can change the return value to a char* but then we would have to allocate memory
				if(strcmp(data_recieved, "quit") == 0) {
					printf("User Quitting Now.\n");
					return QUIT;
				}
				// 0 maps to other, 1 maps to no, 2 maps to yes
				char* status = parse_client_data(data_recieved); //we can change the return value to a char* but then we would have to allocate memory
				SendData(sock_fd, newSocket, status);
			}
			return 0;
		} else {
			//wait(&pid);
			printf("in parent!!!\n");
		}
	}
}

int SendData(int sock_fd, int newSocket, char* data_recieved) {
	(void)sock_fd;
	//Finally, a message can be sent!
	if(send(newSocket,data_recieved,strlen(data_recieved),0) < 0) {
		printf("Error sending message\n");
		return -1;
	}
	printf("Message Successfully Sent.\n");
	return 0;
}
