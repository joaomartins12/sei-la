
#include "stdafx.h"
#include "CsGBObj_Portal.h"


CSGBMEMPOOL_CPP( CsGBObj_Portal );


CsGBObj_Portal::CsGBObj_Portal()
{
	m_dwPortalType = 0;
	m_dwPortalTypeID = 0;
	m_dwTargetMapID = 0;

	m_SrcTarget.Reset();
	m_DestTarget.Reset();	
	m_vViewTarget = NiPoint2::ZERO;
}

CsGBObj_Portal::~CsGBObj_Portal()
{

}

void CsGBObj_Portal::Delete()
{
	m_dwPortalType = 0;
	m_dwPortalTypeID = 0;
	m_dwTargetMapID = 0;
	m_SrcTarget.Reset();
	m_DestTarget.Reset();	

	CsGBObject::Delete();
}

void CsGBObj_Portal::Init( NiNode* pNiNode, int nGetID, sINFO* pObjectInfo, sUSER_DEFINE* pUserDefine )
{
	CsGBObject::Init( pNiNode, nGetID, pObjectInfo, pUserDefine );

	if( _GetPropFactor()->IsProp( CsGBObjProp::PROP_ABV ) == false )
	{
		m_pPropFactor->InsertProp( CsGBObjProp::PROP_ABV, this );
	}
}

//======================================================================================================
//
//		Portal Data
//
//======================================================================================================

void CsGBObj_Portal::CalSrcTarget()
{
	assert_cs( m_Node.m_pNiNode != NULL );
	m_Node.m_pNiNode->Update( 0.0f );
	NiBound bound = m_Node.m_pNiNode->GetWorldBound();
	NiPoint3 ptBoundCenter = bound.GetCenter();
	m_SrcTarget.s_ptCenterPos = NiPoint2( ptBoundCenter.x, ptBoundCenter.y );

	// 여유분 2미터 상승
	m_SrcTarget.s_fRadius = bound.GetRadius() + 200.0f;
}

//======================================================================================================
//
//		Save Load
//
//======================================================================================================

UINT CsGBObj_Portal::SelfSaveSize()
{
	return	sizeof( DWORD ) +
			sizeof( DWORD ) +
			sizeof( sTARGET )*2;
}

UINT CsGBObj_Portal::GetSaveSize()
{	
	return	CsGBObject::GetSaveSize() +
			SelfSaveSize();
}

bool CsGBObj_Portal::SaveExtraData( char* pData, UINT& uiOffset )
{
	if( CsGBObject::SaveExtraData( pData, uiOffset ) == false )
		return false;

	memcpy( &pData[ uiOffset ], &m_dwPortalTypeID, sizeof( DWORD ) );
	uiOffset += sizeof( DWORD );
	memcpy( &pData[ uiOffset ], &m_dwTargetMapID, sizeof( DWORD ) );
	uiOffset += sizeof( DWORD );
	memcpy( &pData[ uiOffset ], &m_SrcTarget, sizeof( sTARGET ) );
	uiOffset += sizeof( sTARGET );
	memcpy( &pData[ uiOffset ], &m_DestTarget, sizeof( sTARGET ) );
	uiOffset += sizeof( sTARGET );

	return true;
}

void CsGBObj_Portal::LoadExtraData( char* pData, UINT& uiOffset, DWORD dwCheckPlag )
{
	CsGBObject::LoadExtraData( pData, uiOffset, dwCheckPlag );

	// 체크
	if( ( dwCheckPlag & sUSER_DEFINE::UD_PORTAL ) == 0 )
	{
		// 메세지 박스 한번만 뜨게
		static bool bMsg = false;
		if( bMsg == false )
		{
			CsMessageBoxA( MB_OK, "%s\n\n오브젝트 속성이 변경 되었습니다. 다시 세이브해주십시오.", m_Info.s_cObjectPath );
			bMsg = true;
		}
	}
	else
	{
		memcpy( &m_dwPortalTypeID, &pData[ uiOffset ], sizeof( DWORD ) );
		uiOffset += sizeof( DWORD );
		memcpy( &m_dwTargetMapID, &pData[ uiOffset ], sizeof( DWORD ) );
		uiOffset += sizeof( DWORD );
		memcpy( &m_SrcTarget, &pData[ uiOffset ], sizeof( sTARGET ) );
		uiOffset += sizeof( sTARGET );
		memcpy( &m_DestTarget, &pData[ uiOffset ], sizeof( sTARGET ) );
		uiOffset += sizeof( sTARGET );
	}
	

	if( m_pUserDefine != NULL )
	{
		m_dwPortalTypeID = PORTAL_TO_MAPPORTAL( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID, m_dwPortalTypeID );

		// 포탈의 값을 액셀에서 가져온다.
		CsMapPortal* pMapPortal = nsCsMapTable::g_pMapPortalMng->GetPortal_ByObjectID( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID, GetInfo()->s_dwUniqID );

#ifndef _GIVE
		if( pMapPortal == NULL )
		{
			// 임시 로드임
			if( GetInfo()->s_dwUniqID > 2000000000 )
				return;

			// 가져 오지 못했을경우 20미터 내외라면 포탈을 가져 온다
			CsMapPortal* pMapPortal = nsCsMapTable::g_pMapPortalMng->GetPortal_ByPos( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID, (int)GetInfo()->s_trObject.m_Translate.x, (int)GetInfo()->s_trObject.m_Translate.y );
			if( pMapPortal != NULL )
			{
				CsMessageBox( MB_OK, _T( "포탈 오브젝트의 ID가 일치 하지 않습니다. 맵제작자 분들이 다시 세이브 해서 맵데이터 올려주세요." ) );
			}
		}
#endif

		if( pMapPortal != NULL )
		{
			m_dwPortalTypeID = pMapPortal->GetInfo()->s_dwPortalID; 
			m_dwPortalType = pMapPortal->GetInfo()->s_dwPortalType; 
			m_dwTargetMapID = pMapPortal->GetInfo()->s_dwDestMapID;
			m_SrcTarget.s_ptCenterPos = NiPoint2(	(float)pMapPortal->GetInfo()->s_nSrcTargetX,
				(float)pMapPortal->GetInfo()->s_nSrcTargetY	);
			m_SrcTarget.s_fRadius = (float)pMapPortal->GetInfo()->s_nSrcRadius;
			m_Node.m_pNiNode->SetTranslate( NiPoint3(	(float)pMapPortal->GetInfo()->s_nSrcTargetX,
				(float)pMapPortal->GetInfo()->s_nSrcTargetY,
				m_Node.m_pNiNode->GetTranslate().z ) );
			m_DestTarget.s_ptCenterPos = NiPoint2(	(float)pMapPortal->GetInfo()->s_nDestTargetX,
				(float)pMapPortal->GetInfo()->s_nDestTargetY	);
			m_DestTarget.s_fRadius = (float)pMapPortal->GetInfo()->s_nDestRadius;
			m_Node.m_pNiNode->Update( 0.0f, false );

			m_vViewTarget.x = (float)pMapPortal->GetInfo()->s_nViewTargetX;
			m_vViewTarget.y = (float)pMapPortal->GetInfo()->s_nViewTargetY;
		}
		else
		{
			// 임시 로드임
			if( GetInfo()->s_dwUniqID > 2000000000 )
				return;

			CsMessageBox( MB_OK, _T( "근처에 존재 하는 포탈이 없습니다." ) );
			m_dwPortalType = 0;
			m_dwPortalTypeID = 0;
		}
	}	
}