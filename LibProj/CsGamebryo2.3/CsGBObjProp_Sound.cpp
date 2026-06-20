
#include "stdafx.h"
#include "CsGBObjProp_Sound.h"

CSGBMEMPOOL_CPP( CsGBObjProp_Sound );

CsGBObjProp_Sound::CsGBObjProp_Sound():m_fAniTime(0.0f),m_fLoopTime(0.0f),m_fMinDist(0.0f),m_fMaxDist(0.0f)
{
	memset( m_cPath, 0, sizeof(char)*MAX_FILENAME );
}

CsGBObjProp_Sound::~CsGBObjProp_Sound()
{
}

CsGBObjProp* CsGBObjProp_Sound::GetInstance( CsGBObject* pParentObj )
{
	CsGBObjProp_Sound* pProp = CsGBObjProp_Sound::NewInstance();
	pProp->Init( pParentObj );
	return pProp;
}

CsGBObjProp* CsGBObjProp_Sound::GetInstance( NiNode* pParentNiNode )
{
	CsGBObjProp_Sound* pProp = CsGBObjProp_Sound::NewInstance();
	pProp->Init( pParentNiNode );
	return pProp;
}


void CsGBObjProp_Sound::Delete()
{
	CsGBObjProp::Delete();	
	memset( m_cPath, 0, sizeof(char) *MAX_FILENAME );
	m_fAniTime = 0.0f;
	m_fLoopTime = 0.0f;
	m_fMinDist = 0.0f;
	m_fMaxDist = 0.0f;
}

void CsGBObjProp_Sound::Init( CsGBObject* pParentObj )
{
	CsGBObjProp::Init( pParentObj );

	m_fAniTime = 0.0f;
	sUDD_Sound* pUDD = (sUDD_Sound*)pParentObj->GetUserDefine()->GetData( sUDD_Sound::Type() );
	assert_cs( pUDD != NULL );
	m_fLoopTime = pUDD->s_fLoopTime;
	m_fMinDist = pUDD->s_fMinDist;
	m_fMaxDist = pUDD->s_fMaxDist;
	strcpy_s( m_cPath, MAX_FILENAME, pUDD->s_cFileName );
}

void CsGBObjProp_Sound::Init( NiNode* pParentNiNode )
{
	assert_cs( false );
	CsGBObjProp::Init( pParentNiNode );
}

void CsGBObjProp_Sound::Update( float fElapsedTime, float fAniTime )
{
	if( nsCsGBTerrain::g_pCsSoundMng == NULL )
		return;

	if( ( m_fAniTime -= fElapsedTime ) < 0.0f )
	{
		m_fAniTime = m_fLoopTime;
		nsCsGBTerrain::g_pCsSoundMng->PlaySound( m_cPath, m_pParentNiNode->GetTranslate(), m_fMinDist, m_fMaxDist );
	}
}