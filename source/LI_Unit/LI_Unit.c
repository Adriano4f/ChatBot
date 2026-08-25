#include "LI_Unit.h"
  #include "GUtils.h"
  #include "CPU.h"


// STD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char GlobalInputBuffer[INPUT_BUFFER_SIZE];
int LettersGraph[255][255];


static void
FreeTokens
  (char **Token,
  size_t count)
{
  for ( size_t i = 0; i < count; ++i )
    free(Token[i]);
  free(Token);
}


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
    case EOF_ERROR:
      // No further input can arrive, so asking again would loop forever
      Exit = 1;
      return (LI_info){ NULL, NULL };
    case INPUT_READ_FAILED_LI:
    case UNKNOWN_ERROR:
      // Returning {NULL, NULL} makes Input being handled again
      return (LI_info){ NULL, NULL };
  }
  
  const size_t len = strlen(GlobalInputBuffer);
  if( len <= 1 )
    return (LI_info){ NULL, GlobalInputBuffer };
  
  // The copy is taken before Tokenise, which splits the buffer in place
  char *Input = (char *)AllocPtr( (len + 1) * sizeof(char) );
  if ( Input == NULL )
    return (LI_info){ NULL, NULL };
  memcpy( Input, GlobalInputBuffer, len + 1 );
  
  char** Tokens = Tokenise();
  if ( Tokens == NULL )
  {
    free(Input);
    return (LI_info){ NULL, NULL };
  }
  
  return (LI_info){ Tokens, Input };
}


LI_info
Linked
  (const void* process)
{
  return (LI_info){ NULL, NULL };
}


void
FreeLIInfo
  (LI_info *info)
{
  if ( info->Tokens == NULL ) // Input then aliases GlobalInputBuffer, which is static
    return;
  
  for ( size_t i = 0; info->Tokens[i] != NULL; ++i )
    free(info->Tokens[i]);
  free(info->Tokens);
  free(info->Input);
  
  info->Tokens = NULL;
  info->Input = NULL;
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
  printf("%s%s%s",
    Txt(CYAN),
    GlobalInputBuffer,
    Txt(CRESET) ); // Debug
  return 0;
}


char
**Tokenise
  (void)
{
  size_t sz = TOKEN_CAPACITY_STEP;
  char **Token = (char **)AllocPPtr( sz * sizeof(char *) );
  if ( Token == NULL )
    return NULL;
  
  size_t i = 0;
  for ( char *tmp = strtok( GlobalInputBuffer, DELIMITERS );
    tmp != NULL;
    tmp = strtok( NULL, DELIMITERS ) )
  {
    if ( i + 1 >= sz ) // One slot is always reserved for the NULL terminator
    {
      char **tmpPPtr = (char **)ReallocPPtr
        ( (sz + TOKEN_CAPACITY_STEP) * sizeof(char *), (void **)Token );
      if ( tmpPPtr == NULL )
      {
        PrtDbgError ( "Unexpected Error.", "LI -> Tokenise" );
        FreeTokens ( Token, i );
        return NULL;
      }
      Token = tmpPPtr;
      sz += TOKEN_CAPACITY_STEP;
    }
    
    const size_t len = strlen(tmp);
    Token[i] = (char *)AllocPtr ( (len + 1) * sizeof(char) );
    if ( Token[i] == NULL )
    {
      FreeTokens ( Token, i );
      return NULL;
    }
    memcpy ( Token[i], tmp, len + 1 );
    
    printf ("%s%s%s\t",
      Txt ( CYAN ),
      Token[i],
      Txt ( CRESET ) ); // Debug
    
    ++i;
  }
  
  if ( i == 0 )
  {
    free(Token);
    return NULL;
  }
  
  Token[i] = NULL;
  return Token;
}

  