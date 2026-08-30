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
#define ALLOC_FAILED_LI 253000
#define TOKENISE_FAILED_LI 252000

#define DELIMITERS " \t\n.,!?'"
#define BUFFER_SIZE 512


// H

extern char GlobalInputBuffer[INPUT_BUFFER_SIZE];

typedef struct LI_info
{
  char** Tokens;
  char* Input;
  int err; // SUCESS or one of the *_LI / EOF_ERROR / UNKNOWN_ERROR codes above
} LI_info;

// Functions

LI_info
HandleInput
  (int8_t linked, 
  const void* process);

LI_info
Linked
  (const void* process);

int 
GetInput
  (void);

char
**Tokenise
  (void);

void
FreeLI_info
  (LI_info *info);


#endif
