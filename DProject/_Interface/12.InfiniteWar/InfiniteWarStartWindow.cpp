#include "stdafx.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

#include "InfiniteWarStartWindow.h"

//////////////////////////////////////////////////////////////////////////
CInfiniteWarStartWindow::sStepUIControls::sStepUIControls():m_pMonsterImage(NULL),m_pMonsterName(NULL),m_pBackGlow(NULL)
{

}

CInfiniteWarStartWindow::sStepUIControls::~sStepUIControls()
{

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CInfiniteWarStartWindow::CInfiniteWarStartWindow():
m_pCloseBtn(NULL),m_pStepList(NULL),m_pNextChallengeBtn(NULL),m_pTotalPoint(NULL)
{

}

CInfiniteWarStartWindow::~CInfiniteWarStartWindow()
{
	Destroy();

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

cBaseWindow::eWINDOW_TYPE CInfiniteWarStartWindow::GetWindowType()
{ 
	return WT_INFINITEWAR_START_WINDOW; 
}

void CInfiniteWarStartWindow::Destroy()
{
	cBaseWindow::Delete();	
}

void CInfiniteWarStartWindow::DeleteResource()
{
	DeleteScript();	
}

void CInfiniteWarStartWindow::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	CsPoint winPos( (g_nScreenWidth >> 1) - (806 >> 1), (g_nScreenHeight>>1) - (644>>1) );
	InitScript( "InfiniteMatch\\Infinite_Match_window.tga", winPos, CsPoint( 806, 644 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );
	AddTitle( UISTRING_TEXT("INFINITEWAR_START_WINDOW_TITLE").c_str(),CsPoint( 0, 13 ), CFont::FS_16 );

	m_pCloseBtn = AddButton( CsPoint( 762, 16 ), CsPoint( 30, 28 ), CsPoint( 0, 28 ), "CommonUI\\CommonUI_close.tga" );	
	if( m_pCloseBtn )
		m_pCloseBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CInfiniteWarStartWindow::_CloseWindow );

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_13);
		ti.SetText( UISTRING_TEXT("INFINITEWAR_START_WINDOW_BATTLE_STEP_TEXT").c_str() );// 대전 단계
		ti.s_eTextAlign = DT_CENTER;
		AddText( &ti, CsPoint( 108, 77 ) );
	}

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_13);
		ti.SetText( UISTRING_TEXT("INFINITEWAR_START_WINDOW_BATTLE_TARGET_TEXT").c_str() );// 대전 상대
		ti.s_eTextAlign = DT_CENTER;
		AddText( &ti, CsPoint( 377, 77 ) );
	}

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_13);
		ti.SetText( UISTRING_TEXT("INFINITEWAR_START_WINDOW_GETPOINT_TEXT").c_str() );// 포인트 획득
		ti.s_eTextAlign = DT_CENTER;
		AddText( &ti, CsPoint( 667, 77 ) );
	}

	m_pStepList = NiNew cListBox;
	if( m_pStepList )
	{
		m_pStepList->Init( m_pRoot, CsPoint(32, 105), CsPoint( 743, 420 ), NULL, false );
		cScrollBar* pScrollBar = NiNew cScrollBar;
		if(pScrollBar)
		{
			pScrollBar->Init( cScrollBar::TYPE_SEALMASTER, NULL, CsPoint( 0, 0 ), CsPoint( 16, 260 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint(0,0), CsPoint( 743, 420 ) ), 5, false );
			m_pStepList->SetScrollBar( pScrollBar );
		}

		AddChildControl( m_pStepList );
	}


	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_13);
		ti.SetText( UISTRING_TEXT("INFINITEWAR_START_WINDOW_CURRENT_POINT_TEXT").c_str() );// 현재 획득 포인드
		ti.s_eTextAlign = DT_RIGHT;
		AddText( &ti, CsPoint( 555, 548 ) );
	}

	{
		cText::sTEXTINFO ti;// 현재 획득 포인드
		ti.Init(CFont::FS_13,NiColor(0.0f, 1.0f, 0.0f));
		ti.s_eTextAlign = DT_RIGHT;
		ti.SetText(L"");
		m_pTotalPoint = AddText( &ti, CsPoint( 736, 548 ) );
	}

	m_pNextChallengeBtn = AddButton( CsPoint( 272, 588 ), CsPoint( 128, 48 ), CsPoint( 0, 48 ), "CommonUI\\CommonUI_btn_s.tga" );
	if( m_pNextChallengeBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_13);
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("INFINITEWAR_START_WINDOW_START_BTN").c_str() );
		m_pNextChallengeBtn->SetText( &ti );
		m_pNextChallengeBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CInfiniteWarStartWindow::_NextChallengeBtnEvent );			
	}

	m_pGiveupBtn = AddButton( CsPoint( 401, 588 ), CsPoint( 128, 48 ), CsPoint( 0, 48 ), "CommonUI\\CommonUI_btn_s.tga" );
	if( m_pGiveupBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_13);
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("INFINITEWAR_START_WINDOW_GIVEUP_BTN").c_str() );
		m_pGiveupBtn->SetText( &ti );
		m_pGiveupBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CInfiniteWarStartWindow::_ChallengeGiveupBtnEvent );
	}

	_CreateBattleList();

	// 파티장만 전투 시작 및 종료 권한 부여
	if( !GetSystem()->IsPartyMaster() || GetSystem()->IsStageAllClear() )
	{
		if( m_pNextChallengeBtn )
			m_pNextChallengeBtn->SetEnable(false);

		if( m_pGiveupBtn )
			m_pGiveupBtn->SetEnable(false);
	}

	_UpdateTotalPoint( GetSystem()->GetTotalHavePoint() );
	_SetCurrentChallengeStage( GetSystem()->GetNextChallengeStageIdx() );
}

void CInfiniteWarStartWindow::_CreateBattleList()
{
	CInfiniteWarContents::MAP_WARINFO const* mapInfo = GetSystem()->GetInfiniteWarTatalInfo();
	SAFE_POINTER_RET( mapInfo );
	
	CInfiniteWarContents::MAP_WARINFO_CIT it = mapInfo->begin();
	for( ; it != mapInfo->end(); ++it )
		_makeSetListItem( it->first, it->second, m_pStepList );
}

void CInfiniteWarStartWindow::_makeSetListItem(int const& nStep, CInfiniteWarContents::sInfiniteWarInfo const& stepInfo, cListBox * pList)
{
	SAFE_POINTER_RET( pList );

	cListBoxItem* pAddItem = NiNew cListBoxItem;
	SAFE_POINTER_RET( pAddItem );

	cString * pItem = NiNew cString;
	SAFE_POINTER_RET( pItem );

	sStepUIControls controls;

	// 배경 Glow
	controls.m_pBackGlow = NiNew cSprite;
	if( controls.m_pBackGlow )
	{
		controls.m_pBackGlow->Init( NULL, CsPoint::ZERO, CsPoint( 652, 69 ), "InfiniteMatch\\Infinite_Match_glow.tga", false );
		cString::sSPRITE* paddImg = pItem->AddSprite( controls.m_pBackGlow, CsPoint::ZERO, CsPoint(  652, 69 )  );
		if( paddImg )
			paddImg->SetAutoPointerDelete(true);

		paddImg->SetVisible(false);
	}

	{	// 단계
		cText::sTEXTINFO ti2;
		ti2.Init( &g_pEngine->m_FontText, CFont::FS_12 );
		std::wstring msg = UISTRING_TEXT("INFINITEWAR_START_WINDOW_STEP_TEXT");
		DmCS::StringFn::ReplaceAll( msg, L"#step#", nStep );
		ti2.SetText( msg.c_str() );
		ti2.s_eTextAlign = DT_CENTER;
		pItem->AddText( &ti2, CsPoint(60,27) );	
	}

	cSprite* pImgBack = NiNew cSprite;
	if( pImgBack )
	{
		pImgBack->Init( NULL, CsPoint::ZERO, CsPoint( 62, 62 ), "InfiniteMatch\\Infinite_Match_circle.tga", false );
		cString::sSPRITE* paddImg = pItem->AddSprite( pImgBack, CsPoint( 170, 4 ), CsPoint( 62, 62 )  );
		if( paddImg )
			paddImg->SetAutoPointerDelete(true);
	}

	controls.m_pMonsterImage= NiNew cSprite;
	if( controls.m_pMonsterImage )
	{
		controls.m_pMonsterImage->Init( NULL, CsPoint::ZERO, CsPoint( 57, 57 ), stepInfo.GetMonsterIconFile().c_str(), false,NiColor::WHITE, false );
		cString::sSPRITE* paddImg = pItem->AddSprite( controls.m_pMonsterImage, CsPoint( 172, 6 ), CsPoint( 57, 57 )  );
		if( paddImg )
			paddImg->SetAutoPointerDelete(true);
	}

	{	// 클리어한 디지몬 이름
		cText::sTEXTINFO ti2;
		ti2.Init( &g_pEngine->m_FontText, CFont::FS_12 );
		controls.m_pMonsterName = pItem->AddText( &ti2, CsPoint(244,27) );
		controls.m_pMonsterName->SetText( stepInfo.GetMonsterName().c_str() );
	}

	// 획득 포인드 정보
	if( stepInfo.s_nWinPoint > 0 )
	{
		cText::sTEXTINFO ti2;
		ti2.Init( &g_pEngine->m_FontText, CFont::FS_12,NiColor(0.0f,1.0f,0.0f) );
		std::wstring msg = UISTRING_TEXT("INFINITEWAR_START_WINDOW_WIN_POINT_TEXT");
		DmCS::StringFn::ReplaceAll( msg, L"#win#", stepInfo.s_nWinPoint );
		ti2.SetText( msg.c_str() );
		pItem->AddText( &ti2, CsPoint(600,15) );	
	}
	
	// 패배 포인드 정보
	if( stepInfo.s_nLosePoint > 0 )
	{
		cText::sTEXTINFO ti2;
		ti2.Init( &g_pEngine->m_FontText, CFont::FS_12,NiColor(1.0f,0.0f,0.0f) );
		std::wstring msg = UISTRING_TEXT("INFINITEWAR_START_WINDOW_LOSE_POINT_TEXT");
		DmCS::StringFn::ReplaceAll( msg, L"#lose#", stepInfo.s_nLosePoint );
		ti2.SetText( msg.c_str() );
		pItem->AddText( &ti2, CsPoint(600,38) );	
	}

	cSprite* pLineBack = NiNew cSprite;
	if( pLineBack )
	{
		pLineBack->Init( NULL, CsPoint::ZERO, CsPoint( 730, 7 ), "InfiniteMatch\\Infinite_Match_line.tga", false );
		cString::sSPRITE* paddImg = pItem->AddSprite( pLineBack, CsPoint(0,63), CsPoint( 730, 7 )  );
		if( paddImg )
			paddImg->SetAutoPointerDelete(true);
	}

	pAddItem->SetItem( pItem );
	m_mapStepControls.insert( std::make_pair( nStep, controls ) );
	pList->AddItem( pAddItem );
}

void CInfiniteWarStartWindow::_UpdateStageInfo( int const& nStage )
{
	std::map<int, sStepUIControls>::iterator it = m_mapStepControls.find( nStage );
	if( it == m_mapStepControls.end() )
		return;

	CInfiniteWarContents::sStageInfo const* pStageInfo = GetSystem()->GetStageInfo( nStage );
	SAFE_POINTER_RET( pStageInfo );

	if( it->second.m_pMonsterImage )
		it->second.m_pMonsterImage->ChangeTexture( pStageInfo->GetMonsterIconFile().c_str() );

	if( it->second.m_pMonsterName )
		it->second.m_pMonsterName->SetText( pStageInfo->GetMonsterName().c_str() );
}

void CInfiniteWarStartWindow::Update(float const& fDeltaTime)
{
	if( GetSystem() )
	{
		CsC_AvObject* pTarget = GetSystem()->GetTargetNpc();	
		if( pTarget )
		{
			assert_cs( pTarget->GetLeafRTTI() == RTTI_NPC );
			if( ( (CNpc*)pTarget )->CheckTamerDist() == false )
				_CloseWindow(NULL, NULL);
		}
	}

	_UpdateMoveWindow();
}

bool CInfiniteWarStartWindow::OnEscapeKey()
{
	_CloseWindow(NULL, NULL);
	return true;
}

cBaseWindow::eMU_TYPE CInfiniteWarStartWindow::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	//if( muReturn == MUT_OUT_WINDOW )	
	//	return muReturn;

	if( m_pCloseBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if( m_pNextChallengeBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if( m_pGiveupBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if(m_pStepList && m_pStepList->Update_ForMouse(CURSOR_ST.GetPos()) )
		return muReturn;

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 700, 48 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();		

	return muReturn;
}

void CInfiniteWarStartWindow::_CloseWindow( void* pSender, void* pData )
{
	if( GetSystem() )
		GetSystem()->OpenCloseStartWindow(false);
}

void CInfiniteWarStartWindow::Render()
{
	RenderScript();
}

void CInfiniteWarStartWindow::ResetDevice()
{	
	ResetDeviceScript();
}

void CInfiniteWarStartWindow::_UpdateTotalPoint( DWORD const& dwTotalPoint )
{
	SAFE_POINTER_RET( m_pTotalPoint );
	std::wstring value = DmCS::StringFn::getNumberFormatW( dwTotalPoint );		
	std::wstring msg = UISTRING_TEXT("INFINITEWAR_START_WINDOW_CURRENT_POINT");
	DmCS::StringFn::ReplaceAll( msg, L"#havePoint#", value );
	m_pTotalPoint->SetText( value.c_str() );
}

void CInfiniteWarStartWindow::_NextChallengeBtnEvent(void* pSender, void* pData)
{
	GetSystem()->SendInfiniteWarTryNextStage_TargetNpc();
}

void CInfiniteWarStartWindow::_ChallengeGiveupBtnEvent(void* pSender, void* pData)
{
	GetSystem()->AskGiveup();
}

void CInfiniteWarStartWindow::_SetCurrentChallengeStage(int const& nChallengeStage)
{
	std::map<int, sStepUIControls>::iterator it = m_mapStepControls.find( nChallengeStage );
	for( ; it != m_mapStepControls.end(); ++it )
	{
		SAFE_POINTER_CON( it->second.m_pBackGlow );
		if( it->first == nChallengeStage )
			it->second.m_pBackGlow->SetVisible(true);
		else
			it->second.m_pBackGlow->SetVisible(false);
	}	
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool CInfiniteWarStartWindow::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eUpdate_Total_Point, this );
	GetSystem()->Register( SystemType::eUpdate_Stage_Info, this );
	return true;
}

void CInfiniteWarStartWindow::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eUpdate_Total_Point:
		{
			DWORD dwTotalPoint = 0;
			kStream >> dwTotalPoint;
			_UpdateTotalPoint( dwTotalPoint );
		}
		break;
	case SystemType::eUpdate_Stage_Info:
		{
			int nStageNumber = 0;
			kStream >> nStageNumber;
			_UpdateStageInfo( nStageNumber );
		}
		break;
	}
}