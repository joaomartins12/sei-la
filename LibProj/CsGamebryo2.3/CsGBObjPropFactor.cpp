
#include "stdafx.h"
#include "CsGBObjPropFactor.h"

CSGBMEMPOOL_CPP( CsGBObjPropFactor );

CsGBObjPropFactor::CsGBObjPropFactor()
{

}

CsGBObjPropFactor::~CsGBObjPropFactor()
{

}

void CsGBObjPropFactor::Delete()
{
	std::map< DWORD, CsGBObjProp* >::iterator it = m_mapProp.begin();
	for( ; it!=m_mapProp.end(); ++it )
	{
		CsGBObjProp::DeleteProp( it->second );
	}
	m_mapProp.clear();
}

CsGBObjProp* CsGBObjPropFactor::InsertProp( CsGBObjProp::ePROP_TYPE eType, CsGBObject* pParentObject )
{
	CsGBObjProp* pReturnProp = NULL;

	assert_cs( m_mapProp.find( eType ) == m_mapProp.end() );

	switch( eType )
	{
	case CsGBObjProp::PROP_ALPHA:		
		pReturnProp = CsGBObjProp_Alpha::GetInstance( pParentObject );
		break;
	case CsGBObjProp::PROP_ABV:
		pReturnProp = CsGBObjProp_ABV::GetInstance( pParentObject );
		break;
	case CsGBObjProp::PROP_AV_CAP:
		pReturnProp = CsGBObjProp_AvCap::GetInstance( pParentObject );
		break;
	case CsGBObjProp::PROP_AV_TBP:
		pReturnProp = CsGBObjProp_AvTbp::GetInstance( pParentObject );		
		break;
	case CsGBObjProp::PROP_SOUND:
		pReturnProp = CsGBObjProp_Sound::GetInstance( pParentObject );		
		break;
	case CsGBObjProp::PROP_ACTOR:
		pReturnProp = CsGBObjProp_Actor::GetInstance( pParentObject );
		break;
	default:
		assert_cs( false );
	}

	m_mapProp[ eType ] = pReturnProp;
	return pReturnProp;	
}

CsGBObjProp* CsGBObjPropFactor::InsertProp( CsGBObjProp::ePROP_TYPE eType, NiNode* pParentNiNode )
{
	CsGBObjProp* pReturnProp = NULL;

	assert_cs( m_mapProp.find( eType ) == m_mapProp.end() );

	switch( eType )
	{
	case CsGBObjProp::PROP_ALPHA:		
		pReturnProp = CsGBObjProp_Alpha::GetInstance( pParentNiNode );
		break;
	case CsGBObjProp::PROP_ABV:
		pReturnProp = CsGBObjProp_ABV::GetInstance( pParentNiNode );
		break;
	case CsGBObjProp::PROP_AV_CAP:
		pReturnProp = CsGBObjProp_AvCap::GetInstance( pParentNiNode );
		break;
	case CsGBObjProp::PROP_AV_TBP:
		pReturnProp = CsGBObjProp_AvTbp::GetInstance( pParentNiNode );		
		break;
	case CsGBObjProp::PROP_SOUND:
		pReturnProp = CsGBObjProp_Sound::GetInstance( pParentNiNode );		
		break;
	case CsGBObjProp::PROP_ACTOR:
		pReturnProp = CsGBObjProp_Actor::GetInstance( pParentNiNode );
		break;
	default:
		assert_cs( false );
	}

	m_mapProp[ eType ] = pReturnProp;
	return pReturnProp;	
}

void CsGBObjPropFactor::Update( float fElapsedTime, float fAniTime )
{
	std::map< DWORD, CsGBObjProp* >::iterator it = m_mapProp.begin();
	for( ; it!=m_mapProp.end(); ++it )
	{
		it->second->Update( fElapsedTime, fAniTime );
	}
}

//==========================================================================================
//
//		Prop
//
//==========================================================================================

CsGBObjProp* CsGBObjPropFactor::GetProp( CsGBObjProp::ePROP_TYPE eType )
{
	if( m_mapProp.find( eType ) == m_mapProp.end() )
		return NULL;
	return m_mapProp[ eType ];
}

bool CsGBObjPropFactor::IsProp( CsGBObjProp::ePROP_TYPE eType )
{
	return ( m_mapProp.find( eType ) != m_mapProp.end() );
}