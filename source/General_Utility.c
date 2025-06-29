#include "../include/General_Utility.h"

// STD
#include <stdio.h>
#include <stdlib.h>

const char*
Txt
	(const char* text)
{
	return text;
}

void 
PrtError
	(const char *msg,
	int64_t error_code)
{
	printf("%s%s ERROR CODE: %s%ld%s\n",
					Txt(RED), msg, Txt(MAGENTA), error_code, Txt(CRESET) );
}

void 
PrtDbgError
	(const char *msg,
	const char* error_msg)
{
	printf("%s%s %s%s%s",
			Txt(RED), msg, Txt(YELLOW), error_msg, Txt(CRESET) );
}

// == MEMORY MANAGEMENT ==

void 
*PtrVerify
	(void *ptr,
	const char* msg,
	const char* error_msg)
{
	if ( ptr == NULL )
	{
		PrtDbgError(msg, error_msg);
		return NULL;
	}
	else
		return ptr;
}


char 
*AllocCharPtr
	(size_t nmemb)
{
	char *tmp_buffer = (char*)malloc(nmemb * sizeof(char));
	return (char *) PtrVerify
	( 	
		(void *)tmp_buffer, 
		"Allocation Error.", 
		"LI -> AllocCharPtr" 
	);
}


char 
*ReallocCharPtr
	(size_t nmemb, char *ptr)
{
	char *tmp_buffer = (char*)realloc(ptr, nmemb * sizeof(char));
		return (char *) PtrVerify
	( 	
		(void *)tmp_buffer, 
		"Reallocation Error.", 
		"LI -> ReallocCharPtr" 
	);
}


char 
**AllocCharPPtr
	(size_t nmemb)
{
	char **tmp_buffer = (char**)malloc(nmemb * sizeof(char*));
	return (char **) PtrVerify
	( 	
		(void *)tmp_buffer, 
		"Allocation Error.", 
		"LI -> AllocCharPPtr" 
	);
}


char 
**ReallocCharPPtr
	(size_t nmemb, char **ptr)
{
	char **tmp_buffer = (char**)realloc(ptr, nmemb * sizeof(char*));
	return (char **) PtrVerify
	( 	
		(void *)tmp_buffer, 
		"Reallocation Error.", 
		"LI -> ReallocCharPPtr" 
	);
}
