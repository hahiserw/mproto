#include <unistd.h>
#include <arpa/inet.h>

#include <errno.h>

#include <string.h>

#include "server.h"
#include "main.h"
#include "connection.h"
#include "message.h"

#include <stdio.h> // stderr
#include <stdlib.h>

 
/* what do
- wait for client
- check if client's message size is sane
- confirm if it is, otherwise tell him to get lost
- start reading messages checking checksum (ack'ing every single one?)
	- if message is sent more than once ignore it
	- if same messages is sent more than 10 times, tell client to get his shit together and kick him
	- if there aren't all parts -------------------^ (or wait for retransmission) // is it a flaw?
- put them all together and display
*/


int negotiate_params(int client, struct info *answer)
{
	size_t info_size = sizeof(struct info);

	struct info question;
	size_t read_length = read(client, &question, info_size);
	if (read_length != info_size)
		die("reading question");

	if (MIN_PART_SIZE > question.part_size)
		answer->part_size = MIN_PART_SIZE;
	else if (answer->part_size > MAX_PART_SIZE)
		answer->part_size = MAX_PART_SIZE;
	else
		answer->part_size = question.part_size;

	answer->message_length = question.message_length;
	answer->parts          = ceil(answer->message_length, answer->part_size);

	LOG(DEBUG, "Client wanted %i, received %i", question.part_size, answer->part_size);

	if (question.part_size != answer->part_size)
		return -1;

	int write_length = write(client, answer, info_size);
	LOG(DEBUG, "Written answer length: %i", write_length);

	return 0;
}


void create_server()
{
	create_connection();
	LOG(DEBUG, "Connection created");

	struct sockaddr_in client_addr;
	socklen_t client_addr_length = sizeof(client_addr);

	for (;;) {
		int client = accept(connection, (struct sockaddr *)&client_addr, &client_addr_length);
		if (client < 0)
			continue;

		LOG(INFO, "* Client connected");

		struct info info;
		if (negotiate_params(client, &info) < 0) {
			LOG(INFO, "Killing client. Reason: wanted %u as a part size", info.part_size);
			close(client);
			continue;
		}

		if (info.message_length >= MAX_MESSAGE_SIZE) {
			LOG(INFO, "Client sent to large message (%i)", info.message_length);
			die("receiving message");
		}

		char text[MAX_MESSAGE_SIZE];

		unsigned int sent = 0;

		sent = get_message(client, text, &info);

		LOG(DEBUG, "get message status: %u", sent);

		//if (sent < 0) {}


		text[info.message_length] = 0;
		LOG(MESSAGE, text);

		LOG(INFO, "* Client disconnected");
		close(client);

		sleep(1); // To not get spammed?
	}
}


unsigned int get_message(int client, char *text, struct info *info)
{
	unsigned int sum = 0;

	unsigned int parts     = info->parts;
	unsigned int part_size = info->part_size;

	// XXX
	//size_t retransmit_size = ceil(MAX_MESSAGE_SIZE, part_size) >> 3;
	//char *retransmit = (char *)malloc(retransmit_size);

	//ret_init(retransmit, part_size);

	//int write_length = write(client, retransmit, retransmit_size);
	//if (write_length != retransmit_size) {
	//	LOG(INFO, "Could not send retranstmit bits to client");
	//	// XXX Message could contain garbage
	//}

	//free(retransmit);


	int tries = 1;

	int index;
	int got;

	while (sum < parts) {
		got = get_part(client, text, &index, part_size);

		if (got > 0) {
			//sum += got;
			//ret_clear(retransmit, index);
			++sum;
		} else if (got == 0) {
			LOG(DEBUG, "got 0 (try %i)", tries);
			sleep(1);
			if (tries++ == 5)
				break;
		} else {
			errno = -got;
			LOG(DEBUG, "got < 0 (%i)", errno);
			errno = -got;
			perror("didn't read");
		}
	}

	return sum;
}


int get_part(int client, char *text, int *index, int part_size)
{
	int data_size = sizeof(struct message) - 1 + part_size;
	struct message *data = malloc(data_size);

	unsigned int local_checksum;

	ssize_t read_length = read(client, data, data_size);
	if (read_length < 0)
		read_length = -errno;

	// XXX
	LOG(DEBUG, "Read value: %i", read_length);

	*index          = data->index; // TODO Check if number from range
	int size        = data->size;
	char *text_part = data->text;

	convert(text_part); // Changes structure's field xP

	LOG(DEBUG, "Got part (%3u): '%s' (%u) #%u", index+1, text_part, size, data->checksum);

	// Make sure there is no garbage left
	//if (size < part_size)
	//	text_part[size + 1] = 0;

	local_checksum = compute_checksum(text_part);
	if (data->checksum == local_checksum) {
		// Message delivered correctly; clear retransmit bit
		//retransmit[index >> 3] &= ~((index & 7) << 1);
		//++sent;

		strncpy(&text[*index * part_size], text_part, size);
	} else {
		LOG(ERROR, "Wrong part's checksum! Got %u, should be %u", data->checksum, local_checksum);
	}


	free(data);

	return read_length;
}
