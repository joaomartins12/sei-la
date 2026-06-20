

#include "stdafx.h"
#include "Npc.h"

THREAD_MEMPOOL_CPP( CNpc )
#define PARENT_CLASS	CsC_AvObject
CsCRTTI_CPP( PARENT_CLASS, CNpc, RTTI_NPC )

CNpc::CNpc()
{
	m_pFT_MapNpc = NULL;
	m_pFT_Npc = NULL;
	m_pCharImage = NULL;
	m_pQuestOwner = NULL;
}

void CNpc::Delete()
{
	PARENT_CLASS::Delete();

	m_pFT_MapNpc = NULL;
	m_pFT_Npc = NULL;
	
}

void CNpc::PreDelete()
{	
	g_pCharResMng->CharImageResDelete( &m_pCharImage );
	m_pQuestOwner = NULL;

	PARENT_CLASS::PreDelete();

	m_Option.Delete();
}

void CNpc::Init( uint nUID, uint nFileTableID, sCREATEINFO* pProp )
{
	m_eNpcRenderType = NRT_NORMAL;

	// File table
	assert_cs( m_pQuestOwner == NULL );
	assert_cs( m_pFT_MapNpc == NULL );
	assert_cs( m_pFT_Npc == NULL );
	m_pFT_MapNpc = nsCsMapTable::g_pMapNpcMng->GetNpc( nFileTableID );
	m_pFT_Npc = nsCsFileTable::g_pNpcMng->GetNpc( nFileTableID );
	m_bEnableUse = true;

	// Position value
	NiPoint3 vPos = NiPoint3((float)m_pFT_MapNpc->GetInfo()->s_nInitPosX, (float)m_pFT_MapNpc->GetInfo()->s_nInitPosY, (float)m_pFT_MapNpc->GetInfo()->s_fRotation);
	vPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight(vPos);

	PARENT_CLASS::Init(nUID, nFileTableID, vPos, (float)m_pFT_MapNpc->GetInfo()->s_fRotation, pProp);

	m_Option.Init( this, m_pFT_Npc->GetInfo()->s_szName );
	m_Option.SetPenName( m_pFT_Npc->GetInfo()->s_szPenName );

	GetBaseStat()->SetHP( 255 );

	m_eQuestViewType = QV_NONE;

	m_pCharImage = g_pCharResMng->CharImageResLoad( GetModelID() );

	GetProp_Alpha()->SetUseDistAlpha( true );

	if( m_pFT_Npc->GetInfo()->s_dwModelID == 95001 )
	{
		GetProp_Animation()->SetUseIdleAni( false );
		SetAnimation( ANI::NPC_CLOSE );
	}
}

void CNpc::Init_NoFT( uint nUID, sCREATEINFO* pProp, eNPC_RENDER_TYPE nrt )
{
	m_eNpcRenderType = nrt;

	// File table
	assert_cs( m_pQuestOwner == NULL );
	assert_cs( m_pFT_MapNpc == NULL );
	assert_cs( m_pFT_Npc == NULL );

	PARENT_CLASS::Init( nUID, 0, NiPoint3::ZERO, 0.0f, pProp );
}

void CNpc::Init_NoMapFT( uint nUID, uint nFileTableID, sCREATEINFO* pProp, eNPC_RENDER_TYPE nrt )
{
	m_eNpcRenderType = nrt;

	// File table
	assert_cs( m_pQuestOwner == NULL );
	assert_cs( m_pFT_MapNpc == NULL );
	assert_cs( m_pFT_Npc == NULL );
	m_pFT_Npc = nsCsFileTable::g_pNpcMng->GetNpc( nFileTableID );

	PARENT_CLASS::Init( nUID, nFileTableID, NiPoint3::ZERO, 0.0f, pProp );
}

UINT CNpc::GetModelID() const
{ 
	SAFE_POINTER_RETVAL( m_pFT_Npc, GetIDX() );
	CsNpc::sINFO const* pInfo = m_pFT_Npc->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, GetIDX() );

	return pInfo->s_dwModelID; 
}

void CNpc::Update( float fDeltaTime )
{
	PARENT_CLASS::Update( fDeltaTime );

	if( m_bEnableUse == false )
		return;

	if( m_pFT_Npc->GetInfo()->s_eType == nsCsNpcTable::NT_PORTAL )
	{
		NiPoint3 pos = GetPos();
		pos.z += GetToolHeight() + 250;

		sNPC_TYPE_PORTAL* pType = (sNPC_TYPE_PORTAL*)m_pFT_Npc->GetType();
		switch(pType->s_nPortalType)
		{
		case 0:
			g_pCharResMng->NpcMarkInsert( GetIDX(), pos, cData_QuestOwner::sNpcOwner::TRAIN );
			break;
		case 1:
			g_pCharResMng->NpcMarkInsert( GetIDX(), pos, cData_QuestOwner::sNpcOwner::NORMAL );
			break;
		}
	}

	switch( m_eQuestViewType )
	{
	case QV_NONE:	
		if( m_pQuestOwner && ( GetProp_Alpha()->IsHideDistOut() == false ) )
		{
			if( m_pQuestOwner->IsDisplayRev() )
			{
				NiPoint3 pos = GetPos();
				pos.z += GetToolHeight();

				g_pCharResMng->NpcMarkInsert( GetIDX(), pos, m_pQuestOwner->GetDisplayRev() );
			}
		}
		break;
	case QV_NPCTALK:
		if( GetProp_Alpha()->IsHideDistOut() == false )
		{
			NiPoint3 pos = GetPos();
			pos.z += GetToolHeight();

			g_pCharResMng->NpcMarkInsert( GetIDX(), pos, cData_QuestOwner::sNpcOwner::TALK );
		}
		break;
	default:
		assert_cs( false );
	}	
}

void CNpc::Render()
{
	if( m_bEnableUse == false )
		return;

	PARENT_CLASS::Render();
}

void CNpc::Render_PostChar()
{
	if( m_bEnableUse == false )
		return;

	if( GetProp_Alpha()->IsHideDistOut() == false )
	{
		m_Option.RenderName();
	}
}

//=============================================================================================================
//
//	CallBack
//
//=============================================================================================================

bool CNpc::CallBack_EndofSequence( DWORD dwAniID )
{
	// 디지바이스로 퀘스트 진행 시 NPC Type 생성 안됨. Type이 생성 안된 상태에서 GetType 호출 시 참조 에러 발생.
	// GetFTNpc 가 NULL일때 해당 Animation의 동작이 완료되도록 함.	lks007  12.09.10
	if( NULL == m_pFT_Npc )
		return PARENT_CLASS::CallBack_EndofSequence( dwAniID );

	sNPC_TYPE_BASE* pNpcType = m_pFT_Npc->GetType();
	if( NULL == pNpcType )
		return PARENT_CLASS::CallBack_EndofSequence( dwAniID );

	// 알부화기 전용 애니메이션
	if( pNpcType->s_eNpcType == nsCsNpcTable::NT_MAKE_TACTICS ) 
	{
		switch( dwAniID )
		{		
		case ANI::NPC_TACTICS_SUCCESS:
		case ANI::NPC_TACTICS_FAIL2:
		case ANI::NPC_TACTICS_FAIL1:
			if( g_pGameIF && g_pGameIF->IsActiveWindow( cBaseWindow::WT_MAKE_TACTICS ) )
			{
				cMakeTactics* pMakeTactics = g_pGameIF->GetMakeTactics();
				if( pMakeTactics )
					pMakeTactics->ProcessResult();
			}
			return true;
		case ANI::NPC_OPEN1:
			SetAnimation( ANI::NPC_OPEN_IDLE1 );
			return true;
		case ANI::NPC_OPEN2:
			SetAnimation( ANI::NPC_OPEN_IDLE2 );
			return true;
		}
	}

	// 캡슐머신
	if( pNpcType->s_eNpcType == nsCsNpcTable::NT_CAPSULE_MACHINE )
	{
		switch( dwAniID )
		{
		case ANI::NPC_YES:
			SetAnimation( ANI::NPC_NO );
			break;
		case ANI::NPC_NO:
			{
				if( g_pDataMng )
				{
					cData_ServerSync* pServerSync = g_pDataMng->GetServerSync();
					if( pServerSync )
						pServerSync->RecvServer( cData_ServerSync::CASH_CAPSULE_COIN_CLIENT, 0 );
				}
				SetAnimation( ANI::NPC_TALK );
			}
			break;
		}
		return true;
	}
	// 레어머신
 	if( pNpcType->s_eNpcType == nsCsNpcTable::NT_GOTCHA_MACHINE )
 	{
// 		switch( dwAniID )
// 		{
// 		case ANI::NPC_TACTICS_SUCCESS:
// 			{
// 				SetAnimation( ANI::NPC_NO );
// 				//여기가 문제다 애니가 다 돌기전에 가챠머신 닫힘 -> 펑!
// 				//일단은 고쳐보았다
// 				SAFE_POINTER_BEK( g_pGameIF );
// 				cGotchaMachine* pGotcha = g_pGameIF->GetGotchaMachine();
// 				SAFE_POINTER_BEK( pGotcha );
// 				pGotcha->m_eStat = cGotchaMachine::COMP;
// 			}
// 			break;
// 		}
		switch( dwAniID )
		{
		case ANI::NPC_YES:
		case ANI::NPC_TACTICS_SUCCESS:
			{
				SetAnimation( ANI::IDLE_NORMAL );
				GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_ITEM_GOTCHAMACHINE, NULL );
			}
			break;
		}
 	}

	switch( dwAniID )
	{
		// 이벤트 상점
	case ANI::NPC_OPEN1:
		SetDefault_IdleAnimation();
		return true;
	case ANI::NPC_CLOSE:
		SetDefault_IdleAnimation();
		return true;
	}

	return PARENT_CLASS::CallBack_EndofSequence( dwAniID );
}

//====================================================================================
//
// NPC functions
//
//====================================================================================
void CNpc::CheckQuestExtra()
{
	if( m_pFT_Npc->GetExtra( nsCsNpcTable::NE_QUEST ) == NULL )
		return;

	sNPC_EXTRA_QUEST* pExtra = (sNPC_EXTRA_QUEST*)m_pFT_Npc->GetExtra( nsCsNpcTable::NE_QUEST );
	// 초기 상태 입력
	bool bEnableUse = ( pExtra->s_eInitState == sNPC_EXTRA_QUEST::NEQ_SHOW );

	cData_Quest* pDataQuest = g_pDataMng->GetQuest();

	// 퀘스트 완료 상태에 따른 상태 변화
	int nCnt = pExtra->s_vpAct.Size();
	for( int i=0; i<nCnt; ++i )
	{
		sNPC_EXTRA_QUEST::sACT* pAct = pExtra->s_vpAct[ i ];
		switch( pAct->s_eActType )
		{
		case sNPC_EXTRA_QUEST::NEA_QUEST_COMP:
			{
				std::list< int >::iterator it = pAct->s_list.begin();
				std::list< int >::iterator itEnd = pAct->s_list.end();
				for( ; it!=itEnd; ++it )
				{
					if( pDataQuest->IsCompleate( *it ) == false )
						goto NEXT_CHECK_COMP;
				}

				// 전부 성공 하였다면
				bEnableUse = ( pAct->s_eCompState == sNPC_EXTRA_QUEST::NEQ_SHOW );
NEXT_CHECK_COMP:;
			}
			break;
		case sNPC_EXTRA_QUEST::NEA_QUEST_PROCCOMP:
			{
				std::list< int >::iterator it = pAct->s_list.begin();
				std::list< int >::iterator itEnd = pAct->s_list.end();
				for( ; it!=itEnd; ++it )
				{
					if( pDataQuest->IsProcess( *it ) == false )
						goto NEXT_CHECK_PROCCOMP;

					if( pDataQuest->GetProcess( *it )->s_CurCompRequireGroup.IsCompleate() == false )
						goto NEXT_CHECK_PROCCOMP;
				}

				// 전부 성공 하였다면
				bEnableUse = ( pAct->s_eCompState == sNPC_EXTRA_QUEST::NEQ_SHOW );
NEXT_CHECK_PROCCOMP:;
			}
			break;
		default:
			assert_cs( false );
		}
	}

	SetEnableUse( bEnableUse );
}

void CNpc::SetEnableUse(bool bEnable)
{
	if( m_bEnableUse == bEnable )
		return;

	m_bEnableUse = bEnable;
	if( !m_bEnableUse )
		RemoveShaderEffect_Highlight();
}

bool CNpc::CheckTamerDist(float Dist)
{
	float fCheckDist = Dist;

	if( m_pFT_Npc )
	{
		CsNpc::sINFO* pFTInfo = m_pFT_Npc->GetInfo();
		if( pFTInfo && pFTInfo->s_eMoveType == nsCsNpcTable::MT_MOVE )
			fCheckDist *= 3.0f;
	}

	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pUser, false );

	return ( ( GetPos2D() - pUser->GetPos2D() ).Length() < fCheckDist );
}

void CNpc::Try_Talk()
{
	if( m_pFT_Npc && ( m_pFT_Npc->GetInfo()->s_eMoveType == nsCsNpcTable::MT_MOVE ) )
		return;

	SetAnimation( ANI::NPC_TALK );
}

void CNpc::Try_Yes()
{
	if( m_pFT_Npc && ( m_pFT_Npc->GetInfo()->s_eMoveType == nsCsNpcTable::MT_MOVE ) )
		return;

	SetAnimation( ANI::NPC_YES );
}

void CNpc::Try_No()
{
	if( m_pFT_Npc && ( m_pFT_Npc->GetInfo()->s_eMoveType == nsCsNpcTable::MT_MOVE ) )
		return;

	SetAnimation( ANI::NPC_NO );
}

void CNpc::SetOpenAnimation()
{
	if( NULL == m_pProperty[ CsC_Property::ANIMATION ] )
		return;

	CsNpc::sINFO* pFTNpc = GetFTNpc()->GetInfo();
	switch( pFTNpc->s_eType )
	{
	case nsCsNpcTable::NT_MAKE_TACTICS:
		{
			bool nIsEnable = false;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_MAKETACTICS_DIGITAMA_ISENABLE, &nIsEnable);

			if( nIsEnable == true )
			{
				if( GetProp_Animation()->IsSequence( ANI::NPC_OPEN1 ) == true )
					SetAnimation( ANI::NPC_OPEN1 );
			}
			else
			{
				if( GetProp_Animation()->IsSequence( ANI::NPC_OPEN2 ) == true )
					SetAnimation( ANI::NPC_OPEN2 );
			}
		}		
		break;
	default:
		if( GetProp_Animation()->IsSequence( ANI::NPC_OPEN1 ) == true )
			SetAnimation( ANI::NPC_OPEN1 );
		break;
	}	
}

void CNpc::SetCloseAnimation()
{
	if( NULL == m_pProperty[ CsC_Property::ANIMATION ] )
		return;

	if( GetProp_Animation()->IsSequence( ANI::NPC_CLOSE ) == true )
		SetAnimation( ANI::NPC_CLOSE );
}

void* CNpc::PlaySound( char* cSoundPath )
{ 
	return g_pSoundMgr->PlaySystemSound( cSoundPath, GetPos(), this );
}

nsCsNpcTable::eNPC_TYPE CNpc::GetNPCType()
{
	SAFE_POINTER_RETVAL( m_pFT_Npc, nsCsNpcTable::NT_NONE );
	SAFE_POINTER_RETVAL( m_pFT_Npc->GetInfo(), nsCsNpcTable::NT_NONE );
	return m_pFT_Npc->GetInfo()->s_eType;
}

bool CNpc::IsDisplayType( DWORD const& dwDisplayType ) const
{
	SAFE_POINTER_RETVAL( m_pFT_Npc, false );
	CsNpc::sINFO* pInfo = m_pFT_Npc->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, false );
	if( pInfo->s_nDisplayPlag & dwDisplayType )
		return true;
	return false;
}
#undef PARENT_CLASS

