
#include "stdafx.h"
#include "MapResurrection.h"

void CsMapResurrection::Delete()
{

}

void CsMapResurrection::Init( sINFO* pSrcInfo )
{
	memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) );
}

