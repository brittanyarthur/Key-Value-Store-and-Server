/*
* Brittany Arthur (bharthur)
* Jason Heron (jheron)
* Peter Froud (pfroud)
*/

#include <stdlib.h> //free()
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h> //sleep
#include <ctype.h> //tolower()
#include <assert.h>

/** FUNCTION PROTOTYPES */
int openSocket(int remote_port, const char* remote_IP);
int recieveData(int sock_fd);
int writeData(int sock_fd);
char* do_insert();
char* do_delete();
char* do_lookup();
char* do_init();

/** Stores the filename of the currently opened hashtable */
char* table_name;

/**

Returns: 0 when exited normally, something else if something didn't work.
*/
int main() {

	table_name = calloc(256, sizeof(char));
	assert(table_name != NULL);
	//table_name = "";

	int sock_fd = openSocket(10732, "127.0.0.1");
	printf("Socket has been opened\n");

	int write_result = 0;
	int recieve_result = 0;

	while(1) {
		if(sock_fd != -1) {
			write_result = writeData(sock_fd);
			if(write_result == 1) {
				printf("We hope you enjoyed using the hashtable.\n");
				if(close(sock_fd) == -1) perror("client: close");
				return 0;
			}
		} else {
			printf("Error: file descriptor error\n");
			exit(EXIT_FAILURE);
		}
		if(write_result != -1) {
			recieve_result = recieveData(sock_fd);
		}
		if(recieve_result == -1) {
			printf("An Error has occured.\n");
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}

/**
[what does this do]

port:
remote_IP:

Returns:
*/
int openSocket(int port, const char* remote_IP) {
	struct sockaddr_in remote_server;

	//create socket

	/*
	AF_INET refers to addresses from the internet, IP addresses specifically. PF_INET refers to anything in the protocol, usually sockets/ports.
   SOCK_DGRAM is used for UDP packets, SOCK_STREAM for TCP.
	"...Passing PF_NET ensures that the connection works right. AF = Address Family. PF = Protocol Family."
	http://stackoverflow.com/questions/1593946/what-is-af-inet-and-why-do-i-need-it
	*/

	int sock_fd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ); //socket == file descriptor, thus: mysocket
	if(sock_fd == -1) {
		perror("client: couldn't create socket");
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

	if (connect(sock_fd, address_info, sizeof(remote_server)) < 0 ) {
		perror("client: failed to connect");
		return -1;
	}

	return sock_fd;
}


/**

sock_fd: file descriptor for socket.

Returns: 0 on success; -1 or error, 1 to quit.
*/
int writeData(int sock_fd) {
	//Ask user for action
	int no_response = 1;
	int quit = 1;
	char* write_buffer;
	char* command_buffer;
	char com;

	while(no_response) { //loop until there is a response

		printf("\n****TABLE: %s****", table_name);
		printf("\n[I]nsert: add a key/value pair to keystore.\n[D]elete: delete a key/value pair from keystore.\n[L]ookup: lookup value in keystore.\n[S]etup: configure keystore.\n[Q]uit: quit \n");

		command_buffer = calloc(5, sizeof(char));
		assert(command_buffer != NULL);
		fgets(command_buffer, 5, stdin);
		strtok(command_buffer, "\n"); //strip trailing newline

		com = tolower(command_buffer[0]); //get the only character

		if(com == 'q') {
			printf("EXITING NOW\n");
			if(close(sock_fd)==-1) perror("client: close");
			return 1;
		}

		//Parse user action decision
		no_response = 0;
		if(strcmp(table_name, "")==0) {
			printf("You've gotsta set up your hashtable!\n");
			write_buffer = do_init();
		} else {

			switch(com) {
			case 'i':
				write_buffer = do_insert();
				break;

			case 'd':
				write_buffer = do_delete();
				break;

			case 'l':
				write_buffer = do_lookup();
				break;

			case 's':
				write_buffer = do_init();
				break;

			default:
				printf("Invalid command. Please try again.\n");
				no_response = 1;
				break;
			}
		}
	}

	if(quit == 0) { //User would like to quit out of server
		return 1;
	}

	//Send formed packet to server
	//printf("writing to server: %s\n", write_buffer);
	int result_status = write(sock_fd, write_buffer, strlen(write_buffer));
	if(result_status < 0) {
		printf("An error occured sending data from the client to the server.\n");
		return -1;
	}

	return 0;
}


/**

sock_fd: file descriptor for socket.

Returns:
*/
int recieveData(int sock_fd) {
	char* reply_buffer = calloc(sizeof(char), 256);
	assert(reply_buffer != NULL);

	// recv() will block until there is some data to read.
	if(recv(sock_fd, reply_buffer, 256, 0) < 0) {
		perror("client: couldn't recieve message");
	} else {
		printf("Data recieved from server is: %s\n",reply_buffer);
	}
	return 1;
}



/**

Returns: formatted packet to send to server to insert.
*/
char* do_insert() {
   char* value_buffer = calloc(256, sizeof(char));
   char* key_buffer = calloc(256, sizeof(char));
   assert(key_buffer != NULL);
   assert(value_buffer != NULL);

	printf("\nKey: ");
	fgets(key_buffer, 256, stdin);
	strtok(key_buffer, "\n"); //strip trailing newline

	printf("\nValue: ");
	fgets(value_buffer, 256, stdin);
	strtok(value_buffer, "\n"); //strip trailing newline

	char* formatter = "<cmd>insert</cmd><name></name><length>NONE</length><size>NONE</size><key></key><value></value>";
	int cmdsize = strlen(key_buffer) + strlen(value_buffer) + strlen(formatter) + strlen(table_name) + 1;
   char* packet = calloc(cmdsize, sizeof(char));
   assert(packet != NULL);

	sprintf(packet, "<cmd>insert</cmd><name>%s</name><length>NONE</length><size>NONE</size><key>%s</key><value>%s</value>",table_name, key_buffer, value_buffer);

	//printf("packet is %s\n", packet);
	return packet;
}

/**

Returns: formatted packet to send to server to delete.
*/
char* do_delete() {
	char* formatter = "<cmd>delete</cmd><name></name><length>NONE</length><size>NONE</size><key></key><value>NONE</value>";
	char* key_buffer = calloc(256, sizeof(char));
	assert(key_buffer != NULL);

	printf("\nKey: ");
	fgets(key_buffer, 256, stdin);
	strtok(key_buffer, "\n"); //strip trailing newline

	int cmdsize = strlen(key_buffer) + strlen(formatter) + strlen(table_name) + 1;
   char* packet = calloc(cmdsize, sizeof(char));
   assert(packet != NULL);

	sprintf(packet, "<cmd>delete</cmd><name>%s</name><length>NONE</length><size>NONE</size><key>%s</key><value>NONE</value>", table_name, key_buffer);
	//printf("packet is %s\n", packet);
	return packet;
}

/**

Returns: formatted packet to send to server to lookup.
*/
char* do_lookup() {
	char* formatter = "<cmd>lookup</cmd><name></name><length>NONE</length><size>NONE</size><key></key><value>NONE</value>";
	char* key_buffer = calloc(256, sizeof(char));
	assert(key_buffer != NULL);

	printf("\nKey: ");
	fgets(key_buffer,256, stdin);
	strtok(key_buffer, "\n"); //strip trailing newline

	int cmdsize = strlen(key_buffer) + strlen(formatter) + strlen(table_name) + 1;
	char* packet = calloc(cmdsize, sizeof(char));
   assert(packet != NULL);

	sprintf(packet, "<cmd>lookup</cmd><name>%s</name><length>NONE</length><size>NONE</size><key>%s</key><value>NONE</value>", table_name, key_buffer);

	//printf("packet is %s\n", packet);
	return packet;
}

/**

Returns: formatted packet to send to server to init.
*/
char* do_init() {
	char* name_buffer = calloc(256, sizeof(char));
	assert(name_buffer != NULL);

   printf("\nTable Name: ");
	fgets(name_buffer, 256, stdin);
	strtok(name_buffer, "\n"); //strip trailing newline

	strcpy(table_name, name_buffer);

	//this assumes table is on client side, will work for assignment but is wrong
	if(access(table_name, W_OK) != -1) {
		//file exists
		char* formatter = "<cmd>init</cmd><name></name><length>NONE</length><size>NONE</size><key>NONE</key><value>NONE</value>";
      int cmdsize = strlen(name_buffer) + strlen(formatter) + 1;

      char* packet = calloc(cmdsize, sizeof(char));
		assert(packet != NULL);

		printf("Found pre-existing table, CONNECTING\n");
		sprintf(packet, "<cmd>init</cmd><name>%s</name><length>NONE</length><size>NONE</size><key>NONE</key><value>NONE</value>", name_buffer);
		//printf("packet is %s\n", packet);
		return packet;

	} else {
		//file doesn't exist
      char* formatter = "<cmd>init</cmd><name></name><length></length><size></size><key>NONE</key><value>NONE</value>";
		char* length_buffer = calloc(256, sizeof(char));
		char* size_buffer = calloc(256, sizeof(char));
		assert(size_buffer != NULL);
		assert(length_buffer != NULL);

		printf("CREATING NEW TABLE\n");

		printf("\nLength of entry: "); //entry
		fgets(length_buffer, 256, stdin);
		strtok(length_buffer, "\n"); //strip trailing newline

		printf("\nNumber of entries: "); //num entries
		fgets(size_buffer, 256, stdin);
		strtok(size_buffer, "\n"); //strip trailing newline

		int cmdsize = strlen(name_buffer) + strlen(length_buffer) + strlen(size_buffer) + strlen(formatter) + 1;
		char* packet = calloc(cmdsize, sizeof(char));
      assert(packet != NULL);

		sprintf(packet, "<cmd>init</cmd><name>%s</name><length>%s</length><size>%s</size><key>NONE</key><value>NONE</value>", name_buffer, length_buffer, size_buffer);
		//printf("packet is %s\n", packet);
		return packet;
	}
}
