
#include "stdafx.h"
#include "QuestComp.h"

#define IF_QUESTCOMP_ICON_SIZE	CsPoint( 32, 32 )
#define IF_QUESTCOMP_STRLIST_POS		CsPoint( 16, 59 )
#define IF_QUESTCOMP_REQUITE_POS		CsPoint( 90, 238 )
#define IF_QUESTCOMP_REQUITE_ITEM_DELTA_POS		CsPoint( 0, -5 )

cQuestComp::cQuestComp()
:m_nRequiteItemIconCount(0),
m_OpenSlotIdx(0),
m_pkSPOpenSlot(NULL),
m_pkBTOpenSlot(NULL)
{
	m_pTarget = NULL; 
}


void cQuestComp::Destroy()
{
	//2017-01-10-nova 퀘스트완료 종료시 비활성화된 인벤토리 활성화 (#4308)

	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_TALK )  )	// 2017-01-11-nova 대화창이 있을경우는 있을경우는 인벤토리를 비활성화 상태로 둔다
	{
		bool bIsEnable = true;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::SET_ENABLE_INVENTORY, &bIsEnable );
	}

	cBaseWindow::Delete();
}

void cQuestComp::DeleteResource()
{
	m_pTarget = NULL;
	
	DeleteScript();

	m_StringList.Delete();
	m_RequiteString.Delete();
	SAFE_NIDELETE( m_pBGWindow );
	SAFE_NIDELETE( m_pExpSprite );
}

void cQuestComp::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	m_pTarget = NULL;

	InitScript( NULL, CsPoint( 272, 11 ), CsPoint( 558, 284 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::CENTER );

	m_pBGWindow = NiNew cPopUp;
	m_pBGWindow->Init( m_pRoot, CsPoint::ZERO, CsPoint( 558, 284 ), CsPoint( 32, 32 ), "Control_G\\Popup\\W2Dlg", "tga", false );

	AddSprite( CsPoint( 20, 223 ), CsPoint( 494, 55 ), "QuestRev\\Quest_Point.tga" );

	m_pCancelButton = AddButton( CsPoint( 519, 20 ), CsPoint( 21, 22 ), CsPoint( 0, 22 ), "QuestRev\\System_back.tga", cWindow::SD_Wa3 );

	m_pCompleateButton = AddButton( CsPoint( 434, 234 ), CsPoint( 61, 29 ), cButton::IMAGE_NOR_3, UISTRING_TEXT( "COMMON_TXT_COMPLETE" ).c_str(), cWindow::SD_Qu2 );
	if( m_pCompleateButton )
		m_pCompleateButton->SetControlName("QuestCompleteBtn");

	m_pExpSprite = NiNew cSprite;
	m_pExpSprite->Init( NULL, CsPoint::ZERO, CsPoint( 27, 17 ), "System\\Exp.tga", false );

	m_pScrollBar = AddScrollBar( cScrollBar::TYPE_2, CsPoint( 521, 60 ), CsPoint( 16, 153 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint( 11, 60 ), CsPoint( 505, 236 ) ), 6 );

	CsPoint ptNamePos = CsPoint( 40, 10 );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_20, NiColor( 0.2f, 0.6f, 0.8f ) );	
	ti.s_bOutLine = false;
	m_pName = AddText( &ti, ptNamePos );

	m_pScrollBar->SetEnableRenderFromEnableScroll( true );

	m_nRequiteItemIconCount = 0;
	m_OpenSlotIdx = -1;
	m_pkSPOpenSlot = NULL;
	m_bUseTamerDistCancel = true;

	//2017-01-10-nova 퀘스트완료 생성시 활성화된 인벤토리 비활성화 (#4308)
	bool bIsEnable = false;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::SET_ENABLE_INVENTORY, &bIsEnable );
}

void cQuestComp::Update(float const& fDeltaTime)
{
	g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_QUEST_COMP );

	if( m_bUseTamerDistCancel )
	{
		// 테이머와의 거리 체크
		if( ( (CNpc*)m_pTarget )->CheckTamerDist() == false )
		{
			Close();
			return;
		}
	}	
}

cBaseWindow::eMU_TYPE
cQuestComp::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		{
			cTalk* pTalk = (cTalk*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_TALK );
			pTalk->SetTarget( m_pTarget, false );
			g_pNpcMng->SetZoomNpc( m_pTarget );
			Close( false );
		}
		return muReturn;
	default:
		return muReturn;
	}

	switch( m_pCompleateButton->Update_ForMouse() )
	{
	case cButton::ACTION_CLICK:	
		_CompleteQuest();
		return muReturn;
	}

	// 보상 아이템 툴팁 창
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
	for( int i=0; i<m_nRequiteItemIconCount; ++i )
	{
		if( m_RequiteItemIFIcon[ i ].MouseOn( ptMouseLocalPos ) == true )
		{
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( GetRootClient() + m_RequiteItemIFIcon[ i ].GetPos(), IF_QUESTCOMP_ICON_SIZE, TOOLTIP_MAX_SIZE, 
												cTooltip::ITEM, m_nRequiteItemType[ i ], cBaseWindow::WT_QUEST_COMP );
		}
	}
	if(m_pkBTOpenSlot != NULL && m_pkBTOpenSlot->PtInWindowRect(CURSOR_ST.GetPos()))
	{
#ifdef VERSION_USA
		int nMaxTooltipX = 340;
#else
		int nMaxTooltipX = TOOLTIP_MAX_SIZE;
#endif
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( m_pkBTOpenSlot->GetPos(), IF_QUESTCOMP_ICON_SIZE, 
			nMaxTooltipX, cTooltip::QUICK_EVOL, cBaseWindow::WT_NEW_DIGIMONSTATUS, m_OpenSlotIdx );
	}

	m_pScrollBar->Update_ForMouse();

	return muReturn;
}

void cQuestComp::Render()
{
	m_pBGWindow->Render();

	RenderScript();

	m_StringList.Render( IF_QUESTCOMP_STRLIST_POS + GetRootClient(), TALK_LINE_DELTA );
	m_RequiteString.Render( IF_QUESTCOMP_REQUITE_POS + GetRootClient() );

	for( int i=0; i<m_nRequiteItemIconCount; ++i )
	{
		// 마스크 있다면 마스크 랜더링
		if( m_RequiteItemIFIcon[ i ].s_eState != sIFIcon::NONE )
		{			
			m_RequiteItemIFIcon[ i ].RenderMask( GetRootClient() + m_RequiteItemIFIcon[ i ].GetPos(), IF_QUESTCOMP_ICON_SIZE );
		}		
	}
}


void cQuestComp::SetTarget( CsC_AvObject* pTarget, int nQuestFTID )
{
	assert_cs( m_pTarget == NULL );
	assert_cs( pTarget != NULL );
	
	m_pTarget = pTarget;
	if( m_pTarget->GetLeafRTTI() == RTTI_NPC )
	{
		if( ( (CNpc*)m_pTarget )->GetNpcRenderType() != CNpc::NRT_NORMAL )
			DisableTamerDistCancel();
	}
	
	m_pQuestFT = nsCsFileTable::g_pQuestMng->GetQuest( nQuestFTID );

	// 이름	
	m_pName->SetText( m_pQuestFT->m_szTitleText );

	m_StringList.Delete();

	// 본문	
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_13, NiColor( 0.8f, 0.8f, 0.8f ) );	
	ti.s_bOutLine = false;

	TCHAR sz[QUEST_COMPLEATE_LEN + 64] = { 0, };
	g_pStringAnalysis->StringTransfer(sz, QUEST_COMPLEATE_LEN + 64, m_pQuestFT->m_szCompleate);
	g_pStringAnalysis->Cut_LinkParcing(&m_StringList, IF_TALK_STRING_CUT, sz, &ti, cButton::IMAGE_ALPHA_2);
			
	m_pScrollBar->SetRange( CsPoint( 0, m_StringList.GetSize() + 3 ) );
	m_StringList.SetBuddy( m_pScrollBar );


	// 보상	
	CsQuestRequiteGroup* pRequiteGroup = m_pQuestFT->GetRequiteGroup();
	int nRequiteCount = pRequiteGroup->GetListCount();

	int nExp = 0;
	u8 nMoney = 0;
	m_nRequiteItemIconCount = 0;
	m_OpenSlotIdx = -1;
	DWORD dwDigimonTBIDX = 0;
	m_pkSPOpenSlot = NULL;
	cItemInfo item[ 6 ]; //chu8820 이벤트 보상 6개로 주려고 임시로 수정 5 -> 6

	// 경험치, 돈, 아이템 순으로 집어 넣자		
	CsQuestRequite* pRequite;
	for( int i=0; i<nRequiteCount; ++i )
	{
		pRequite = pRequiteGroup->Get( i );

		assert_cs( pRequite->GetMethodID() == CsQuestRequite::RM_GIVE );
		switch( pRequite->GetType() )
		{
		case CsQuestRequite::EXP:
			{
				//nExp = g_pDataMng->GetQuest()->GetQuestExp( m_pQuestFT, pRequite->GetTypeID() );
				nExp = pRequite->GetTypeID();
			}
			break;
		case CsQuestRequite::MONEY:
			{
				nMoney = pRequite->GetTypeID();
			}
			break;
		case CsQuestRequite::ITEM:
			{
				item[ m_nRequiteItemIconCount ].m_nType = pRequite->GetTypeID();
				item[ m_nRequiteItemIconCount ].m_nCount = pRequite->GetTypeCount();
				++m_nRequiteItemIconCount;
			}
			break;
		case CsQuestRequite::EVOSLOT:
			{
				dwDigimonTBIDX = pRequite->GetTypeID();
				m_OpenSlotIdx = pRequite->GetTypeCount();
			}
			break;

		default:
			assert_cs( false );
		}
	}

	ti.s_pFont = &g_pEngine->m_FontText;
	ti.s_eFontSize = CFont::FS_12;
	ti.s_Color = FONT_WHITE;

	if( nExp != 0 )
	{
		ti.SetText( nExp );
		m_RequiteString.AddText( &ti );
		m_RequiteString.TailAddSizeX( 5 );

		//exp마크
		m_RequiteString.AddSprite( m_pExpSprite, CsPoint( 0, 1 ) );
		m_RequiteString.TailAddSizeX( 20 );
	}

	if( nMoney != 0 )
	{
		m_RequiteString.AddMoneyIF( cMoneyIF::DISP_SIMPLE_1, nMoney, FONT_WHITE, CFont::FS_12, DT_LEFT );
		m_RequiteString.TailAddSizeX( 20 );
	}

	CsSIZE size = (CsSIZE)IF_QUESTCOMP_ICON_SIZE;
	for( int i=0; i<m_nRequiteItemIconCount; ++i )
	{
		m_RequiteString.AddIcon( IF_QUESTCOMP_ICON_SIZE, ICONITEM::ARMOR_1, item[ i ].m_nType, item[ i ].m_nCount, IF_QUESTCOMP_REQUITE_ITEM_DELTA_POS );		
		CsPoint pos = IF_QUESTCOMP_REQUITE_POS + IF_QUESTCOMP_REQUITE_ITEM_DELTA_POS + CsPoint( m_RequiteString.GetMaxSize().x - size.cx, 0 );

		m_RequiteItemIFIcon[ i ].Init( CsRect( pos, size ) );
		m_nRequiteItemType[ i ] = item[ i ].m_nType;

		m_RequiteString.TailAddSizeX( 7 );
	}

	if(m_OpenSlotIdx > 0 && dwDigimonTBIDX > 0)
	{
		CDigimonEvolveInfo* pDigimonEvInfo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( dwDigimonTBIDX );
		if( pDigimonEvInfo )
		{
			CDigimonEvolveObj* pFTEvolObj = pDigimonEvInfo->GetEvolveObjByEvoSlot( m_OpenSlotIdx );
			if(pFTEvolObj != NULL)
			{
				CsDigimon* pkTableInfo = nsCsFileTable::g_pDigimonMng->GetDigimon( pFTEvolObj->m_dwID );
				if(pkTableInfo != NULL)
				{
					std::string kPath = g_pModelDataMng->GetSmallModelIconFile( pkTableInfo->GetInfo()->s_dwModelID );
					cSprite* m_pkSPOpenSlot = NiNew cSprite;	// 1
					m_pkSPOpenSlot->Init( NULL, CsPoint::ZERO, IF_QUESTCOMP_ICON_SIZE, kPath.c_str(), false, NiColor::WHITE, false );
					cString::sSPRITE* pkAddedSp = m_RequiteString.AddSprite( m_pkSPOpenSlot, IF_QUESTCOMP_REQUITE_ITEM_DELTA_POS, IF_QUESTCOMP_ICON_SIZE  );	
					if( pkAddedSp )
						pkAddedSp->SetAutoPointerDelete(true);

					CsPoint kDelta = IF_QUESTCOMP_ICON_SIZE * - 1.0f;
					kDelta.y = IF_QUESTCOMP_REQUITE_ITEM_DELTA_POS.y;
					m_pkBTOpenSlot = NiNew cButton;
					m_pkBTOpenSlot->Init( NULL, CsPoint::ZERO, IF_QUESTCOMP_ICON_SIZE, "QuestRev\\Quest_Reward_Evoslot.tga", false );
					cString::sBUTTON* pkAddedBT = m_RequiteString.AddButton( m_pkBTOpenSlot, 0, kDelta, IF_QUESTCOMP_ICON_SIZE  );	
					if( pkAddedBT )
						pkAddedBT->SetAutoPointerDelete(true);
				}
			}
		}
	}
}


void cQuestComp::ResetDevice()
{	
	ResetDeviceScript();

	m_pBGWindow->ResetDevice();	
}

bool cQuestComp::OnMacroKey(const MSG& p_kMsg)
{
	switch( p_kMsg.message )
	{
	case WM_KEYDOWN:
		{
			if( GLOBALINPUT_ST.GetEqualSnKey( p_kMsg.wParam, DMKEY::NPC_ACTION ) )
			{
				_CompleteQuest();
				return true;
			}
		}
		break;
	}
	return false;
}

void cQuestComp::_CompleteQuest()
{
	CsC_AvObject* pTarget = m_pTarget;
	Close( false );
	// 즉각 퀘스트라면 바로 수락해 버리자
	if( m_pQuestFT->IsImmediate() == true )
	{
		// 퀘스트 시작
		switch( m_pQuestFT->GetStartTarget_Type() )
		{
		case CsQuest::ST_NPC:
			{					
				g_pDataMng->GetQuest()->RevQuest_ByNpc( m_pQuestFT, pTarget );
				g_pDataMng->GetQuest()->CalProcess();
			}
			break;
		case CsQuest::ST_DIGIVICE:
			{
				g_pDataMng->GetQuest()->RevQuest_ByDigivice( m_pQuestFT );
				g_pDataMng->GetQuest()->CalProcess();
			}
			break;
		default:
			assert_cs( false );
		}
	}

	// 퀘스트 완료
	if( g_pDataMng->GetQuest()->CompQuest( m_pQuestFT, pTarget ) )
	{
		( (CNpc*)pTarget )->Try_Yes();

		// 남아 있는 디지바이스 퀘가 있는지 체크
		if( g_pDataMng->GetQuestOwner()->IF_DigiviceQuest_Exist() )
			g_pDataMng->GetQuestOwner()->IF_DigiviceQuest_Open();

		int nEventSceneNum = nsCsFileTable::g_pQuestMng->GetQuest( m_pQuestFT->GetUniqID() )->m_nEvent[ CsQuest::EVENT_QUEST_REQUITE ];
		if( nsCsFileTable::g_pSceneDataMng->IsData( (DWORD)nEventSceneNum ) )
			g_pEventScene->EventStart( nEventSceneNum );

		if( cTalk::IsHaveQuest((CNpc*)pTarget ) )
		{
			cBaseWindow* pBase = g_pGameIF->GetDynamicIF( cBaseWindow::WT_TALK );
			if( pBase )
			{
				static_cast<cTalk*>(pBase)->SetTarget( pTarget, false );
				g_pNpcMng->SetZoomNpc( pTarget );
			}
		}
	}
	// 퀘스트 완료 실패
	else
	{
		// 즉각 퀘스트라면 퀘스트 취소
		if( m_pQuestFT->IsImmediate() )
			g_pDataMng->GetQuest()->DropProcQuest( m_pQuestFT->GetUniqID() );

		Close( false );
	}
}
