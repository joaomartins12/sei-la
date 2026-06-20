
#include "stdafx.h"
#include "MapStart.h"

void CsMapStart::Delete()
{

}

void CsMapStart::Init( sINFO* pSrcInfo )
{
	memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) );
}

