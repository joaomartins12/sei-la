
#include "stdafx.h"
#include "MapTableObj.h"

CsMapTableObj::CsMapTableObj()
{
	m_pData = NULL;
}

void CsMapTableObj::Delete()
{
	SAFE_DELETE_ARRAY( m_pData );
}

void CsMapTableObj::Init( sINFO* pSrcInfo )
{
	memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) );
}

void CsMapTableObj::SaveBin( FILE* fp )
{
	fwrite( GetInfo(), sizeof( CsMapTableObj::sINFO ), 1, fp );
	fwrite( GetData(), sizeof( char ), GetInfo()->s_nDataSize, fp );
}

void CsMapTableObj::LoadBin( FILE* fp )
{
	fread( GetInfo(), sizeof( CsMapTableObj::sINFO ), 1, fp );

	AllocData();
	fread( m_pData, sizeof( char ), GetInfo()->s_nDataSize, fp );
}

void CsMapTableObj::LoadFilePack( int nHandle )
{
	_read( nHandle, GetInfo(), sizeof( CsMapTableObj::sINFO ) );

	AllocData();
	_read( nHandle, m_pData, sizeof( char )*GetInfo()->s_nDataSize );
}

void CsMapTableObj::AllocData()
{
	assert_cs( m_pData == NULL );
	assert_cs( m_Info.s_nDataSize > 0 );
	m_pData = csnew char[ m_Info.s_nDataSize ];
}