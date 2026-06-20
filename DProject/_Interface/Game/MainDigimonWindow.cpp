
#include "stdafx.h"
#include "MainDigimonWindow.h"


cMainDigimonWindow::cMainDigimonWindow()
:m_pLevelBG_Normal(NULL)
#ifdef SDM_TAMER_XGUAGE_20180628
,m_pWindowBGXAnti(NULL)
,m_pLevelBG_XAnti(NULL)
#endif
{
	m_pTargetObject = NULL;
	m_pWindowBG = NULL;
}

void cMainDigimonWindow::Destroy()
{
	cBaseWindow::Delete();
	m_pTargetObject = NULL;	
}

void cMainDigimonWindow::DeleteResource()
{
	DeleteScript();

	SAFE_NIDELETE( m_pHPBar );
	SAFE_NIDELETE( m_pDSBar );
	SAFE_NIDELETE( m_pExpBar );
	SAFE_NIDELETE( m_pWindowBG );
	SAFE_NIDELETE( m_pLevelBG_Normal );
#ifdef SDM_TAMER_XGUAGE_20180628
	SAFE_NIDELETE( m_pWindowBGXAnti );
	SAFE_NIDELETE( m_pLevelBG_XAnti );
#endif
	SAFE_NIDELETE( m_pBattleSprite );	
	SAFE_NIDELETE( m_pBattleTagNum );
}

void cMainDigimonWindow::Create( int nValue /*=0*/ )
{
	if( cBaseWindow::Init() == false )
		return;

	assert_cs( m_pTargetObject == NULL );
	InitScript( NULL, CsPoint( 22, 96 ), CsPoint( 214, 65 ), false, IFREGION_X::LEFT, IFREGION_Y::TOP );

	// 뒷배경
	assert_cs( m_pWindowBG == NULL );
	m_pWindowBG = NiNew cSprite;
	m_pWindowBG->Init( m_pRoot, CsPoint::ZERO, CsPoint( 214, 65 ), "TargetWindow\\DigimonTarget.tga", false );

	//Normal Level 배경
	assert_cs( m_pLevelBG_Normal == NULL);
	m_pLevelBG_Normal = NiNew cSprite;
	m_pLevelBG_Normal->Init( m_pRoot, CsPoint( -5, 42 ), CsPoint( 28, 27 ), "TargetWindow\\DigimnMonsterlevel.tga", false );

#ifdef SDM_TAMER_XGUAGE_20180628
	//X Anti 배경
	assert_cs( m_pWindowBGXAnti == NULL);
	m_pWindowBGXAnti = NiNew cSprite;
	m_pWindowBGXAnti->Init( m_pRoot, CsPoint::ZERO, CsPoint( 214, 65 ), "TargetWindow\\DigimonTarget_X.tga", false );

	//Normal Level 배경
	assert_cs( m_pLevelBG_XAnti == NULL);
	m_pLevelBG_XAnti = NiNew cSprite;
	m_pLevelBG_XAnti->Init( m_pRoot, CsPoint( -5, 42 ), CsPoint( 28, 27 ), "TargetWindow\\DigimnMonsterlevel_X.tga", false );
#endif


	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	ti.s_eTextAlign = DT_CENTER;


	////// 디지몬 레벨 배경
	//AddSprite( CsPoint( -5, 42 ), CsPoint( 28, 27 ), "TargetWindow\\DigimnMonsterlevel.tga" );

	// 이름
	m_pNameText = AddText( &ti, CsPoint( 139, 7 ) );

	// 레벨
	m_pLevelText = AddText( &ti, CsPoint( 9, 49 ) );

	// 배틀 태그 숫자
	ti.s_eFontSize = CFont::FS_9;
	m_pBattleTagNum = NiNew cText;
#ifdef LJW_TACTICS_SLOT_EXTEND_190529
	m_pBattleTagNum->Init( m_pRoot, CsPoint( 2, 96 ), &ti, false );
	m_rcBattleTag.SetRect( CsPoint( -9, 78 ), CsSIZE( 22, 22 ) );
#else
	m_pBattleTagNum->Init( m_pRoot, CsPoint( 38, 78 ), &ti, false );
	m_rcBattleTag.SetRect( CsPoint( 2, 78 ), CsSIZE( 22, 22 ) );
#endif

	// hp
	m_pHPBar = NiNew cProgressBar;
	m_pHPBar->Init( m_pRoot, cProgressBar::BAR_3IMAGE, CsPoint( 80, 28 ), CsPoint( 126, 8 ), CsPoint( 2, 2 ), "TargetWindow\\HpBar1.tga", false );		
	m_pHPBar->SetRenderPosText( cProgressBar::TEXT_VALUE );

	// Ds
	m_pDSBar = NiNew cProgressBar;
	m_pDSBar->Init( m_pRoot, cProgressBar::BAR_3IMAGE, CsPoint( 80, 42 ), CsPoint( 126, 8 ), CsPoint( 2, 2 ), "TargetWindow\\DsBar1.tga", false );
	m_pDSBar->SetRenderPosText( cProgressBar::TEXT_VALUE );

	// Exp
	m_pExpBar = NiNew cProgressBar;
	m_pExpBar->Init( m_pRoot, cProgressBar::BAR_1IMAGE, CsPoint( 55, 56 ), CsPoint( 157, 5 ), CsPoint::ZERO, "TargetWindow\\Expbar.tga", false );

	ti.s_eFontSize = CFont::FS_9;
	ti.s_eTextAlign = DT_RIGHT;
	ti.SetText( _T( "" ) );
	m_pExpPercentText = AddText( &ti, CsPoint( 212, 64 ) );

	// 경험치 툴팁 영역
	m_rcExpRegion.SetRect( CsPoint( 55, 52 ), CsSIZE( 157, 10 ) );

	// 전투 표시
	m_pBattleSprite = NiNew cSprite;
	m_pBattleSprite->Init( m_pRoot, CsPoint::ZERO, CsPoint( 214, 65 ), "TargetWindow\\DigimonTarget_Mask.tga", false );

	m_bBattle = false;
	m_fBattleAlpha = -1.0f;
}

void cMainDigimonWindow::Update(float const& fDeltaTime)
{
	SAFE_POINTER_RET( m_pTargetObject );
	CsC_AvObject::sBASE_STAT* pBaseStat = m_pTargetObject->GetBaseStat();
	SAFE_POINTER_RET( pBaseStat );

	// 레벨
	if( pBaseStat->GetLevel() != CsC_AvObject::INVALIDE_STAT )
		m_pLevelText->SetText( pBaseStat->GetLevel() );
	else
		m_pLevelText->SetText( _T( "?" ) );

	// hp
	m_pHPBar->SetRange( CsPoint( 0, pBaseStat->GetMaxHP() ) );
	m_pHPBar->SetBarPos( pBaseStat->GetHP() );

	// Ds
	m_pDSBar->SetRange( CsPoint( 0, pBaseStat->GetMaxDS() ) );
	m_pDSBar->SetBarPos( pBaseStat->GetDS() );
	// Exp
	int nMaxEXP = static_cast<int>(FMDigimon::GetMaxExp( pBaseStat->GetLevel() ));
	int nCurEXP = static_cast<int>(pBaseStat->GetExp());
	m_pExpBar->SetRange( CsPoint( 0, nMaxEXP ) );
	m_pExpBar->SetBarPos( nCurEXP );

	TCHAR sz[ 32 ];
	_stprintf_s( sz, 32, _T( "%.3f%%" ), nCurEXP/(float)nMaxEXP*100.0f );
	m_pExpPercentText->SetText( sz );

	_UpdateBattle();
}

void cMainDigimonWindow::_UpdateBattle()
{
#define ALPHA_TIME		0.5f

	if( m_bBattle == false )
	{
		m_fBattleAlpha = -1.0f;
		return;
	}

	if( m_fBattleAlpha < 0.0f )
	{
		m_fBattleAlpha = 1.0f;
		m_nBattleAlphaType = BAT_DECREASE;
	}

	switch( m_nBattleAlphaType )
	{
	case BAT_DECREASE:
		{
			if( (m_fBattleAlpha -= g_fDeltaTime/ALPHA_TIME ) < 0.0f )
			{
				m_fBattleAlpha = 0.0f;
				m_nBattleAlphaType = BAT_INCREASE;
			}
		}
		break;
	case BAT_INCREASE:
		{
			if( (m_fBattleAlpha += g_fDeltaTime/ALPHA_TIME ) > 1.0f )
			{
				m_fBattleAlpha = 1.0f;
				m_nBattleAlphaType = BAT_DECREASE;
			}
		}
		break;
	}
}

cBaseWindow::eMU_TYPE
cMainDigimonWindow::Update_ForMouse()
{
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
	if( m_rcBattleTag.PtInRect( ptMouseLocalPos ) == TRUE )
	{
		if( g_pDataMng->GetInven()->GetFirstSlot_Item_TypeLS( nItem::BattleTag, 0 ) != cData_Inven::INVALIDE_INVEN_INDEX )
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( ptMouseLocalPos, CsPoint::ZERO, 210, cTooltip::BATTLE_TAG, 0 );		
	}

	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;
	
	if( m_rcExpRegion.PtInRect( ptMouseLocalPos ) == TRUE )
	{
		// 경험치 툴팁
		CsC_AvObject::sBASE_STAT* pBaseStat = m_pTargetObject->GetBaseStat();
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip(	m_rcExpRegion.GetPos() + GetRootClient(), m_rcExpRegion.GetSize(), TOOLTIP_MAX_SIZE, cTooltip::EXP,
												pBaseStat->GetExp(), FMDigimon::GetMaxExp( pBaseStat->GetLevel() ) );
	}	

	return muReturn;
}

void cMainDigimonWindow::Render()
{
	// 얼굴 이미지
	if( m_pTargetObject != NULL )
	{
		CDigimon* pkDigimon = (CDigimon*)m_pTargetObject;
		if(pkDigimon != NULL)
		{
			CsDigimon* pkFTDigimon = pkDigimon->GetFTDigimon();
#ifdef SDM_TAMER_XGUAGE_20180628
			//배경 
			if(pkFTDigimon != NULL){
				if( pkFTDigimon->isX_Anti() == true )
					m_pWindowBGXAnti->Render();
				else
					m_pWindowBG->Render();
			}
#else
			m_pWindowBG->Render();
#endif
			if( m_bBattle ){
				m_pBattleSprite->SetAlpha( m_fBattleAlpha );
				m_pBattleSprite->Render();
			}

			sCHAR_IMAGE* pCharImage = pkDigimon->GetCharImage();
			if( pCharImage != NULL )
				pCharImage->Render_L( CsPoint( m_pRoot->GetClient().x + 4, m_pRoot->GetClient().y + 4 ), CsPoint( 57, 57 ) );
#ifdef SDM_TAMER_XGUAGE_20180628
			if(pkFTDigimon != NULL){
				if( pkFTDigimon->isX_Anti() == true )
					m_pLevelBG_XAnti->Render();
				else
					m_pLevelBG_Normal->Render();
			}
#else
			m_pLevelBG_Normal->Render();
#endif
		}
	}
	
	RenderScript();

	m_pHPBar->Render();
	m_pDSBar->Render();
	m_pExpBar->Render();
	int nInvenIndex = g_pDataMng->GetInven()->GetFirstSlot_Item_TypeLS( nItem::BattleTag, 0 );
	if( nInvenIndex != cData_Inven::INVALIDE_INVEN_INDEX )
	{
		assert_cs( m_pBattleTagNum != NULL );

		int nCount = g_pDataMng->GetInven()->GetCount_Item_TypeLS( nItem::BattleTag, 0 );

		TCHAR sz[ 32 ];
		_stprintf_s( sz, 32, _T( "x%d" ), nCount );
		m_pBattleTagNum->SetText( sz );
		m_pBattleTagNum->Render();
#ifdef LJW_TACTICS_SLOT_EXTEND_190529
		g_pIconMng->RenderItem( g_pDataMng->GetInven()->GetData( nInvenIndex )->GetType(), GetRootClient() + CsPoint( -9, 73 ), CsPoint( 22, 22 ) );
#else
		g_pIconMng->RenderItem( g_pDataMng->GetInven()->GetData( nInvenIndex )->GetType(), GetRootClient() + CsPoint( 2, 73 ), CsPoint( 22, 22 ) );
#endif
	}
}

void cMainDigimonWindow::OnLButtonUp( CsPoint pos )
{
	if( IsInWindow( pos ) == false )
		return;

	g_pCharResMng->SetTargetMark( m_pTargetObject );	
}

void cMainDigimonWindow::SetTargetObject( CsC_AvObject* pTarget )
{
	m_pTargetObject = pTarget;	

	if( m_pTargetObject != NULL )
	{
		// 이름
		m_pNameText->SetText( ( (CDigimonUser*)m_pTargetObject )->GetCharOption()->GetName() );
	}	
}

void cMainDigimonWindow::ResetDevice()
{
	ResetDeviceScript();
	m_pWindowBG->ResetDevice();
	m_pLevelBG_Normal->ResetDevice();
#ifdef SDM_TAMER_XGUAGE_20180628
	m_pWindowBGXAnti->ResetDevice();
	m_pLevelBG_XAnti->ResetDevice();
#endif
	m_pBattleSprite->ResetDevice();
	m_pHPBar->ResetDevice();
	m_pDSBar->ResetDevice();
	m_pExpBar->ResetDevice();
	m_pBattleTagNum->ResetDevice();
}