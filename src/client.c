#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>

#include <string.h>

#include "client.h"
#include "main.h"
#include "connection.h"
#include "message.h"


extern int simulate_sending;
// FIXME Make sure server is not down while sending
/* what do?
- get message from user
	- remove ugly characters / tell him to retype the message
	- chop it and for each part
		- compute checksum
		- pack it up
- establish connection
- tell what size the message part is, and how many parts
- wait for server's response
- send parts
	- retransmit if missing
*/ 

// negotiate options
unsigned int prepare_connection(unsigned int message_length, unsigned int part_size)
{
	size_t read_length;
	size_t write_length;

	struct info question;
	question.part_size      = part_size; // GLOBAL
	question.message_length = message_length;
	question.parts          = ceil(message_length, part_size);

	size_t info_size = sizeof(struct info);

	if (simulate_sending)
		write_length = info_size;
	else
		write_length = write(connection, &question, info_size);

	if (write_length != info_size)
		die("writing question");


	struct info answer;

	if (simulate_sending)
		read_length = info_size;
	else
		read_length = read(connection, &answer, info_size);

	if (read_length != info_size)
		die("reading answer");

	if (!simulate_sending) {
		if (answer.part_size      == question.part_size
		&&  answer.message_length == question.message_length
		&&  answer.parts          == question.parts) {
			LOG(DEBUG, "Server has accepted part size %u (%u)", part_size, answer.part_size);
		} else {
			LOG(INFO, "Server didn't accept part size (%u)", answer.part_size);
			return 0; // RETURN
		}
	}

	return answer.part_size;
}

void create_client(unsigned int part_size, char *message)
{
	int write_length;

	size_t text_length;
	char text[MAX_MESSAGE_SIZE];

	if (message) {
		text_length = strlen(message);
		
		if (!simulate_sending) {
			create_connection();
			LOG(DEBUG, "Connection created");
		}

		strncpy(text, message, MAX_MESSAGE_SIZE);

		part_size = prepare_connection(text_length, part_size);
		if (part_size == 0)
			die("preparing connection");

		LOG(DEBUG, "{ Message from command line to send: '%s' (%u)", text, text_length);
		write_length = send_message(/*connection, */text, text_length, part_size);
		LOG(DEBUG, "} Message written; packet size: %i", write_length);

		if (!simulate_sending)
			close_connection();

		LOG(INFO, "Nothing more to do. Exiting.");
	} else {

		LOG(INFO, "Type message and press enter");

		for (;;) {
			printf(CLIENT_PROMPT);

			text_length = read_line(text, MAX_MESSAGE_SIZE);

			if (!simulate_sending) {
				create_connection();
				LOG(DEBUG, "Connection created");
			}

			part_size = prepare_connection(text_length, part_size);
			if (part_size == 0)
				die("preparing connection");

			LOG(DEBUG, "Message to send: '%s' (%u)", text, text_length);

			write_length = send_message(/*connection, */text, text_length, part_size);
			//sleep(1); // Prevent client from spamming

			if (!simulate_sending)
				close_connection();
		}

		LOG(INFO, "Server closed connection?");
	}
}


int send_message(/*int connection, */const char *text, size_t text_length, int part_size)
{
	unsigned int sum = 0;
	int parts = ceil(text_length, part_size);

	LOG(MESSAGE, text);

	// XXX
	//size_t retransmit_size = ceil(MAX_MESSAGE_SIZE, part_size) >> 3;
	//char *retransmit = malloc(retransmit_size);
	
	int sent;

	for (int index = 0; index < parts; ++index) {
		sent = send_part(text, part_size, index);

		//if (index == 2) { // Psikus
		//	//close(client);
		//	close(connection);
		//}

		if (sent > 0) {
			sum += sent;
			//ret_clear(retransmit, index);
			//retransmit[index >> 3] &= ~(1 << (index & 7));
		} else if (sent == 0) {
			LOG(DEBUG, "sent 0 bytes?");
		} else {
			// Damn!
			errno = -sent;
			LOG(DEBUG, "sent < 0 (%i)", errno);
			perror("didn't write");
			break;
		}
	}

	//int read_length = read(connection, retransmit, retransmit_size);

	//do {
	//	send_stuff();
	//} while (parts_left);

	//free(retransmit);

	return sum;
}


int send_part(const char *text, unsigned int part_size, unsigned int index)
{
	int write_length;
	char text_part[part_size];

	int data_size = sizeof(struct message) - 1 + part_size;
	struct message *data = malloc(data_size);


	strncpy(text_part, &text[index * part_size], part_size); // Get part from user's input

	size_t part_length = strlen(text_part);
	pack_data(data, text_part, part_length, index, part_size); // strlen...

	LOG(DEBUG, "Sending part (%3u): '%s' (%u) #%u", index+1, text_part, part_length, data->checksum);

	if (simulate_sending) {
		write_length = data_size;
	} else {
		write_length = write(connection, data, data_size);
		if (write_length < 0)
			write_length = -errno;
	}


	free(data);

	return write_length;
}


int read_line(char *buffer, int buffer_size)
{
	int i = 0;

	// :D
	// TODO return also on ^D
	while ((i[buffer] = getchar()) != '\n' && i < buffer_size)
		++i;

	buffer[i] = 0;

	return i;
}

