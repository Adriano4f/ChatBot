#include "Core/CPU/CPU.h"
#include "Utils/Hash/Hash.h"
#include <stdio.h>
short int Exit;

int
CentralProcess (void)
{
  char msg[] = "droga";
  char vl[] = "Cocaina";
  char msg2[] = "el caco de";
  char vl2[] = "Lauriano";
  Hash *h = Hinit();
  INSERT(h, msg, sizeof(msg), vl, sizeof(vl));
  INSERT(h, msg2, sizeof(msg2), vl2, sizeof(vl2));
  DISPLAY(h, CHAR);
  char vl1[] = "Metanfetamina";
  INSERT(h, msg, sizeof(msg), vl1, sizeof(vl1));
  DISPLAY(h, CHAR);
  printf("%s", (const char*) FETCH(h, msg, sizeof(msg))->value );
  Exit = 0;
  while(!Exit)
  {
    LI_info InputInfo = HandleInput(0, 0); // Temporary until multithread is used
    if( InputInfo.Tokens == NULL )
      continue;
    

    
    
  }
  return SUCESS;
}

