
#include "stdafx.h"
#include "MapPortal.h"

void CsMapPortal::Delete()
{

}

void CsMapPortal::Init( sINFO* pSrcInfo )
{
	m_pLinkNpcType = NULL;
	memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) );
}

