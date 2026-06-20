
#include "stdafx.h"
#include "MoveMng.h"

void cMoveMng::sSIGN::SetTranslate( int x, int y )
{
	NiPoint3 v( (float)x, (float)y, 0 );
	v.z = nsCsGBTerrain::g_pCurRoot->GetHeight( v );
	SetTranslate( v );
}

void cMoveMng::sSIGN::SetTranslate( NiPoint3 vPos )
{
	vPos.z += 200;
	s_bView = true;
	s_Node.m_pNiNode->SetTranslate( vPos );	
}

void cMoveMng::sSIGN::Update()
{
	if( s_bView == false )
		return;

	NiPoint3 ptWorld = s_Node.m_pNiNode->GetTranslate();
	// 카메라와의 거리 측정
	float fScale = NiSqrt( ( ptWorld - CAMERA_ST.GetWorldTranslate() ).Length() );
	s_Node.m_pNiNode->SetScale( fScale*0.01f );	
	s_Node.m_pNiNode->Update( 0.0f );
}

void cMoveMng::sSIGN::UpdateRender()
{
	if( s_bView == false )
		return;

	Update();
	s_Node.RenderAbsolute();
}


cMoveMng::cMoveMng()
{
	m_pMoveObject = NULL;
}

void cMoveMng::Delete()
{
	m_PickSign.Delete();
	m_CheckPointSign.Delete();
	m_ObjectCurPosSign.Delete();

	DeleteObject();
}

void cMoveMng::DeleteObject()
{
	if( m_pMoveObject == NULL )
		return;

	sTCUnit* pDelUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
	pDelUnit->s_pLoadedObject = m_pMoveObject;
	g_pThread->DeleteChar( pDelUnit );
	m_pMoveObject = NULL;
}

void cMoveMng::Init()
{
	{
		NiStream kStream;
		if( !kStream.Load( "ToolData\\ObjectMove\\SignRed.nif" ) )
		{
			CsMessageBoxA( MB_OK, "경로에서 오브젝트를 찾지 못했습니다." );
			return;
		}

		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
		m_PickSign.s_Node.SetNiObject( pNode, CGeometry::Normal );
	}

	{
		NiStream kStream;
		if( !kStream.Load( "ToolData\\ObjectMove\\SignGreen.nif" ) )
		{
			CsMessageBoxA( MB_OK, "경로에서 오브젝트를 찾지 못했습니다." );
			return;
		}

		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);		
		m_CheckPointSign.s_Node.SetNiObject( pNode, CGeometry::Normal );
	}

	{
		NiStream kStream;
		if( !kStream.Load( "ToolData\\ObjectMove\\SignBlue.nif" ) )
		{
			CsMessageBoxA( MB_OK, "경로에서 오브젝트를 찾지 못했습니다." );
			return;
		}

		NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);		
		m_ObjectCurPosSign.s_Node.SetNiObject( pNode, CGeometry::Normal );
	}
}

CsC_AvObject* cMoveMng::LoadNpc( DWORD dwNpcID )
{
	DeleteObject();

	// 로드시 필요한 초기화
	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File );
	pUint->s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE | PR_MOVECON_ENABLE;
	pUint->s_Prop.s_dwOptionPlag = _OP_ALL;
	pUint->s_Prop.s_eInsertGeomType = CGeometry::CharOutLine;

	// 객체 할당
	DWORD dwModelID = nsCsFileTable::g_pNpcMng->GetNpc( dwNpcID )->GetInfo()->s_dwModelID;
	m_pMoveObject = NiNew cMoveNpc;
	m_pMoveObject->Init( 0, dwNpcID, NiPoint3::ZERO, 0, &pUint->s_Prop );
	m_pMoveObject->GetProp_Alpha()->SetUseDistAlpha( false );

	// 테이블 연결
	CsMoveObject* pFTMO = nsCsFileTable::g_pMoveObjectMng->GetObject( CsMoveObject::MO_NPC, dwNpcID );
	m_pMoveObject->GetProp_MoveControl()->SetFTMoveObject( pFTMO );

	// 로드 설정
	pUint->s_pLoadedObject = m_pMoveObject;	 
	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, g_pModelDataMng->GetData( dwModelID )->GetInfo()->s_cKfmPath );

	// 쓰레드 로드
	g_pThread->LoadChar( pUint );
	return m_pMoveObject;

}

CsC_AvObject* cMoveMng::LoadMonster( DWORD dwMonID )
{
	DeleteObject();

	// 로드시 필요한 초기화
	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File );
	pUint->s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE | PR_MOVECON_ENABLE;
	pUint->s_Prop.s_dwOptionPlag = _OP_ALL;
	pUint->s_Prop.s_eInsertGeomType = CGeometry::CharOutLine;

	// 객체 할당
	DWORD dwModelID = nsCsFileTable::g_pMonsterMng->GetMonster( dwMonID )->GetInfo()->s_dwModelID;	
	m_pMoveObject = NiNew cMoveMonster;
	m_pMoveObject->Init( 0, dwMonID, NiPoint3::ZERO, 0, &pUint->s_Prop );
	m_pMoveObject->GetProp_Alpha()->SetUseDistAlpha( false );

	// 테이블 연결
	CsMoveObject* pFTMO = nsCsFileTable::g_pMoveObjectMng->GetObject( CsMoveObject::MO_MONSTER, dwMonID );
	m_pMoveObject->GetProp_MoveControl()->SetFTMoveObject( pFTMO );

	// 로드 설정
	pUint->s_pLoadedObject = m_pMoveObject;	 
	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, g_pModelDataMng->GetData( dwModelID )->GetInfo()->s_cKfmPath );

	// 쓰레드 로드
	g_pThread->LoadChar( pUint );
	return m_pMoveObject;

}

void cMoveMng::Update()
{
	if( m_pMoveObject == NULL )
		return;

	float fDeltaTime = 0.0;
	if( g_pWinLBottom->_IsPlay() == true )
	{
		fDeltaTime = g_fDeltaTime*g_pWinLBottom->_GetPreViewSpeed();
	}
	else
	{
		m_pMoveObject->GetProp_Alpha()->Update( g_fDeltaTime );
	}

	m_pMoveObject->Update( fDeltaTime );	
}

void cMoveMng::Render()
{
	m_PickSign.UpdateRender();

	if( m_pMoveObject == NULL )
		return;

	// 체크 포인트 랜더링
	CsMoveObject::eKIND kind;
	switch( m_pMoveObject->GetLeafRTTI() )
	{
	case RTTI_MOVE_NPC:
		kind = CsMoveObject::MO_NPC;
		break;
	case RTTI_MOVE_MON:
		kind = CsMoveObject::MO_MONSTER;
		break;
	default:
		assert_cs( false );
	}

	CsMoveObject* pMOFT = nsCsFileTable::g_pMoveObjectMng->GetObject( kind, m_pMoveObject->GetFTID() );
	std::list< CsMoveObject::sACT_BASE* >* pList = pMOFT->GetActList();
	std::list< CsMoveObject::sACT_BASE* >::iterator it = pList->begin();
	std::list< CsMoveObject::sACT_BASE* >::iterator itEnd = pList->end();
	for( ; it!=itEnd; ++it )
	{
		sSIGN* pSign = ( g_pWinLBottom->GetCurEditAct() == (*it) ) ? &m_PickSign : &m_CheckPointSign;

		switch( (*it)->GetType() )
		{
		case CsMoveObject::AT_START:
			{
				CsMoveObject::sACT_START* pAct = (CsMoveObject::sACT_START*)(*it);
				pSign->SetTranslate( pAct->s_PosX, pAct->s_PosY );
				pSign->UpdateRender();
			}
			break;
		case CsMoveObject::AT_MOVE:
			{
				CsMoveObject::sACT_MOVE* pAct = (CsMoveObject::sACT_MOVE*)(*it);
				pSign->SetTranslate( pAct->s_PosX, pAct->s_PosY );
				pSign->UpdateRender();
			}
			break;
		case CsMoveObject::AT_WAIT:
		case CsMoveObject::AT_GOTO_FIRST_LOOP:
			continue;
		default:
			assert_cs( false );
		}
	}

	m_pMoveObject->Render();
	m_ObjectCurPosSign.SetTranslate( (int)m_pMoveObject->GetPos().x, (int)m_pMoveObject->GetPos().y );
	m_ObjectCurPosSign.UpdateRender();
}

void cMoveMng::OnLButtonDown( CsPoint pt )
{
	if( m_pMoveObject == NULL )
		return;

	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
	if( pRecord == NULL )
		return;

	// 좌표 컨트롤에 입력
	CsPoint ptIntersect = CsPoint( (int)pRecord->GetIntersection().x, (int)pRecord->GetIntersection().y );
	if( g_pWinLBottom->_SM_Set_Pos( ptIntersect ) )
	{
		Set_PickPos( true, ptIntersect );
	}
	else
	{
		Set_PickPos( false, ptIntersect );
	}

	// 현재 오브젝트에 반영해야하는가
}

void cMoveMng::Set_PickPos( bool bEnable, CsPoint pt )
{
	assert_cs( nsCsGBTerrain::g_pCurRoot );
	NiPoint3 pos = NiPoint3( (float)pt.x, (float)pt.y, 0 );
	pos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( pos );

	if( bEnable )
		m_PickSign.SetTranslate( pos );
	else
		m_PickSign.s_bView = false;
}