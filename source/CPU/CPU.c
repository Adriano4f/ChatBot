#include "CPU.h"
#include "Hash.h"
#include "GUtils.h"
#include <stdio.h>
short int Exit;

int
CentralProcess
  (void)
{
  char msg[] = "droga";
  char vl[] = "Cocaina";
  char msg2[] = "el caco de";
  char vl2[] = "Lauriano";
  Hash *h = Hinit();
  if ( NULL == h )
  {
    PrtDbgError( "Could not create the hash table.", "CPU -> CentralProcess" );
    return ERROR;
  }

  int Herr = INSERT(h, msg, sizeof(msg), vl, sizeof(vl));
  if ( !Herr )
    Herr = INSERT(h, msg2, sizeof(msg2), vl2, sizeof(vl2));
  if ( Herr )
  {
    PrtError( "Hash insertion failed.", Herr );
    DESTROY(h);
    return ERROR;
  }
  if ( DISPLAY(h, CHAR) )
    PrtDbgError( "Could not display the hash table.", "CPU -> CentralProcess" );
  char vl1[] = "Metanfetamina";
  Herr = INSERT(h, msg, sizeof(msg), vl1, sizeof(vl1));
  if ( Herr )
  {
    PrtError( "Hash insertion failed.", Herr );
    DESTROY(h);
    return ERROR;
  }
  if ( DISPLAY(h, CHAR) )
    PrtDbgError( "Could not display the hash table.", "CPU -> CentralProcess" );

  const RHEntry *entry = FETCH(h, msg, sizeof(msg));
  if ( NULL == entry )
    PrtDbgError( "Key not found.", "CPU -> CentralProcess" );
  else
    printf("%s", (const char*) entry->value );

  Exit = 0;
  int status = SUCESS;
  while(!Exit)
  {
    LI_info InputInfo = HandleInput(0, 0); // Temporary until multithread is used
    if ( InputInfo.err )
    {
      switch ( InputInfo.err )
      {
        case EOF_ERROR:
          Exit = 1; // Nothing left to read, terminate instead of looping forever
          break;
        case INPUT_READ_FAILED_LI:
          continue; // Recoverable, ask for input again
        default:
          PrtError( "Input handling failed.", InputInfo.err );
          Exit = 1;
          status = ERROR;
          break;
      }
      FreeLI_info(&InputInfo);
      continue;
    }

    if( InputInfo.Tokens == NULL )
    {
      FreeLI_info(&InputInfo);
      continue;
    }
    
    int LPerr = ProcessInput(InputInfo);
    if ( LPerr )
      PrtError( "Input processing failed.", LPerr );

    FreeLI_info(&InputInfo);
  }

  DESTROY(h);
  return status;
}
