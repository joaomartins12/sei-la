
#include "stdafx.h"
#include "CsGBObjOptFactor.h"

CSGBMEMPOOL_CPP( CsGBObjOptFactor );

CsGBObjOptFactor::CsGBObjOptFactor()
{
}

CsGBObjOptFactor::~CsGBObjOptFactor()
{
}

void CsGBObjOptFactor::Delete()
{
	std::map< DWORD, CsGBObjOpt* >::iterator it = m_mapOption.begin();
	for( ; it!=m_mapOption.end(); ++it )
	{
		CsGBObjOpt::DeleteOption( it->second );
	}
	m_mapOption.clear();
}

CsGBObjOpt* CsGBObjOptFactor::InsertOption( CsGBObjOpt::eOPT_TYPE eType, CsGBObject* pParentObject )
{
	CsGBObjOpt* pReturnOpt = NULL;
	switch( eType )
	{
	case CsGBObjOpt::OPT_DARKMAP:
		assert_cs( m_mapOption.find( CsGBObjOpt::OPT_DARKMAP ) == m_mapOption.end() );
		pReturnOpt = CsGBObjOpt_DarkMap::GetInstance( pParentObject );
		m_mapOption[ CsGBObjOpt::OPT_DARKMAP ] = pReturnOpt;
		break;
	default:
		assert_cs( false );
	}

	return pReturnOpt;	
}

void CsGBObjOptFactor::ResetOption()
{
	std::map< DWORD, CsGBObjOpt* >::iterator it = m_mapOption.begin();
	for( ; it!=m_mapOption.end(); ++it )
	{
		it->second->ResetOption();
	}
}


UINT CsGBObjOptFactor::GetSaveSize()
{
	UINT uiReturn = 0;
	std::map< DWORD, CsGBObjOpt* >::iterator it = m_mapOption.begin();
	for( ; it!=m_mapOption.end(); ++it )
	{
		uiReturn += it->second->GetSaveSize();
	}
	return uiReturn;
}

void CsGBObjOptFactor::Save( char* pData, UINT& uiOffset )
{
	std::map< DWORD, CsGBObjOpt* >::iterator it = m_mapOption.begin();
	for( ; it!=m_mapOption.end(); ++it )
	{
		it->second->Save( pData, uiOffset );
	}
}


//
//void CsGBObjOptFactor::Update( float fElapsedTime )
//{
//	std::map< DWORD, CsGBObjOpt* >::iterator it = m_mapOption.begin();
//	for( ; it!=m_mapOption.end(); ++it )
//	{
//		it->second->Update( fElapsedTime );
//	}
//}