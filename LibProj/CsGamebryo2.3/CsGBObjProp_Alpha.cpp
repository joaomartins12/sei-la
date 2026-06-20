
#include "stdafx.h"
#include "CsGBObjProp_Alpha.h"

float	CsGBObjProp_Alpha::m_fDestAlphaValue = 0.15f;
float	CsGBObjProp_Alpha::m_fDeltaAlphaValue = 0.85f;
float	CsGBObjProp_Alpha::m_fDestAlphaTime = 0.7f;

CSGBMEMPOOL_CPP( CsGBObjProp_Alpha );

CsGBObjProp_Alpha::CsGBObjProp_Alpha():m_eAlphaType(AT_NONE),m_fCurAlphaValue(0.0f)
{

}

CsGBObjProp_Alpha::~CsGBObjProp_Alpha()
{

}

CsGBObjProp* CsGBObjProp_Alpha::GetInstance( CsGBObject* pParentObj )
{
	CsGBObjProp_Alpha* pAlpha = CsGBObjProp_Alpha::NewInstance();
	pAlpha->Init( pParentObj );
	return pAlpha;
}

CsGBObjProp* CsGBObjProp_Alpha::GetInstance( NiNode* pParentNiNode )
{
	CsGBObjProp_Alpha* pAlpha = CsGBObjProp_Alpha::NewInstance();
	pAlpha->Init( pParentNiNode );
	return pAlpha;
}


void CsGBObjProp_Alpha::Delete()
{
	m_CsBackupProp.Delete();
	CsGBObjProp::Delete();	
}

void CsGBObjProp_Alpha::Init( CsGBObject* pParentObj )
{
	CsGBObjProp::Init( pParentObj );

	m_eAlphaType = AT_NONE;
	m_fCurAlphaValue = 1.0f;
}

void CsGBObjProp_Alpha::Init( NiNode* pParentNiNode )
{
	CsGBObjProp::Init( pParentNiNode );

	m_eAlphaType = AT_NONE;
	m_fCurAlphaValue = 1.0f;
}


void CsGBObjProp_Alpha::Update( float fElapsedTime, float fAniTime )
{
	switch( m_eAlphaType )
	{
	case AT_NONE:	return;
	case AT_ALPHA_DECREASE:
		if( ( m_fCurAlphaValue -= ( CsGBObjProp_Alpha::m_fDeltaAlphaValue/CsGBObjProp_Alpha::m_fDestAlphaTime )*fElapsedTime )
									< CsGBObjProp_Alpha::m_fDestAlphaValue )
		{
			m_fCurAlphaValue = m_fDestAlphaValue;
			m_eAlphaType = AT_NONE;
		}
		if( m_CsBackupProp.s_listProperty.empty() == true )
			nsCSGBFUNC::SetMaterial_Alpha( m_pParentNiNode, m_fCurAlphaValue, &m_CsBackupProp );
		else
			nsCSGBFUNC::SetMaterial_Alpha( m_pParentNiNode, m_fCurAlphaValue );
		break;
	case AT_ALPHA_INCREASE:
		if( ( m_fCurAlphaValue += ( CsGBObjProp_Alpha::m_fDeltaAlphaValue/CsGBObjProp_Alpha::m_fDestAlphaTime )*fElapsedTime ) > 1.0f )
		{
			m_fCurAlphaValue = 1.0f;
			nsCSGBFUNC::SetPropertyBack( m_pParentNiNode, &m_CsBackupProp );
			m_CsBackupProp.Delete();
			m_eAlphaType = AT_NONE;
		}
		else
		{
			nsCSGBFUNC::SetMaterial_Alpha( m_pParentNiNode, m_fCurAlphaValue );
		}		
		break;
	}
}




