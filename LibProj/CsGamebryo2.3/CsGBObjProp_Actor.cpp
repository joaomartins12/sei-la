
#include "stdafx.h"
#include "CsGBObjProp_Actor.h"

CSGBMEMPOOL_CPP( CsGBObjProp_Actor );

CsGBObjProp_Actor::CsGBObjProp_Actor():m_pActorMng(NULL)
{
	memset(m_cKey, 0, sizeof(char)*32);
}

CsGBObjProp_Actor::~CsGBObjProp_Actor()
{

}

CsGBObjProp* CsGBObjProp_Actor::GetInstance( CsGBObject* pParentObj )
{
	CsGBObjProp_Actor* pActor = CsGBObjProp_Actor::NewInstance();
	pActor->Init( pParentObj );
	return pActor;
}

CsGBObjProp* CsGBObjProp_Actor::GetInstance( NiNode* pParentNiNode )
{
	CsGBObjProp_Actor* pActor = CsGBObjProp_Actor::NewInstance();
	pActor->Init( pParentNiNode );
	return pActor;
}


void CsGBObjProp_Actor::Delete()
{
	CsGBObjProp::Delete();

	m_pActorMng = 0;
}

void CsGBObjProp_Actor::Init( CsGBObject* pParentObj )
{
	CsGBObjProp::Init( pParentObj );
}

void CsGBObjProp_Actor::Init( NiNode* pParentNiNode )
{
	CsGBObjProp::Init( pParentNiNode );
}


void CsGBObjProp_Actor::Update( float fElapsedTime, float fAniTime )
{
	if( m_pActorMng )
		m_pActorMng->Update( fAniTime );
}


void CsGBObjProp_Actor::InitActor( char* cNifPath, sUDD_ACTOR* pUDD )
{
	if( m_pActorMng )
		m_pActorMng = 0;

	char cPath[ MAX_PATH ];
	int nLen = (int)strlen( cNifPath );

	{
		strcpy_s( cPath, MAX_PATH, cNifPath );
		cPath[ nLen - 4 ] = NULL;
		strcat_s( cPath, MAX_PATH, ".kfm" );

		m_pActorMng = NiActorManager::Create( cPath );
		assert_csm( m_pActorMng, L"kfm 파일이 존재 하지 않습니다." );

		m_pActorMng->ChangeNIFRoot( m_pParentNiNode );
	}
	
	SetAnimation( 1 );
	strcpy_s( m_cKey, 32, pUDD->s_cKey );
}



