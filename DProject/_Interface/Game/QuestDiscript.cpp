
#include "stdafx.h"
#include "QuestDiscript.h"


cQuestDiscript::cQuestDiscript()
:m_OpenDigimonID(0),
m_pkBTOpenSlot(NULL),
m_OpenBaseDigimonID(0)
{

}

void cQuestDiscript::Destroy()
{
	cBaseWindow::Delete();
}

void cQuestDiscript::DeleteResource()
{
	DeleteScript();

	SAFE_NIDELETE( m_pMoney );
	m_StringBody.Delete();
	mEvoSlot.Delete();
}

void cQuestDiscript::Create(int nValue /* = 0  */)
{
	int nDelta = 57;

	if( IsLive() == false )
	{
		CsSIZE size( 32, 32 );
		CsPoint pos( 18, 320 + nDelta );
		for( int i=0; i<6; ++i )
		{
			m_RequiteItemIFIcon[ i ].Init( CsRect( pos, size ) );
			pos.x += 47;
		}
	}
	else
	{
		SetPos( g_pGameIF->GetQuestList()->GetDiscriptPos() );
		ResetDevice();
	}

	if( cBaseWindow::Init() == false )
		return;

	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUEST_LIST ) );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	ti.SetText( (int)0 );
	ti.s_eTextAlign = DT_RIGHT;

	InitScript( "QuestList\\Quest_Conwin.tga", g_pGameIF->GetQuestList()->GetDiscriptPos() , CsPoint( 302, 491 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP, false );
	AddTitle( UISTRING_TEXT( "QUESTDISCRIPT_TITLE_QUEST_INFO" ).c_str() );
	
	// 포기 버튼
	m_pDropButton = AddButton( CsPoint( 233, 399 + nDelta ), CsPoint( 61, 29 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "QUESTDISCRIPT_BTN_GIVEUP" ).c_str() );	

	ti.SetText( UISTRING_TEXT( "QUESTDISCRIPT_QUEST_REWARD" ).c_str() );
	ti.s_eFontSize = CFont::FS_11;
	ti.s_Color = NiColor( 84/255.0f, 198/255.0f, 239/255.0f );
	AddText( &ti, CsPoint( 85, 285 + nDelta ) );

	// 경험치
	ti.s_eFontSize = CFont::FS_10;
	ti.s_Color = NiColor::WHITE;
	m_pExpText = AddText( &ti, CsPoint( 130, 367 + nDelta ) );	

	// 스크롤바
	m_pScrollBar = AddScrollBar( cScrollBar::TYPE_1, CsPoint( 274, 38 ), CsPoint( 16, 228 + nDelta ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint( 10, 34 ), CsPoint( 273, 272 + nDelta ) ), 15 );	

	// 나가기 버튼
	m_pCancelButton = AddButton( CsPoint( 280, 6 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );

	SetLimitScreen( false );

	// 돈
	m_pMoney = NiNew cMoneyIF;
	m_pMoney->Init( cMoneyIF::DISP_SIMPLE_1, (u8)0, false );

	m_pFTQuest = NULL;

	m_OpenDigimonID = 0;
	m_OpenBaseDigimonID = 0;
}

bool cQuestDiscript::Close(bool bSound /* = true  */)
{
	if( IsEnableWindow() == false )
		return false;

	if( bSound )
	{
		PlayCloseSound();
	}

	//DeleteResource();
	SetShowWindow( false );

	return true;
}

void cQuestDiscript::Update(float const& fDeltaTime)
{

}

cBaseWindow::eMU_TYPE
cQuestDiscript::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
 	if( muReturn == MUT_OUT_WINDOW )	
 		return muReturn;

	if( m_pScrollBar->Update_ForMouse() != cScrollBar::ACTION_NONE )
		return muReturn;

	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		Close();		
		if( g_pGameIF->GetQuestList() )
		{
			g_pGameIF->GetQuestList()->ReleaseSelectQuest();
		}
		return muReturn;
	default:
		return muReturn;
	}

	// 보상 아이템 툴팁 창
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
	for( int i=0; i<m_nRequiteItemCount; ++i )
	{
		if( m_RequiteItemIFIcon[ i ].MouseOn( ptMouseLocalPos ) == true )
		{
			CsPoint pos = m_RequiteItemIFIcon[ i ].GetPos() + GetRootClient();
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 32, 32 ), 
				TOOLTIP_MAX_SIZE, cTooltip::ITEM, m_RequiteItemInfo[ i ].m_nType, cBaseWindow::WT_QUEST_DISCRIPT );
		}
	}
	if(m_pkBTOpenSlot != NULL && m_pkBTOpenSlot->PtInWindowRect(CURSOR_ST.GetPos()))
	{
#ifdef VERSION_USA
		int nMaxTooltipX = 340;
#else
		int nMaxTooltipX = TOOLTIP_MAX_SIZE;
#endif
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( m_pkBTOpenSlot->GetPos(), CsPoint( 32, 32 ), 
			TOOLTIP_MAX_SIZE, cTooltip::QUICK_EVOL_SIMPLE, m_OpenBaseDigimonID, m_OpenDigimonID );
	}
	if(m_pDropButton->Update_ForMouse() == cButton::ACTION_CLICK)
	{
// #ifdef NEW_TUTORIAL_SYSTEM
// 		if( g_pResist->IsTutorial())
// 		{
// #if ( defined VERSION_KOR || defined VERSION_USA )		//2016-03-10-nova 구 튜토리얼 퀘스트 포기 가능하도록
// 			cPrintMsg::PrintMsg( 20062 );
// #else
// 			cPrintMsg::PrintMsg( cPrintMsg::QUEST_DROP_BTN );
// #endif
// 			return MUT_NONE;
// 		}
// 		else
// #endif
		{
			if( m_pFTQuest->IsQuestType( CsQuest::QT_TUTORIAL ) )
			{
				DWORD dwQuestID = m_pFTQuest->GetUniqID();
				GAME_EVENT_ST.OnEvent( EVENT_CODE::TUTORIALQUEST_ASK_CANCEL, &dwQuestID );
			}
			else
				cPrintMsg::PrintMsg( cPrintMsg::QUEST_DROP_BTN );
			return muReturn;
		}	
	}	
	int nActive1;
	int nActive2;
	CsPoint ptPoint = CsPoint( 15, 40 ) + GetRootClient();			
	cString::sBUTTON* pButtonInfo = (cString::sBUTTON*)m_StringBody.Update_ForMouse_Control( nActive1, nActive2, ptPoint, 4 );

	// [10/20/2016 hyun] 퀘스트 헬퍼창 사용하지않는다
// 	if( pButtonInfo != NULL )
// 	{
// 		g_pGameIF->GetDynamicIF( cBaseWindow::WT_HELPER );
// 		g_pGameIF->GetHelper()->SetMessage( pButtonInfo->s_nValue1 );
// 	}	
	// @@ 여기까지

	return muReturn;
}

void cQuestDiscript::Render()
{
	RenderScript();

	m_StringBody.Render( CsPoint( 15, 40 ) + GetRootClient(), 4 );

	m_pMoney->Render( CsPoint( 149, 364 + 57 ) + GetRootClient() );

	CsPoint pos;
	for( int i=0; i<m_nRequiteItemCount; ++i )
	{
		pos = m_RequiteItemIFIcon[ i ].GetPos() + GetRootClient();
		g_pIconMng->RenderItem( &m_RequiteItemInfo[ i ], pos, true );
		// 마스크 있다면 마스크 랜더링
		if( m_RequiteItemIFIcon[ i ].s_eState != sIFIcon::NONE )
		{
			m_RequiteItemIFIcon[ i ].RenderMask( pos, CsPoint( 32, 32 ) );
		}
	}
	mEvoSlot.Render(GetRootClient());
}

void cQuestDiscript::ResetDevice()
{	
	ResetDeviceScript();

	if( m_pMoney )
		m_pMoney->ResetDevice();

	m_StringBody.ResetDevice();
	mEvoSlot.ResetDevice();
}

void cQuestDiscript::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
}

void cQuestDiscript::SetQuest( DWORD dwQuestFTID, bool bKeepScroll /*=false*/ )
{
	m_pFTQuest = nsCsFileTable::g_pQuestMng->GetQuest( dwQuestFTID );

	//===================================================================================================
	// 보상
	//===================================================================================================
	int nExp = 0;
	u8 nMoney = 0;
	m_nRequiteItemCount = 0;
	int nOpenSlotIdx = -1;
	m_OpenBaseDigimonID = 0;

	CsQuestRequiteGroup* pRequiteGroup = m_pFTQuest->GetRequiteGroup();
	for( int i=0; i<pRequiteGroup->GetListCount(); ++i )
	{
		CsQuestRequite* pRequite = pRequiteGroup->Get( i );
		assert_cs( pRequite->GetMethodID() == CsQuestRequite::RM_GIVE );
		switch( pRequite->GetType() )
		{
		case CsQuestRequite::MONEY:
			nMoney = pRequite->GetTypeID();
			break;
		case CsQuestRequite::EXP:
			//nExp = g_pDataMng->GetQuest()->GetQuestExp( m_pFTQuest, pRequite->GetTypeID() );
			nExp = pRequite->GetTypeID();
			break;
		case CsQuestRequite::ITEM:
			m_RequiteItemInfo[ m_nRequiteItemCount ].m_nType = pRequite->GetTypeID();
			m_RequiteItemInfo[ m_nRequiteItemCount ].m_nCount = pRequite->GetTypeCount();
			++m_nRequiteItemCount;
			break;
		case CsQuestRequite::EVOSLOT:
			{
				m_OpenBaseDigimonID = pRequite->GetTypeID();
				nOpenSlotIdx = pRequite->GetTypeCount();
			}
			break;
		default:
			assert_cs( false );
		}
	}

	m_pExpText->SetText( nExp );
	m_pMoney->SetMoney( nMoney );

	mEvoSlot.Delete();
	if(nOpenSlotIdx > 0 && m_OpenBaseDigimonID > 0)
	{
		const int StartLT = 57;
		const int fInterval = 47;
		CsSIZE size( 32, 32 );
		CsPoint pos( 18 + m_nRequiteItemCount *  fInterval, 320 + StartLT);

		CDigimonEvolveInfo* pDEvo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( m_OpenBaseDigimonID );
		if( NULL != pDEvo )
		{
			CDigimonEvolveObj* pEvoObj = pDEvo->GetEvolveObjByEvoSlot( nOpenSlotIdx );
			if( NULL != pEvoObj )
			{
				CsDigimon* pkTableInfo = nsCsFileTable::g_pDigimonMng->GetDigimon( pEvoObj->m_dwID );
				if(pkTableInfo != NULL)
				{
					m_OpenDigimonID = pEvoObj->m_dwID;

					std::string kPath = g_pModelDataMng->GetSmallModelIconFile( pkTableInfo->GetInfo()->s_dwModelID );
					cSprite* m_pkSPOpenSlot = NiNew cSprite;	// 1
					m_pkSPOpenSlot->Init( NULL, CsPoint::ZERO, CsPoint(32,32), kPath.c_str(), false, NiColor::WHITE, false );
					cString::sSPRITE* pkAddedSp = mEvoSlot.AddSprite( m_pkSPOpenSlot, pos, CsPoint(32,32)  );	
					if( pkAddedSp )
						pkAddedSp->SetAutoPointerDelete(true);

					m_pkBTOpenSlot = NiNew cButton;
					m_pkBTOpenSlot->Init( NULL, CsPoint::ZERO, CsPoint(32,32), "QuestRev\\Quest_Reward_Evoslot.tga", false );
					cString::sBUTTON* pkAddedBT = mEvoSlot.AddButton( m_pkBTOpenSlot, 0, pos - CsPoint(32,0), CsPoint(32,32)  );	
					if( pkAddedBT )
						pkAddedBT->SetAutoPointerDelete(true);
				}
			}
		}
	}

	//===================================================================================================
	// 바디
	//===================================================================================================
	m_StringBody.Delete();
	
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );	

	// 제목
	cString* pString = NiNew cString;
	ti.s_Color = g_pDataMng->GetQuest()->GetTitleColor( m_pFTQuest );

#ifndef _GIVE	// 로컬라이징 퀘스트 진행 시 ID 표시용	lks007
	TCHAR TempTest[ QUEST_SIMPLE_LEN ] = {0, };
	TCHAR szInt[32] = {0, };
	_stprintf_s( szInt, 32, _T(" Q_ID = %d"), m_pFTQuest->GetUniqID() );
	wcscpy( TempTest, m_pFTQuest->m_szTitleText );
	wcscat( TempTest, szInt );
	ti.SetText( TempTest );
#else
	ti.SetText( m_pFTQuest->m_szTitleText );
#endif

	pString->AddText( &ti );
	m_StringBody.AddTail( pString );
	m_StringBody.AddTailEmpty( CFont::FS_10 );

	// 보상 Npc
	pString = NiNew cString;
	ti.s_eFontSize = CFont::FS_10;
	ti.s_Color = FONT_WHITE;
	std::wstring wsReward;
	DmCS::StringFn::Format( wsReward, L"%s:", UISTRING_TEXT( "QUESTDISCRIPT_REWARD" ).c_str() );
	ti.SetText( wsReward.c_str() );
	pString->AddText( &ti );
	pString->TailAddSizeX( 2 );

	//TCHAR str[ MAX_PATH ];
	switch( m_pFTQuest->GetQuestTarget() )
	{
	case CsQuest::QTG_SELF:
		{
			if( m_pFTQuest->GetStartTarget_Type() == CsQuest::ST_NPC )
			{
				CsNpc::sINFO* pFTNpcInfo = nsCsFileTable::g_pNpcMng->GetNpc( m_pFTQuest->GetStartTarget_ID() )->GetInfo();
				assert_cs( pFTNpcInfo != NULL );				
			
				MakeFindWayButton( pString, m_pFTQuest->GetStartTarget_ID(), CsQuestCompRequire::NPCTALK, pFTNpcInfo->s_szName );
			}
			else
			{
				ti.SetText( UISTRING_TEXT( "COMMON_TXT_DIGIVICE" ).c_str() );
				pString->AddText( &ti );
			}
		}
		break;
	case CsQuest::QTG_NPC:
		{
			CsNpc::sINFO* pFTNpcInfo = nsCsFileTable::g_pNpcMng->GetNpc( m_pFTQuest->GetQuestTargetValue1() )->GetInfo();
			assert_cs( pFTNpcInfo != NULL );

			MakeFindWayButton( pString, m_pFTQuest->GetQuestTargetValue1(),CsQuestCompRequire::NPCTALK, pFTNpcInfo->s_szName );
		}
		break;
	case CsQuest::QTG_COMPLETE:
		{
			ti.SetText( UISTRING_TEXT( "COMMON_TXT_DIGIVICE" ).c_str() );
			pString->AddText( &ti );
		}
		break;
	default:
		assert_cs( false );
	}
	m_StringBody.AddTail( pString );


	// 위치
	CsMapList::sINFO* pFTMapList = NULL;
	switch( m_pFTQuest->GetQuestTarget() )
	{
	case CsQuest::QTG_SELF:
		{
			if( m_pFTQuest->GetStartTarget_Type() == CsQuest::ST_NPC )
			{
				CsNpc::sINFO* pFTNpcInfo = nsCsFileTable::g_pNpcMng->GetNpc( m_pFTQuest->GetStartTarget_ID() )->GetInfo();
				assert_cs( pFTNpcInfo != NULL );
				pFTMapList = nsCsMapTable::g_pMapListMng->GetList( pFTNpcInfo->s_dwMapID )->GetInfo();
				assert_cs( pFTMapList != NULL );							
			}
		}
		break;
	case CsQuest::QTG_NPC:
		{
			CsNpc::sINFO* pFTNpcInfo = nsCsFileTable::g_pNpcMng->GetNpc( m_pFTQuest->GetQuestTargetValue1() )->GetInfo();
			assert_cs( pFTNpcInfo != NULL );
			pFTMapList = nsCsMapTable::g_pMapListMng->GetList( pFTNpcInfo->s_dwMapID )->GetInfo();
			assert_cs( pFTMapList != NULL );			
		}
		break;
	case CsQuest::QTG_COMPLETE:
		{
		}
		break;
	default:
		assert_cs( false );
	}

	if( pFTMapList != NULL )
	{
		pString = NiNew cString;
		ti.s_eFontSize = CFont::FS_10;
		ti.s_Color = FONT_WHITE;
		std::wstring wsLocation;
		DmCS::StringFn::Format( wsLocation, L"%s : ", UISTRING_TEXT( "QUESTDISCRIPT_LOCATION" ).c_str() );
		ti.SetText( wsLocation.c_str() );
		pString->AddText( &ti );

		ti.SetText( pFTMapList->s_szMapDiscript.c_str() );
		ti.s_Color = FONT_WHITE;
		pString->AddText( &ti );
		m_StringBody.AddTail( pString );
	}

	// 퀘스트타입
	pString = NiNew cString;
	ti.s_eFontSize = CFont::FS_10;
	ti.s_Color = FONT_WHITE;
	std::wstring wsType;
	DmCS::StringFn::Format( wsType, L"%s : ", UISTRING_TEXT( "COMMON_TXT_TYPE" ).c_str() );
	ti.SetText( wsType.c_str() );
	pString->AddText( &ti );
	pString->TailAddSizeX( 2 );
	switch( m_pFTQuest->GetQuestType() )
	{
	case CsQuest::QT_MAIN:
		{
			ti.SetText( UISTRING_TEXT( "QUESTDISCRIPT_MAIN_QUEST" ).c_str() );			
			pString->AddText( &ti );
		}
		break;
	case CsQuest::QT_SUB:
		{
			ti.SetText( UISTRING_TEXT( "COMMON_TXT_SUBQUEST" ).c_str() );
			pString->AddText( &ti );
		}
		break;
	case CsQuest::QT_REPEAT:
		{
			ti.SetText( UISTRING_TEXT( "COMMON_TXT_REPEATABLE_QUEST" ).c_str() );
			pString->AddText( &ti );
		}
		break;
	case CsQuest::QT_DAILY:
		{
			ti.SetText( UISTRING_TEXT( "COMMON_TXT_DAILY_QUEST" ).c_str() );
			pString->AddText( &ti );
		}
		break;
	case CsQuest::QT_EVENTREPEAT:
		{
			ti.SetText( UISTRING_TEXT( "COMMON_TXT_EVENT_QUEST" ).c_str() );
			pString->AddText( &ti );
		}
		break;
// 		
// 	case CsQuest::QT_TIME:
// 		{
// 			ti.SetText( _LAN( "시간제한 퀘스트" ) );
// 			pString->AddText( &ti );
// 		}
// 		break;
// 		
	case CsQuest::QT_JOINTPROGRESS:
		{		
			ti.SetText( UISTRING_TEXT( "QUEST_JOGRESS_QUEST" ).c_str() );
			pString->AddText( &ti );
		}
		break;
#ifdef SDM_TAMER_XGUAGE_20180628
	case CsQuest::QT_XANTI_JOINTPROGRESS:
		{		
			ti.SetText( UISTRING_TEXT( "QUEST_XJOGRESS_QUEST" ).c_str() );
			pString->AddText( &ti );
		}
		break;
#endif
	case CsQuest::QT_TUTORIAL:
		{
			ti.SetText( UISTRING_TEXT( "COMMON_TXT_TUTORIAL_QUEST" ).c_str() );
			pString->AddText( &ti );
		}break;
	default:
		assert_cs( false );
	}
	m_StringBody.AddTail( pString );
	m_StringBody.AddTailEmpty( CFont::FS_10 );

	int nSimpleTextCut = 234;
	int nbodyTextCut = 232;

	// 내용
	pString = NiNew cString;
	ti.s_eFontSize = CFont::FS_12;
	ti.s_Color = NiColor( 43/255.0f, 158/255.0f, 236/255.0f );

#ifndef _GIVE	// 로컬라이징 퀘스트 진행 시 ID 표시용	lks007
	CsQuestCompRequireGroup* TempGroup = &g_pDataMng->GetQuest()->GetProcess( m_pFTQuest->GetUniqID() )->s_CurCompRequireGroup;
	TCHAR szCompID[QUEST_SIMPLE_LEN] = {0, };

	if(TempGroup && TempGroup->GetList())
	{
		int TempSize = TempGroup->GetList()->size();
		if( TempSize == 1 )
			_stprintf_s( szCompID, QUEST_SIMPLE_LEN, _T("Type = %d, ID = %d"), TempGroup->Get(0)->GetType(), TempGroup->Get(0)->GetTypeID() );
		else if( TempSize != 0 )
			_stprintf_s( szCompID, QUEST_SIMPLE_LEN, _T("Type = %d, ID = %d, Type = %d, ID = %d"), 
			TempGroup->Get(0)->GetType(), TempGroup->Get(0)->GetTypeID(), TempGroup->Get(1)->GetType(), TempGroup->Get(1)->GetTypeID() );
	}

	wcscpy( TempTest, UISTRING_TEXT( "QUESTDISCRIPT_OBJECTIVE" ).c_str() );	
	wcscat( TempTest, szCompID );
	ti.SetText( TempTest );
#else
	ti.SetText( UISTRING_TEXT( "QUESTDISCRIPT_OBJECTIVE" ).c_str() );	
#endif
	pString->AddText( &ti );
	m_StringBody.AddTail( pString );
	ti.s_Color = FONT_WHITE;
	ti.s_eFontSize = CFont::FS_10;
	g_pStringAnalysis->Cut_Parcing( &m_StringBody, nSimpleTextCut, m_pFTQuest->m_szSimpleText, &ti );

	ti.s_Color = NiColor( 1, 1, 0 );
	ti.s_eFontSize = CFont::FS_10;
	TCHAR szHelper[ QUEST_HELPER_LEN ];
	CsQuestCompRequireGroup* pApplyGroup = &g_pDataMng->GetQuest()->GetProcess( m_pFTQuest->GetUniqID() )->s_CurCompRequireGroup;
	SAFE_POINTER_RET( pApplyGroup );

	g_pStringAnalysis->Quest_Parcing( szHelper, QUEST_HELPER_LEN, m_pFTQuest->m_szHelperText, pApplyGroup );
	g_pStringAnalysis->Cut_Parcing( &m_StringBody, nSimpleTextCut*2, szHelper, &ti );
	
	const CsQuestCompRequire::LIST* pDestList =  pApplyGroup->GetList(); // 퀘스트 수행 목록 리스트
	CsQuestCompRequire::LIST::const_iterator iter	 = pDestList->begin();
	CsQuestCompRequire::LIST::const_iterator enditer = pDestList->end();

	for( int i = 0; i < pDestList->size(); ++i )
		m_StringBody.RemoveTail();

	for( int i = 0; iter != enditer; ++ iter, ++i ) // 수행 목록
	{
		if( i < g_pStringAnalysis->m_vecStringTotalText.size() ) // 표시할 스트링이 있을때만 
		{
			m_StringBody.AddTail( MakeFindWayButton((*iter), g_pStringAnalysis->m_vecStringTotalText[i].c_str()) );
		}
	}

	bool bComplete = g_pDataMng->GetQuest()->GetProcess( m_pFTQuest->GetUniqID() )->s_bCompleate; // 완료 여부
	if( pDestList->empty() ) // 퀘스트 수행 목록이 없다
	{
		if( !g_pStringAnalysis->m_vecStringTotalText.empty() )
		{
			m_StringBody.RemoveTail();
			m_StringBody.AddTail( MakeFindWayButton( m_pFTQuest->GetQuestTargetValue1(), CsQuestCompRequire::NPCTALK, 
				g_pStringAnalysis->m_vecStringTotalText[0].c_str(),	bComplete ? FONT_GREEN : FONT_WHITE ) );
		}
	}
	
	m_StringBody.AddTailEmpty( CFont::FS_10 );

	// 설명
	pString = NiNew cString;
	ti.s_eFontSize = CFont::FS_12;
	ti.s_Color = NiColor( 43/255.0f, 158/255.0f, 236/255.0f );
	ti.SetText( UISTRING_TEXT( "QUESTDISCRIPT_DETAIL" ).c_str() );
	pString->AddText( &ti );
	m_StringBody.AddTail( pString );
	ti.s_Color = FONT_WHITE;
	ti.s_eFontSize = CFont::FS_10;

	TCHAR sz[ QUEST_BODY_LEN + 64 ];
	g_pStringAnalysis->StringTransfer( sz, QUEST_BODY_LEN + 64, m_pFTQuest->m_szBodyText );
	g_pStringAnalysis->Cut_LinkParcing( &m_StringBody, nbodyTextCut, sz, &ti, cButton::IMAGE_ALPHA_1 );

	m_StringBody.SetBuddy( m_pScrollBar );
	m_pScrollBar->SetRange( CsPoint( 0, m_StringBody.GetSize() + 4 ) );
	
	if( bKeepScroll == false )
	{
		m_pScrollBar->SetCurPosIndex( 0 );
	}
}


void cQuestDiscript::OnSetFocus()
{
	cBaseWindow::OnSetFocus();
	if( g_pGameIF->GetQuestList() )
		g_pGameIF->SetFocusWindow( g_pGameIF->GetQuestList(), false );
}

void cQuestDiscript::ProcessFindWay( void* pSender, void* pData )
{
	SAFE_POINTER_RET(pSender);
	cButton* pButton = static_cast<cButton*>(pSender);
	cQuestFindWay findWayInfo = *static_cast<cQuestFindWay*>(pButton->GetUserData());

	if( g_pHelpArrow->SetArrowPoint( findWayInfo.m_nDestTableID, findWayInfo.m_nDestMapID, findWayInfo.m_nDestType, const_cast<TCHAR*>(findWayInfo.m_strDestName.c_str()) ))
	{
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ZONEMAP ) )
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_ZONEMAP );
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_ZONEMAP );
	}
}

cString* cQuestDiscript::MakeFindWayButton( int nTargetID, CsQuestCompRequire::eTYPE eQuestCompleteType, const TCHAR* pButtonText, NiColor color )
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );

	CsPoint ptPos1;
	int nBase1;
	cText::GetStringSize( &ti, ptPos1, nBase1, pButtonText );
	ti.SetText( pButtonText );

	cString* pDestString = NiNew cString;
	cQuestFindWay FindWayInfo;
	if( FindWayInfo.Initialize(nTargetID , eQuestCompleteType) ) // 길찾기 정보 초기화 성공
	{
		ti.s_Color = color;

		cButton* pDestButton = NiNew cButton;
		pDestButton->Init( NULL, CsPoint::ZERO, ptPos1, cButton::IMAGE_ALPHA_1, &ti, false, CsPoint( 0, -1 ) ); 			
		pDestButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cQuestDiscript::ProcessFindWay );
		pDestButton->SetUserData( new cQuestFindWay(FindWayInfo) );
		pDestString->AddButton( pDestButton, 0, CsPoint::ZERO, ptPos1 );
	}
	else	// 초기화 실패
	{
		ti.s_Color = color;
		pDestString->AddText( &ti );
	}
	return pDestString;
}

bool cQuestDiscript::MakeFindWayButton( cString* pString, int nTargetID, CsQuestCompRequire::eTYPE eQuestCompleteType, const TCHAR* pButtonText )
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );

	cQuestFindWay FindWayInfo;
	if( FindWayInfo.Initialize(nTargetID, eQuestCompleteType) ) // 길찾기 정보 초기화 성공
	{
		CsPoint ptPos;
		int nBase;
		cText::GetStringSize( &ti, ptPos, nBase, pButtonText );

		ti.SetText( pButtonText );
		ti.s_Color = FONT_GREEN;

		cButton* pButton = NiNew cButton;
		pButton->Init( NULL, CsPoint::ZERO, ptPos, cButton::IMAGE_ALPHA_1, &ti, false, CsPoint( 0, -1 ) );
		pButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cQuestDiscript::ProcessFindWay );
		pButton->SetUserData( new cQuestFindWay(FindWayInfo) );
		pString->AddButton( pButton, 0, CsPoint::ZERO, ptPos );
	}
	else // 실패
	{
		ti.SetText( pButtonText );
		pString->AddText( &ti );
	}
	return true;
}

cString* cQuestDiscript::MakeFindWayButton( CsQuestCompRequire* pCompleteInfo, const TCHAR* pButtonText )
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );

	CsPoint ptPos1;
	int nBase1;
	cText::GetStringSize( &ti, ptPos1, nBase1, pButtonText );
	ti.SetText( pButtonText );

	cString* pDestString = NiNew cString;
	cQuestFindWay FindWayInfo;
	if( FindWayInfo.Initialize(pCompleteInfo->GetTypeID(), pCompleteInfo->GetType(), pCompleteInfo->GetTargetID()) ) // 길찾기 정보 초기화 성공
	{
		if( pCompleteInfo->IsCompleat() )
			ti.s_Color = FONT_GREEN;

		cButton* pDestButton = NiNew cButton;
		pDestButton->Init( NULL, CsPoint::ZERO, ptPos1, cButton::IMAGE_ALPHA_1, &ti, false, CsPoint( 0, -1 ) ); 			
		pDestButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cQuestDiscript::ProcessFindWay );
		pDestButton->SetUserData( new cQuestFindWay(FindWayInfo) );
		pDestString->AddButton( pDestButton, 0, CsPoint::ZERO, ptPos1 );
	}
	else // 실패
	{
		ti.s_Color = FONT_WHITE;
		pDestString->AddText( &ti );
	}
	return pDestString;
}
