#include "StdAfx.h"
#include "Loading.h"
#include "../../Flow/Flow.h"
#include "../../Flow/FlowMgr.h"


CLoading::CLoading(void):m_pLoadBar(NULL)
{
}

CLoading::~CLoading(void)
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
	Destroy();
}


void CLoading::Destroy()
{
	DeleteScript();
}

void CLoading::Create()
{
	InitScript( NULL, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight), false );
	std::string file = GetSystem()->GetLoadMapImgFile();
	AddSprite( CsPoint( g_nScreenWidth/2 - 1024/2, g_nScreenHeight/2 - 768/2 ), CsPoint( 1024, 768 ), file.c_str(), true );

	_MakeBattleLoadUI();
	_MakeTip();
	_MakeLoadingGuageBar();
}

void CLoading::_MakeLoadingGuageBar()
{
	cScriptUI* pGuage = NiNew cScriptUI;
	SAFE_POINTER_RET( pGuage );

	pGuage->InitScript( NULL, CsPoint( (g_nScreenWidth/2) - (804/2), g_nScreenHeight - 40 - 23 ), CsPoint( 804, 40), false );

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_16 );
		ti.s_Color = FONT_WHITE;
		ti.s_eTextAlign = DT_CENTER;			
		ti.SetText( _T( "Loading...." ) );	
		pGuage->AddText( &ti, CsPoint( 804/2, 0 ) );
	}

	m_pLoadBar = NiNew cProgressBar;
	if( m_pLoadBar )
	{
		m_pLoadBar->Init( pGuage->GetRoot(), cProgressBar::BAR_1IMAGE, CsPoint( 0, 24 ), CsPoint( 804, 11 ), CsPoint::ZERO, "loading\\Bar.tga", false );
		m_pLoadBar->SetRange( CsPoint( 0, 100 ) );
		m_pLoadBar->SetBarPos( 0 );
		pGuage->AddChildControl( m_pLoadBar );
	}
	
	AddChildScriptUI( pGuage );
}

void CLoading::_MakeTip()
{
	std::wstring tipString = GetSystem()->GetLoadingTipString();
	if( tipString.empty() )
		return;

	std::string tipImg = GetSystem()->GetTipImgFile();
	if( tipImg.empty() )
		return;		

	cScriptUI* pTipUI = NiNew cScriptUI;
	SAFE_POINTER_RET( pTipUI );

	pTipUI->InitScript( NULL, CsPoint( 0, (g_nScreenHeight / 2) + 50 ), CsPoint( g_nScreenWidth, 278), false );

	float fRate = (float)550 / (float)DEFAULT_SCREEN_WIDTH;
	int x = (int)( fRate*g_nScreenWidth );
	CsPoint DefaultPoint = CsPoint( x, 222 );
	CsPoint TextSize;
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
	ti.s_Color = NiColor( 253.0f/ 255.0f, 1.0f, 0.0f );
	cText* pText = pTipUI->AddText( &ti, CsPoint::ZERO, true );
	if( pText )
	{
		pText->SetText( tipString.c_str(), 620 );

		TextSize = pText->GetStringSize() / 2;
		pText->SetPos( DefaultPoint - CsPoint( TextSize.x, 0 ) + CsPoint( 30, 0 ) );
	}


	pTipUI->AddSprite( CsPoint( DefaultPoint.x - TextSize.x - 143 - 25, 0), CsPoint( 278, 278 ), tipImg.c_str() );



	AddChildScriptUI( pTipUI );
}
//////////////////////////////////////////////////////////////////////////
// 배틀 로딩 UI
//////////////////////////////////////////////////////////////////////////
void CLoading::_MakeBattleLoadUI()
{
	LoadingContents::sBattleLoadingInfo const& sInfo = GetSystem()->GetBattleLoadingInfo();
		
	if( !sInfo.m_bBattleLoading )
		return;
	
	cScriptUI* pBattleLoadUI = NiNew cScriptUI;
	SAFE_POINTER_RET( pBattleLoadUI );

	pBattleLoadUI->InitScript( NULL, CsPoint( g_nScreenWidth/2 - 1024/2, g_nScreenHeight/2 - 768/2), CsPoint( 1024, 768), false);
	pBattleLoadUI->AddSprite( CsPoint::ZERO, CsPoint(1024, 128), "Battle\\BattleTypeSelect\\battle_mode_Top.tga");	
	pBattleLoadUI->AddSprite( CsPoint::ZERO, CsPoint( 1024, 768 ), "Battle\\Loading\\batte_Loading_Window.tga" );

	cImage * pVersusType = NiNew cImage;
	if( pVersusType )
	{
		CsPoint pos;
		pos.x = (1024/2) - (196 / 2);
		pos.y = ((768 - 150) / 2 + 150) - (93 / 2);
		pVersusType->Init( pBattleLoadUI->GetRoot(), pos, CsPoint(196, 93), "Battle\\BattleModeSelect\\battle_versus.tga", false );
		pVersusType->SetTexToken( CsPoint( 0, 93 ) );
		pBattleLoadUI->AddChildControl( pVersusType );
		pVersusType->SetState(sInfo.m_nBattleMode - 1);

// 		switch( sInfo.m_nBattleMode )
// 		{
// 		case eDuelSubCategory::SINGLE_DUEL:		pVersusType->SetState(cImage::NORMAL0);	break;
// 		case eDuelSubCategory::DOUBLE_DUEL:		pVersusType->SetState(cImage::NORMAL1);	break;
// 		case eDuelSubCategory::TRIPLE_DUEL:		pVersusType->SetState(cImage::NORMAL2);	break;
// 		case eDuelSubCategory::QUADRUPLE_DUEL:	pVersusType->SetState(cImage::NORMAL3);	break;
// 		case eDuelSubCategory::OCTUPLE_DUEL:	pVersusType->SetState(cImage::NORMAL4);	break;
// 		case eDuelSubCategory::SEXDECUPLE_DUEL:	pVersusType->SetState(cImage::NORMAL5);	break;
// 		}
	}

	int nPlayerCount = 0;
	switch( sInfo.m_nBattleMode )
	{
	case eDuelSubCategory::SINGLE_DUEL:		nPlayerCount = eDuelSubCategoryMaxTeamCNT::SINGLE;	break;
	case eDuelSubCategory::DOUBLE_DUEL:		nPlayerCount = eDuelSubCategoryMaxTeamCNT::DOUBLE;	break;
	case eDuelSubCategory::TRIPLE_DUEL:		nPlayerCount = eDuelSubCategoryMaxTeamCNT::TRIPLE;	break;
	case eDuelSubCategory::QUADRUPLE_DUEL:	nPlayerCount = eDuelSubCategoryMaxTeamCNT::QUADRUPLE;	break;
	case eDuelSubCategory::OCTUPLE_DUEL:	nPlayerCount = eDuelSubCategoryMaxTeamCNT::OCTUPLE;	break;
	case eDuelSubCategory::SEXDECUPLE_DUEL:	nPlayerCount = eDuelSubCategoryMaxTeamCNT::SEXDECUPLE;	break;
	}

	int nCount = 0;
	int nLineCount = nPlayerCount / 8;
	if( nLineCount > 0 )
		nCount = 8;
	else
	{
		nLineCount = 1;
		nCount = nPlayerCount % 8;
	}

	CsPoint size;
	size.x = 86 * nCount;
	size.y = 86 * nLineCount;

	cGridListBox* pBlueTeamList = NiNew cGridListBox;
	if( pBlueTeamList )
	{ 
		pBlueTeamList->Init( pBattleLoadUI->GetRoot(), CsPoint(1024 / 2 - (size.x / 2), 298 - (size.y / 2)), size, CsPoint::ZERO, CsPoint(86, 86), 
								   cGridListBox::LowRightDown, cGridListBox::LeftTop,NULL, false, 0 );

		BWait::sWait::PLAYER_INFO_CIT it = sInfo.m_myBattleTeam.begin();
		for( int n = 0 ; it != sInfo.m_myBattleTeam.end(); ++it, ++n )
			_AddTeamPlayerUI( pBlueTeamList, (*it), n );

		pBattleLoadUI->AddChildControl(pBlueTeamList);
	}

	cGridListBox* pRedTeamList= NiNew cGridListBox;
	if( pRedTeamList )
	{
		pRedTeamList->Init( pBattleLoadUI->GetRoot(), CsPoint(1024 / 2 - (size.x / 2), 623 - (size.y / 2)), size, CsPoint::ZERO, CsPoint(86, 86), 
			cGridListBox::LowRightDown, cGridListBox::LeftTop,NULL, false, 0 );

		BWait::sWait::PLAYER_INFO_CIT it = sInfo.m_otherBattleTeam.begin();
		for( int n = 0 ; it != sInfo.m_otherBattleTeam.end(); ++it, ++n )
			_AddTeamPlayerUI( pRedTeamList, (*it), n );

		pBattleLoadUI->AddChildControl(pRedTeamList);
	}

	AddChildScriptUI( pBattleLoadUI );	
}

void CLoading::_AddTeamPlayerUI( cGridListBox* pControl, BWait::sWait::sPlayerInfo const& playerList, int nCount )
{
	SAFE_POINTER_RET(pControl);

	cString* pString = NiNew cString;
	SAFE_POINTER_RET(pString);

	cSprite* pBackImg = NiNew cSprite;		
	pBackImg->Init( NULL,CsPoint::ZERO, CsPoint( 68, 68 ), "Battle\\BattleModeSelect\\Slot_Bg.tga", true, NiColor::WHITE );
	cString::sSPRITE* pImg = pString->AddSprite( pBackImg, CsPoint(9,9) );
	if( pImg )
		pImg->SetAutoPointerDelete(true);

	if( playerList.m_bIsMe )
	{
		cSprite* pMyMaskImg = NiNew cSprite;		
		pMyMaskImg->Init( NULL, CsPoint::ZERO, CsPoint(86, 86), "Battle\\BattleModeSelect\\SelectedSlot_Img.tga", true, NiColor::WHITE );
		cString::sSPRITE* paddImg = pString->AddSprite( pMyMaskImg,CsPoint::ZERO );
		if( paddImg )
			paddImg->SetAutoPointerDelete(true);		
	}	

	cSprite* pDigimonImg = NiNew cSprite;		
	pDigimonImg->Init( NULL, CsPoint::ZERO, CsPoint( 64, 64 ), playerList.m_DigimonIconFileName.c_str(), true, NiColor::WHITE, false );
	cString::sSPRITE* paddImg = pString->AddSprite( pDigimonImg,CsPoint(11,11) );
	if( paddImg )
		paddImg->SetAutoPointerDelete(true);

	
	cGridListBoxItem* addItem  = NiNew cGridListBoxItem(nCount, CsPoint( 86, 86 ));			
	addItem->SetItem(pString);
	if( playerList.m_bIsMe )
		pControl->InsertItem( 0, addItem );
	else
		pControl->AddItem( addItem );
}
//////////////////////////////////////////////////////////////////////////

void CLoading::ResetDevice()
{
	ResetDeviceScript();
}

void CLoading::Render()
{
	RenderScript();
}

// =========================================================
// 100%로를 기준으로 로딩바 다음 단계 설정
// =========================================================
void CLoading::_UpdateProgressBar(int const& nValue)
{
	SAFE_POINTER_RET( m_pLoadBar );
	m_pLoadBar->SetBarPos( nValue );
}

void CLoading::Update(float const& fDeltaTime)
{
}

void CLoading::_PlayerLoadEnd(ContentsStream const& kStream)
{
// 	std::wstring playerName;
// 	kStream >> playerName;
// 
// 	std::map<std::wstring, cText*>::iterator it = m_BattlePlayer.find( playerName );
// 	if( it == m_BattlePlayer.end() )
// 		return;
// 
// 	SAFE_POINTER_RET( it->second );
// 
// 	it->second->SetColor( FONT_GREEN );
}


bool CLoading::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eLoadingProgress, this );
	//GetSystem()->Register( SystemType::eBattlePlayerLoadEnd, this );
	
	return true;
}

void CLoading::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eLoadingProgress:		
		{
			int nProgressValue = 0;
			kStream >> nProgressValue;
			_UpdateProgressBar(nProgressValue);
		}
		break;
	//case SystemType::eBattlePlayerLoadEnd:	_PlayerLoadEnd( kStream );		break;
	}
}