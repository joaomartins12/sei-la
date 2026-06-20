#include "mss.h"
#include "imssapi.h"

#include <stdio.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
extern "C" void Platform_CloseFile(PLATFORM_FILE* i_File)
{
    fclose(*(FILE**)i_File->plat_specific);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
extern "C" void Platform_SeekFromBeginning(PLATFORM_FILE* i_File, S32 i_Offset)
{
    fseek(*(FILE**)i_File->plat_specific, i_Offset + i_File->start_pos, SEEK_SET);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
extern "C" U32 Platform_ReadFile(PLATFORM_FILE* i_File, void* o_Buffer, U32 i_ReadBytes)
{
    return (U32)fread(o_Buffer, 1, i_ReadBytes, *(FILE**)i_File->plat_specific);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
extern "C" int Platform_OpenFile(PLATFORM_FILE* i_File, char const * fn )
{
    RR_COMPILER_ASSERT( sizeof(int) <= MAX_PLATFILE_SPECIFIC_SIZE );

    FILE* fh = fopen(fn, "rb");
    if (fh == 0)
    	return 0;

    fseek(fh, 0, SEEK_END);
    i_File->raw_length = ftell(fh);
    fseek(fh, 0, SEEK_SET);

    *(FILE**)i_File->plat_specific = fh;
    
    return 1;
}
