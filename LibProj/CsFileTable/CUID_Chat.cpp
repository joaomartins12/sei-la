

#include "stdafx.h"
#include "Cuid_Chat.h"



void CsCuid_Chat::Delete()
{

}

void CsCuid_Chat::Init( sINFO* pSrcInfo )
{
	memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) );
}

