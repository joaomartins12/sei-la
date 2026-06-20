

#include "stdafx.h"
#include "CsGBAttachCamera.h"

void CsGBAttachCamera::Delete()
{
	if( m_pmapInfo != NULL )
	{
		sINFO* pInfo = NULL;
		NiTMapIterator itObj = m_pmapInfo->GetFirstPos();
		while( itObj != NULL )
		{
			const char* pcName;
			m_pmapInfo->GetNext( itObj, pcName, pInfo );
			pInfo->Delete();
			SAFE_NIDELETE( pInfo );
		}
		m_pmapInfo->RemoveAll();
		SAFE_NIDELETE( m_pmapInfo );
	}

	m_pCurApplyCamera = NULL;
}

void CsGBAttachCamera::Init()
{
	m_pmapInfo = NiNew NiTStringPointerMap< sINFO* >;
	m_pCurApplyCamera = NULL;
}

void CsGBAttachCamera::AttachCamera( char* cName, bool bOwner )
{
	sINFO* pSrcInfo;
	bool bFind = m_pmapInfo->GetAt( cName, pSrcInfo );
	if( bFind == false )
	{
		char cPath[ MAX_PATH ];
		sprintf_s( cPath, MAX_PATH, "Data\\Camera\\%s", cName );

		NiStream stream;
		if( stream.Load( cPath ) == false )
		{			
			CsMessageBoxA( MB_OK, "\n카메라 로드 실패\n\n\nObject Path = %s\n", cPath );
			return;
		}

		pSrcInfo = NiNew sINFO;
		
		// 카메라 오브젝트
		NiNode* pRootNode = (NiNode*)stream.GetObjectAt( 0 );
		UINT nCount = pRootNode->GetArrayCount();
		NiNode* pNode;		
		for( UINT i=0; i<nCount; ++i )
		{
			pNode = (NiNode*)pRootNode->GetAt( i );

			if( NiIsKindOf( NiNode, pNode ) == false )
				continue;

			assert_cs( NiIsKindOf( NiNode, pNode ) == true );
			if( strcmp( pNode->GetName(), "Camera01" ) == 0 )
			{
				pSrcInfo->s_pCamera = pNode;
				break;
			}
		}		
		assert_cs( pSrcInfo->s_pCamera != NULL );
		// 애니메이션 타임
		nsCSGBFUNC::EndAnimationTime( pSrcInfo->s_fEndTime, pSrcInfo->s_pCamera );
		pSrcInfo->s_pCamera->Update( 0, true );
		pSrcInfo->s_bOwner = bOwner;
		NiTransform tr = pSrcInfo->s_pCamera->GetAt( 0 )->GetWorldTransform();
		tr.Invert( pSrcInfo->s_InverseTR );

		m_pmapInfo->SetAt( cName, pSrcInfo );
	}

	m_pCurApplyCamera = pSrcInfo;
	m_pCurApplyCamera->Reset();	
}

void CsGBAttachCamera::Update( float fDeltaTime, NiCamera* pSrcCamera )
{
	if( m_pCurApplyCamera == NULL )
		return;

	if( (m_pCurApplyCamera->s_fAniTime += fDeltaTime) >= m_pCurApplyCamera->s_fEndTime )
	{
		m_pCurApplyCamera->Reset();
		m_pCurApplyCamera = NULL;
		return;
	}

	assert_cs( NiIsKindOf( NiCamera, m_pCurApplyCamera->s_pCamera->GetAt( 0 ) ) == true );
#ifdef CRASHDUMP_0305			// chu8820 - 카메라 로드 실패( C002 )
	if( m_pCurApplyCamera->s_pCamera == NULL )
	{
		DUMPLOG( " C002 - 1 " );
		return;
	}
	else if( m_pCurApplyCamera->s_pCamera->GetAt(0) == NULL )
	{
		DUMPLOG( " C002 - 2 " );
		return;
	}
#endif
	m_pCurApplyCamera->s_pCamera->Update( m_pCurApplyCamera->s_fAniTime );
	
	if( pSrcCamera )
	{
		NiCamera* pCamera =	(NiCamera*)m_pCurApplyCamera->s_pCamera->GetAt( 0 );
		float fScale = sqrt( CAMERA_ST.GetDist() )*0.1f;
		NiTransform trMul = pCamera->GetWorldTransform()*m_pCurApplyCamera->s_InverseTR;
		NiTransform* p = ( (NiTransform*)&pCamera->GetWorldTransform() );
		p->m_Translate = ( trMul.m_Translate*fScale + pSrcCamera->GetWorldTransform().m_Translate );
		p->m_Rotate = pSrcCamera->GetWorldTransform().m_Rotate;

		pCamera->SetViewFrustum( pSrcCamera->GetViewFrustum() );
		pCamera->SetViewPort( pSrcCamera->GetViewPort() );
	}
}