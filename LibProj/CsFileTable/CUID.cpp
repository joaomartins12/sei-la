
#include "stdafx.h"
#include "Cuid.h"



void CsCuid::Delete()
{

}

void CsCuid::Init( sINFO* pSrcInfo )
{
	memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) );
}

