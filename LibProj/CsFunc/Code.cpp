
#include "stdafx.h"
#include "Code.h"

void Encryption_Simple( char* src, char* dest )
{
	int i, len;
	unsigned char bt, cXorValue = 31;

	// --인코드(간단한 암호화)------------
	len = (int)strlen(src);
	assert_cs( len < CODE_MAX );
	for(i=0; i<len; i++)
	{
		bt = (unsigned char)src[i];
		bt = bt ^ cXorValue;
		dest[i] = (char)(bt);
	}

	dest[len] = '\0';
}

void Decryption_Simple( char* src, char* dest )
{
	int i, len;
	unsigned char bt, cXorValue = 31;

	// --디코드(간단한 암호화)------------
	len = (int)strlen(src);
	assert_cs( len < CODE_MAX );
	for(i=0; i<len; i++)
	{
		bt = (unsigned char)src[i];
		bt = bt ^ cXorValue;
		dest[i] = (char)(bt);
	}

	dest[len] = '\0';
}



