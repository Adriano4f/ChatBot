#include "LI_Unit.h"
  #include "GUtils.h"


// STD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char GlobalInputBuffer[INPUT_BUFFER_SIZE];
int LettersGraph[255][255];


LI_info
HandleInput
  (int8_t linked, 
  const void* process)
{
  if(linked)
  {
    return Linked(process);
  }
  
  int err = GetInput();
  switch(err)
  {
    // TODO: add error handling for each case
    case EOF_ERROR:
      // Print that program will terminate because of End Of File
      break;
    case INPUT_READ_FAILED_LI:
      // Ask for input again and return (LI_info){NULL, NULL} -> returning {NULL, NULL} will make Input being handled again
      break;
    case UNKNOWN_ERROR:
      // Idk, do something or ask ChatGPT
      break;
  }
  
  
  if( strlen(GlobalInputBuffer) <= 1 )
    return (LI_info){ NULL, GlobalInputBuffer };
  
  char** Tokens = Tokenise();
  
  LI_info info = 
  {
    Tokens,
    (char *)malloc( strlen(GlobalInputBuffer) * sizeof(char) + 1 )
  };
  strcpy( info.Input, GlobalInputBuffer );
  
  return info;
}


LI_info
Linked
  (const void* process)
{
  return (LI_info){ NULL, NULL };
}


int
GetInput
  (void)
{
  if(fgets(GlobalInputBuffer, sizeof(GlobalInputBuffer), stdin) == NULL)
  {
    if ( feof(stdin) )
    {
      PrtError( "End of file reached.", EOF_ERROR );
      return EOF_ERROR;
    }
    else if ( ferror(stdin) )
    {
      PrtError( "Error reading input.", INPUT_READ_FAILED_LI );
      return INPUT_READ_FAILED_LI;
    }
    // In case an unexpected error happens, both ifs above won't be ran and won't return
    PrtDbgError( "Unexpected Error.", "LI -> GetInput" );
    return UNKNOWN_ERROR;
  }
  PrtColored(CYAN, GlobalInputBuffer); // Debug
  return 0;
}


char
**Tokenise
  (void)
{
  size_t sz = 500;
  char **Token = (char **)AllocPPtr(sz);
  char *first = strtok(GlobalInputBuffer, DELIMITERS);
  
  if ( !first ) 
    return NULL;
  Token[0] = (char *)AllocPtr (BUFFER_SIZE * sizeof(char)); // BUFFER_SIZE == 512
  strcpy ( Token[0], first );
  Token[0] = (char *)ReallocPtr ( strlen( Token[0]), Token[0] );
  
  char **tmpPPtr;
  char *tmp;
  int i = 1;
  while ( (tmp = strtok( NULL, DELIMITERS ) ) != NULL ) 
  {
    Token[i] = (char *)AllocPtr ( (strlen(tmp) + 5) * sizeof(char) );
    strcpy ( Token[i], tmp );
    ++i;
    
    PrtColored(CYAN, Token[i-1]); // Debug
    printf("\t");
    
    if ( i < sz-100 )
      continue;
    
    // Resize
    sz += 500;
    if ( (tmpPPtr = (char **)ReallocPPtr ( (sz) * sizeof(char), (void **)Token ) ) != NULL )
      Token = tmpPPtr;
    else
    {
      PrtDbgError ( "Unexpected Error.", "LI -> Tokenise" );
      return NULL;
    }
    
  }
  
  if ( (tmpPPtr = (char **)ReallocPPtr( (i+5) * sizeof(char), (void **)Token ) ) != NULL )
      Token = tmpPPtr;
  Token[i] = NULL;
  return Token;
}

  