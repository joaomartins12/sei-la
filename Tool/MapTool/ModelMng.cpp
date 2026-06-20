
#include "stdafx.h"
#include "ModelMng.h"

cModelMng		g_ModelMng;


cModelMng::cModelMng()
{
	m_pModelNpc = NULL;
	m_pSelectNpc = NULL;
}

void cModelMng::Delete()
{
	_DeleteNpc();
	m_mapRegistNpc.clear();	
}

void cModelMng::Init()
{
}

void cModelMng::PostViewReset()
{	
}

void cModelMng::PreViewDelete()
{
	_DeleteNpc();
}

void cModelMng::HideModelTab()
{
	//g_ToolNiNodeMng.GetAxisRotation()->ShowAllAxis();
	SelectNpcToResistNpc();
	DeleteModelNpc();
}

void cModelMng::Update()
{
	_UpdateNpc();
}

void cModelMng::Render()
{
	_RenderNpc();
}

void cModelMng::PreSave( const char* cFile, bool bAutoSave )
{
	if( bAutoSave == true )
		return;

	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	// 기존 그룹 제거
	nsCsMapTable::g_pMapNpcMng->DeleteGroup( dwMapID );

	CsMapNpc* pMapNpc = NULL;
	CsMapNpc::sINFO info;

	CNpc* pNpc = NULL;
	std::map< UINT, CNpc* >::iterator it = m_mapRegistNpc.begin();
	for( ; it != m_mapRegistNpc.end(); ++it )
	{
		pNpc = it->second;
		SAFE_POINTER_CON( pNpc );

		if( nsCsMapTable::g_pMapNpcMng->IsNpc( pNpc->GetFTID() ) )
			continue;

		info.Init( dwMapID, pNpc->GetFTID() );
		info.s_nInitPosX = (int)pNpc->GetPos().x;
		info.s_nInitPosY = (int)pNpc->GetPos().y;
		info.s_fRotation = pNpc->GetCurRot();

		nsCsMapTable::g_pMapNpcMng->AddNpc( &info );
	}
}

//===================================================================================================
//
//		모델 뷰
//
//===================================================================================================

void cModelMng::SetModelView( DWORD dwNpcID )
{	
	SelectNpcToResistNpc();
	DeleteModelNpc();

	// 배치된 몬스터 인지 판단
	if( IsResistNpc( dwNpcID ) == true )
	{
		_SetSelectNpc( dwNpcID%90000 );
	}
	else
	{
		AddModelNpc( dwNpcID, NULL );
	}
}

//===================================================================================================
//
//		NPC
//
//===================================================================================================

void cModelMng::_DeleteNpc()
{
	DeleteModelNpc();
	DeleteSelectNpc();
	DeleteResistNpc();
}

void cModelMng::DeleteResistNpc()
{
	CNpc* pNpc = NULL;
	std::map< UINT, CNpc* >::iterator it = m_mapRegistNpc.begin();
	for( ; it != m_mapRegistNpc.end(); )
	{
		pNpc = it->second;
		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
		pUnit->s_pLoadedObject = pNpc;
		m_mapRegistNpc.erase( it++ );
		g_pThread->DeleteChar( pUnit );
	}	
}

void cModelMng::_DeleteResistNpc( DWORD dwNpcID )
{
	DWORD dwID = dwNpcID % 90000;

	std::map< UINT, CNpc* >::iterator it = m_mapRegistNpc.find( dwID );
	if( it == m_mapRegistNpc.end() )
		return;

	CNpc* pNpc = it->second;
	sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
	pUnit->s_pLoadedObject = pNpc;
	g_pThread->DeleteChar( pUnit );

	m_mapRegistNpc.erase( it );
}

CNpc* cModelMng::_AddResistNpc( DWORD dwFTID, NiPoint3 ptPos, float fRot, DWORD dwAniID, bool bResistNpc )
{
	sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::KFM_File );
	pUnit->s_Prop.s_dwPropPlag = PR_EFFECT_ENABLE | PR_ANI_ENABLE | PR_PATH_NORMAL;
	pUnit->s_Prop.s_dwOptionPlag = OP_ABVWIRE | OP_FOG | OP_STAT;
	pUnit->s_Prop.s_eInsertGeomType = CGeometry::CharOutLine;

	UINT nIDX = dwFTID%90000;
	CNpc* pNpc = CNpc::NewInstance();
	pNpc->Init( nIDX, dwFTID, ptPos, fRot, &pUnit->s_Prop );
	pNpc->GetProp_Alpha()->SetUseDistAlpha( false );
	pNpc->SetAnimation( dwAniID );
	strcpy_s( pUnit->s_cFilePath, OBJECT_PATH_LEN, g_pModelDataMng->GetData( pNpc->GetModelID() )->GetInfo()->s_cKfmPath );
	pUnit->s_pLoadedObject = pNpc;

	if( bResistNpc == true )
	{
		pNpc->GetProp_Alpha()->SetCurAlphaValue( 1.0f );
		std::map< UINT, CNpc* >::iterator it = m_mapRegistNpc.find( pNpc->GetIDX() );
		if( it == m_mapRegistNpc.end() )
			m_mapRegistNpc.insert( std::make_pair( pNpc->GetIDX(), pNpc ) );
	}

	g_pThread->LoadChar( pUnit );

	pNpc->Update( 0.0f );
	nsCsGBTerrain::g_pCurRoot->GetLightMng()->ApplyChar( pNpc->GetCsNode()->m_pNiNode );
	pNpc->GetCsNode()->m_pNiNode->UpdateEffects();
	pNpc->GetCsNode()->ActiveShader();
	return pNpc;
}

CNpc* cModelMng::_AddResistNpc( DWORD dwFTID, NiPoint2 ptInitPos, float fRot, DWORD dwAniID /*=ANI::IDLE_NORMAL*/ )
{
	if( nsCsFileTable::g_pNpcMng->IsNpc( dwFTID ) == false )
		return NULL;

	if( nsCsGBTerrain::g_pCurRoot->IsInTerrain( ptInitPos.x, ptInitPos.y ) == false )
	{
		static bool bMsg = false;
		if( bMsg == false )
		{			
			CsMessageBox( MB_OK, L"지형의 크기를 벋어난 NPC 가 존재합니다. NpcID = %d", dwFTID );
			bMsg = true;
		}
		return NULL;
	}
	
	DWORD dwModelID = nsCsFileTable::g_pNpcMng->GetNpc( dwFTID )->GetInfo()->s_dwModelID;

	if( g_pModelDataMng->IsData( dwModelID ) == false )
	{
		static bool bMsg = false;
		if( bMsg == false )
		{			
			CsMessageBox( MB_OK, L"케릭터 툴에 등록 되지 않은 Npc입니다. NpcID = %d, ModelID = %d", dwFTID, dwModelID );
			bMsg = true;
		}
		return NULL;
	}

	NiPoint3 ptPos = NiPoint3( ptInitPos.x, ptInitPos.y, nsCsGBTerrain::g_pCurRoot->GetHeight( ptInitPos ) );

	return _AddResistNpc( dwFTID, ptPos, fRot, dwAniID, true );
}

void cModelMng::ResetResistMapNpc()
{
	assert_cs( nsCsGBTerrain::g_pCurRoot != NULL );

	DeleteModelNpc();
	DeleteSelectNpc();
	DeleteResistNpc();

	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	if( dwMapID == 0 )
		return;

	//MAP상에 존재 하는 엔피씨 배치
	CsMapNpc::sINFO* pInfo = NULL;
	CNpc* pNpc = NULL;
	MAP_MAPNPC* pmapNpc = nsCsMapTable::g_pMapNpcMng->GetNpcMap();
	MAP_MAPNPC_IT it = pmapNpc->begin();
	for( ; it!=pmapNpc->end(); ++it )
	{
		pInfo = it->second->GetInfo();
		if( pInfo->s_dwMapID != dwMapID )
			continue;

		NiPoint2 ptInitPos = NiPoint2( (float)pInfo->s_nInitPosX, (float)pInfo->s_nInitPosY );
		if( ptInitPos.x > nsCsGBTerrain::g_pCurRoot->GetMapSizeWidth() )
			ptInitPos.x = nsCsGBTerrain::g_pCurRoot->GetMapSizeWidth();
		if( ptInitPos.y > nsCsGBTerrain::g_pCurRoot->GetMapSizeHeight() )
			ptInitPos.y = nsCsGBTerrain::g_pCurRoot->GetMapSizeHeight();

		_AddResistNpc( pInfo->s_dwNpcID, ptInitPos, pInfo->s_fRotation, ANI::IDLE_NORMAL );
	}
}

void cModelMng::DeleteSelectNpc()
{
	if( m_pSelectNpc == NULL )
		return;

	sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
	pUnit->s_pLoadedObject = m_pSelectNpc;
	g_pThread->DeleteChar( pUnit );

	m_pSelectNpc = NULL;

	g_ToolNiNodeMng.GetAxisRotation()->ReleaseVisible();
}

void cModelMng::_SetSelectNpc( UINT nIDX )
{
	SelectNpcToResistNpc();

	std::map< UINT, CNpc* >::iterator it = m_mapRegistNpc.find( nIDX );
	if( it == m_mapRegistNpc.end() )
		return;

	m_pSelectNpc = it->second;
	m_pSelectNpc->GetProp_Alpha()->SetCurAlphaValue( CsC_AlphaProp::AT_KEEP, 0.5f );
	m_mapRegistNpc.erase( it );

	g_ToolNiNodeMng.GetAxisRotation()->ReleaseVisible();
	g_ToolNiNodeMng.GetAxisRotation()->InsertVisible( m_pSelectNpc->GetCsNode()->m_pNiNode, cAxisRotation::AT_NPC );
}

void cModelMng::DeleteModelNpc()
{
	if( m_pModelNpc == NULL )
		return;

	sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
	pUnit->s_pLoadedObject = m_pModelNpc;
	g_pThread->DeleteChar( pUnit );

	m_pModelNpc = NULL;
	m_bRenderModelNpc = false;
}

void cModelMng::AddModelNpc( DWORD dwFTID, CNpc* pSrcNpc )
{
	DeleteModelNpc();

	if( pSrcNpc == NULL )
	{
		m_pModelNpc = _AddResistNpc( dwFTID, NiPoint3::ZERO, 0.0f, ANI::IDLE_NORMAL, false );		
	}
	else
	{
		m_pModelNpc = m_pSelectNpc;		
	}
	m_pModelNpc->GetProp_Alpha()->SetCurAlphaValue( 0.5f );
}

void cModelMng::_MoveModelNpc( CPoint pt )
{
	if( m_pModelNpc == NULL )
		return;

	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
	if( pRecord == NULL )
	{
		m_bRenderModelNpc = false;
		return;
	}
	m_bRenderModelNpc = true;

	NiPoint3 ptIntersect = pRecord->GetIntersection();
	NiTransform kTr = 	m_pModelNpc->GetCsNode()->m_pNiNode->GetWorldTransform();
	kTr.m_Translate = ptIntersect;
	m_pModelNpc->GetCsNode()->m_pNiNode->SetLocalTransform(kTr);
	m_pModelNpc->GetCsNode()->m_pNiNode->Update(0.0f);
	//m_pModelNpc->SetPos( NiPoint2(ptIntersect.x, ptIntersect.y) );

}

void cModelMng::_PickNpcModel( CPoint pt )
{
	CsGBPick pick( false );
	NiPoint3 origin, dir;
	CAMERA_ST.WindowPointToRay( pt.x, pt.y, origin, dir );

	// 레지스트 모델중에서
	CNpc* pPickNpc = NULL;
	float fLength = FLT_MAX;
	std::map< UINT, CNpc* >::iterator it = m_mapRegistNpc.begin();
	for( ; it != m_mapRegistNpc.end(); ++it )
	{
		CNpc* pNpc = it->second;
		SAFE_POINTER_CON( pNpc );
		if( pNpc->PickObject( &pick, &origin, &dir, fLength ) == true )
		{
			pPickNpc = pNpc;
		}
	}

	if( pPickNpc == NULL )
		return;

	// 리스트에서 선택	
	GET_MV_MAINTAB_DLG->_GetDlgModel()->_SetMapNpcList( pPickNpc->GetFTID() );
	_SetSelectNpc( pPickNpc->GetIDX() );
}

void cModelMng::ModelNpcToSelectNpc()
{
	if( m_pModelNpc == NULL )
		return;

	m_pSelectNpc = m_pModelNpc;
	m_pModelNpc = NULL;
	m_bRenderModelNpc = false;

	GET_MV_MAINTAB_DLG->_GetDlgModel()->_ResistNpcList( m_pSelectNpc->GetFTID(), (int)m_pSelectNpc->GetPos().x, (int)m_pSelectNpc->GetPos().y );	

	g_ToolNiNodeMng.GetAxisRotation()->ReleaseVisible();
	g_ToolNiNodeMng.GetAxisRotation()->InsertVisible( m_pSelectNpc->GetCsNode()->m_pNiNode, cAxisRotation::AT_NPC );
}

void cModelMng::SelectNpcToResistNpc()
{
	if( m_pSelectNpc == NULL )
		return;

	NiMatrix3 mat = m_pSelectNpc->GetCsNode()->m_pNiNode->GetWorldRotate();
	float x, y, z;
	mat.ToEulerAnglesZXY( z, x, y );

	_AddResistNpc( m_pSelectNpc->GetFTID(), m_pSelectNpc->GetCsNode()->m_pNiNode->GetWorldTranslate(), z, m_pSelectNpc->GetProp_Animation()->GetAnimationID(), true );
	DeleteSelectNpc();
}

void cModelMng::SelectNpcToModelNpc()
{
	if( m_pSelectNpc == NULL )
		return;

	GET_MV_MAINTAB_DLG->_GetDlgModel()->_UnresistNpcList( m_pSelectNpc->GetFTID() );
	AddModelNpc( m_pSelectNpc->GetFTID(), m_pSelectNpc );
	m_pSelectNpc = NULL;

	g_ToolNiNodeMng.GetAxisRotation()->ReleaseVisible();
}

void cModelMng::SelectNpc_ApplyTransform( NiTransform tr )
{
	if( m_pSelectNpc == NULL )
		return;

	NiTransform trResult = tr;
	trResult.m_fScale = m_pSelectNpc->GetCsNode()->m_pNiNode->GetScale();
	m_pSelectNpc->GetCsNode()->m_pNiNode->SetLocalTransform( trResult );
}

void cModelMng::RotationModelNpcOnKey( bool bPlus )
{
	float fAngle;
	if( bPlus )	fAngle = NI_PI*0.25f;
	else		fAngle = -NI_PI*0.25f;
	NiTransform tr;
	tr.MakeIdentity();
	tr.m_Rotate.MakeZRotation( fAngle );

	if( m_pModelNpc != NULL )
	{
		NiTransform trDest = m_pModelNpc->GetCsNode()->m_pNiNode->GetLocalTransform();
		NiTransform trResult = trDest*tr;
		m_pModelNpc->GetCsNode()->m_pNiNode->SetLocalTransform( trResult );
	}
	else
	{
		assert_cs( m_pSelectNpc != NULL );
		NiTransform trDest = m_pSelectNpc->GetCsNode()->m_pNiNode->GetLocalTransform();
		NiTransform trResult = trDest*tr;
		m_pSelectNpc->GetCsNode()->m_pNiNode->SetLocalTransform( trResult );

		GET_MV_OBJECTCONTROL_DLG->_ResetValue( trResult );
	}
}

bool cModelMng::IsResistNpc( DWORD dwNpcID )
{
	UINT nIDX = dwNpcID%90000;
	std::map< UINT, CNpc* >::iterator it = m_mapRegistNpc.find( nIDX );
	if( it == m_mapRegistNpc.end() )
		return false;
	return true;
}

void cModelMng::_UpdateNpc()
{
	if( ( IsClientRect() == true )&&( m_bRenderModelNpc == true ) )
	{
		m_pModelNpc->Update( g_fElapsedTime );
	}

	if( m_pSelectNpc )
	{
		m_pSelectNpc->Update( g_fElapsedTime );
	}

	std::map< UINT, CNpc* >::iterator it = m_mapRegistNpc.begin();
	for( ; it != m_mapRegistNpc.end(); ++it )
	{
		if( it->second )
			it->second->Update( g_fElapsedTime );
	}
}

void cModelMng::_RenderNpc()
{
	NiCamera* pCamera = CAMERA_ST.GetCameraObj();
	g_pRenderer->SetCameraData( pCamera );

	if( ( IsClientRect() == true )&&( m_bRenderModelNpc == true ) )
	{
		m_pModelNpc->Render();
	}

	if( m_pSelectNpc )
	{
		m_pSelectNpc->Render();
	}

	std::map< UINT, CNpc* >::iterator it = m_mapRegistNpc.begin();
	for( ; it != m_mapRegistNpc.end(); ++it )
	{
		if( it->second )
			it->second->Render();
	}
}

void cModelMng::_LButtonDown_Npc( CPoint pt )
{
	if( GetModelNpc() != NULL )
	{
		_MoveModelNpc( pt );
		ModelNpcToSelectNpc();
	}
	else
	{
		_PickNpcModel( pt );
	}
}

void cModelMng::_MouseMove_Npc( CPoint pt )
{
	_MoveModelNpc( pt );
}

//===================================================================================================
//
//		SAVE_LOAD
//
//===================================================================================================


//===================================================================================================
//
//		Mouse
//
//===================================================================================================
void cModelMng::LButtonDown( CPoint pt )
{
	if( GET_MV_MAINTAB_DLG->_GetSelTabIndex() != _DlgMainTab::TT_MODEL )
		return;

	_LButtonDown_Npc( pt );
}

void cModelMng::MouseMove( CPoint pt )
{
	if( GET_MV_MAINTAB_DLG->_GetSelTabIndex() != _DlgMainTab::TT_MODEL )
		return;

	_MouseMove_Npc( pt );
}

void cModelMng::ApplyLight( NiLight* pLight )
{
	CNpc* pNpc;
	std::map< UINT, CNpc* >::iterator it = m_mapRegistNpc.begin();
	for( ; it != m_mapRegistNpc.end(); ++it )
	{
		pNpc = it->second;
		SAFE_POINTER_CON( pNpc );
		CsNodeObj* pNode = pNpc->GetCsNode();
		SAFE_POINTER_CON( pNode );
		NiNodePtr pNodePtr = pNode->m_pNiNode;
		SAFE_POINTER_CON( pNodePtr );
		pNodePtr->AttachEffect( pLight );
		pNode->ActiveShader();
	}
}

void cModelMng::RemoveLight( NiLight* pLight )
{
	CNpc* pNpc;
	std::map< UINT, CNpc* >::iterator it = m_mapRegistNpc.begin();
	for( ; it != m_mapRegistNpc.end(); ++it )
	{
		pNpc = it->second;
		SAFE_POINTER_CON( pNpc );
		CsNodeObj* pNode = pNpc->GetCsNode();
		SAFE_POINTER_CON( pNode );
		NiNodePtr pNodePtr = pNode->m_pNiNode;
		SAFE_POINTER_CON( pNodePtr );
		pNodePtr->DetachEffect( pLight );
		pNode->ActiveShader();
	}
}