
#include "stdafx.h"
#include "MapCharLight.h"


void CsMapCharLight::Delete()
{
	assert_cs( m_pInfo != NULL );
	SAFE_DELETE( m_pInfo );
}

void CsMapCharLight::Init( sINFO_BASE* pSrcInfo )
{
	assert_cs( m_pInfo == NULL );
	if( pSrcInfo->s_nType == 0 )		// Direction
	{
		m_pInfo = csnew sINFO_DIR;
		memcpy( m_pInfo, pSrcInfo, sizeof( sINFO_DIR ) );
	}
	else
	{
		assert_cs( pSrcInfo->s_nType == 1 );		// Point
		m_pInfo = csnew sINFO_POINT;
		memcpy( m_pInfo, pSrcInfo, sizeof( sINFO_POINT ) );
	}	
}

