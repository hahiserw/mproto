#ifndef _CONFIG_H
#define _CONFIG_H


/* 
 * Plik konfiguracyjny dla mproto
 */


// Port na którym działa protokół
#define M_PORT 21212

// Znak zachęty dla klienta w trybie interaktywnym
#define CLIENT_PROMPT "> "

// Maksymalna długość wiadomości
#define MAX_MESSAGE_SIZE 4096

// Zakres wielkości fragmentu wiadomości akceptowanego przez serwer
#define MIN_PART_SIZE 6
#define MAX_PART_SIZE 666

// Wyłącz kolorowanie komunikatów
//#define NOCOLORS


#endif
