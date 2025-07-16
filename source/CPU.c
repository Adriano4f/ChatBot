#include "../include/CPU.h"
#include "../include/Hash.h"

short int Exit;

int 
CentralProcess
	(void)
{
	Exit = 0;
	while(!Exit)
	{
		LI_info InputInfo = HandleInput(0, 0); // Temporary until multithread is used
		if( InputInfo.Tokens == NULL )
			continue;
		
		int LPerr = ProcessInput(InputInfo);
		if ( LPerr )
		{
			// TODO: Errors for LP_Unit
		}
		
	}
	return SUCESS;
}

