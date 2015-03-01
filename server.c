/*
* Brittany Arthur (bharthur)
* Jason Heron (jheron)
* Peter Froud (pfroud)
*/
#include <stdio.h>
#include <string.h>     //strlen
#include <sys/socket.h>
#include <stdlib.h>
#include <assert.h>
#include <arpa/inet.h>  //inet_addr
#include <unistd.h>     //fork(), access(2)
#include <sys/wait.h>   //wait()
#include "kvs2.h"

/** FUNCTION PROTOTYPES */
int openSocket(int port);
int listenIncomingConnection(int sock_fd);
int acceptConnections(int sock_fd);
char* recieveData(int newSocket);
int sendData(int sock_fd, int newSocket, char* data_recieved);
char* parse_client_data(char* reply_buffer);
char* do_init(char* name, char* length, char* size);
char* do_insert(char* name, char* key, char* value);
char* do_lookup(char* name, char* key);
char* do_delete(char* name, char* key);

#define QUIT 1

typedef struct sockaddr_in sockaddr_in;

/** questionable mutex */
typedef enum {IN_USE, FREE} mutex_type;
mutex_type mutex;

/**
The server opens a socket, then in acceptConnections enters an infinite loop where it listens for a connection, receives data, parses the input, and sends a response.

Returns: 0 when exited normally, something else if something didn't work.
*/
int main() {

	mutex = FREE;

	// create socket
	int port = 10732;
	int sock_fd = openSocket(port); //bind
	if(sock_fd != -1) {
		printf("Connected to port %d. Ctrl-c to quit.\n", port);
		//Accept the incoming connection
		acceptConnections(sock_fd);
	}

	return 0;
}


/**
Opens a socket. Almost identical to function in client.c with same name.

port: the port number on which to create the socket.

Returns: file descriptor to socket, or -1 if error.
*/
int openSocket(int port) {
	struct sockaddr_in socketinfo;
	//create socket
	int sock_fd = socket( PF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1) {
		perror("server: couldn't create socket");
		return -1;
	}

	//socket binds to localhost //address in network byte order
	socketinfo.sin_addr.s_addr = inet_addr("127.0.0.1");

	//in internet family
	socketinfo.sin_family = AF_INET;

	//connect socket to the port in network byte order
	socketinfo.sin_port = htons(port);

	//make pointer to socket info
	struct sockaddr_in* info_ptr = &socketinfo;

	//convert type
	struct sockaddr* address_info = (struct sockaddr*)info_ptr;

	//bind this remote server socket to port
	if (bind( sock_fd, address_info, sizeof(socketinfo) ) < 0 ) {
		perror("server: failed to bind socket");
		return -1;
	}

	return sock_fd;
}


/**
In an infinite loop, listens for a connection then makes a child process which receives, parses, and sends data.

sock_fd: file descriptor for socket.

Returns: only returns when user requested to quit,
*/
int acceptConnections(int sock_fd) {

	while(1) {
		//Listen for an incoming connection
		if(listenIncomingConnection(sock_fd) == -1) printf("Error while listening.\n");

		struct sockaddr_in newclient; //accept creates a new socket
		socklen_t size = sizeof newclient;
		int newSocket = 0;

		//waiting to accept a connection
		newSocket = accept(sock_fd, (struct sockaddr *) &newclient, &size);

		int pid = fork();
		if(pid == 0) { //child process

			// loop for an ongoing conversation with the client
			while(1) {

				// 0 maps to other, 1 maps to no, 2 maps to yes
				char* data_recieved = recieveData(newSocket); //we can change the return value to a char* but then we would have to allocate memory

				if(strcmp(data_recieved, "quit") == 0) {
					printf("User quitting now.\n");
					return QUIT;
				}

				// 0 maps to other, 1 maps to no, 2 maps to yes
				char* status = parse_client_data(data_recieved); //we can change the return value to a char* but then we would have to allocate memory
				sendData(sock_fd, newSocket, status);
			}
			return 0;
      } else if(pid==-1) {
         perror("server: fork");
      } else {
        //parent does nothing.
      }
	}
}


/**
Uses syscall listen to prepare socket for use.

sock_fd: file descriptor for socket.

Returns: 0 on success.
*/
int listenIncomingConnection(int sock_fd) {
	//Listen for incoming connections. A max of 1 connection can happen.
	if(listen(sock_fd,1) < 0) {
		perror("server: listen");
		return -1;
	}
	return 0;
}

/**
Receives a packet from the client, checks if want to quit, then returns the packet.

newSocket: the socket from which to read data.

Returns: the received packet.
*/
char* recieveData(int newSocket) {
	//get the incoming message from the client.
	char* reply_buffer = calloc(256, sizeof(char));
	assert(reply_buffer != NULL);

	// recv() will block until there is some data to read.
	if(recv(newSocket, reply_buffer, 256, 0) < 0) {
		perror("server: couldn't recieve message");
		return "-1";
	} else {
		//printf("Received packet: %s\n",reply_buffer);
		printf("\n\nReceived packet\n");
		if(strcmp(reply_buffer, "quit") == 0) {
			printf("EXITING NOW\n");
			if(close(newSocket)==-1) perror("client: close");
			return "quit";
		}

		if(strcmp(reply_buffer, "") == 0) {
			return "quit";
		}

		return reply_buffer; // 0 maps to other
	}
}

/**
Extracts information from a packet, interprets the command, and calls a helper function to perform the action.

reply_buffer: packet to interpret.

Returns: a code corresponding to the result.
*/
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

    // %[^<] uses a regex to match any characters that are not '<'.
	sscanf(reply_buffer, "<cmd>%[^<]</cmd><name>%[^<]</name><length>%[^<]</length><size>%[^<]</size><key>%[^<]</key><value>%[^<]</value>",
		   command, name, length,size, key, value);

	printf("  command=\"%s\"\n", command);
	printf("  name=\"%s\"\n", name);
	if(strcmp(length, "NONE")) printf("  length=\"%s\"\n", length);
   if(strcmp(size, "NONE")) 	printf("  size=\"%s\"\n", size);
	if(strcmp(key, "NONE"))    printf("  key=\"%s\"\n", key);
	if(strcmp(value, "NONE"))  printf("  value=\"%s\"\n", value);

	if(!strcmp(command, "init")) {
		return do_init(name, length, size);
	} else if(!strcmp(command, "insert")) {
		return do_insert(name, key, value);
	} else if(!strcmp(command, "delete")) {
		return do_delete(name, key);
	} else if(!strcmp(command, "lookup")) {
		return do_lookup(name, key);
	}
	return "[513] LUL WUT?"; //from assignment spec
}


/**
Sends a packet to the server.

sock_fd: unused.
newSocket: file descriptor for socket.
data_received: packet to send.

Returns: 0 on success, or -1 on error.
*/
int sendData(int sock_fd, int newSocket, char* data_recieved) {
	(void)sock_fd;
	//Finally, a message can be sent!
	if(send(newSocket,data_recieved,strlen(data_recieved),0) < 0) {
		perror("server: couldn't send message");
		return -1;
	}
	return 0;
}



/**
Performs a hashtable initialize and returns a success or failure message.

name: filename of the hashtable.
length: length of each hashtable entry.
size: number of entries in the hashtable.

Returns: 201 on success.
*/
char* do_init(char* name, char* length, char* size) {
  int t_size = atoi(size);
  int t_length = atoi(length);
  FILE* my_data = initialize(name, t_size, t_length);
  fclose(my_data);
  printf("  [201] INIT SUCCESS");
	return "[201] INIT SUCCESS";
}

/**
Performs a hashtable insert and returns a success or failure message.

name: filename of the hashtable.
key: key of the entry to insert.
value: value of the entry to insert.

Returns: a code corresponding to the result.
*/
char* do_insert(char* name, char* key, char* value) {
   if(mutex == IN_USE) return "[400] IN USE";
   mutex = IN_USE;

	//printf("Inserting <\"%s\",\"%s\">\n",key,value);
	FILE* my_data = get_hashfile(name);

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
		printf("  [201] INSERT_SUCCESS");
		return "[201] INSERT_SUCCESS";
	}
	//insert failure
	mutex = FREE;
   printf("  [400] INSERT FAILURE");
	return "[400] INSERT FAILURE";
}

/**
Performs a hashtable fetch and returns a success or failure message.

name: filename of the hashtable.
key: key of the entry to fetch.

Returns: a code corresponding to the result.
*/
char* do_lookup(char* name, char* key) {
   if(mutex == IN_USE) return "[400] IN USE";
   mutex = IN_USE;

	FILE* my_data = get_hashfile(name);
	char* result = calloc(max_value_size, sizeof(char));
	assert(result != NULL);

	int length;
	int* len = &length;

	if(fetch(my_data, result, key, len) == -1) {
      mutex = FREE;
      printf("  [404] KEY NOT FOUND");
		return "[404] KEY NOT FOUND";
   }
	printf("[200] FOUND: %s", result);
	fclose(my_data);
	mutex = FREE;
	return result;
}

/**
Performs a hashtable delete and returns a success or failure message.

name: filename of the hashtable.
key: key of the entry to delete.

Returns:
*/
char* do_delete(char* name, char* key) {
   if(mutex == IN_USE) return "[400] IN USE";
   mutex = IN_USE;

	FILE* my_data = get_hashfile(name);
	delete(my_data, key);
	fclose(my_data);
	 mutex = FREE;
	 printf("  [201] DELETE SUCCESS");
	return "[201] DELETE SUCCESS";
}
