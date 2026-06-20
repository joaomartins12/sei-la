
#include "stdafx.h"
#include "MapRegion.h"

void CsMapRegion::Delete()
{

}

void CsMapRegion::Init( sINFO* pSrcInfo )
{
	memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) );
}

