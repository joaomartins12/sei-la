#include "stdafx.h"
#include "MainFrame_TargetUI.h"


cTargetUI::cTargetUI()
{

}

cTargetUI::~cTargetUI()
{

}

bool cTargetUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eSetTarget,			this );
	GetSystem()->Register( SystemType::eSetTargetMonster,	this );
	GetSystem()->Register( SystemType::eReleaseTarget,		this );
	GetSystem()->Register( SystemType::eUpdateTarget,		this );

	return true;
}

void cTargetUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eSetTarget:
		{
			SetShowWindow( true );
			_SetTargetUI();
		}
		break;
	case SystemType::eSetTargetMonster:
		{
			UINT nDigimonID = 0;
			bool bOwner = false;
			kStream >> nDigimonID >> bOwner;
			SetShowWindow( true );
			_SetTargetMonsterUI( nDigimonID, bOwner );
		}
		break;
	case SystemType::eReleaseTarget:
		{
			SetShowWindow( false );
			m_sBaseUI.SetVisible( false );
			m_sAttriUI.SetVisible( false );
			m_sTimerUI.SetVisible( false );
		}
		break;
	case SystemType::eUpdateTarget:
		{
			SetShowWindow( true );
			_UpdateTargetUI();
		}
		break;
	}
}

cBaseWindow::eWINDOW_TYPE cTargetUI::GetWindowType()
{
	return WT_TARGET_WINDOW;
}

void cTargetUI::Destroy()
{
	cBaseWindow::Delete();
}

void cTargetUI::DeleteResource()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	DeleteScript();
}

bool cTargetUI::Close(bool bSound /* = true */)
{
	if( IsEnableWindow() == false )
		return false;

	if( bSound == true )
	{
		PlayCloseSound();
	}

	// static UI
	SetShowWindow( false );
	return true;
}

void cTargetUI::Create(int nValue /*= 0 */)
{
	if( !cBaseWindow::Init() )
		return;

	InitScript( NULL, CsPoint( 334, 12 ), CsPoint( 293, 72 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::CENTER );

	m_sBaseUI.pTargetBg = AddSprite( CsPoint( 40, 0 ), CsPoint( 223, 65 ), "TargetWindow\\MonsterTarget.tga" );
	if( m_sBaseUI.pTargetBg )
		m_sBaseUI.pTargetBg->AddEvent( cSprite::SPRITE_RBUP_EVENT, this, &cTargetUI::_OnRClickTargetWindow );
	m_sBaseUI.pTargetMark = AddSprite( CsPoint( 192, -7 ), CsPoint( 77, 80 ), "TargetWindow\\CMonster_Mark.tga" );
	m_sBaseUI.pTargetLevel = AddSprite( CsPoint( 243, 46 ), CsPoint( 29, 29 ), "TargetWindow\\CMonster_Level.tga" );
	m_sBaseUI.pTargetImg = AddSprite( CsPoint( 202, 4 ), CsPoint( 57, 57 ), "", true, NiColor::WHITE, false );
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		m_sBaseUI.pPenNameTxt = AddText( &ti, CsPoint( 120, 1 ) );	// 펜 이름
		m_sBaseUI.pNameTxt = AddText( &ti, CsPoint( 120, 15 ) );		// 이름
		m_sBaseUI.pLevelTxt = AddText( &ti, CsPoint( 257, 54 ) );	// 레벨
	}
	m_sBaseUI.pHpPb = NiNew cUI_ProgressBar;
	if( m_sBaseUI.pHpPb )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_9, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
		m_sBaseUI.pHpPb->Init( m_pRoot, CsPoint( 47, 40 ), CsPoint( 146, 8 ), false );
		m_sBaseUI.pHpPb->SetViewTextType( cUI_ProgressBar::INT_PERSENT );
		m_sBaseUI.pHpPb->SetStringControl( &ti );
		m_sBaseUI.pHpPb->SetRenderText( true );
		m_sBaseUI.pHpPb->SetGuageImage( "TargetWindow\\HpBar1.tga" );
		m_sBaseUI.pHpPb->SetRange( 1.0f );
		AddChildControl( m_sBaseUI.pHpPb );
	}

	// 일반 속성
	m_sAttriUI.pNormalAttri = NiNew cImage;
	if( m_sAttriUI.pNormalAttri )
	{
		m_sAttriUI.pNormalAttri->Init( m_pRoot, CsPoint( 0, 14 ), CsPoint( 40, 40 ), "CommonUI\\Digimon_Attributetype.tga", false );
		m_sAttriUI.pNormalAttri->SetTexToken( CsPoint( 36, 0 ) );
		AddChildControl( m_sAttriUI.pNormalAttri );
	}
	m_sAttriUI.pNormalUpper = AddSprite( CsPoint( 0, -6 ), CsPoint( 40, 20 ), "DigimonStatus\\Element\\data_arrow_up.tga" );
	m_sAttriUI.pNormalEqual = AddSprite( CsPoint( -20, 14 ), CsPoint( 20, 40 ), "DigimonStatus\\Element\\data_arrow_left.tga" );
	m_sAttriUI.pNormalLower = AddSprite( CsPoint( 0, 54 ), CsPoint( 40, 20 ), "DigimonStatus\\Element\\data_arrow_down.tga" );

	// 자연 속성
	m_sAttriUI.pNatureComp = AddSprite( CsPoint( 260, 24 ), CsPoint( 27, 27 ), "DigimonStatus\\Element\\button_gold_small.tga" );
	m_sAttriUI.pNatureAttri = NiNew cImage;
	if( m_sAttriUI.pNatureAttri )
	{
		m_sAttriUI.pNatureAttri->Init( m_pRoot, CsPoint( 260, 24 ), CsPoint( 25, 25 ), "CommonUI\\Nature_S.tga", false );
		m_sAttriUI.pNatureAttri->SetTexToken( CsPoint( 26, 26 ) );
		AddChildControl( m_sAttriUI.pNatureAttri );
	}

	// 공격 성향
	m_sAttriUI.pBattleAttri = NiNew cImage;
	if( m_sAttriUI.pBattleAttri )
	{
		m_sAttriUI.pBattleAttri->Init( m_pRoot, CsPoint( 40, 54 ), CsPoint( 19, 19 ), "Icon\\MonsterAI.tga", false );
		m_sAttriUI.pBattleAttri->SetTexToken( CsPoint( 19, 19 ) );
		AddChildControl( m_sAttriUI.pBattleAttri );
	}

	// 소환 타이머 모래시계
	m_sTimerUI.pTimerImg = NiNew cSpriteAni;
	if( m_sTimerUI.pTimerImg )
	{
		m_sTimerUI.pTimerImg->Init( cSpriteAni::LOOP, m_pRoot, CsPoint( 166, 76 ), CsPoint( 23, 23 ), "TargetWindow\\Time\\Ani_", "tga", 4, false );
		m_sTimerUI.pTimerImg->SetAniTime( 0.2f );
		AddChildControl( m_sTimerUI.pTimerImg );
	}

	// 소환 타이머
	m_sTimerUI.pTimerPb = NiNew cUI_TimerProgressBar;
	if( m_sTimerUI.pTimerPb )
	{
		m_sTimerUI.pTimerPb->Init( m_pRoot, CsPoint( 190, 79 ), CsPoint( 79, 15 ), false );
		m_sTimerUI.pTimerPb->SetViewTextType( cUI_ProgressBar::NONE );
		m_sTimerUI.pTimerPb->SetDirType( cUI_ProgressBar::RIGHT_LEFT );
		m_sTimerUI.pTimerPb->SetBackImage( "TargetWindow\\Time\\TimeBar_BG.tga" );
		m_sTimerUI.pTimerPb->SetCoverImage( "TargetWindow\\Time\\TimeBar_BGMask.tga" );
		m_sTimerUI.pTimerPb->SetGuageImage( "TargetWindow\\Time\\TimeBar.tga" );
		m_sTimerUI.pTimerPb->SetProgressRect( CsRect( CsPoint( 3, 3 ), CsSIZE( 73, 9 ) ) );
		AddChildControl( m_sTimerUI.pTimerPb );
	}

	m_sBaseUI.SetVisible( false );
	m_sAttriUI.SetVisible( false );
	m_sTimerUI.SetVisible( false );
}

void cTargetUI::Update(float const& fDeltaTime)
{
	UpdateScript( fDeltaTime );

	if( !GetSystem()->GetTargetObject() )
		return;

	_UpdateTargetUI();
}

cBaseWindow::eMU_TYPE cTargetUI::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( MUT_OUT_WINDOW == muReturn )
		return muReturn;

	if( m_sBaseUI.Update_ForMouse() )
		return muReturn;

	if( m_sAttriUI.Update_ForMouse() )
		return muReturn;

	return muReturn;
}

void cTargetUI::Render()
{
	RenderScript();
}

void cTargetUI::ResetDevice()
{
	ResetDeviceScript();
}

bool cTargetUI::OnEscapeKey()
{
	return GetSystem()->ReleaseTarget();
}

void cTargetUI::_SetBaseUI(USHORT const& nClassType, bool const& bOwner)
{
	m_sBaseUI.SetVisible( true );

	std::string strImgPath = GetSystem()->GetTargetImgFile();

	std::wstring wsName;
	std::wstring wsPenName;
	GetSystem()->GetTargetName( wsName, wsPenName );

	int nLevel = 0;
	float fHpRate = 0.0f;
	GetSystem()->GetTargetStat( nLevel, fHpRate );

	m_sBaseUI.SetBaseInfo( strImgPath, wsName, wsPenName, nLevel, fHpRate );
	m_sBaseUI.SetClassInfo( nClassType );
	m_sBaseUI.SetOwner( bOwner );
}

void cTargetUI::_SetTargetUI()
{
	_SetBaseUI( 0, true );
	m_sAttriUI.SetVisible( false );
	m_sTimerUI.SetVisible( false );
}

void cTargetUI::_SetTargetMonsterUI(UINT const& nDigimonID, bool const& bOwner)
{
	CsMonster::sINFO* pTargetInfo = GetSystem()->GetMonsterFTInfo( nDigimonID );
	SAFE_POINTER_RET( pTargetInfo );

	_SetBaseUI( pTargetInfo->s_nClass, bOwner );

	m_sAttriUI.SetVisible( true );
	// 일반 속성
	int nNormalAttrIdx = GetSystem()->GetNormalAttributeIndex( pTargetInfo->s_dwModelID );
	SystemType::eCompareResult eNormalComp = GetSystem()->GetNormalCompare( pTargetInfo->s_dwModelID );
	m_sAttriUI.SetNormalAttribute( nNormalAttrIdx, eNormalComp );

	// 자연 속성
	int nNatureAttrIdx = GetSystem()->GetNatureAttributeIndex( pTargetInfo->s_dwModelID );
	SystemType::eCompareResult eNatureComp = GetSystem()->GetNatureCompare( pTargetInfo->s_dwModelID );
	m_sAttriUI.SetNatureAttribute( nNatureAttrIdx, eNatureComp );

	// 공격 속성
	int nAttackAttriIdx = pTargetInfo->s_nBattle == 1 ? 0 : 1;
	m_sAttriUI.SetBattleAttribute( nAttackAttriIdx );

	// 소환 시간
	UINT nExpireTS = GetSystem()->GetExpireTime() - _TIME_TS;
	UINT nTotalExpireTS = GetSystem()->GetTotalExpireTime();
	m_sTimerUI.SetTimer( nExpireTS, nTotalExpireTS );
}

void cTargetUI::_UpdateTargetUI()
{
	int nLevel = 0;
	float fHpRate = 0.0f;
	GetSystem()->GetTargetStat( nLevel, fHpRate );

	// hp bar
	m_sBaseUI.SetHpRate( fHpRate );

	// timer bar
	UINT nExpireTS = GetSystem()->GetExpireTime() - _TIME_TS;
	UINT nTotalExpireTime = GetSystem()->GetTotalExpireTime();
	m_sTimerUI.SetTimer( nExpireTS, nTotalExpireTime );
}

void cTargetUI::_OnRClickTargetWindow(void* pSender, void* pData)
{
	GetSystem()->OpenPopUpWindow();
}

//////////////////////////////////////////////////////////////////////////
// Base UI
void cTargetUI::sBaseUI::SetBaseInfo(std::string const& strImgPath, std::wstring const& wsName, std::wstring const& wsPenName,
									 int const& nLevel, float const& fHpRate)
{
	if( pTargetImg )
		pTargetImg->ChangeTexture( strImgPath.c_str(), false );

	if( pNameTxt )
		pNameTxt->SetText( wsName.c_str() );

	if( pPenNameTxt )
		pPenNameTxt->SetText( wsPenName.c_str() );

	if( wsPenName.empty() )
		pTargetBg->ChangeTexture( "TargetWindow\\MonsterTarget.tga" );
	else
		pTargetBg->ChangeTexture( "TargetWindow\\MonsterTarget2.tga" );

	if( pLevelTxt )
	{
		if( 0 == nLevel )
			pLevelTxt->SetText( L"?" );
		else
			pLevelTxt->SetText( nLevel );
	}

	SetHpRate( fHpRate );
}

void cTargetUI::sBaseUI::SetClassInfo(USHORT nClassType)
{
	SAFE_POINTER_RET( pTargetMark );
	SAFE_POINTER_RET( pTargetLevel );

	switch( nClassType )
	{
	case nMonster::ClassNormalBoss:
	case nMonster::ClassRaid:
	case nMonster::ClassBlackRaid:
		{
			pTargetMark->ChangeTexture( "TargetWindow\\CMonster_Mark.tga" );
			pTargetLevel->ChangeTexture( "TargetWindow\\CMonster_Level.tga" );
		}
		break;

	case nMonster::ClassEliteBoss:
	case nMonster::ClassSpecial:
	case nMonster::ClassBlack:
	case nMonster::ClassBuffRaid:
		{
			pTargetMark->ChangeTexture( "TargetWindow\\BMonster_Mark.tga" );
			pTargetLevel->ChangeTexture( "TargetWindow\\BMonster_Level.tga" );
			if( pLevelTxt )
				pLevelTxt->SetText( L"" );
		}
		break;

	default:
		{
			pTargetMark->SetVisible( false );
			pTargetLevel->ChangeTexture( "TargetWindow\\CMonster_Level.tga" );
		}
		break;
	}
}

void cTargetUI::sBaseUI::SetOwner(bool bOwner)
{
	float fAlpah = bOwner ? 1.0f : 0.6f;
	if( pTargetBg )
		pTargetBg->SetAlpha( fAlpah );
	if( pTargetMark )
		pTargetMark->SetAlpha( fAlpah );
	if( pTargetLevel )
		pTargetLevel->SetAlpha( fAlpah );
	if( pTargetImg )
		pTargetImg->SetAlpha( fAlpah );
}

void cTargetUI::sBaseUI::SetHpRate(float fHpRate)
{
	SAFE_POINTER_RET( pHpPb );
	pHpPb->SetProgressPos( fHpRate );
}

void cTargetUI::sBaseUI::SetVisible(bool bVisible)
{
	if( pTargetBg )
		pTargetBg->SetVisible( bVisible );
	if( pTargetMark )
		pTargetMark->SetVisible( bVisible );
	if( pTargetLevel )
		pTargetLevel->SetVisible( bVisible );
	if( pTargetImg )
		pTargetImg->SetVisible( bVisible );

	if( pNameTxt )
		pNameTxt->SetVisible( bVisible );
	if( pPenNameTxt )
		pPenNameTxt->SetVisible( bVisible );
	if( pLevelTxt )
		pLevelTxt->SetVisible( bVisible );

	if( pHpPb )
		pHpPb->SetVisible( bVisible );
}

bool cTargetUI::sBaseUI::Update_ForMouse()
{
	if( pTargetBg && pTargetBg->Update_ForMouse() == cSprite::ACTION_CLICK )
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////
// Attribute UI
void cTargetUI::sAttributeUI::SetBattleAttribute(int const& nAttriIdx)
{
	SAFE_POINTER_RET( pBattleAttri );
	pBattleAttri->SetState( nAttriIdx );
}

void cTargetUI::sAttributeUI::SetNormalAttribute(int const& nAttriIdx, int const& nCompType)
{
	SetVisibleNormalAttribute( false );
	if( -1 == nAttriIdx || CMainFrameContents::eCompError == nCompType )
		return;

	if( pNormalAttri )
	{
		pNormalAttri->SetVisible( true );
		pNormalAttri->SetState( nAttriIdx );
	}

	switch( nCompType )
	{
	case CMainFrameContents::eUpper:
		{
			if( pNormalUpper )
				pNormalUpper->SetVisible( true );
		}
		break;
	case CMainFrameContents::eLower:
		{
			if( pNormalLower )
				pNormalLower->SetVisible( true );
		}
		break;
	case CMainFrameContents::eEqual:
		{
			if( pNormalEqual )
				pNormalEqual->SetVisible( true );
		}
		break;
	}
}

void cTargetUI::sAttributeUI::SetNatureAttribute(int const& nAttriIdx, int const& nCompType)
{
	SetVisibleNatureAttribute( false );
	if( -1 == nAttriIdx || CMainFrameContents::eCompError == nCompType )
		return;

	if( pNatureAttri )
	{
		pNatureAttri->SetVisible( true );
		pNatureAttri->SetState( nAttriIdx );
	}

	SAFE_POINTER_RET( pNatureComp );
	pNatureComp->SetVisible( true );
	pNatureComp->SetPos( CsPoint( 260, 24 ) );

	switch( nCompType )
	{
	case CMainFrameContents::eUpper:
		pNatureComp->SetRotation2( CsD2R( 315 ) );
		break;
	case CMainFrameContents::eLower:
		pNatureComp->SetRotation2( CsD2R( 135 ) );
		break;
	case CMainFrameContents::eEqual:
		pNatureComp->SetRotation2( CsD2R( 45 ) );
		break;
	}
}

void cTargetUI::sAttributeUI::SetVisibleNormalAttribute(bool bVisible)
{
	if( pNormalAttri )
		pNormalAttri->SetVisible( bVisible );
	if( pNormalUpper )
		pNormalUpper->SetVisible( bVisible );
	if( pNormalLower )
		pNormalLower->SetVisible( bVisible );
	if( pNormalEqual )
		pNormalEqual->SetVisible( bVisible );
}

void cTargetUI::sAttributeUI::SetVisibleNatureAttribute(bool bVisible)
{
	if( pNatureAttri )
		pNatureAttri->SetVisible( bVisible );
	if( pNatureComp )
		pNatureComp->SetVisible( bVisible );
}

void cTargetUI::sAttributeUI::SetVisible(bool bVisible)
{
	SetVisibleNormalAttribute( bVisible );
	SetVisibleNatureAttribute( bVisible );
	if( pBattleAttri )
		pBattleAttri->SetVisible( bVisible );
}

bool cTargetUI::sAttributeUI::Update_ForMouse()
{
	SAFE_POINTER_RETVAL( TOOLTIPMNG_STPTR, false );
	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RETVAL( pTooltip, false );

	if( pNormalAttri && pNormalAttri->Update_ForMouse() )
	{
		CsPoint patch = pNormalAttri->GetSize();
		patch += CsPoint( 14, 40 );
		pTooltip->SetTooltip( pNormalAttri->GetPos(), patch, 210, cTooltip::TARGET_ELEMENT,
			pNormalAttri->GetState() + nsCsDigimonTable::AT_NONE );
		return true;
	}

	if( pNatureAttri && pNatureAttri->Update_ForMouse() )
	{
		pTooltip->SetTooltip( pNatureAttri->GetPos(), pNatureAttri->GetSize(), 210, cTooltip::TARGET_NATURAL,
			pNatureAttri->GetState() + nsCsDigimonTable::NT_ICE );
		return true;
	}

	if( pBattleAttri && pBattleAttri->Update_ForMouse() )
	{
		pTooltip->SetTooltip( pBattleAttri->GetPos(), pBattleAttri->GetSize(), 210, cTooltip::TARGET_ATTACK,
			pBattleAttri->GetState() == 0 ? 1/*Defensive*/ : 3/*Aggressive*/ );
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
// Timer UI
void cTargetUI::sTimerUI::SetTimer(UINT const& nExpireTS, UINT const& nTotalExpireTS)
{
	SAFE_POINTER_RET( pTimerImg );
	SAFE_POINTER_RET( pTimerPb );

	if( 0 == nTotalExpireTS )
	{
		SetVisible( false );
		return;
	}

	SetVisible( true );
	pTimerImg->PlayOn( true );
	pTimerPb->SetTimer( nExpireTS, nTotalExpireTS );
	pTimerPb->Start();
}

void cTargetUI::sTimerUI::SetVisible(bool bVisible)
{
	if( pTimerImg )
		pTimerImg->SetVisible( bVisible );

	if( pTimerPb )
		pTimerPb->SetVisible( bVisible );
}
