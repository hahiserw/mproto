#ifndef _MAIN_H
#define _MAIN_H


#include "config.h"


#define ceil(a, b) (a / b + (a % b? 1: 0))
#define LOG(level, ...) \
	for (;;) { if (level <= verbosity) log_message(level, ##__VA_ARGS__); break; }


enum log_level {
	ERROR,
	MESSAGE,
	INFO,
	DEBUG
};


extern enum log_level verbosity;
extern char *message;
extern int part_size;


void usage();
void parse_args(int, char *[]);
void log_message(enum log_level, const char *, ...);
void die(const char *);
void handle_signal(int);


#endif
