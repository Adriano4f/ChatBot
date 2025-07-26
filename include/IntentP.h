#ifndef _IntentP_
#define _IntentP_ 

// Custom includes
#include "LI_Unit.h"

// H
typedef enum 
{
  STATEMENT,
  QUESTION,
  COMMAND,
  EXCLAMATION,
  UNKNOWN,
} SentenceType;

typedef struct Intent_info
{
  SentenceType intent;
  
}  Intent_info;

// Functions

Intent_info
GetIntent
  (LI_info InputInfo);


#endif

