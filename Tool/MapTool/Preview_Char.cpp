
#include "stdafx.h"
#include "Preview_Char.h"

void cPreviewObj_Char::Delete()
{	
	MAP_MODELRES_IT it = m_mapModelRes.begin();
	for( ; it!=m_mapModelRes.end(); ++it )
	{
		it->second->Delete();
		m_mempoolModelRes.Push( it->second );
	}
	m_mapModelRes.clear();
}

void cPreviewObj_Char::Init()
{
}

void cPreviewObj_Char::Update( CsGBSubCamera* pCamera )
{
	if( m_pCurModelView != NULL )
	{
		NiBound Bound = m_pCurModelView->GetNiNode()->GetWorldBound();
		pCamera->SetRotation( 0.0f, 0.0f );
		pCamera->SetTranslate( NiPoint3( 0.0f, 0.0f, Bound.GetCenter().z ) );
		pCamera->SetDist( Bound.GetRadius()*1.3f + 120.0f );
		pCamera->_UpdateCamera();

		m_pCurModelView->Update( g_fElapsedTime, true );
	}
}

void cPreviewObj_Char::Render( CsGBSubCamera* pCamera )
{
	if( m_pCurModelView != NULL )
		NiDrawScene( pCamera->GetCameraObj(), m_pCurModelView->GetNiNode(), *g_pCuller );
}

bool cPreviewObj_Char::SetModelView( DWORD dwModelKey )
{
	m_pCurModelView = GetModelRes( dwModelKey );
	if( m_pCurModelView == NULL )
		return false;

	m_pCurModelView->ResetAni();
	m_pCurModelView->Update( 0.0f, true );

	return true;
}

void cPreviewObj_Char::ReleaseModelView()
{
	m_pCurModelView = NULL;
}

//===================================================================================================
//
//		모델 리소스
//
//===================================================================================================

cModel_Res* cPreviewObj_Char::GetModelRes( DWORD dwModelKey )
{
	if( m_mapModelRes.find( dwModelKey ) == m_mapModelRes.end() )
	{
		if( g_pModelDataMng->IsData( dwModelKey ) == false )
			return NULL;

		char* cModelPath = g_pModelDataMng->GetData( dwModelKey )->GetInfo()->s_cKfmPath;
		cModel_Res* pModelRes =  m_mempoolModelRes.Pop();
		pModelRes->Init( NiActorManager::Create( cModelPath ), dwModelKey );		

		NiCollisionData* pCollision = NiGetCollisionData( pModelRes->GetNiNode() );
		if( pCollision == NULL )
		{
			CsMessageBoxA( MB_OK, "충돌박스 등록이 안되어 있습니다. \n%s", cModelPath );
			pModelRes->Delete();
			m_mempoolModelRes.Push( pModelRes );
			return NULL;
		}

		m_mapModelRes[ dwModelKey ] = pModelRes;
		NiNodePtr pNiColNode = NiDrawableBV::CreateWireframeBV( pCollision->GetModelSpaceABV(), NiColor( 0, 0, 1 ) );
		assert_cs( pNiColNode != NULL );
		pModelRes->GetNiNode()->AttachChild( pNiColNode );
		pNiColNode->UpdateEffects();
		pNiColNode->UpdateProperties();
		pNiColNode->Update( 0.0f );
		pNiColNode->SetAppCulled(true);
	}
	return m_mapModelRes[ dwModelKey ];
}


