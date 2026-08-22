#ifndef _LI_UNIT_
#define _LI_UNIT_

// Language Input Unit

// STD
#include <stddef.h>
#include <stdint.h>

// Defines
#define SUCESS 0
#define ERROR 1
#define UNKNOWN_ERROR -1
#define INPUT_BUFFER_SIZE 1024
#define EOF_ERROR 254000
#define INPUT_READ_FAILED_LI 255000

#define DELIMITERS " \t\n.,!?'"
#define BUFFER_SIZE 512


// H

extern char GlobalInputBuffer[INPUT_BUFFER_SIZE];
extern int LettersGraph[255][255];

typedef struct LI_info
{
  char** Tokens;
  char* Input;
} LI_info;

// Functions

LI_info
HandleInput (int8_t linked, const void* process);

LI_info
Linked (const void* process);

int
GetInput (void);

char **
Tokenise (void);


#endif
