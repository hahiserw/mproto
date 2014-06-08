#ifndef _SERVER_H
#define _SERVER_H

#include "message.h"


int negotiate_params(int, struct info *);
void create_server();
unsigned int get_message(int, char *, struct info *);
int get_part(int, char *, int *, int);


#endif
