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
    case EOF_ERROR:
      // Nothing left to read, the caller is expected to terminate
      return (LI_info){ NULL, NULL, EOF_ERROR };
    case INPUT_READ_FAILED_LI:
      // Recoverable, the caller asks for input again
      return (LI_info){ NULL, NULL, INPUT_READ_FAILED_LI };
    case UNKNOWN_ERROR:
      PrtDbgError( "Input could not be read.", "LI -> HandleInput" );
      return (LI_info){ NULL, NULL, UNKNOWN_ERROR };
  }
  
  
  if( strlen(GlobalInputBuffer) <= 1 )
    return (LI_info){ NULL, GlobalInputBuffer, SUCESS };
  
  // Copied before tokenising, strtok writes into GlobalInputBuffer
  char *Input = (char *)AllocPtr( strlen(GlobalInputBuffer) + 1 );
  if ( NULL == Input )
    return (LI_info){ NULL, NULL, ALLOC_FAILED_LI };
  strcpy( Input, GlobalInputBuffer );
  
  char** Tokens = Tokenise();
  if ( NULL == Tokens )
  {
    free(Input);
    return (LI_info){ NULL, NULL, TOKENISE_FAILED_LI };
  }
  
  return (LI_info){ Tokens, Input, SUCESS };
}


LI_info
Linked
  (const void* process)
{
  return (LI_info){ NULL, NULL, SUCESS };
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
  return SUCESS;
}


static void
FreeTokens
  (char **Tokens,
  size_t count)
{
  for ( size_t i = 0; i < count; ++i )
    free(Tokens[i]);
  free(Tokens);
}


char
**Tokenise
  (void)
{
  size_t sz = 500; // Amount of slots, not bytes
  char **Token = (char **)AllocPPtr( sz * sizeof(char *) );
  if ( NULL == Token )
    return NULL;

  char *first = strtok(GlobalInputBuffer, DELIMITERS);
  
  if ( !first ) 
  {
    free(Token);
    return NULL;
  }
  Token[0] = (char *)AllocPtr ( strlen(first) + 1 );
  if ( NULL == Token[0] )
  {
    free(Token);
    return NULL;
  }
  strcpy ( Token[0], first );
  
  char **tmpPPtr;
  char *tmp;
  size_t i = 1;
  while ( (tmp = strtok( NULL, DELIMITERS ) ) != NULL ) 
  {
    Token[i] = (char *)AllocPtr ( strlen(tmp) + 1 );
    if ( NULL == Token[i] )
    {
      FreeTokens( Token, i );
      return NULL;
    }
    strcpy ( Token[i], tmp );
    ++i;
    
    printf ("%s%s%s\t",
      Txt ( CYAN ),
      Token[i-1],
      Txt ( CRESET ) ); // Debug
    
    if ( i + 1 < sz )
      continue;
    
    // Resize, one extra slot is kept for the NULL terminator
    sz += 500;
    tmpPPtr = (char **)ReallocPPtr ( sz * sizeof(char *), (void **)Token );
    if ( NULL == tmpPPtr )
    {
      PrtDbgError ( "Could not grow the token list.", "LI -> Tokenise" );
      FreeTokens( Token, i );
      return NULL;
    }
    Token = tmpPPtr;
    
  }
  
  tmpPPtr = (char **)ReallocPPtr( (i+1) * sizeof(char *), (void **)Token );
  if ( NULL != tmpPPtr ) // Shrinking failure is harmless, the bigger block stays in use
      Token = tmpPPtr;
  Token[i] = NULL;
  return Token;
}


void
FreeLI_info
  (LI_info *info)
{
  if ( NULL == info )
    return;

  if ( NULL != info->Tokens )
  {
    for ( size_t i = 0; NULL != info->Tokens[i]; ++i )
      free(info->Tokens[i]);
    free(info->Tokens);
    info->Tokens = NULL;
  }

  if ( NULL != info->Input && GlobalInputBuffer != info->Input )
    free(info->Input);
  info->Input = NULL;
}
