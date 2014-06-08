#ifndef _MESSAGE_H
#define _MESSAGE_H


struct info {
	unsigned int part_size;
	unsigned int message_length;
	unsigned int parts;
};

// http://stackoverflow.com/questions/4426016/sending-a-struct-with-char-pointer-inside
struct message {
	unsigned int index;
	unsigned int size;
	unsigned int checksum;
	char text[1]; // IT MUST BE THE LAST FIELD
};


void pack_data(struct message *, const char *, int, int, int);
void remove_bad_chars();
unsigned int compute_checksum(const char *message);
void convert(char *);

void debug_message(struct message *);

//void ret_init(char *, unsigned int);
//void ret_clear(char *, unsigned int);
//void ret_cet(char *, unsigned int);


#endif
