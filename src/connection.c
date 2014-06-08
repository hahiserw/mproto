#include <stdio.h>
#include <unistd.h>

//#include <sys/socket.h>
//#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

#include "connection.h"
#include "main.h"


extern int server;
extern char *destination;

int connection;



void create_connection()
{
	connection = socket(AF_INET, SOCK_STREAM, 0);
	if (connection < 0)
		die("opening socket");

	struct sockaddr_in server_addr;
	// Is it nessesary?
	memset(&server_addr, 0, sizeof(server_addr)); // Change it to not use string.h

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(M_PORT);

	if (server) {
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // All interfaces

		// Don't wait to reuse previous address?
		int sockoptval = 1;
		if (setsockopt(connection, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int)) < 0)
			die("setting to reuse prevoius address");

		// Set connection timeout
		// Should it be for client as well?
		struct timeval timeout ;
		timeout.tv_sec  = 5; // XXX
		timeout.tv_usec = 0;
		if (setsockopt(connection, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
			die("setting receiving timeout");
		if (setsockopt(connection, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0)
			die("setting sending timeout");

		if (bind(connection, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
			die("binding");

		if (listen(connection, 1) < 0)
			die("listening");
	} else {
		if (inet_pton(AF_INET, destination, &server_addr.sin_addr) <= 0)
			die("converting ip");

		if (connect(connection, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
			die("connecting to the server (Is server running?)");
	}
}


void close_connection()
{
	close(connection);
}
