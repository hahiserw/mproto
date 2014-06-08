#ifndef _CLIENT_H
#define _CLIENT_H


int read_line(char *, int);
int send_message(/*int, */const char *, size_t, int);
unsigned int prepare_connection(unsigned int, unsigned int);
void create_client(unsigned int, char *);
int send_part(const char *, unsigned int, unsigned int);


#endif
