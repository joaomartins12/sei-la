
#include "stdafx.h"
#include "TamerWindow.h"
#include "../Adapt/AdaptPartySystem.h"

cTamerWindow::cTamerWindow():m_pTargetObject(NULL),m_pWindowBG(NULL),m_pTamerLvBG(NULL),
m_pHPBarMin(NULL),m_pHPBarMax(NULL),m_pFPBar(NULL),m_pHostSprite(NULL),m_pNameText(NULL),
m_pLevelText(NULL),m_pDSBar(NULL),m_pExpBar(NULL),m_pExpPercentText(NULL),m_pHPText(NULL),
m_pAdaptParty(NULL)
{
	m_pAdaptParty = new AdaptPartySystem;
	if( m_pAdaptParty )
		m_pAdaptParty->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CT_PARTYSYSTEM ) );
}

void cTamerWindow::Destroy()
{
	m_pAdaptParty = 0;
	cBaseWindow::Delete();
}

void cTamerWindow::DeleteResource()
{
	DeleteScript();

	SAFE_NIDELETE( m_pWindowBG );
	SAFE_NIDELETE( m_pHPBarMin );
	SAFE_NIDELETE( m_pHPBarMax );
	SAFE_NIDELETE( m_pFPBar );
	SAFE_NIDELETE( m_pHostSprite );
	SAFE_NIDELETE( m_pDSBar );
	SAFE_NIDELETE( m_pExpBar );
	SAFE_NIDELETE( m_pHPText );
}

void cTamerWindow::Create( int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	assert_cs( m_pTargetObject == NULL );

	InitScript( NULL, CsPoint::ZERO, CsPoint( 236, 96 ), false, IFREGION_X::LEFT, IFREGION_Y::TOP );
	// 레벨 배경
	m_pTamerLvBG = AddSprite( CsPoint( 0, 69 ), CsPoint( 35, 35 ), "TargetWindow\\TamerLevel.tga" );

	// 뒷배경
	assert_cs( m_pWindowBG == NULL );
	m_pWindowBG = NiNew cSprite;
	m_pWindowBG->Init( m_pRoot, CsPoint::ZERO, CsPoint( 236, 96 ), "TargetWindow\\TamerTarget.tga", false );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
	ti.s_eTextAlign = DT_CENTER;
	// 이름
	m_pNameText = AddText( &ti, CsPoint( 162, 20 ) );

	// 레벨
	m_pLevelText = AddText( &ti, CsPoint( 18, 78 ) );

	// hp
	m_pHPBarMin = NiNew cProgressBar;
	m_pHPBarMin->Init( m_pRoot, cProgressBar::BAR_3IMAGE, CsPoint( 101, 49 ), CsPoint( 126, 8 ), CsPoint( 2, 2 ), "TargetWindow\\Hp3Bar1.tga", false );

	m_pHPBarMax = NiNew cProgressBar;
	m_pHPBarMax->Init( m_pRoot, cProgressBar::BAR_3IMAGE, CsPoint( 101, 49 ), CsPoint( 126, 8 ), CsPoint( 2, 2 ), "TargetWindow\\HpBar1.tga", false );

	m_pFPBar = NiNew cProgressBar;
	m_pFPBar->Init( m_pRoot, cProgressBar::BAR_3IMAGE, CsPoint( 101, 49 ), CsPoint( 126, 8 ), CsPoint( 2, 2 ), "TargetWindow\\Hp2Bar1.tga", false );

	// hp text
	ti.Init( CFont::FS_9, NiColor::WHITE );
	ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
	m_pHPText = NiNew cText;
	m_pHPText->Init( m_pRoot, CsPoint( 164, 53 ), &ti, false );

	// Ds
	m_pDSBar = NiNew cProgressBar;
	m_pDSBar->Init( m_pRoot, cProgressBar::BAR_3IMAGE, CsPoint( 101, 64 ), CsPoint( 126, 8 ), CsPoint( 2, 2 ), "TargetWindow\\DsBar1.tga", false );
	m_pDSBar->SetRenderPosText( cProgressBar::TEXT_VALUE );

	// Exp
	m_pExpBar = NiNew cProgressBar;
	m_pExpBar->Init( m_pRoot, cProgressBar::BAR_1IMAGE, CsPoint( 87, 79 ), CsPoint( 146, 5 ), CsPoint( 0, 0 ), "TargetWindow\\Expbar.tga", false );

	m_pHostSprite = NiNew cSprite;
	m_pHostSprite->Init( m_pRoot, CsPoint( 4, 5 ), CsPoint( 22, 25 ), "Party\\Host.tga", false );
	m_pHostSprite->SetVisible(false);

	ti.s_eFontSize = CFont::FS_9;
	ti.s_eTextAlign = DT_RIGHT;
	ti.SetText( _T( "" ) );
	m_pExpPercentText = AddText( &ti, CsPoint( 235, 85 ) );

	// 경험치 툴팁 영역
	m_rcExpRegion.SetRect( CsPoint( 87, 75 ), CsSIZE( 146, 10 ) );	
}

void cTamerWindow::Update(float const& fDeltaTime)
{
	SAFE_POINTER_RET( m_pTargetObject );
	CsC_AvObject::sBASE_STAT* pBaseStat = m_pTargetObject->GetBaseStat();
	SAFE_POINTER_RET( pBaseStat );
	// 레벨
	if( m_pLevelText )
	{
		if( pBaseStat->GetLevel() != CsC_AvObject::INVALIDE_STAT )
			m_pLevelText->SetText( pBaseStat->GetLevel() );
		else
			m_pLevelText->SetText( _T( "?" ) );
	}

	if( m_pHPBarMin )
	{
		m_pHPBarMin->SetRange( CsPoint( 0, pBaseStat->GetMaxHP() ) );
		m_pHPBarMin->SetBarPos( pBaseStat->GetHP() );
	}
	if( m_pHPBarMax )
	{
		m_pHPBarMax->SetRange( CsPoint( 0, pBaseStat->GetMaxHP() ) );
		m_pHPBarMax->SetBarPos( pBaseStat->GetHP() );
	}
	if( m_pFPBar )
	{
		m_pFPBar->SetRange( CsPoint( 0, pBaseStat->GetMaxHP() ) );
		m_pFPBar->SetBarPos( pBaseStat->GetFP() );
	}

	if( m_pHPText )
	{
		std::wstring wsText;
		DmCS::StringFn::Format( wsText, L"%d / %d", pBaseStat->GetHP(), pBaseStat->GetMaxHP() );
		m_pHPText->SetText( wsText.c_str() );
	}

	// Ds
	if( m_pDSBar )
	{
		m_pDSBar->SetRange( CsPoint( 0, pBaseStat->GetMaxDS() ) );
		m_pDSBar->SetBarPos( pBaseStat->GetDS() );
	}

	// Exp
	int nMaxEXP = static_cast<int>(FMTamer::GetMaxExp( pBaseStat->GetLevel() ));
	int nCurEXP = static_cast<int>(pBaseStat->GetExp());
	if( m_pExpBar )
	{
		m_pExpBar->SetRange( CsPoint( 0, nMaxEXP ) );
		m_pExpBar->SetBarPos( nCurEXP );
	}

	if( m_pExpPercentText )
	{
		TCHAR sz[ 32 ];
		_stprintf_s( sz, 32, _T( "%.3f%%" ), nCurEXP/(float)nMaxEXP*100.0f );
		m_pExpPercentText->SetText( sz );
	}
}

cBaseWindow::eMU_TYPE
cTamerWindow::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
	if( m_rcExpRegion.PtInRect( ptMouseLocalPos ) == TRUE )
	{
		// 경험치 툴팁
		CsC_AvObject::sBASE_STAT* pBaseStat = m_pTargetObject->GetBaseStat();
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip(	m_rcExpRegion.GetPos(), m_rcExpRegion.GetSize(), TOOLTIP_MAX_SIZE, cTooltip::EXP,
												pBaseStat->GetExp(), FMTamer::GetMaxExp( pBaseStat->GetLevel() ) );
	}

	return muReturn;
}

void cTamerWindow::Render()
{
	// 뒷 배경
	m_pWindowBG->Render();

	// 얼굴 이미지
	if( m_pTargetObject != NULL )
	{
		sCHAR_IMAGE* pCharImage = ( (CTamerUser*)m_pTargetObject )->GetCharImage();
		if( pCharImage != NULL )
			pCharImage->Render_L( CsPoint( m_pRoot->GetClient().x + 6, m_pRoot->GetClient().y + 6 ), CsPoint( 84, 84 ) );
	}

	RenderScript();

	CsC_AvObject::sBASE_STAT* pBaseStat = m_pTargetObject->GetBaseStat();
	if( pBaseStat->GetFP() < pBaseStat->GetHP() )
	{
		m_pHPBarMax->Render();
		m_pFPBar->Render();
	}
	else		// hp 보다 피로도가 더 높을 때
	{
		m_pFPBar->Render();
		m_pHPBarMin->Render();
	}

	if( m_pHPText )
		m_pHPText->Render();

	m_pDSBar->Render();
	m_pExpBar->Render();
	m_pHostSprite->Render();
}

void cTamerWindow::OnRButtonUp( CsPoint pos )
{
	if( IsInWindow( pos ) == false )
		return;	

	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
		return;

	if( m_pAdaptParty && !m_pAdaptParty->IsJoinParty() )
 		return;

  	cPopUpWindow::ePARTY_TYPE pt = m_pAdaptParty->IsMePartyMaster() ? cPopUpWindow::PARTY_HS_MY : cPopUpWindow::PARTY_NR_MY;
 	g_pGameIF->GetPopup()->SetPopup( CURSOR_ST.GetPos() + CsPoint( 15, 0 ), CsPoint( 130, 0 ), cPopUpWindow::PARTY_MENU, pt );
}

void cTamerWindow::OnLButtonUp( CsPoint pos )
{
	if( IsInWindow( pos ) == false )
		return;

	if( nsCsGBTerrain::g_nSvrLibType != nLIB::SVR_BATTLE )
		g_pCharResMng->SetTargetMark( m_pTargetObject );	
}

void cTamerWindow::SetTargetObject( CsC_AvObject* pTarget )
{
	m_pTargetObject = pTarget;	

	if( m_pTargetObject != NULL )
	{
		// 이름
		m_pNameText->SetText( ( (CTamerUser*)m_pTargetObject )->GetCharOption()->GetName() );		
	}	
}

void cTamerWindow::ResetDevice()
{
	ResetDeviceScript();
	m_pWindowBG->ResetDevice();
	m_pHPBarMin->ResetDevice();
	m_pHPBarMax->ResetDevice();
	m_pFPBar->ResetDevice();
	m_pDSBar->ResetDevice();
	m_pExpBar->ResetDevice();
	m_pHostSprite->ResetDevice();
	m_pHPText->ResetDevice();
}

void cTamerWindow::SetPartyMaster(bool bValue)
{
	if( m_pHostSprite )
		m_pHostSprite->SetVisible(bValue);
}

// Vip Mebmer Ship 일때 테이머 배경 이미지 변경
void cTamerWindow::ChangeVipMemberShip( bool bValue )
{
	if( bValue )
	{
		if( m_pWindowBG )
			m_pWindowBG->ChangeTexture( "TargetWindow\\TamerTarget_MM.tga" );
		if( m_pTamerLvBG )
			m_pTamerLvBG->ChangeTexture( "TargetWindow\\TamerLevel_MM.tga" );
	}
	else
	{
		if( m_pWindowBG )
			m_pWindowBG->ChangeTexture( "TargetWindow\\TamerTarget.tga" );
		if( m_pTamerLvBG )
			m_pTamerLvBG->ChangeTexture( "TargetWindow\\TamerLevel.tga" );
	}
}