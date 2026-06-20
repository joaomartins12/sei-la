
#include "stdafx.h"
#include "QuestRev.h"

#define IF_QUESTRECV_ICON_SIZE			CsPoint( 32, 32 )
#define IF_QUESTRECV_STRLIST_POS		CsPoint( 16, 59 )
#define IF_QUESTRECV_REQUITE_POS		CsPoint( 90, 238 )
#define IF_QUESTRECV_REQUITE_ITEM_DELTA_POS		CsPoint( 0, -5 )

cQuestRev::cQuestRev()
:m_OpenSlotIdx(0),
m_pkBTOpenSlot(NULL)
{ 
	m_pTarget = NULL; 
}
void cQuestRev::Destroy()
{
	cBaseWindow::Delete();
}

void cQuestRev::DeleteResource()
{
	m_pTarget = NULL;

	DeleteScript();

	m_StringList.Delete();
	m_RequiteString.Delete();
	SAFE_NIDELETE( m_pBGWindow );
	SAFE_NIDELETE( m_pExpSprite );
}

void cQuestRev::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	m_pTarget = NULL;

	InitScript( NULL, CsPoint( 272, 11 ), CsPoint( 558, 284 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::CENTER );

	m_pBGWindow = NiNew cPopUp;
	m_pBGWindow->Init( m_pRoot, CsPoint::ZERO, CsPoint( 558, 284 ), CsPoint( 32, 32 ), "Control_G\\Popup\\WDlg", "tga", false );

	AddSprite( CsPoint( 20, 223 ), CsPoint( 494, 55 ), "QuestRev\\Quest_Point.tga" );

	m_pCancelButton = AddButton( CsPoint( 519, 20 ), CsPoint( 21, 22 ), CsPoint( 0, 22 ), "QuestRev\\System_back.tga", cWindow::SD_Wa3 );

	m_pAcceptButton = AddButton( CsPoint( 434, 234 ), CsPoint( 61, 29 ), cButton::IMAGE_NOR_3, UISTRING_TEXT( "QUESTRECV_BTN_ACCEPT" ).c_str(), cWindow::SD_Qu1 );
	if( m_pAcceptButton )
		m_pAcceptButton->SetControlName( "QuestAcceptBtn" );

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
	m_bUseTamerDistCancel = true;
}

void cQuestRev::Update(float const& fDeltaTime)
{
	g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_QUEST_REV );

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
cQuestRev::Update_ForMouse()
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

	switch( m_pAcceptButton->Update_ForMouse() )
	{
	case cButton::ACTION_CLICK:
		_AcceptQuest();
		return muReturn;
	}

	// 보상 아이템 툴팁 창
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
	for( int i=0; i<m_nRequiteItemIconCount; ++i )
	{
		if( m_RequiteItemIFIcon[ i ].MouseOn( ptMouseLocalPos ) == true )
		{
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( GetRootClient() + m_RequiteItemIFIcon[ i ].GetPos(), IF_QUESTRECV_ICON_SIZE, TOOLTIP_MAX_SIZE, 
												cTooltip::ITEM, m_nRequiteItemType[ i ], cBaseWindow::WT_QUEST_REV );
		}
	}

	if(m_pkBTOpenSlot != NULL && m_pkBTOpenSlot->PtInWindowRect(CURSOR_ST.GetPos()))
	{
#ifdef VERSION_USA
		int nMaxTooltipX = 340;
#else
		int nMaxTooltipX = TOOLTIP_MAX_SIZE;
#endif
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( m_pkBTOpenSlot->GetPos(), IF_QUESTRECV_ICON_SIZE, 
			nMaxTooltipX, cTooltip::QUICK_EVOL, cBaseWindow::WT_NEW_DIGIMONSTATUS, m_OpenSlotIdx );
	}


	m_pScrollBar->Update_ForMouse();


	int nActive1;
	int nActive2;
	CsPoint ptPoint = IF_QUESTRECV_STRLIST_POS + GetRootClient();			
	cString::sBUTTON* pButtonInfo = (cString::sBUTTON*)m_StringList.Update_ForMouse_Control( nActive1, nActive2, ptPoint, TALK_LINE_DELTA );		
	if( pButtonInfo != NULL )
	{
	}

	return muReturn;
}

void cQuestRev::Render()
{
	m_pBGWindow->Render();

	RenderScript();

	m_StringList.Render( IF_QUESTRECV_STRLIST_POS + GetRootClient(), TALK_LINE_DELTA );
	m_RequiteString.Render( IF_QUESTRECV_REQUITE_POS + GetRootClient() );

	for( int i=0; i<m_nRequiteItemIconCount; ++i )
	{
		// 마스크 있다면 마스크 랜더링
		if( m_RequiteItemIFIcon[ i ].s_eState != sIFIcon::NONE )
		{			
			m_RequiteItemIFIcon[ i ].RenderMask( GetRootClient() + m_RequiteItemIFIcon[ i ].GetPos(), IF_QUESTRECV_ICON_SIZE );
		}		
	}
}

DWORD cQuestRev::GetQuestUIDX() const
{
	SAFE_POINTER_RETVAL( m_pQuestFT, 0 );
	return m_pQuestFT->GetUniqID();
}

void cQuestRev::SetTarget( CsC_AvObject* pTarget, int nQuestFTID )
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

	TCHAR sz[ QUEST_BODY_LEN + 64 ];
	g_pStringAnalysis->StringTransfer( sz, QUEST_BODY_LEN + 64, m_pQuestFT->m_szBodyText );
	g_pStringAnalysis->Cut_LinkParcing( &m_StringList, IF_TALK_STRING_CUT, sz, &ti, cButton::IMAGE_ALPHA_2 );
	m_pScrollBar->SetRange( CsPoint( 0, m_StringList.GetSize() + 3 ) );
	m_StringList.SetBuddy( m_pScrollBar );


	// 보상	
	CsQuestRequiteGroup* pRequiteGroup = m_pQuestFT->GetRequiteGroup();
	int nRequiteCount = pRequiteGroup->GetListCount();

	int nExp = 0;
	u8 nMoney = 0;
	m_nRequiteItemIconCount = 0;
	m_OpenSlotIdx = -1;
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
				m_OpenSlotIdx = pRequite->GetTypeID();
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

	CsSIZE size = (CsSIZE)IF_QUESTRECV_ICON_SIZE;
	for( int i=0; i<m_nRequiteItemIconCount; ++i )
	{
		m_RequiteString.AddIcon( IF_QUESTRECV_ICON_SIZE, ICONITEM::ARMOR_1, item[ i ].m_nType, item[ i ].m_nCount, IF_QUESTRECV_REQUITE_ITEM_DELTA_POS );		
		CsPoint pos = IF_QUESTRECV_REQUITE_POS + IF_QUESTRECV_REQUITE_ITEM_DELTA_POS + CsPoint( m_RequiteString.GetMaxSize().x - size.cx, 0 );

		m_RequiteItemIFIcon[ i ].Init( CsRect( pos, size ) );
		m_nRequiteItemType[ i ] = item[ i ].m_nType;

		m_RequiteString.TailAddSizeX( 7 );
	}
	if(m_OpenSlotIdx > 0)
	{
		CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );
		if(pDUser != NULL && pDUser->GetFTEvol() != NULL)
		{
			CDigimonEvolveObj* pFTEvolObj = pDUser->GetFTEvol()->GetEvolveObjByEvoSlot( m_OpenSlotIdx );
			if(pFTEvolObj != NULL)
			{
				CsDigimon* pkTableInfo = nsCsFileTable::g_pDigimonMng->GetDigimon( pFTEvolObj->m_dwID );
				if(pkTableInfo != NULL)
				{
					std::string kPath = g_pModelDataMng->GetSmallModelIconFile( pkTableInfo->GetInfo()->s_dwModelID );
					cSprite* m_pkSPOpenSlot = NiNew cSprite;	// 1
					m_pkSPOpenSlot->Init( NULL, CsPoint::ZERO, IF_QUESTRECV_ICON_SIZE, kPath.c_str(), false, NiColor::WHITE, false );
					cString::sSPRITE* pkAddedSp = m_RequiteString.AddSprite( m_pkSPOpenSlot, IF_QUESTRECV_REQUITE_ITEM_DELTA_POS, IF_QUESTRECV_ICON_SIZE  );	
					if( pkAddedSp )
						pkAddedSp->SetAutoPointerDelete(true);

					CsPoint kDelta = IF_QUESTRECV_ICON_SIZE * - 1.0f;
					kDelta.y = IF_QUESTRECV_REQUITE_ITEM_DELTA_POS.y;
					m_pkBTOpenSlot = NiNew cButton;
					m_pkBTOpenSlot->Init( NULL, CsPoint::ZERO, IF_QUESTRECV_ICON_SIZE, "QuestRev\\Quest_Reward_Evoslot.tga", false );
					cString::sBUTTON* pkAddedBT = m_RequiteString.AddButton( m_pkBTOpenSlot, 0, kDelta, IF_QUESTRECV_ICON_SIZE  );	
					if( pkAddedBT )
						pkAddedBT->SetAutoPointerDelete(true);
				}
			}
		}
	}
}

void cQuestRev::ResetDevice()
{	
	ResetDeviceScript();

	m_pBGWindow->ResetDevice();	
}

bool cQuestRev::OnMacroKey(const MSG& p_kMsg)
{
	switch( p_kMsg.message )
	{
	case WM_KEYDOWN:
		{
			if( p_kMsg.wParam == VK_PROCESSKEY )
			{
				uint nScanCode = (p_kMsg.lParam >> 16) & 0xff;
				BYTE byCode = ::MapVirtualKey( nScanCode, 3/*MAPVK_VSC_TO_VK_EX*/ );
				if( GLOBALINPUT_ST.GetEqualSnKey( byCode, DMKEY::NPC_ACTION ) )
					return false;
			}

			if( GLOBALINPUT_ST.GetEqualSnKey( p_kMsg.wParam, DMKEY::NPC_ACTION ) )
			{
				_AcceptQuest();
				return true;
			}
		}
		break;
	}
	return false;
}

void cQuestRev::_NewBiQuestCheck()
{	
	cQuestFindWay::eFindWayDestType eType = cQuestFindWay::FindWay_None;

	switch( m_pQuestFT->GetUniqID() )
	{
	case 143:  // 신입대원 1				
	case 144:  // 신입대원 2-1		
	case 508:  // 신입대원 2-2		
	case 145:  // 신입대원 3-1		
	case 509:  // 신입대원 3-2							
		eType = cQuestFindWay::FindWay_Npc;
		break;			
	case 146:  // 신입대원 4
		{
			UINT nIDX = m_pTarget->GetIDX();
			UINT nWindowType = cBaseWindow::WT_DIGITAMA_SCAN;
			ContentsStream kTmp;
			kTmp << nWindowType << nIDX;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_OPEN_NPC, &kTmp );

			g_pGameIF->GetDigitamaScan()->SetArrowShow( true );
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_INVENTORY, 0 ) )
			{
				GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_INVENTORY, NULL );

				if( g_pDataMng->GetPCbang()->GetPcBang() )
				{
					g_pGameIF->CloseDynamicIF( cBaseWindow::WT_PCBANGINVEN );
				}
			}
			else
			{
				bool bSound = true;
				GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INVENTORY, &bSound );
			}

		}
		break;
	case 622:  // 회수용 디지몬알 구해오기
		{			
			g_pHelpArrow->SetArrowPoint( 0, 101, cQuestFindWay::FindWay_Map, nsCsMapTable::g_pMapListMng->GetList( 101 )->GetInfo()->s_szMapDiscript.c_str() );
		}
		break;
	}				

	if( eType == cQuestFindWay::FindWay_Npc )
	{	
		CsNpc* pFTNpc = nsCsFileTable::g_pNpcMng->GetNpc( m_pQuestFT->GetQuestTargetValue1() );	
		g_pHelpArrow->SetArrowPoint( m_pQuestFT->GetQuestTargetValue1(), pFTNpc->GetInfo()->s_dwMapID, eType ,pFTNpc->GetInfo()->s_szName );	
	}	
}

void cQuestRev::_AcceptQuest()
{
	// 퀘스트 목록 가득참
	if( g_pDataMng->GetQuest()->GetInfo()->GetExecuteCount() >= nQuest::MaxExecute )
	{
		cPrintMsg::PrintMsg( 14002 );
	}
	// 실제 퀘스트 받을수 있는지 최종 체크
	else if( cData_Quest::CheckPreRequire( m_pQuestFT ) == false )
	{
		cPrintMsg::PrintMsg( 14011 );
	}
	else
	{
		CsC_AvObject* pTarget = m_pTarget;

		Close( false );

		// 퀘스트 시작
		switch( m_pQuestFT->GetStartTarget_Type() )
		{
		case CsQuest::ST_NPC:
			{					
				g_pDataMng->GetQuest()->RevQuest_ByNpc( m_pQuestFT, pTarget );
				g_pDataMng->GetQuest()->CalProcess();	// 진행되는 퀘스트 점검
				assert_cs( pTarget->GetLeafRTTI() == RTTI_NPC );
				( (CNpc*)pTarget )->Try_Yes();	// NPC 자세 취하기
				_NewBiQuestCheck();	// 신입 퀘스트에 대한 특정 코드
			}
			break;
		case CsQuest::ST_DIGIVICE:
			{
				g_pDataMng->GetQuest()->RevQuest_ByDigivice( m_pQuestFT );
				g_pDataMng->GetQuest()->CalProcess();
				_NewBiQuestCheck();					
			}
			break;
		default:
			assert_cs( false );
		}

		// 바로 완료라면 - 그리고 즉시 완료 퀘스트라면 완료창 띄워주자					
		if( ( g_pDataMng->GetQuest()->IsProcess( m_pQuestFT->GetUniqID() ) )&&
			( g_pDataMng->GetQuest()->GetProcess( m_pQuestFT->GetUniqID() )->s_bCompleate )&&
			( m_pQuestFT->GetQuestTarget() == CsQuest::QTG_COMPLETE ) )
		{
			( (cQuestComp*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_QUEST_COMP ) )->SetTarget( g_pNpcMng->GetDigivice(), m_pQuestFT->GetUniqID() );
			if( m_pQuestFT->GetModelID_Comp() != 0 )
			{
				g_pNpcMng->SetZoomNpc( g_pNpcMng->LoadZoomNpcTemp( m_pQuestFT->GetModelID_Comp() ) );
			}
		}
		// 아니라면 - 남아 있는 디지바이스 퀘가 있는지 체크
		else 
		{
			if( g_pDataMng->GetQuestOwner()->IF_DigiviceQuest_Exist() )
				g_pDataMng->GetQuestOwner()->IF_DigiviceQuest_Open();

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

		int nEventSceneNum = nsCsFileTable::g_pQuestMng->GetQuest( m_pQuestFT->GetUniqID() )->m_nEvent[ CsQuest::EVENT_QUEST_START ];
		if( nsCsFileTable::g_pSceneDataMng->IsData( (DWORD)nEventSceneNum ) == true )
		{
			g_pEventScene->EventStart( nEventSceneNum );	
		}
		nEventSceneNum = nsCsFileTable::g_pQuestMng->GetQuest( m_pQuestFT->GetUniqID() )->m_nEvent[ CsQuest::EVENT_QUEST_PROC ];
		if( nsCsFileTable::g_pSceneDataMng->IsData( (DWORD)nEventSceneNum ) == true )
		{
			g_pEventScene->EventStart( nEventSceneNum );	
		}		
	}
}

