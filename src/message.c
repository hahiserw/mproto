#include <string.h>

#include "main.h"
#include "message.h"



void debug_message(struct message *m)
{
	LOG(DEBUG, "index    = %10u", m->index);
	LOG(DEBUG, "size     = %10u", m->size);
	unsigned int c = m->checksum;
	LOG(DEBUG, "checksum = %10u (%3u %3u %3u %3u)", c, c >> 24, c >> 16 & 0xff, c >> 8 & 0xff, c & 0xff);
	LOG(DEBUG, "text     = '%s' (%i)", m->text, strlen(m->text));
}


void pack_data(struct message *message, const char *text, int text_size, int index, int part_size)
{
	message->size  = text_size;
	message->index = index;

	memcpy(message->text, text, text_size);
	// Make sure there is no garbage left
	// FIXME Get part_size from init_client
	if (text_size < part_size)
		memset((message->text + text_size), 0, part_size - text_size);

	message->checksum = compute_checksum(text);

	convert(message->text);

}

 
void remove_bad_chars()
{
}


// Modified Fletcher's checksum
unsigned int compute_checksum(const char *text)
{
	unsigned int s1 = 0;
	unsigned int s2 = 0;

	for (char *c = (char *)text; *c; ++c) {
		s1 = (s1 + *c) & 0xffff; // % 65535;
		s2 = (s2 + s1) & 0xffff; // % 65535;
	}

	//LOG(DEBUG, "'%s', %u, %u", text, s1, s2);

	return (s2 << 16) | s1;
}
//{
//	unsigned int csum = 0;
//	char *c = (char *)text;
//
//	while (*c) {
//		//csum ^= *c << ((3 - ((c - text) % 4)) * 8);
//		csum ^= *c << ((3 - ((c - text) & 3)) * 8);
//		++c;
//	}
//
//	return csum;
//}


// No hacker can crack this!
void convert(char *m)
{
	while (*m)
		*m++ ^= 0xff;
}


// Set retransmission bits to be later cleared if part came
//void ret_init(char *bits, unsigned int parts)
//{
//	unsigned int i = 0;
//	for (; i < (parts >> 3); ++i)
//		bits[i] = 0xff;
//
//	bits[i] = (1 << ((parts & 7) + 1)) - 1;
//}
//
//void ret_clear(char *bits, unsigned int index)
//{
//	bits[index >> 3] &= ~(1 << (index & 7));
//}
//
//void ret_set(char *bits, unsigned int index)
//{
//	bits[index >> 3] |= 1 << (index & 7);
//}
