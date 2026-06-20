
#include "stdafx.h"
#include "NpcMng.h"

CNpcMng*	g_pNpcMng = NULL;

CsC_AvObject* CNpcMng::CALLBACK_GetNpcObject( DWORD dwId )
{
	CsC_AvObject* pNpcObject = g_pNpcMng->GetNpc( dwId );	
	return pNpcObject;
}

void CNpcMng::DeleteChar( CsC_AvObject* pObject )
{
	sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
	pUnit->s_pLoadedObject = (CsC_Object*)pObject;	

	g_pThread->DeleteChar( pUnit );
}

void CNpcMng::Delete()
{
	CsVectorLink< CNpc* >::iterator* it = m_vnNpc.Begin();
	CsVectorLink< CNpc* >::iterator* itEnd = m_vnNpc.End();
	for( ; it!=itEnd; it = it->s_pNext )
	{
		DeleteChar( it->s_Element );
	}
	m_vnNpc.Destroy();	
}

void CNpcMng::Init()
{
	m_vnNpc.Init( MAX_CHAR_ID_COUNT );
	m_dwOldMapID = (DWORD)-1;

	g_pEventSceneMng->ResistCallBack_EventObject( CALLBACK_GetNpcObject );
}

void CNpcMng::ResetShader()
{
	CsC_AvObject* pAvObject;
	CsNodeObj* pNode;

	CsVectorLink< CNpc* >::iterator* it = m_vnNpc.Begin();
	CsVectorLink< CNpc* >::iterator* itEnd = m_vnNpc.End();
	for( ; it!=itEnd; it = it->s_pNext )
	{
		pAvObject = it->s_Element;
		if( pAvObject->IsLoad() == false )
			continue;

		pNode = pAvObject->GetCsNode();
		if( pNode == NULL )
			continue;

		pNode->ActiveShader();
	}
}

void CNpcMng::NpcReset()
{
	// 벡터 링크 리셋
	CsVectorLink< CNpc* >::iterator* it = m_vnNpc.Begin();
	CsVectorLink< CNpc* >::iterator* itEnd = m_vnNpc.End();
	CsVectorLink< CNpc* >::iterator* itNext = it;
	while( itNext!=itEnd )
	{
		it = itNext;
		itNext = it->s_pNext;
		DeleteChar( it->s_Element );
		it->Delete();
	}
	m_vnNpc.Reset();
}

bool CNpcMng::LoadMap( DWORD dwMapID )
{
	m_dwOldMapID = dwMapID;

	// 엔피씨 생성 - 일단은 걍 다~!!
	MAP_MAPNPC* pmapNpc = nsCsMapTable::g_pMapNpcMng->GetGroup( dwMapID )->GetNpcMap();
	MAP_MAPNPC_IT itmapNpc = pmapNpc->begin();
	MAP_MAPNPC_IT itEndmapNpc = pmapNpc->end();
	for( ;itmapNpc!=itEndmapNpc; ++itmapNpc )
	{
		assert_csm1( nsCsFileTable::g_pNpcMng->IsNpc( itmapNpc->second->GetInfo()->s_dwNpcID ), L"Map.xls에는 존재 하지만 Npc.xls 에 %d Npc가 존재하지 않습니다.", itmapNpc->second->GetInfo()->s_dwNpcID );
		assert_cs( itmapNpc->second->GetInfo()->s_dwNpcID > NPC_FT_MASK );
		assert_cs( itmapNpc->second->GetInfo()->s_dwNpcID%NPC_FT_MASK < OBJECT_MAX_COUNT );
		AddNpc( itmapNpc->second->GetInfo()->s_dwNpcID%NPC_FT_MASK, itmapNpc->second->GetInfo()->s_dwNpcID );
	}
	return true;
}

void CNpcMng::AddNpc( uint nIDX, uint nFileTableID )
{
	assert_cs( nIDX < MAX_CHAR_ID_COUNT );
	// 로드시 필요한 초기화
	sTCUnit* pUint = sTCUnit::NewInstance( sTCUnit::KFM_File );

	// 초기화 - Prop 초기화 포함
	pUint->s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();	
	pUint->s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_ATTACK_NONE | PR_EFFECT_ENABLE | PR_PATH_NORMAL ;
	pUint->s_Prop.s_dwOptionPlag = _OP_ALL;

	CNpc* pNpc = CNpc::NewInstance();
	pNpc->Init( nIDX, nFileTableID, &pUint->s_Prop );
	m_vnNpc.SetData( nIDX, pNpc );

#ifdef CLIENT_MONSTER
	pNpc->GetProp_Alpha()->SetUseDistAlpha( false );
#endif

	// 로드 설정
	pUint->s_pLoadedObject = (CsC_Object*)pNpc;

	DWORD dwModelID = nsCsFileTable::g_pNpcMng->GetNpc( nFileTableID )->GetInfo()->s_dwModelID;	
	strcpy_s( pUint->s_cFilePath, OBJECT_PATH_LEN, g_pModelDataMng->GetData( dwModelID )->GetInfo()->s_cKfmPath );

	// 쓰레드 로드	
	g_pThread->LoadChar( pUint );
}

void CNpcMng::Update()
{
	CsVectorLink< CNpc* >::iterator* it = m_vnNpc.Begin();
	CsVectorLink< CNpc* >::iterator* itEnd = m_vnNpc.End();
	for( ; it!=itEnd; it = it->s_pNext )
	{
		it->s_Element->Update( g_fDeltaTime );
	}
}

void CNpcMng::Render()
{
	CsVectorLink< CNpc* >::iterator* it = m_vnNpc.Begin();
	CsVectorLink< CNpc* >::iterator* itEnd = m_vnNpc.End();
	for( ; it!=itEnd; it = it->s_pNext )
	{
		it->s_Element->Render();
	}
}

CNpc* CNpcMng::GetNpc( uint nIDX )
{
	assert_cs( m_vnNpc.IsElement( nIDX%NPC_FT_MASK ) == true );
	return m_vnNpc[ nIDX%NPC_FT_MASK ];
}

