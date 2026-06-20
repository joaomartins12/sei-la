
#include "stdafx.h"
#include "CsGBObj_Rot.h"

CSGBMEMPOOL_CPP( CsGBObj_Rot );

CsGBObj_Rot::CsGBObj_Rot():m_bLimit(false),m_bPlus(false),m_fCurRot(0.0f),m_pUDDRotation(NULL)
{
}

CsGBObj_Rot::~CsGBObj_Rot()
{
	m_pUDDRotation = NULL;
}

void CsGBObj_Rot::Init( NiNode* pNiNode, int nGetID, sINFO* pObjectInfo, sUSER_DEFINE* pUserDefine )
{
	CsGBObject::Init( pNiNode, nGetID, pObjectInfo, pUserDefine );
	
	m_fCurRot = 0.0f;
	m_bPlus = true;
	m_bLimit = ( ( m_pUserDefine->s_dwPlag & sUSER_DEFINE::UD_ROTATION_LIMIT ) != 0 );

	m_pUDDRotation = (sUDD_Rotation*)pUserDefine->GetData( sUDD_Rotation::Type() );
}

void CsGBObj_Rot::Update( float fElapsedTime, bool bCulling /*=true*/ )
{
	if( m_bCallReady == false )
		return;

	if( IsEnableUse() == false )
		return;

	if( m_bLimit )		_UpdateRotationLimit( fElapsedTime );
	else				_UpdateRotation( fElapsedTime );

	CsGBObject::Update( fElapsedTime, bCulling );
}

void CsGBObj_Rot::_UpdateRotation( float fElapsedTime )
{
	m_fCurRot += fElapsedTime*m_pUDDRotation->s_fRotSpeed;
	NiMatrix3 matRot;
	matRot.MakeZRotation( m_fCurRot );
	m_Node.m_pNiNode->SetRotate( matRot );
}

void CsGBObj_Rot::_UpdateRotationLimit( float fElapsedTime )
{
	if( m_bPlus == true )
	{
		m_fCurRot += fElapsedTime*m_pUDDRotation->s_fRotSpeed;
		if( ( m_bLimit == true )&&( m_fCurRot > m_pUDDRotation->s_fRotLimit_Max ) )
		{
			m_fCurRot = m_pUDDRotation->s_fRotLimit_Max;
			m_bPlus = false;
		}
	}
	else
	{
		m_fCurRot -= fElapsedTime*m_pUDDRotation->s_fRotSpeed;
		if( m_fCurRot < m_pUDDRotation->s_fRotLimit_Min )
		{
			m_fCurRot = m_pUDDRotation->s_fRotLimit_Min;
			m_bPlus = true;
		}
	}
	NiMatrix3 matRot;
	matRot.MakeZRotation( m_fCurRot );
	m_Node.m_pNiNode->SetRotate( matRot );
}