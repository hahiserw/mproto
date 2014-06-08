#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#include <string.h>

#include "main.h"
#include "connection.h"
#include "server.h"
#include "client.h"



int server = 0;
/*static*/ enum log_level verbosity = MESSAGE;
char *destination = 0;
int simulate_sending = 0;

int user_part_size = 10;
char *message = 0;



int main(int argc, char *argv[])
{
	parse_args(argc, argv);
	LOG(DEBUG, "Arguments parsed");

	signal(SIGINT,  handle_signal);
	signal(SIGTERM, handle_signal);
	LOG(DEBUG, "Signals attached");

	if (server) {
		LOG(INFO, "Entering server mode");
		create_server();
	} else {
		LOG(INFO, "Entering client mode");
		create_client(user_part_size, message);
	}

	//LOG(INFO, "The end?");
	return EXIT_SUCCESS;
}


void handle_signal(int signal)
{
	switch (signal) {
	case SIGINT:
	case SIGTERM:
		close_connection();
		LOG(INFO, "* Program terminated");
		exit(EXIT_FAILURE);
	}
}


void usage()
{
	fprintf(stderr, "server usage: mproto [-v]\n");
	fprintf(stderr, "client usage: mproto [-v] [-s] [-p size] [-m message] address\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "mproto is a simple message protocol with an advanced message encryption (xor).\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "A connection will be attempted if an address is specified,\n");
	fprintf(stderr, "otherwise mproto will run as a server.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  -h  Display this helpful text\n");
	fprintf(stderr, "  -v  Increment verbosity level (more -v, more verbose)\n");
	fprintf(stderr, "  -p  Part size. Choose between %u and %u\n", MIN_PART_SIZE, MAX_PART_SIZE);
	fprintf(stderr, "  -m  Send given message\n");
	fprintf(stderr, "  -s  Simulate sending\n");
}

void parse_args(int argc, char *argv[])
{
	int option;

	while ((option = getopt(argc, argv, "vhm:p:s")) != -1) {
		switch (option) {
		case 'v':
			++verbosity;
			break;
		case 's':
			simulate_sending = 1;
			break;
		case 'm':
			message = optarg;
			 if (strlen(message) > MAX_MESSAGE_SIZE) {
				 fprintf(stderr, "Message size should be smaller than %i\n", MAX_MESSAGE_SIZE);
				 usage();
				 exit(EXIT_FAILURE);
			 }
			break;
		case 'p':
			user_part_size = atoi(optarg);
			if (MIN_PART_SIZE <= user_part_size && user_part_size <= MAX_PART_SIZE)
				break;
			fprintf(stderr, "Part size should be in range %i..%i\n", MIN_PART_SIZE, MAX_PART_SIZE);
		case 'h':
		default:
			usage();
			exit(EXIT_FAILURE);
		}
	}

	if (optind < argc) {
		destination = argv[optind];
		LOG(DEBUG, "Specified server's IP to connect to: %s", destination);
	} else {
		server = 1;
	}
}

void log_message(enum log_level level, const char *message, ...)
{
	//if (level > verbosity)
	//	return;

	FILE *logfd = stderr;

	time_t now = time(NULL);
	if (now < 0) {
		fprintf(logfd, "Cannot get current time\n");
		exit(EXIT_FAILURE);
	}

	struct tm *local_now = localtime(&now);
	if (!local_now) {
		fprintf(logfd, "Cannot convert current time\n");
		exit(EXIT_FAILURE);
	}
	char timestamp[16];
	if (strftime(timestamp, sizeof(timestamp), "%d %b %T", local_now) == 0)
		sprintf(timestamp, "timestamp error");

	char *level_string = 0;
#ifndef NOCOLORS
	char *color_string = 0;
	switch (level) {
	case ERROR:   level_string = "error"; color_string = "31"; break;
	case MESSAGE: level_string =   "MSG"; color_string = "33"; break;
	case INFO:    level_string =  "info"; color_string = "34"; break;
	case DEBUG:   level_string = "DEBUG"; color_string = "35"; break;
	}
	fprintf(logfd, "[%15s %5s] \033[%sm", timestamp, level_string, color_string);
#else
	switch (level) {
	case ERROR:   level_string = "error"; break;
	case MESSAGE: level_string =   "MSG"; break;
	case INFO:    level_string =  "info"; break;
	case DEBUG:   level_string = "DEBUG"; break;
	}
	fprintf(logfd, "[%15s %5s] ", timestamp, level_string);
#endif

	va_list formats;
	va_start(formats, message);
	vfprintf(logfd, message, formats);
	va_end(formats);

#ifndef NOCOLORS
	fprintf(logfd, "\033[m\n");
#else
	fprintf(logfd, "\n");
#endif

	//fclose(logfd);
}

void die(const char *message)
{
	int last_errno = errno;
	LOG(ERROR, "* Error: %s (errno: %i)", message, errno);

	errno = last_errno;
	perror(0);

	//if (connection)
	//	close_connection();

	exit(EXIT_FAILURE);
}
