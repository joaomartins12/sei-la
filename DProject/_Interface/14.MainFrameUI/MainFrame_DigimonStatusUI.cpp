#include "stdafx.h"
#include "MainFrame_DigimonStatusUI.h"

namespace
{
	std::wstring BuildSafeOwnedText( TCHAR const* pText )
	{
		cText::sTEXTINFO textInfo;
		textInfo.SetText( pText );
		std::wstring sanitized;
		sanitized.reserve( textInfo.s_strText.size() );
		for( size_t i = 0; i < textInfo.s_strText.size(); ++i )
		{
			TCHAR ch = textInfo.s_strText[i];
			if( ch == _T('\t') || ch == _T('\n') || ch == _T('\r') || ( ch >= 0x20 && ch != 0x7F ) )
				sanitized.push_back( ch );
		}
		return sanitized;
	}

	void SetSafeOwnedText( cText* pText, TCHAR const* pValue )
	{
		if( pText == NULL )
			return;

		std::wstring ownedText = BuildSafeOwnedText( pValue );
		pText->SetText( ownedText.c_str() );
	}

	void SetSafeOwnedText( cText* pText, std::wstring const& value )
	{
		SetSafeOwnedText( pText, value.c_str() );
	}

	void SetSafeOwnedText( cString::sTEXT* pText, TCHAR const* pValue )
	{
		if( pText == NULL )
			return;

		std::wstring ownedText = BuildSafeOwnedText( pValue );
		pText->SetText( ownedText.c_str() );
	}

	void SetSafeOwnedText( cString::sTEXT* pText, std::wstring const& value )
	{
		SetSafeOwnedText( pText, value.c_str() );
	}

	void SetSafeOwnedText( cText::sTEXTINFO& textInfo, TCHAR const* pValue )
	{
		std::wstring ownedText = BuildSafeOwnedText( pValue );
		textInfo.SetText( ownedText.c_str() );
	}

	void SetSafeOwnedText( cText::sTEXTINFO& textInfo, std::wstring const& value )
	{
		SetSafeOwnedText( textInfo, value.c_str() );
	}
}


#define IF_ABIL_COUNT 9
#define IF_SKILL_COUNT 6

cDigimonStatusUI::cDigimonStatusUI() :
m_pAbilList(NULL),
m_pSkillGrid(NULL),

m_pBackGround(NULL),
m_pLineX(NULL),
m_pLineY(NULL),

m_pMyNormal(NULL),
m_pNormalFull(NULL),
m_pNormalExp(NULL),
m_pMyNature(NULL),
m_pNatureFull(NULL),
m_pNatureExp(NULL),

m_pDigimonExpBar(NULL),
m_pSkillExpBar(NULL),
m_pSkillExp(NULL),
m_pSkillPoint(NULL),

m_pDigimonLevel(NULL),
m_pDigimonName(NULL),
m_pEnchant(NULL),
m_pEnchantMax(NULL),
m_pDigimonRank(NULL),

m_pTalkButton(NULL),
m_pHelpButton(NULL),
m_pCloseButton(NULL),

m_pRenderDigimon(NULL),
m_pRenderXDigimon(NULL),
m_bPendingUpdateStat(false),
m_bPendingUpdateSkill(false),
m_bPendingChangeDigimon(false)
{

}

cDigimonStatusUI::~cDigimonStatusUI()
{
	if( m_pRenderDigimon )
		SAFE_NIDELETE( m_pRenderDigimon );
	if( m_pRenderXDigimon )
		SAFE_NIDELETE( m_pRenderXDigimon );
	if( m_pBackGround )
		SAFE_NIDELETE( m_pBackGround );
	if( m_pLineX )
		SAFE_NIDELETE( m_pLineX );
	if( m_pLineY )
		SAFE_NIDELETE( m_pLineY );
}

bool cDigimonStatusUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eUpdateStat, this );
	GetSystem()->Register( SystemType::eUpdateSkill, this );
	GetSystem()->Register( SystemType::eChangeDigimon, this );

	return true;
}

void cDigimonStatusUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eUpdateStat:
		m_bPendingUpdateStat = true;
		break;
	case SystemType::eUpdateSkill:
		m_bPendingUpdateSkill = true;
		break;
	case SystemType::eChangeDigimon:
		m_bPendingChangeDigimon = true;
		break;
	}
}

cBaseWindow::eWINDOW_TYPE cDigimonStatusUI::GetWindowType()
{
	return WT_NEW_DIGIMONSTATUS;
}

void cDigimonStatusUI::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	cBaseWindow::Delete();
}

void cDigimonStatusUI::DeleteResource()
{
	DeleteScript();
	m_BaseInfo.Delete();
}

void cDigimonStatusUI::Create(int nValue /*= 0 */)
{
	if( !cBaseWindow::Init() )
		return;

	SetRootClient( CsPoint( 100, 100 ) );
	InitScript( NULL, m_ptRootClient, CsPoint( 808, 676 ), true );
	/*AddSprite( CsPoint::ZERO, CsPoint( 808, 676 ), "DigimonStatus_New\\Digimon_Status_Win.tga" );*/
	AddTitle( UISTRING_TEXT( "DIGIMONSTATUS_TITLE_TEXT" ).c_str(), CsPoint( 0, 10 ) );

	m_pBackGround = NiNew cSprite;
	if( m_pBackGround )
		m_pBackGround->Init( NULL, CsPoint::ZERO, CsPoint( 808, 676 ), "DigimonStatus_New\\Digimon_Status_Win.tga", false );

	m_pLineX = NiNew cSprite;
	if( m_pLineX )
		m_pLineX->Init( NULL, CsPoint::ZERO, CsPoint( 6, 6 ), "DigimonStatus_New\\Digimon_Evo_Line_X.tga", false );

	m_pLineY = NiNew cSprite;
	if( m_pLineY )
		m_pLineY->Init( NULL, CsPoint::ZERO, CsPoint( 6, 6 ), "DigimonStatus_New\\Digimon_Evo_Line_Y.tga", false );

	AddSprite( CsPoint( 233, 71 ), CsPoint( 32, 32 ), "DigimonStatus_New\\Digimon_Status_Enchant_01.tga" );

	{	// 스테이터스
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_11, NiColor( 122.f/255.f, 254.f/255.f, 0 ) );
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( UISTRING_TEXT( "DIGIMONSTATUS_STATUS_TEXT" ).c_str() );
		AddText( &ti, CsPoint( 24, 53 ) );
	}

	{	// 스킬
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_11, NiColor( 188.f/255.f, 97.f/255.f, 1 ) );
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( UISTRING_TEXT( "DIGIMONSTATUS_SKILL_TEXT" ).c_str() );
		AddText( &ti, CsPoint( 24, 530 ) );
	}

	{	// 스킬 숙련도
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor( 76.f/255.f, 242.f/255.f, 1 ) );
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( UISTRING_TEXT( "DIGIMONSTATUS_SKILLEXP_TEXT" ).c_str() );
		AddText( &ti, CsPoint( 118, 560 ) );

		cText::sTEXTINFO ti2;
		ti2.Init( CFont::FS_12, NiColor( 188.f/255.f, 97.f/255.f, 1 ) );
		ti2.s_eTextAlign = DT_LEFT;
		ti2.SetText( 0 );
		m_pSkillExp = AddText( &ti2, CsPoint( 195, 559 ) );
	}

	{	// 스킬 포인트
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor( 76.f/255.f, 242.f/255.f, 1 ) );
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( UISTRING_TEXT( "DIGIMONSTATUS_SKILLPOINT_TEXT" ).c_str() );
		AddText( &ti, CsPoint( 234, 560 ) );

		cText::sTEXTINFO ti2;
		ti2.Init( CFont::FS_12, NiColor( 188.f/255.f, 97.f/255.f, 1 ) );
		ti2.s_eTextAlign = DT_LEFT;
		ti2.SetText( 0 );
		m_pSkillPoint = AddText( &ti2, CsPoint( 303, 559 ) );
	}

	{	// 진화 & 라이딩
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_11, NiColor( 1, 174.f/255.f, 0 ) );
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( UISTRING_TEXT( "DIGIMONSTATUS_EVORIDE_TEXT" ).c_str() );
		AddText( &ti, CsPoint( 384, 53 ) );
	}


	{
		// 레벨
		std::wstring wsLevel;
		DmCS::StringFn::Format( wsLevel, L"%s %d", L"Lv.", 1 );
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor( 76.f/255.f, 242.f/255.f, 1 ) );
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( wsLevel.c_str() );
		m_pDigimonLevel = AddText( &ti, CsPoint( 24, 80 ) );
	}

	{
		// 이름
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor( 76.f/255.f, 242.f/255.f, 1 ) );
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( L"" );
		m_pDigimonName = AddText( &ti, CsPoint( 80, 80 ) );
	}

	{
		// 강화 수치
		std::wstring wsEnchant;
		DmCS::StringFn::Format( wsEnchant, L"%d/%d", 0, nLimit::MaxInchantLevel );
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor( 76.f/255.f, 242.f/255.f, 1 ) );
		ti.s_eTextAlign = DT_RIGHT;
		ti.SetText( wsEnchant.c_str() );
		m_pEnchant = AddText( &ti, CsPoint( 309, 80 ) );
	}

	// 강화 수치 - MAX
	m_pEnchantMax = AddSprite( CsPoint( 263, 75 ), CsPoint( 52, 26 ), "DigimonStatus_New\\Digimon_Status_Enchant_02.tga" );

	// 토크 버튼
	m_pTalkButton = AddButton( CsPoint( 145, 120 ), CsPoint( 24, 20 ), CsPoint( 0, 20 ), "DigimonStatus_New\\Digimon_talk.tga" );
	if( m_pTalkButton )
		m_pTalkButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cDigimonStatusUI::_OnClickTalkButton );

	// 도움말 버튼
	m_pHelpButton = AddButton( CsPoint( 748, 8 ), CsPoint( 24, 32 ), CsPoint( 0, 32 ), "DigimonStatus_New\\Digimon_help_btn.tga" );
	if( m_pHelpButton )
		m_pHelpButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cDigimonStatusUI::_OnClickHelpButton );

	// 닫기 버튼
	m_pCloseButton = AddButton( CsPoint( 778, 11 ), CsPoint( 30, 28 ), CsPoint( 0, 28 ), "Event\\event_close.tga" );
	if( m_pCloseButton )
		m_pCloseButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cDigimonStatusUI::_OnClickCloseButton );

	// 디지몬 경험치
	m_pDigimonExpBar = NiNew cUI_ProgressBar;
	if( m_pDigimonExpBar )
	{
		m_pDigimonExpBar->Init( m_pRoot, CsPoint( 29, 107 ), CsPoint( 292, 4 ), false );
		m_pDigimonExpBar->SetViewTextType( cUI_ProgressBar::NONE );
		m_pDigimonExpBar->SetGuageImage( "DigimonStatus_New\\Digimon_gauge_exp.tga" );
		AddChildControl( m_pDigimonExpBar );
	}

	// 스킬 경험치
	m_pSkillExpBar = NiNew cUI_ProgressBar;
	if( m_pSkillExpBar )
	{
		m_pSkillExpBar->Init( m_pRoot, CsPoint( 29, 654 ), CsPoint( 292, 4 ), false );
		m_pSkillExpBar->SetViewTextType( cUI_ProgressBar::NONE );
		m_pSkillExpBar->SetGuageImage( "DigimonStatus_New\\Digimon_gauge_skill.tga" );
		AddChildControl( m_pSkillExpBar );
	}

	m_pRenderDigimon = NiNew cRenderTex;
	if( m_pRenderDigimon )
	{
		if( !m_pRenderDigimon->Init( CsPoint( 160, 251 ), "DigimonStatus_New\\DigimonBack_New.tga" ) )
			SAFE_NIDELETE( m_pRenderDigimon );
	}

#ifdef SDM_TAMER_XGUAGE_20180628
	m_pRenderXDigimon = NiNew cRenderTex;
	if( m_pRenderXDigimon->Init( CsPoint( 160, 251 ), "DigimonStatus_New\\DigimonBack_X1_New.tga" ) == true )
	{
		m_pRenderXDigimon->AddEffectSprite(CsPoint( 160, 251 ), "DigimonStatus_New\\DigimonBack_X2_New.tga");
		m_pRenderXDigimon->StartTextAlpha(0.0f, 1.0f, 1.5f);
	}
	else
		SAFE_NIDELETE( m_pRenderXDigimon );
#endif

	_MakeAbilList();
	_MakeSkillGrid();
	_MakeAttributeInfo();
	_MakeEvoInfo();
	_MakeBaseInfo();

	_UpdateBaseInfo();
}

void cDigimonStatusUI::Update(float const& fDeltaTime)
{
	UpdateScript( fDeltaTime );

	if( m_bPendingChangeDigimon )
	{
		m_bPendingChangeDigimon = false;
		_MakeBaseInfo();
		_UpdateAbility();
		_UpdateBaseInfo();
		_UpdateElement();
		_ResetSKill();
		_UpdateSkill();
		_ResetEvoInfo();
		_UpdateEvoInfo();
	}

	if( m_bPendingUpdateStat )
	{
		m_bPendingUpdateStat = false;
		_MakeBaseInfo();
		_UpdateBaseInfo();
		_UpdateAbility();
		_ResetEvoInfo();
		_UpdateEvoInfo();
		_ResetSKill();
		_UpdateSkill();
	}

	if( m_bPendingUpdateSkill )
	{
		m_bPendingUpdateSkill = false;
		_ResetSKill();
		_UpdateSkill();
	}

	// 디지몬 경험치
	CDigimonUser::sUSER_STAT* pStat = GetSystem()->GetDigimonBaseStat();
	if( pStat && m_pDigimonExpBar )
	{
		m_pDigimonExpBar->SetRange( static_cast<int>( FMDigimon::GetMaxExp( pStat->GetLevel() ) ) );
		m_pDigimonExpBar->SetProgressPos( static_cast<int>( pStat->GetExp() ) );
	}

	// 스킬 경험치
	if( m_pSkillExpBar )
	{
		cEvoUnit* pEvoUnit = GetSystem()->GetEvoUnit();
		SAFE_POINTER_RET( pEvoUnit );
		CsDigimon::sINFO* pFTInfo = GetSystem()->GetDigimonFTInfo();
		SAFE_POINTER_RET( pFTInfo );
		m_pSkillExpBar->SetRange( static_cast<int>(FMDigimon::GetMaxSkillExp( pFTInfo->s_dwDigimonID, pEvoUnit->m_nSkillExpLevel ) ) );
		m_pSkillExpBar->SetProgressPos( static_cast<int>(pEvoUnit->m_nSkillExp) );
	}

	// 기본 속성 경험치
	if( m_pNormalExp && m_pMyNormal )
	{
		m_pNormalExp->SetProgressPos( GetSystem()->GetNormalAttributeExp( m_pMyNormal->GetState() ) );
		if( m_pNormalFull )
			m_pNormalFull->SetVisible( m_pNormalExp->IsMaxGuage() );
	}
	// 자연 속성 경험치
	if( m_pNatureExp && m_pMyNature )
	{
		m_pNatureExp->SetProgressPos( GetSystem()->GetNatureAttributeExp( m_pMyNature->GetState() ) );
		if( m_pNatureFull )
			m_pNatureFull->SetVisible( m_pNatureExp->IsMaxGuage() );
	}

#ifdef SDM_TAMER_XGUAGE_20180628
	if( GetSystem()->IsXAnti() && m_pRenderXDigimon )
		m_pRenderXDigimon->UpdateTextAlpha( fDeltaTime );
#endif

	SAFE_POINTER_RET( g_pCharMng );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser(0);
	SAFE_POINTER_RET( pUser );
	if( !pUser->GetViewSight() && pUser->IsLoad() )
	{
#ifdef SDM_TAMER_XGUAGE_20180628
		if( m_pRenderDigimon || m_pRenderXDigimon )
#else
		if( m_pRenderDigimon )
#endif
		{
			// 오브젝트 위치 설정	
			CsNodeObj* pCsNode = pUser->GetCsNode();
			if( pCsNode != NULL )
			{
				// 시야에 보이지 않으면 애니메이션 업데이트 풀로 다해준다.
				float fAniTime = pUser->GetAniTime();
				CsC_AniProp* pAniProp = pUser->GetProp_Animation();
				SAFE_POINTER_RET( pAniProp );
				NiActorManager* pActorMng = pAniProp->GetActorMng();
				SAFE_POINTER_RET( pActorMng );
				pActorMng->Update( fAniTime );
				pCsNode->m_pNiNode->Update( fAniTime );
			}
		}
	}

	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE cDigimonStatusUI::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( MUT_OUT_WINDOW == muReturn )
		return muReturn;

	if( m_pAbilList && m_pAbilList->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		if( _SetStatToolTip( m_pAbilList->GetMouseOverItem() ) )
			return muReturn;
	}

	if( m_pSkillGrid && m_pSkillGrid->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		if( _SetSkillTooltip( m_pSkillGrid->GetMouseOverItem() ) )
			return muReturn;
	}

	if( m_pTalkButton && m_pTalkButton->Update_ForMouse() )
		return muReturn;

	if( m_pCloseButton && m_pCloseButton->Update_ForMouse() )
		return muReturn;

	if( m_pHelpButton && m_pHelpButton->Update_ForMouse() )
		return muReturn;

	if( m_pMyNormal && m_pMyNormal->Update_ForMouse() )
	{
		DWORD dwExpRate = GetSystem()->GetNormalAttributeExp( m_pMyNormal->GetState() );
		SAFE_POINTER_RETVAL( TOOLTIPMNG_STPTR, muReturn );
		cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL( pTooltip, muReturn );
		pTooltip->SetTooltip(	
			m_pMyNormal->GetPos(), m_pMyNormal->GetSize(),
			TOOLTIP_MAX_SIZE,
			cTooltip::EXP,
			dwExpRate,
			10000,
			m_pMyNormal->GetState() + nsCsDigimonTable::AT_NONE );	// ( data attribute 1 ~ 5 )
		return muReturn;
	}

	if( m_pMyNature && m_pMyNature->Update_ForMouse() )
	{
		DWORD dwExpRate = GetSystem()->GetNatureAttributeExp( m_pMyNature->GetState() );

		SAFE_POINTER_RETVAL( TOOLTIPMNG_STPTR, muReturn );
		cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL( pTooltip, muReturn );
		pTooltip->SetTooltip(
			m_pMyNature->GetPos(), m_pMyNature->GetSize(),
			TOOLTIP_MAX_SIZE,
			cTooltip::EXP,
			dwExpRate,
			10000,
			m_pMyNature->GetState() + nsCsDigimonTable::NT_ICE );	// ( nature attribute 16 ~ 25 )

		return muReturn;
	}

	if( m_pDigimonExpBar && m_pDigimonExpBar->PtInWindowRect( CURSOR_ST.GetPos() ) )
	{
		// 디지몬 경험치 툴팁
		CDigimonUser::sUSER_STAT* pStat = GetSystem()->GetDigimonBaseStat();
		SAFE_POINTER_RETVAL( pStat, muReturn );
		SAFE_POINTER_RETVAL( TOOLTIPMNG_STPTR, muReturn );
		cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL( pTooltip, muReturn );
		pTooltip->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 10, 0 ), TOOLTIP_MAX_SIZE, cTooltip::EXP,
			pStat->GetExp(), FMDigimon::GetMaxExp( pStat->GetLevel() ) );
		return muReturn;
	}

	if( m_pSkillExpBar && m_pSkillExpBar->PtInWindowRect( CURSOR_ST.GetPos() ) )
	{
		// 디지몬 스킬 경험치 툴팁
		cEvoUnit* pEvoUnit = GetSystem()->GetEvoUnit();
		SAFE_POINTER_RETVAL( pEvoUnit, muReturn );

		SAFE_POINTER_RETVAL( TOOLTIPMNG_STPTR, muReturn );
		cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL( pTooltip, muReturn );

		SAFE_POINTER_RETVAL( g_pCharMng, muReturn );
		CDigimonUser* pUser = g_pCharMng->GetDigimonUser(0);
		SAFE_POINTER_RETVAL( pUser, muReturn );

		pTooltip->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 10, 0 ), TOOLTIP_MAX_SIZE, cTooltip::EXP, pEvoUnit->m_nSkillExp,
			FMDigimon::GetMaxSkillExp( pUser->GetFTID(), pEvoUnit->m_nSkillExpLevel ) );
		return muReturn;
	}

	if( _UpdateMouseEvoInfo() )
		return muReturn;

	if( _SetBaseInfoTooltip() )
		return muReturn;

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 806, 40 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cDigimonStatusUI::Render()
{
	if( m_pBackGround )
		m_pBackGround->Render( GetTotalPosition() );

	if( g_pCharMng )
	{
		CDigimonUser* pTarget = g_pCharMng->GetDigimonUser( 0 );
#ifdef SDM_TAMER_XGUAGE_20180628
		if( !GetSystem()->IsXAnti() )
		{
			if( pTarget && m_pRenderDigimon )
			{
				m_pRenderDigimon->BeginRender();
				m_pRenderDigimon->RenderObject( GET_SUBCAMERA( CAMERA_03 ), pTarget, true );
				m_pRenderDigimon->EndRender( GET_SUBCAMERA( CAMERA_03 ), CsPoint( 15, 114 ) + GetTotalPosition() );
			}
		}
		else
		{
			if( pTarget && m_pRenderXDigimon )
			{
				m_pRenderXDigimon->BeginRender();
				m_pRenderXDigimon->RenderObject( GET_SUBCAMERA( CAMERA_03 ), pTarget, true );
				m_pRenderXDigimon->EndRender( GET_SUBCAMERA( CAMERA_03 ), CsPoint( 15, 114 ) + GetTotalPosition() );
			}
		}
#else
		if( pTarget && m_pRenderDigimon )
		{
			m_pRenderDigimon->BeginRender();
			m_pRenderDigimon->RenderObject( GET_SUBCAMERA( CAMERA_03 ), pTarget, true );
			m_pRenderDigimon->EndRender( GET_SUBCAMERA( CAMERA_03 ), CsPoint( 15, 114 ) + GetTotalPosition() );
		}
#endif
	}
	m_BaseInfo.Render_Overlab( GetTotalPosition(), 0 );

	_RenderLine();

	RenderScript();
}

void cDigimonStatusUI::_RenderLine()
{
	CDigimonEvolveInfo* pEvoInfo = GetSystem()->GetEvoInfo();
	SAFE_POINTER_RET( pEvoInfo );

	SAFE_POINTER_RET( g_pCharMng );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RET( pUser );
	int	nCurTree = pUser->GetEvoTree();

	CDigimonEvolveObj* pEvoObj;	// 테이블 정보

	// 라인
	std::map< int, bool > ExistMap;
	for( int i = 0; i < nLimit::EvoUnit; ++i )	// 진화 사이즈만큼 돌려서
	{
		ExistMap.clear();	// 데이터 클리어

		pEvoObj = pEvoInfo->GetEvolveObjByEvoSlot( i );	// n 번째 진화 정보
		SAFE_POINTER_CON( pEvoObj );
		if( 0 == pEvoObj->m_nEnableSlot )
			continue;

		for( int j = 0; j < MAX_EVOLUTION - 1; ++j )	// 6개
		{
			int nSlot = pEvoObj->m_nEvolutionList[ j ].nSlot;
			if( 0 == nSlot )
				continue;

			CDigimonEvolveObj* pDstEvoObj = pEvoInfo->GetEvolveObjByEvoSlot( nSlot );
			SAFE_POINTER_CON( pDstEvoObj );
			if( 0 == pDstEvoObj->m_nEnableSlot )
				continue;

			for( int k = 0; k < MAX_EVOLUTION - 1; ++k ) // 6개
			{
				if( 0 != pDstEvoObj->m_nEvolutionList[ k ].nSlot )
					ExistMap.insert( std::make_pair( pDstEvoObj->m_nEvolutionList[ k ].nSlot, true ) );
			}

			if( ExistMap.find( nSlot ) != ExistMap.end() )	// 데이터가 있으면
				continue;

			/* 테이블에 정의된 현재슬롯 트리 != 현재 찍은 트리*/
			if( pDstEvoObj->m_nEvolutionTree != nCurTree && CDigimonUser::TREE_COMMON != nCurTree )
			{
				if( pUser->IsJogressTree( pDstEvoObj->m_nEvolutionTree ) )
					continue;
			}

			MAP_EVOINFO_IT itSrc = m_mapEvoInfo.find( i );
			if( itSrc == m_mapEvoInfo.end() )
				continue;
			MAP_EVOINFO_IT itDst = m_mapEvoInfo.find( nSlot );
			if( itDst == m_mapEvoInfo.end() )
				continue;
			CsPoint ptSrc = itSrc->second.GetPos();
			CsPoint ptDst = itDst->second.GetPos();
			CsPoint ptIconSize = CsPoint( 32, 32 );
			if( ptSrc.y == ptDst.y )
			{
				CsPoint size;
				size.x = ptDst.x - ptSrc.x;
				size.y = 6;
				m_pLineX->Render( ptSrc, size );
			}
			else if( ptSrc.x == ptDst.x )
			{
				CsPoint size;
				size.x = 6;
				size.y = ptDst.y - ptSrc.y;
				m_pLineY->Render( ptSrc, size );
			}
			else
			{
				// ㄴ
// 				CsPoint pos;
// 				pos.x = ptSrc.x + 60;
// 				pos.y = ptSrc.y + 6;
// 				CsPoint size;
// 				size.x = 6;
// 				size.y = ptDst.y - ptSrc.y;
// 				m_pLineY->Render( pos, size );
// 
// 				pos.x += 6;
// 				pos.y = ptDst.y;
// 				size.x = ptDst.x - pos.x;
// 				size.y = 6;
// 				m_pLineX->Render( pos, size );

				// ㄱ
				CsPoint pos;
				pos.x = ptSrc.x + 6;
				pos.y = (ptDst.y + ptSrc.y) / 2;
				CsPoint size;
				size.x = ptDst.x - ptSrc.x;
				size.y = 6;
				m_pLineX->Render( pos, size );

				pos.x = ptDst.x;
				pos.y += 6;
				size.x = 6;
				size.y = ptDst.y - pos.y - 6;
				m_pLineY->Render( pos, size );
			}
		}
	}
}

void cDigimonStatusUI::ResetDevice()
{
	ResetDeviceScript();
	m_BaseInfo.ResetDevice();

	if( m_pRenderDigimon )
		m_pRenderDigimon->ResetDevice();
	if( m_pRenderXDigimon )
		m_pRenderXDigimon->ResetDevice();
	if( m_pBackGround )
		m_pBackGround->ResetDevice();
	if( m_pLineX )
		m_pLineX->ResetDevice();
	if( m_pLineY )
		m_pLineY->ResetDevice();
}

void cDigimonStatusUI::OnLButtonUp(CsPoint pos)
{

}

void cDigimonStatusUI::OnRButtonUp(CsPoint pos)
{

}

bool cDigimonStatusUI::CursorIconLBtnUp(CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount)
{
	return false;
}

bool cDigimonStatusUI::OnEscapeKey()
{
	GetSystem()->CloseDigimonStatus();
	return true;
}

void cDigimonStatusUI::_MakeAbilList()
{
	m_pAbilList = NiNew cListBox;
	SAFE_POINTER_RET( m_pAbilList );
	
	m_pAbilList->Init( m_pRoot, CsPoint( 175, 115 ), CsPoint( 162, 255 ), false );
	m_pAbilList->SetItemHeight( 255 / 9 );
	AddChildControl( m_pAbilList );

	m_pAbilList->RemoveAllItem();

	for( int i = 0; i < IF_ABIL_COUNT; ++i )
	{
		_AddAbilList( i );
	}
	_UpdateAbility();
}

void cDigimonStatusUI::_AddAbilList(int nIndex)
{
	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	sAbility sAbil;

	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_11, NiColor::WHITE );
	ti.s_eTextAlign = DT_RIGHT;
	ti.SetText( 0 );
	sAbil.pAbilText = pControl->AddText( &ti, CsPoint( 135, 3 ) );
	if( sAbil.pAbilText )
		sAbil.pAbilText->SetAutoPointerDelete( true );

	sAbil.pAbilListItem = NiNew cListBoxItem( nIndex );
	if( sAbil.pAbilListItem )
	{
		sAbil.pAbilListItem->SetItem( pControl );
		sAbil.pAbilListItem->SetUserData( new sAbilInfo( static_cast< eAbilityType >( nIndex ) ) );
		m_pAbilList->AddItem( sAbil.pAbilListItem );
	}

	m_mapAbility.insert( std::make_pair( nIndex, sAbil ) );
}

void cDigimonStatusUI::_UpdateAbility()
{
	std::map< int, sAbility >::iterator it = m_mapAbility.begin();
	std::map< int, sAbility >::iterator itEnd = m_mapAbility.end();
	for( ; it != itEnd; ++it )
	{
		cListBoxItem* pAbilItem = it->second.pAbilListItem;
		SAFE_POINTER_CON( pAbilItem );
		sAbilInfo* pAbilInfo = dynamic_cast< sAbilInfo* >( pAbilItem->GetUserData() );
		SAFE_POINTER_CON( pAbilInfo );
		std::wstring wstr = _AbilToStr( pAbilInfo->eAbilType );

		int nType = _ChangeToSystemAbilType( pAbilInfo->eAbilType );
		bool bIsUpper = GetSystem()->IsUpperDigimonBaseStat( static_cast<SystemType::eAbilType>( nType ) );

		it->second.UpdateAbility( wstr, bIsUpper );
	}
}

int cDigimonStatusUI::_ChangeToSystemAbilType(eAbilityType eType)
{
	switch( eType )
	{
	case eHP: return SystemType::eHP;
	case eDS: return SystemType::eDS;
	case eAT: return SystemType::eAT;
	case eAS: return SystemType::eAS;
	case eCT: return SystemType::eCT;
	case eHT: return SystemType::eHT;
	case eDE: return SystemType::eDE;
	case eBL: return SystemType::eBL;
	case eEV: return SystemType::eEV;
	}
	return -1;
}

std::wstring cDigimonStatusUI::_AbilToStr(eAbilityType eType)
{
	CDigimonUser::sUSER_STAT* pStat = GetSystem()->GetDigimonBaseStat();
	SAFE_POINTER_RETVAL( pStat, L"" );

	std::wstring strPoint;
	switch( eType )
	{
	case eHP:
		DmCS::StringFn::From( strPoint, pStat->GetMaxHP() );
		break;
	case eDS:
		DmCS::StringFn::From( strPoint, pStat->GetMaxDS() );
		break;
	case eAT:
		DmCS::StringFn::From( strPoint, pStat->GetAtt() );
		break;
	case eAS:
		DmCS::StringFn::FloatToString( strPoint, pStat->GetAttackSpeed(), 4 );
		break;
	case eCT:
		DmCS::StringFn::From( strPoint, pStat->GetCritical()/100 );
		strPoint.append( L"%" );
		break;
	case eHT:
		DmCS::StringFn::From( strPoint, pStat->GetHitRate() );
		break;
	case eDE:
		DmCS::StringFn::From( strPoint, pStat->GetDef() );
		break;
	case eBL:
		DmCS::StringFn::From( strPoint, pStat->GetBL() );
		strPoint.append( L"%" );
		break;
	case eEV:
		DmCS::StringFn::From( strPoint, pStat->GetEvade()/100 );
		strPoint.append( L"%" );
		break;
	}

	return strPoint;
}

void cDigimonStatusUI::_MakeSkillGrid()
{
	m_pSkillGrid = NiNew cGridListBox;
	SAFE_POINTER_RET( m_pSkillGrid );

	m_pSkillGrid->Init( m_pRoot, CsPoint( 34, 592 ), CsPoint( 282, 32 ), CsPoint( 18, 0 ), CsPoint( 32, 32 ), cGridListBox::LowRightDown, cGridListBox::LeftTop, false, 6 );
	m_pSkillGrid->SetAutoSelection( false );
	m_pSkillGrid->SetBackOverAndSelectedImgRender( false );
	m_pSkillGrid->AddEvent( cGridListBox::GRID_MOUSE_DOWN_ON_ITEM, this, &cDigimonStatusUI::_OnLClickDownSkill );
	m_pSkillGrid->AddEvent( cGridListBox::GRID_BUTTON_UP, this, &cDigimonStatusUI::_OnClickSkillButton );
	AddChildControl( m_pSkillGrid );

	m_pSkillGrid->RemoveAllItem();

	for( int i = 0; i < IF_SKILL_COUNT; ++i )
	{
		_AddSkillGrid( i );
	}
	_UpdateSkill();
}

void cDigimonStatusUI::_AddSkillGrid(int nIndex)
{
	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	sSkill sDSkill;

	sDSkill.pSkillIcon = pControl->AddSkillIcon( CsPoint( 32, 32 ), 0, 0 );
	if( sDSkill.pSkillIcon )
	{
		sDSkill.pSkillIcon->SetAutoPointerDelete( true );
		sDSkill.pSkillIcon->SetEnable( false );
		sDSkill.pSkillIcon->SetVisible( false );
		sDSkill.pSkillIcon->SetCoolTimeMode( true );
		//sDSkill.pSkillIcon->SetCoolTimeSeq( /*pCoolTime*/ );
	}

	// 스킬 비활성화
	cSprite* pMask = NiNew cSprite;
	pMask->Init( NULL, CsPoint::ZERO, CsPoint( 32, 32 ), false, NiColor::BLACK );
	pMask->SetAlpha( 0.3f );
	sDSkill.pSkillMask = pControl->AddSprite( pMask );
	if( sDSkill.pSkillMask )
	{
		sDSkill.pSkillMask->SetAutoPointerDelete( true );
		sDSkill.pSkillMask->SetVisible( false );
	}

	// max 슬롯 이미지
	cSprite* pSlot = NiNew cSprite;
	pSlot->Init( NULL, CsPoint::ZERO, CsPoint( 42, 42 ), "DigimonStatus_New\\Digimon_skill_frame.tga", false );
	sDSkill.pSkillSlotImg = pControl->AddSprite( pSlot, CsPoint( -5, -5 ) );
	if( sDSkill.pSkillSlotImg )
	{
		sDSkill.pSkillSlotImg->SetAutoPointerDelete( true );
		sDSkill.pSkillSlotImg->SetVisible( false );
	}

	// 슬롯 락 이미지
	cSprite* pLock = NiNew cSprite;
	pLock->Init( NULL, CsPoint::ZERO, CsPoint( 14, 19 ), "DigimonStatus_New\\Digimon_skill_lock.tga", false );
	sDSkill.pSkillLockImg = pControl->AddSprite( pLock, CsPoint( 9, 7 ) );
	if( sDSkill.pSkillLockImg )
	{
		sDSkill.pSkillLockImg->SetAutoPointerDelete( true );
		sDSkill.pSkillLockImg->SetVisible( false );
	}

	// 스킬 레벨 업 버튼
	cButton* pLvUp = NiNew cButton;
	pLvUp->Init( NULL, CsPoint::ZERO, CsPoint( 23, 23 ), "DigimonStatus_New\\Digimon_skill_btn.tga", false );
	pLvUp->SetTexToken( CsPoint( 0, 23 ) );
	pLvUp->SetUserData( new sButtonInfo( eLevelUp, nIndex ) );
	sDSkill.pSkillLvUpBtn = pControl->AddButton( pLvUp, nIndex, CsPoint( 16, 16 ), CsPoint( 23, 23 ) );
	if( sDSkill.pSkillLvUpBtn )
	{
		sDSkill.pSkillLvUpBtn->SetAutoPointerDelete( true );
		sDSkill.pSkillLvUpBtn->SetVisible( false );
	}

	// 스킬 삭제 버튼
	cButton* pDelete = NiNew cButton;
	pDelete->Init( NULL, CsPoint::ZERO, CsPoint( 12, 12 ), "System\\Ch_Close.tga", false );
	pDelete->SetTexToken( CsPoint( 0, 16 ) );
	pDelete->SetUserData( new sButtonInfo( eDelete, nIndex ) );
	sDSkill.pSkillDeleteBtn = pControl->AddButton( pDelete, nIndex, CsPoint( 20, 0 ), CsPoint( 12, 12 ) );
	if( sDSkill.pSkillDeleteBtn )
	{
		sDSkill.pSkillDeleteBtn->SetAutoPointerDelete( true );
		sDSkill.pSkillDeleteBtn->SetVisible( false );
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor( 76.f/255.f, 242.f/255.f, 1 ) );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( L"" );
		sDSkill.pSkillLevelTxt = pControl->AddText( &ti, CsPoint( 16, 40 ) );
		if( sDSkill.pSkillLevelTxt )
		{
			sDSkill.pSkillLevelTxt->SetAutoPointerDelete( true );
			sDSkill.pSkillLevelTxt->SetVisible( false );
		}
	}

	sDSkill.pSkillGridItem = NiNew cGridListBoxItem( nIndex, CsPoint( 32, 32 ) );
	if( sDSkill.pSkillGridItem )
	{
		sDSkill.pSkillGridItem->SetItem( pControl );
		sDSkill.pSkillGridItem->SetUserData( new sSkillInfo( 0, nIndex ) );
		m_pSkillGrid->AddItem( sDSkill.pSkillGridItem );
	}

	m_mapSkill.insert( std::make_pair( nIndex, sDSkill ) );
}

void cDigimonStatusUI::_UpdateSkill()
{
	cEvoUnit* pEvo = GetSystem()->GetEvoUnit();
	SAFE_POINTER_RET( pEvo );
	// 스킬 숙련도
	if(m_pSkillExp )
	{
		if( pEvo->m_nSkillExpLevel < nLimit::SkillLevel )
		{
			m_pSkillExp->SetColor( NiColor( 188.f/255.f, 97.f/255.f, 1 ) );
			m_pSkillExp->SetText( pEvo->m_nSkillExpLevel );
		}
		else
		{
			m_pSkillExp->SetColor( FONT_RED );
			m_pSkillExp->SetText( L"Max" );
		}
	}
	// 스킬 포인트
	if( m_pSkillPoint )
		m_pSkillPoint->SetText( pEvo->m_nSkillPoint );

	std::map< int, sSkill >::iterator it = m_mapSkill.begin();
	std::map< int, sSkill >::iterator itEnd = m_mapSkill.end();
	for( ; it != itEnd; ++it )
	{
		cGridListBoxItem* pSkillItem = it->second.pSkillGridItem;
		SAFE_POINTER_CON( pSkillItem );
		sSkillInfo* pSkillInfo = dynamic_cast< sSkillInfo* >( pSkillItem->GetUserData() );
		SAFE_POINTER_CON( pSkillInfo );
		CsSkill::sINFO* pFTSkillInfo = GetSystem()->GetDigimonFTSkillInfo( pSkillInfo->nSkillIndex );
		if( !pFTSkillInfo )
		{
			pSkillItem->SetEnable( false );
			continue;
		}

		bool bIsLvUp = GetSystem()->IsSkillLevelUp( pSkillInfo->nSkillIndex );
		bool bIsMask = GetSystem()->IsUseSkill( pSkillInfo->nSkillIndex );
#ifdef SDM_DIGIMONSKILL_LV_EXPEND_20181206
		it->second.UpdateSkill( 
			pFTSkillInfo->s_nIcon,
			pEvo->m_nSkillLevel[ pSkillInfo->nSkillIndex ],
			pEvo->m_nSkillMaxLevel[ pSkillInfo->nSkillIndex ],
			bIsLvUp,
			bIsMask	);
#else
		it->second.UpdateSkill( 
			pFTSkillInfo->s_nIcon,
			pEvo->m_nSkillLevel[ pSkillInfo->nSkillIndex ],
			pFTSkillInfo->s_nMaxLevel,
			bIsLvUp,
			bIsMask	);
#endif
		it->second.SetDeleteButton( pFTSkillInfo->s_nMemorySkill );
	}
}

void cDigimonStatusUI::_ResetSKill()
{
	std::map< int, sSkill >::iterator it = m_mapSkill.begin();
	std::map< int, sSkill >::iterator itEnd = m_mapSkill.end();
	for( ; it != itEnd; ++it )
	{
		it->second.ResetSkill();
	}
}

void cDigimonStatusUI::_MakeBaseInfo()
{
	m_BaseInfo.Delete();

	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	CsDigimon::sINFO* pFTInfo = GetSystem()->GetDigimonFTInfo();
	SAFE_POINTER_RET( pFTInfo );

	if( 0 < pFTInfo->s_nDigimonRank )
	{
		cImage* pRank = NiNew cImage;
		if( pRank )
		{
			pRank->Init( NULL, CsPoint::ZERO, CsPoint( 72, 62 ), "CommonUI\\digimon_rank.tga", false );
			pRank->SetTexToken( CsPoint( 72, 62 ) );
			pRank->SetEnable( false );
			cString::sIMAGE* pImg = pControl->AddImage( pRank, pFTInfo->s_nDigimonRank - 1, CsPoint( 17, 117 ) );
			if( pImg )
				pImg->SetAutoPointerDelete( true );
		}
	}

#ifdef SDM_DIGIMON_PARTSSYSTEM_20200115
	// 디지몬 스킨 이펙트가 존재하면
	if( GetSystem()->IsActiveDigimonParts( nsDPART::Skin ) )
	{
		cSprite* pEffect = NiNew cSprite;
		if( pEffect )
		{
			pEffect->Init( NULL, CsPoint::ZERO, CsPoint( 32, 32 ), "DigimonStatus_New\\effect_glow.tga", false );
			cString::sSPRITE* pSprt = pControl->AddSprite( pEffect, CsPoint( 23, 300 ) );
			if( pSprt )
				pSprt->SetAutoPointerDelete( true );
		}
	}
#endif

	{	// 진화 타입
		SAFE_POINTER_RET( nsCsFileTable::g_pBaseMng );
		std::wstring wsEvoType = nsCsFileTable::g_pBaseMng->GetEvoTypeName( pFTInfo->s_eEvolutionType );
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor( 66.f/255.f, 169.f/255.f, 203.f/255.f ) );
		ti.s_eTextAlign = DT_LEFT;
		SetSafeOwnedText( ti, wsEvoType );
		pControl->AddText( &ti, CsPoint( 23, 337 ) );
	}

	{
		// 디지몬 단계
		int nHatchLv = GetSystem()->GetDigimonHatchLv();
		std::wstring wsHatch = UISTRING_TEXT( "DIGIMONSTATUS_BASEINFO_HATCHLV" );
		DmCS::StringFn::Replace( wsHatch, L"#HatchLv#", nHatchLv );
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.s_eTextAlign = DT_LEFT;
		SetSafeOwnedText( ti, wsHatch );
		pControl->AddText( &ti, CsPoint( 23, 356 ) );
	}

	{	// 디지몬 크기 정보
		int nTall = 0;
		float fPercent = 0;
		bool bIsBuff = false;
		GetSystem()->GetDigimonSizeInfo( nTall, fPercent, bIsBuff );
		
		std::wstring wsSize;
		DmCS::StringFn::Format( wsSize, L"%d cm (%.2f%%)", nTall, fPercent );
		NiColor niColor = NiColor::WHITE;
		if( bIsBuff )
			niColor = NiColor( 122.f/255.f, 254.f/255.f, 0 );
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, niColor );
		ti.s_eTextAlign = DT_LEFT;
		SetSafeOwnedText( ti, wsSize );
		cString::sTEXT* pTxt = pControl->AddText( &ti, CsPoint( 23, 375 ) );
		if( pTxt )
			pTxt->SetAutoPointerDelete( true );
	}

	// 성장 타입
		int nGrowType = pFTInfo->s_nDigimonType - 1;
		if( nGrowType < 0 )
			nGrowType = 0;
		cString::sICON* pGrowIcon = pControl->AddIcon( CsPoint( 30, 19 ), ICONITEM::DGrowType, nGrowType, 0, CsPoint( 22, 398 ) );
	if( pGrowIcon )
		pGrowIcon->SetAutoPointerDelete( true );

	CsPoint ptIconPos = CsPoint( 52, 398 );
	// 패밀리 속성
	for( int i = 0; i < DIGIMON_MAX_FAMILY; ++i )
	{
		if( pFTInfo->s_eFamilyType[ i ] != 0 )
		{
				int nFamily = pFTInfo->s_eFamilyType[ i ] - nsCsDigimonTable::FT_DR;
				if( nFamily < 0 )
					nFamily = 0;
				cString::sICON* pFamilyIcon = pControl->AddIcon(
					CsPoint( 19, 19 ),
					ICONITEM::DAttribute,
					cIconMng::Family,
					nFamily,
					ptIconPos );
			ptIconPos.x += 19;
			if( pFamilyIcon )
				pFamilyIcon->SetAutoPointerDelete( true );
		}
	}

	// 라이딩 정보
	bool bIsRide = false;
	int nImgIndex = 1;
	GetSystem()->GetDigimonRideInfo( bIsRide, nImgIndex );
	if( bIsRide )
	{
		cImage* pRide = NiNew cImage;
		if( pRide )
		{
			pRide->Init( NULL, CsPoint::ZERO, CsPoint( 19, 19 ), "DigimonStatus\\Ride.tga", false );
			pRide->SetTexToken( CsPoint( 19, 0 ) );
			pRide->SetEnable( true );
		}
		cString::sIMAGE* pImg = pControl->AddImage( pRide, nImgIndex, ptIconPos );
		if( pImg )
			pImg->SetAutoPointerDelete( true );
	}

	m_BaseInfo.AddTail( pControl );
}

void cDigimonStatusUI::_UpdateBaseInfo()
{
	CDigimonUser::sUSER_STAT* pStat = GetSystem()->GetDigimonBaseStat();
	if( pStat && m_pDigimonLevel )
	{
			// 레벨
			std::wstring wsLevel;
			DmCS::StringFn::Format( wsLevel, L"%s %d", L"Lv.", pStat->GetLevel() );
			SetSafeOwnedText( m_pDigimonLevel, wsLevel );
		}

	if( m_pDigimonName )
	{
			// 이름
			std::wstring wsName = GetSystem()->GetDigimonName();
			SetSafeOwnedText( m_pDigimonName, wsName );
		}

	CDigimonUser::sENCHENT_STAT* pEnchantStat = GetSystem()->GetDigimonEnchantStat();
	if( pEnchantStat && m_pEnchant && m_pEnchantMax )
	{
			// 강화 수치
			std::wstring wsEnchant;
			DmCS::StringFn::Format( wsEnchant, L"%d/%d", pEnchantStat->GetEnchantLevel(), nLimit::MaxInchantLevel );
			SetSafeOwnedText( m_pEnchant, wsEnchant );

		if( nLimit::MaxInchantLevel < pEnchantStat->GetEnchantLevel() )
		{
			m_pEnchant->SetVisible( false );
			m_pEnchantMax->SetVisible( true );
		}
		else
		{
			m_pEnchant->SetVisible( true );
			m_pEnchantMax->SetVisible( false );
		}
	}
}

void cDigimonStatusUI::_MakeAttributeInfo()
{
	// 기본 속성
	m_pMyNormal = NiNew cImage;
	m_pMyNormal->Init( m_pRoot, CsPoint( 29, 436 ), CsPoint( 60, 60 ), "CommonUI\\Normal_L.tga", false );
	m_pMyNormal->SetTexToken( CsPoint( 60, 0 ) );
	m_pMyNormal->AddEvent( cImage::IMAGE_LBUP_EVENT, this, &cDigimonStatusUI::_OnClickAttributeNormal );
	AddChildControl( m_pMyNormal );

	m_pNormalFull = AddSprite( CsPoint( 29, 436 ), CsPoint( 60, 60 ), "DigimonStatus_New\\Digimon_full_charge.tga" );
	if( m_pNormalFull )
		m_pNormalFull->SetVisible( false );

	// 기본 속성 경험치
	m_pNormalExp = NiNew cUI_ProgressBar;
	if( m_pNormalExp )
	{
		m_pNormalExp->Init( m_pRoot, CsPoint( 35, 500 ), CsPoint( 50, 4 ), false );
		m_pNormalExp->SetViewTextType( cUI_ProgressBar::NONE );
		m_pNormalExp->SetBackImage( "DigimonStatus_New\\Digimon_gauge_bg.tga" );
		m_pNormalExp->SetGuageImage( "DigimonStatus_New\\Digimon_gauge_bar.tga" );
		m_pNormalExp->SetRange( 10000 );
		AddChildControl( m_pNormalExp );
	}

	_AddNormalElement( CsPoint( 91, 453 ) );
	_AddNormalElement( CsPoint( 121, 453 ) );
	_AddNormalElement( CsPoint( 151, 453 ) );
	_AddNormalElement( CsPoint( 181, 453 ) );

	// 자연 속성
	m_pMyNature = NiNew cImage;
	m_pMyNature->Init( m_pRoot, CsPoint( 226, 436 ), CsPoint( 60, 60 ), "CommonUI\\Nature_L.tga", false );
	m_pMyNature->SetTexToken( CsPoint( 60, 60 ) );
	m_pMyNature->AddEvent( cImage::IMAGE_LBUP_EVENT, this, &cDigimonStatusUI::_OnClickAttributeNature );
	AddChildControl( m_pMyNature );

	m_pNatureFull = AddSprite( CsPoint( 226, 436 ), CsPoint( 60, 60 ), "DigimonStatus_New\\Digimon_full_charge.tga" );
	if( m_pNatureFull )
		m_pNatureFull->SetVisible( false );

	// 자연 속성 경험치
	m_pNatureExp = NiNew cUI_ProgressBar;
	if( m_pNatureExp )
	{
		m_pNatureExp->Init( m_pRoot, CsPoint( 233, 500 ), CsPoint( 50, 4 ), false );
		m_pNatureExp->SetViewTextType( cUI_ProgressBar::NONE );
		m_pNatureExp->SetBackImage( "DigimonStatus_New\\Digimon_gauge_bg.tga" );
		m_pNatureExp->SetGuageImage( "DigimonStatus_New\\Digimon_gauge_exp.tga" );
		m_pNatureExp->SetRange( 10000 );
		AddChildControl( m_pNatureExp );
	}

	_AddNatureElement( CsPoint( 289, 435 ) );
	_AddNatureElement( CsPoint( 289, 472 ) );

	_UpdateElement();
}

void cDigimonStatusUI::_AddNormalElement(CsPoint &ptStart)
{
	sElementUI sElement;
	sElement.pElement = NiNew cImage;
	sElement.pElement->Init( m_pRoot, ptStart, CsPoint( 26, 26 ), "CommonUI\\Normal_S.tga", false );
	sElement.pElement->SetTexToken( CsPoint( 26, 0 ) );
	AddChildControl( sElement.pElement );

	ptStart.x += 8;
	ptStart.y -= 5;
	sElement.pUpArrow = AddSprite( ptStart, CsPoint( 9, 6 ), "DigimonStatus_New\\Digimon_element_up.tga" );
	sElement.pUpArrow->SetVisible( false );

	ptStart.y += 30;
	sElement.pDownArrow = AddSprite( ptStart, CsPoint( 9, 6 ), "DigimonStatus_New\\Digimon_element_down.tga" );
	sElement.pDownArrow->SetVisible( false );

	m_mapCompNormal.insert( std::make_pair( m_mapCompNormal.size(), sElement ) );
}

void cDigimonStatusUI::_AddNatureElement(CsPoint& ptStart)
{
	sElementUI sElement;
	sElement.pElement = NiNew cImage;
	sElement.pElement->Init( m_pRoot, ptStart, CsPoint( 26, 26 ), "CommonUI\\Nature_S.tga", false );
	sElement.pElement->SetTexToken( CsPoint( 26, 26 ) );
	AddChildControl( sElement.pElement );

	ptStart.x += 8;
	ptStart.y -= 5;
	sElement.pUpArrow = AddSprite( ptStart, CsPoint( 9, 6 ), "DigimonStatus_New\\Digimon_element_up.tga" );
	sElement.pUpArrow->SetVisible( false );

	ptStart.y += 30;
	sElement.pDownArrow = AddSprite( ptStart, CsPoint( 9, 6 ), "DigimonStatus_New\\Digimon_element_down.tga" );
	sElement.pDownArrow->SetVisible( false );

	m_mapCompNature.insert( std::make_pair( m_mapCompNature.size(), sElement ) );
}

void cDigimonStatusUI::_UpdateElement()
{
	// 기본 속성
	int nNormal = GetSystem()->GetNormalAttributeType();
	if( nNormal < 0 )
		nNormal = 0;
	if( 0 != nNormal && m_pMyNormal )
	{
		m_pMyNormal->SetState( nNormal );
		bool bIsFull = false;
		if( GetSystem()->IsActiveRewardNormal( nNormal ) )
			bIsFull = true;
		if( m_pNormalFull )
			m_pNormalFull->SetVisible( bIsFull );
	}

	int nCmpNormal = 0;
	MAP_ELEMENT_IT it = m_mapCompNormal.begin();
	MAP_ELEMENT_IT itEnd = m_mapCompNormal.end();
	for( ; it != itEnd; ++it )
	{
		if( nNormal == nCmpNormal )
			++nCmpNormal;

			bool bIsUp = false;
			if( nsCsFileTable::g_pNatureMng )
			{
				bIsUp = nsCsFileTable::g_pNatureMng->IsSuperiority(
				nsBaseElement::eAtt,
				static_cast<nsBaseElement::eElementType>( nNormal ),
				static_cast<nsBaseElement::eElementType>( nCmpNormal ) );
			}
			it->second.SetSuperiority( bIsUp );
			it->second.ChangeElement( nCmpNormal++ );
		}

	// 자연 속성
	int nNature = GetSystem()->GetNatureAttributeType();
	if( nNature < 0 )
		nNature = 0;
	if( m_pMyNature )
	{
		m_pMyNature->SetState( nNature );
		bool bIsFull = false;
		if( GetSystem()->IsActiveRewardNature( nNature ) )
			bIsFull = true;
		if( m_pNatureFull )
			m_pNatureFull->SetVisible( bIsFull );
	}

	int nUpType = 0, nDownType = 0;
	GetSystem()->GetDigimonNatureInfo( nNature, nUpType, nDownType );
	if( nUpType < 0 ) nUpType = 0;
	if( nDownType < 0 ) nDownType = 0;

	MAP_ELEMENT_IT itNature = m_mapCompNature.find( 0 );
	if( itNature != m_mapCompNature.end() )
	{
		itNature->second.ChangeElement( nUpType );
		itNature->second.SetSuperiority( true );
	}

	itNature = m_mapCompNature.find( 1 );
	if( itNature != m_mapCompNature.end() )
	{
		itNature->second.ChangeElement( nDownType );
		itNature->second.SetSuperiority( false );
	}

	if( nsCsDigimonTable::NT_NONE - 1 == nUpType && nsCsDigimonTable::NT_NONE - 1 == nDownType )
	{
		MAP_ELEMENT_IT itBe = m_mapCompNature.begin();
		MAP_ELEMENT_IT itEn = m_mapCompNature.end();
		for( ; itBe != itEn; ++itBe )
			itBe->second.Reset();
	}
}

void cDigimonStatusUI::_MakeEvoInfo()
{
	for( int i = 0; i < nLimit::EvoUnit; ++i )
	{
		_AddEvoInfo();
	}
	_UpdateEvoInfo();
}

void cDigimonStatusUI::_AddEvoInfo()
{
	int nIndex = m_mapEvoInfo.size();

	sEvoInfoUI sEvo;
	sEvo.pSlotBgImg = AddSprite( CsPoint::ZERO, CsPoint( 44, 43 ), "DigimonStatus_New\\Digimon_evo_slot.tga" );
	if( sEvo.pSlotBgImg )
		sEvo.pSlotBgImg->SetVisible( false );

	sEvo.pDigimonImg = NiNew cImage;
	if( sEvo.pDigimonImg )
	{
		sEvo.pDigimonImg->Init( m_pRoot, CsPoint::ZERO, CsPoint( 32, 32 ), "", false, false );
		sEvo.pDigimonImg->SetVisible( false );
		sEvo.pDigimonImg->SetEnable( false );
		sEvo.pDigimonImg->SetMouseOverImg( "Icon\\Mask_Over.tga" );
		sEvo.pDigimonImg->SetUserData( new sEvoBtnInfo( nIndex ) );
		sEvo.pDigimonImg->AddEvent( cImage::IMAGE_LBUP_EVENT, this, &cDigimonStatusUI::_OnClickDigimonImage );
		AddChildControl( sEvo.pDigimonImg );
	}

	sEvo.pLockImg = AddSprite( CsPoint::ZERO, CsPoint( 32, 32 ), "DigimonStatus_New\\Block.tga" );
	if( sEvo.pLockImg )
		sEvo.pLockImg->SetVisible( false );

	sEvo.pRideFrameImg = AddSprite( CsPoint::ZERO, CsPoint( 42, 42 ), "DigimonStatus_New\\Digimon_riding_frame.tga" );
	if( sEvo.pRideFrameImg )
		sEvo.pRideFrameImg->SetVisible( false );

	sEvo.pEvolBtn = AddButton( CsPoint::ZERO, CsPoint( 32, 32 ), CsPoint( 0, 32 ), "DigimonStatus_New\\BlockBtn.tga" );
	if( sEvo.pEvolBtn )
	{
		sEvo.pEvolBtn->SetEnable( false );
		sEvo.pEvolBtn->SetVisible( false );
		sEvo.pEvolBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cDigimonStatusUI::_OnClickEvoButton );
		sEvo.pEvolBtn->SetUserData( new sEvoBtnInfo( nIndex ) );
	}

	sEvo.pRideBtn = AddButton( CsPoint::ZERO, CsPoint( 23, 23 ), CsPoint( 0, 23 ), "DigimonStatus_New\\Digimon_Evo_btn.tga" );
	if( sEvo.pRideBtn )
	{
		sEvo.pRideBtn->SetEnable( false );
		sEvo.pRideBtn->SetVisible( false );
		sEvo.pRideBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cDigimonStatusUI::_OnClickRideButton );
		sEvo.pRideBtn->SetUserData( new sEvoBtnInfo( nIndex ) );
	}

	sEvo.pSelMaskImg = AddSprite( CsPoint::ZERO, CsPoint( 44, 44 ), "DigimonStatus_New\\Sel_OutMask.tga" );
	if( sEvo.pSelMaskImg )
		sEvo.pSelMaskImg->SetVisible( false );

	m_mapEvoInfo.insert( std::make_pair( nIndex, sEvo ) );
}

void cDigimonStatusUI::_UpdateEvoInfo()
{
	CDigimonEvolveInfo* pEvoInfo = GetSystem()->GetEvoInfo();
	SAFE_POINTER_RET( pEvoInfo );

	SAFE_POINTER_RET( g_pCharMng );
	CDigimonUser* pUser = g_pCharMng->GetDigimonUser(0);
	SAFE_POINTER_RET( pUser );

	for( int i = 0; i < nLimit::EvoUnit; ++i )
	{
		MAP_EVOINFO_IT it = m_mapEvoInfo.find( i );
		if( it == m_mapEvoInfo.end() )
			continue;

		CDigimonEvolveObj* pEvoObj = pEvoInfo->GetEvolveObjByEvoSlot( i );
		SAFE_POINTER_CON( pEvoObj );
		if( 0 == pEvoObj->m_nEnableSlot )
			continue;

		// Check XAnti
		bool bIsXAnti = GetSystem()->IsXAnti( pEvoObj->m_dwID );

		// 진화 디지몬 이미지 세팅
		std::string strPath = GetSystem()->GetEvoFileName( i );
		int nState = cImage::NORMAL2;

		switch( pUser->GetEvoValue( i, CDigimonUser::eActive_Mask ) )
		{
		case CDigimonUser::eActive:
			{
				CDigimonEvolveObj* pCurEvo = pUser->GetFTEvolCurObj();
				SAFE_POINTER_CON( pCurEvo );
				if( 0 == pCurEvo->m_nEnableSlot )
					continue;

				nState = GetSystem()->IsActiveEvoSlot( pCurEvo, i ) ? cImage::NORMAL0 : cImage::NORMAL1;
				if( i == pCurEvo->m_nEvoSlot )
				{
					nState = cImage::NORMAL0;
					it->second.SetSlot( strPath, i, nState, true, false, false, false, bIsXAnti );
				}
				else
				{
					it->second.SetSlot( strPath, i, nState, true, true, false, false, bIsXAnti );
				}
			}
			break;
		case CDigimonUser::eInActive:
			{
				it->second.SetSlot( strPath, i, nState, true, true, false, false, bIsXAnti );
			}
			break;
		case CDigimonUser::eClosed:
			{
				// 모든 슬롯 오픈됐는지 확인
				bool bAllOpen = true;
				for( int j = 1; j < nLimit::EvoUnit; ++j )
				{
					CDigimonEvolveObj* pCheckEvoObj = pEvoInfo->GetEvolveObjByEvoSlot( j );
					SAFE_POINTER_CON( pCheckEvoObj );
					if( 0 == pCheckEvoObj->m_nEnableSlot )
						continue;
					if( GetSystem()->IsActiveEvoSlot( pCheckEvoObj, i ) )
					{
						if( pUser->IsEvoPlag( j, CDigimonUser::eClosed ) )
						{
							bAllOpen = false;
							break;
						}
					}
				}

				int nCurTree = pUser->GetEvoTree();
				if( pEvoObj->m_nEvolutionTree != nCurTree && CDigimonUser::TREE_COMMON != nCurTree )
				{
					if( pUser->IsJogressTree( pEvoObj->m_nEvolutionTree ) )
						continue;
				}
				if( bAllOpen )
					it->second.SetSlot( strPath, i, nState, true, true, true, true, bIsXAnti );
				else
					it->second.SetSlot( strPath, i, nState, true, true, true, false, bIsXAnti );
			}
			break;
		}

		bool bIsRide = false;
		if( GetSystem()->IsActiveOpenRide( i ) )
		{
			it->second.pRideBtn->SetVisible( !bIsRide );
			it->second.pRideBtn->SetEnable( !bIsRide );
		}
		else
		{
			if( GetSystem()->IsOpenedRide( i ) )
			{
				bIsRide = true;
				it->second.pRideBtn->SetVisible( !bIsRide );
				it->second.pRideBtn->SetEnable( !bIsRide );
			}
		}
		if( it->second.pRideFrameImg )
			it->second.pRideFrameImg->SetVisible( bIsRide );

		it->second.SetPos( pEvoObj->m_IconPos, bIsXAnti );
	}
}

bool cDigimonStatusUI::_UpdateMouseEvoInfo()
{
	int nSize = m_mapEvoInfo.size();
	MAP_EVOINFO_IT itEnd = m_mapEvoInfo.end();
	for( int i = 0; i < nSize; ++i )
	{
		MAP_EVOINFO_IT it = m_mapEvoInfo.find( i );
		if( it != itEnd )
		{
			if( it->second.UpdateMouse( i ) )
				return true;
		}
	}
	return false;
}

void cDigimonStatusUI::_ResetEvoInfo()
{
	MAP_EVOINFO_IT it = m_mapEvoInfo.begin();
	MAP_EVOINFO_IT itEnd = m_mapEvoInfo.end();
	for( ; it != itEnd; ++it )
	{
		it->second.Reset();
	}
}

bool cDigimonStatusUI::_RequestEvolve(int nIndex)
{
	CDigimonEvolveObj* pEvoObj = GetSystem()->GetEvoCurObject();
	SAFE_POINTER_RETVAL( pEvoObj, false );
	for( int i = 0; i < MAX_EVOLUTION; ++i )
	{
		if( pEvoObj->m_nEvolutionList[ i ].nSlot == nIndex )
		{
			if( g_pGameIF )
			{
				cQuickEvol* pQuickEvol = g_pGameIF->GetQuickEvol();
				SAFE_POINTER_RETVAL( pQuickEvol, false );
				pQuickEvol->Req_ActivateEvolve( i );
				return true;
			}
		}
	}
	return false;
}

bool cDigimonStatusUI::_SetBaseInfoTooltip()
{
	CsPoint ptPos = CURSOR_ST.GetPos();
	CsPoint ptMouseLocalPos = MousePosToWindowPos( ptPos );

	// 디지몬 속성 툴팁
	cString* pControl = m_BaseInfo.GetTail();
	SAFE_POINTER_RETVAL( pControl, false );
	int nCount = pControl->GetListSize();

	SAFE_POINTER_RETVAL( TOOLTIPMNG_STPTR, false );
	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RETVAL( pTooltip, false );

	for( int i = 0; i < nCount; ++i )
	{
		cString::sELEMENT* pElement = pControl->GetElement( i );
		SAFE_POINTER_CON( pElement );

		switch( pElement->GetType() )
		{
		case cString::sELEMENT::ICON:
			{
				cString::sICON* pIcon = dynamic_cast<cString::sICON*>(pElement);
				CsRect rc = CsRect( pIcon->s_ptLastPos, CsSIZE( pIcon->s_ptIconSize ) );
				if( rc.PtInRect( ptPos ) )
				{
					switch( pIcon->s_nIconType )
					{
					case ICONITEM::DAttribute:
						pTooltip->SetTooltip( ptPos, CsPoint( 10, 0 ), TOOLTIP_MAX_SIZE, cTooltip::D_ATTRIBUTE, pIcon->s_nIndex_1, pIcon->s_nIndex_2 );
						break;
					case ICONITEM::DGrowType:
						pTooltip->SetTooltip( ptPos, CsPoint( 10, 0 ), TOOLTIP_MAX_SIZE, cTooltip::D_GROW_TYPE, pIcon->s_nIndex_1 + nsCsDigimonTable::GT_SHORT_ATTACKER );
						break;
					default:
						assert_cs( false );
					}
					return true;
				}
			}
			break;
		case cString::sELEMENT::IMAGE:
			{
				cString::sIMAGE* pImage = dynamic_cast<cString::sIMAGE*>(pElement);
				CsRect rc = CsRect( pImage->GetWorldPos() + GetTotalPosition(), CsSIZE( pImage->s_ptSize ) );
				if( rc.PtInRect( ptPos ) )
				{
					cImage* pElemImg = pImage->s_pImage;
					SAFE_POINTER_CON( pElemImg );
					if( !pElemImg->GetEnable() )
						continue;
					std::wstring wsExplain;
					switch( pElemImg->GetState() )
					{
					case 0:
						wsExplain = UISTRING_TEXT( "DIGIMONSTATUS_ELEMENT_USABLE_RIDE" );
						break;
					case 1:
						wsExplain = UISTRING_TEXT( "DIGIMONSTATUS_ELEMENT_NOTOPEN_RIDE" );
						break;
					case 2:
						wsExplain = UISTRING_TEXT( "DIGIMONSTATUS_ELEMENT_UNUSABLE_RIDE" );
						break;
					}
					pTooltip->SetTooltip_Text( ptPos, CsPoint( 10, 0 ), wsExplain.c_str() );
					return true;
				}
			}
			break;
		case cString::sELEMENT::SPRITE:
			{
				cString::sSPRITE* pSprite = dynamic_cast<cString::sSPRITE*>(pElement);
				CsRect rc = CsRect( pSprite->GetWorldPos(), CsSIZE( pSprite->s_ptSize ) );
				if( rc.PtInRect( ptPos ) )
				{
					cSprite* pEffectSprt = pSprite->s_pSprite;
					SAFE_POINTER_CON( pEffectSprt );
					if( !pEffectSprt->GetVisible() )
						continue;
					pTooltip->SetTooltip_Text( ptPos, CsPoint( 10, 0 ), UISTRING_TEXT( "DIGIMON_EFFECT_CHANGE_ACTIVE" ).c_str() );
					return true;
				}
			}
			break;
		}
	}

	return false;
}

bool cDigimonStatusUI::_SetEvoInfoTooltip()
{
	

	return false;
}

bool cDigimonStatusUI::_SetStatToolTip(cListBoxItem const* pOverItem)
{
	SAFE_POINTER_RETVAL( pOverItem, false );
	sAbilInfo* pAbilInfo = dynamic_cast< sAbilInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RETVAL( pAbilInfo, false );

	int nAbilType = _ChangeToSystemAbilType( pAbilInfo->eAbilType );
	int nType = 0, nBase = 0, nChipset = 0, nFriendShip = 0;
	GetSystem()->GetDigimonStatInfo( static_cast<SystemType::eAbilType>(nAbilType), nType, nBase, nChipset, nFriendShip );
	
	SAFE_POINTER_RETVAL( TOOLTIPMNG_STPTR, false );
	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RETVAL( pTooltip, false );
	SAFE_POINTER_RETVAL( g_pCharMng, false );
	pTooltip->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 10, 0 ), 190, cTooltip::STAT, nType, nBase, nChipset, nFriendShip, g_pCharMng->GetDigimonUser( 0 ) );
	return true;
}

bool cDigimonStatusUI::_SetSkillTooltip(cGridListBoxItem const* pOverItem)
{
	SAFE_POINTER_RETVAL( pOverItem, false );
	if( !pOverItem->GetEnable() )
		return false;

	sSkillInfo* pSkillInfo = dynamic_cast< sSkillInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RETVAL( pSkillInfo, false );

	bool bIsUseSkill = GetSystem()->IsUseSkill( pSkillInfo->nSkillIndex );

	SAFE_POINTER_RETVAL( TOOLTIPMNG_STPTR, false );
	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RETVAL( pTooltip, false );

	int nTooltipSize;
#ifdef VERSION_USA
	nTooltipSize = 360;
#else
	nTooltipSize = TOOLTIP_MAX_SIZE;
#endif

	int nValue = 0;
	if( !bIsUseSkill )
		nValue = 1000; // 레벨 부족으로 아직 스킬을 배우지 못함

	CDigimonEvolveObj* pEvoObj = GetSystem()->GetEvoCurObject();
	SAFE_POINTER_RETVAL( pEvoObj, false );

	pTooltip->SetTooltip( pOverItem->GetWorldPos(), pOverItem->getItemSize(), nTooltipSize, cTooltip::SKILL, pSkillInfo->nSkillIndex, pEvoObj->m_nEvoSlot, cBaseWindow::WT_NEW_DIGIMONSTATUS, nValue );

	return true;
}

void cDigimonStatusUI::_OnLClickDownSkill(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cGridListBoxItem* pOverItem = static_cast< cGridListBoxItem* >( pData );
	sSkillInfo* pSkillInfo = dynamic_cast< sSkillInfo* >(pOverItem->GetUserData());
	SAFE_POINTER_RET( pSkillInfo );

	if( CURSOR_ST.IsIcon() && pSkillInfo->nSkillIndex == CURSOR_ST.GetIconSlot() )
	{
		CURSOR_ST.ReleaseIcon();
		return;
	}

	if( !GetSystem()->IsUseSkill( pSkillInfo->nSkillIndex ) )
		return;

	CURSOR_ST.SetIcon( CURSOR_ICON::CI_DSKILL, pSkillInfo->nSkillIndex, 1, NULL );
	CURSOR_ST.SetIconSubInfo( 0, 0, 0 );
}

void cDigimonStatusUI::_OnClickSkillButton(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cString::sBUTTON* pStrButton = static_cast< cString::sBUTTON* >( pData );
	cButton* pButton = pStrButton->s_pButton;
	SAFE_POINTER_RET( pButton );
	sButtonInfo* pBtnInfo = dynamic_cast< sButtonInfo* >( pButton->GetUserData() );
	SAFE_POINTER_RET( pBtnInfo );

	CURSOR_ST.ReleaseIcon();

	// 스킬 레벨업
	if( eLevelUp == pBtnInfo->eBtnType )
	{
		GetSystem()->LevelUpDigimonSkill( pBtnInfo->nSkillIndex );
		// 툴팁
// 		CsSkill::sINFO* pFTSkill = GetSystem()->GetDigimonFTSkillInfo( pBtnInfo->nSkillIndex );
// 		SAFE_POINTER_RET( pFTSkill );
// 		std::wstring wsTooltip = UISTRING_TEXT( "DIGIMONSTATUS_LEVELUP_TOOLTIP" );
// 		DmCS::StringFn::Replace( wsTooltip, L"#Point#", pFTSkill->s_nLevelupPoint );
// 		SAFE_POINTER_RET( TOOLTIPMNG_STPTR );
// 		cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
// 		SAFE_POINTER_RET( pTooltip );
// 		pTooltip->SetTooltip_Text( pButton->GetPos(), pButton->GetSize(), wsTooltip.c_str() );
	}
	// 스킬 삭제
	else
		GetSystem()->DeleteDigimonSkill( pBtnInfo->nSkillIndex );
}

void cDigimonStatusUI::_OnClickTalkButton(void* pSender, void* pData)
{
	SAFE_POINTER_RET( g_pGameIF );
	if( !g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGIMON_TALK ))
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_DIGIMON_TALK );
	else
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DIGIMON_TALK );
}

void cDigimonStatusUI::_OnClickHelpButton(void* pSender, void* pData)
{
	GetSystem()->OpenCloseHelpWindow();
}

void cDigimonStatusUI::_OnClickCloseButton(void* pSender, void* pData)
{
	GetSystem()->CloseDigimonStatus();
}

void cDigimonStatusUI::_OnClickEvoButton(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pSender );
	cButton* pEvoBtn = static_cast< cButton* >( pSender );
	sEvoBtnInfo* pEvoInfo = dynamic_cast< sEvoBtnInfo* >( pEvoBtn->GetUserData() );
	SAFE_POINTER_RET( pEvoInfo );
	GetSystem()->OpenEvolveDigimon( pEvoInfo->nEvoIndex );
}

void cDigimonStatusUI::_OnClickRideButton(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pSender );
	cButton* pRideBtn = static_cast< cButton* >( pSender );
	sEvoBtnInfo* pRideInfo = dynamic_cast< sEvoBtnInfo* >( pRideBtn->GetUserData() );
	SAFE_POINTER_RET( pRideInfo );
	GetSystem()->OpenRideDigimon( pRideInfo->nEvoIndex );
}

void cDigimonStatusUI::_OnClickAttributeNormal(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pSender );
	cImage* pNormal = static_cast< cImage* >( pSender );

	if( m_pNormalFull && !m_pNormalFull->GetVisible() )
		return;

	int nState = pNormal->GetState();
	if( 0 != nState )
		GetSystem()->RewardAttributeNormal( nState );
}

void cDigimonStatusUI::_OnClickAttributeNature(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pSender );
	cImage* pNature = static_cast< cImage* >( pSender );

	if( m_pNatureFull && !m_pNatureFull->GetVisible() )
		return;

	int nState = pNature->GetState();
	GetSystem()->RewardAttributeNature( nState );
}

void cDigimonStatusUI::_OnClickDigimonImage(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pSender );
	cImage* pDigimonImg = static_cast< cImage* >( pSender );

	sEvoBtnInfo* pUserData = dynamic_cast< sEvoBtnInfo* >( pDigimonImg->GetUserData() );
	SAFE_POINTER_RET( pUserData );

	GetSystem()->RequestEvolveDigimon( pUserData->nEvoIndex );
}

//////////////////////////////////////////////////////////////////////////////////
void cDigimonStatusUI::sAbility::UpdateAbility(std::wstring wsText, bool bIsUpper)
{
	SAFE_POINTER_RET( pAbilText );
	SetSafeOwnedText( pAbilText, wsText );
	if( bIsUpper )
		pAbilText->SetColor( NiColor( 122.f/255.f, 254.f/255.f, 0 ) );
	else
		pAbilText->SetColor( NiColor::WHITE );
}

void cDigimonStatusUI::sSkill::UpdateSkill(int nSkillType, int nCurLevel, int nMaxLevel, bool bIsLvUp, bool bIsMask)
{
	if( pSkillGridItem )
		pSkillGridItem->SetEnable( true );

	if( pSkillIcon )
	{
		pSkillIcon->SetEnable( true );
		pSkillIcon->SetVisible( true );
		pSkillIcon->ChangeSkillIcon( nSkillType, nSkillType );
	}

	if( pSkillMask )
		pSkillMask->SetVisible( !bIsMask );

	bool bIsMax = ( nCurLevel < nMaxLevel ) ? false : true;
		if( pSkillLevelTxt )
		{
			pSkillLevelTxt->SetVisible( true );
			if( bIsMax )
			{
				SetSafeOwnedText( pSkillLevelTxt, L"Max" );
				pSkillLevelTxt->SetColor( NiColor( 188.f/255.f, 97.f/255.f, 1 ) );
			}
			else
			{
				std::wstring wsLevel;
				DmCS::StringFn::Format( wsLevel, L"%d/%d", nCurLevel, nMaxLevel );
				SetSafeOwnedText( pSkillLevelTxt, wsLevel );
				pSkillLevelTxt->SetColor( NiColor( 76.f/255.f, 242.f/255.f, 1 ) );
			}
		}

	if( pSkillSlotImg )
		pSkillSlotImg->SetVisible( bIsMax );

	if( pSkillLvUpBtn )
	{
		pSkillLvUpBtn->SetEnable( bIsLvUp );
		pSkillLvUpBtn->SetVisible( bIsLvUp );
	}
}

void cDigimonStatusUI::sSkill::ResetSkill()
{
	if( pSkillIcon )
	{
		pSkillIcon->SetVisible( false );
		pSkillIcon->SetEnable( false );
	}
	if( pSkillMask )
	{
		pSkillMask->SetVisible( false );
		pSkillMask->SetEnable( false );
	}
	if( pSkillSlotImg )
		pSkillSlotImg->SetVisible( false );
	if( pSkillLvUpBtn )
	{
		pSkillLvUpBtn->SetVisible( false );
		pSkillLvUpBtn->SetEnable( false );
	}
	if( pSkillDeleteBtn )
	{
		pSkillDeleteBtn->SetVisible( false );
		pSkillDeleteBtn->SetEnable( false );
	}
	if( pSkillLevelTxt )
	{
		SetSafeOwnedText( pSkillLevelTxt, L"" );
		pSkillLevelTxt->SetVisible( false );
	}
}

void cDigimonStatusUI::sSkill::SetDeleteButton(int nMemorySkill)
{
	SAFE_POINTER_RET( pSkillDeleteBtn );
	if( 0 == nMemorySkill )
	{
		pSkillDeleteBtn->SetVisible( false );
		pSkillDeleteBtn->SetEnable( false );
	}
	else
	{
		pSkillDeleteBtn->SetVisible( true );
		pSkillDeleteBtn->SetEnable( true );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
void cDigimonStatusUI::sElementUI::ChangeElement(int nType)
{
	if( pElement )
	{
		pElement->SetState( nType );
		pElement->SetVisible( true );
	}
}

void cDigimonStatusUI::sElementUI::SetSuperiority(bool bIsUp)
{
	if( pUpArrow )
		pUpArrow->SetVisible( bIsUp );
	if( pDownArrow )
		pDownArrow->SetVisible( !bIsUp );
}

void cDigimonStatusUI::sElementUI::Reset()
{
	if( pElement )
		pElement->SetVisible( false );
	if( pUpArrow )
		pUpArrow->SetVisible( false );
	if( pDownArrow )
		pDownArrow->SetVisible( false );
}
/////////////////////////////////////////////////////////////////////////////////////////////////

bool cDigimonStatusUI::sEvoInfoUI::UpdateMouse(int nIndex)
{
	if( !bEnable )
		return false;

	if( pEvolBtn && pEvolBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return true;

	if( pRideBtn && pRideBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return true;

	if( pDigimonImg && pDigimonImg->Update_ForMouse() )
	{
#ifdef VERSION_USA
		int nMaxTooltipX = 340;
#else
		int nMaxTooltipX = TOOLTIP_MAX_SIZE;
#endif
		SAFE_POINTER_RETVAL( TOOLTIPMNG_STPTR, false );
		cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL( pTooltip, false );
		pTooltip->SetTooltip( pDigimonImg->GetPos(), pDigimonImg->GetSize(), nMaxTooltipX, cTooltip::QUICK_EVOL, cBaseWindow::WT_NEW_DIGIMONSTATUS, nIndex );
		return true;
	}

	return false;
}

void cDigimonStatusUI::sEvoInfoUI::Reset()
{
	if( pSlotBgImg )
		pSlotBgImg->SetVisible( false );

	if( pRideFrameImg )
		pRideFrameImg->SetVisible( false );

	if( pLockImg )
		pLockImg->SetVisible( false );

	if( pSelMaskImg )
		pSelMaskImg->SetVisible( false );

	if( pDigimonImg )
	{
		pDigimonImg->SetVisible( false );
		pDigimonImg->SetEnable( false );
	}

	if( pRideBtn )
	{
		pRideBtn->SetVisible( false );
		pRideBtn->SetEnable( false );
	}

	if( pEvolBtn )
	{
		pEvolBtn->SetVisible( false );
		pEvolBtn->SetEnable( false );
	}
}

void cDigimonStatusUI::sEvoInfoUI::SetSlot(std::string strPath, int nEvoSlot, int nState, bool bVisible, bool bEnable, bool bLock, bool bEvoBtn, bool bXAnti)
{
	if( pSlotBgImg )
	{
		if( bXAnti )
		{
			pSlotBgImg->ChangeTexture( "DigimonStatus_New\\IconBG_X.tga" );
			pSlotBgImg->SetSize( CsPoint( 50, 50 ) );
		}
		else
		{
			pSlotBgImg->ChangeTexture( "DigimonStatus_New\\Digimon_evo_slot.tga" );
			pSlotBgImg->SetSize( CsPoint( 44, 43 ) );
		}
		pSlotBgImg->SetVisible( bVisible );
	}

	if( pDigimonImg )
	{
		pDigimonImg->ChangeTexture( strPath.c_str(), false );
		pDigimonImg->SetTexToken( CsPoint( 32, 0 ) );
		pDigimonImg->SetVisible( bVisible );
		pDigimonImg->SetEnable( bEnable );
		pDigimonImg->SetState( nState );

		sEvoBtnInfo* pData = dynamic_cast< sEvoBtnInfo* >( pDigimonImg->GetUserData() );
		if( pData )
			pData->nEvoIndex = nEvoSlot;
	}

	if( pSelMaskImg )
	{
		if( bXAnti )
			pSelMaskImg->ChangeTexture( "DigimonStatus_New\\Sel_OutMask_X.tga" );
		else
			pSelMaskImg->ChangeTexture( "DigimonStatus_New\\Sel_OutMask.tga" );
		pSelMaskImg->SetVisible( !bEnable );
	}

	if( pLockImg )
		pLockImg->SetVisible( bLock );

	if( pEvolBtn )
	{
		pEvolBtn->SetVisible( bEvoBtn );
		pEvolBtn->SetEnable( bEvoBtn );
	}
}

void cDigimonStatusUI::sEvoInfoUI::SetPos(CsPoint ptPos, bool bXAnti)
{
	CsPoint ptCorrect = ptPos + CsPoint( 375, 72 );
	if( pSlotBgImg )
	{
		if( bXAnti )
			pSlotBgImg->SetPos( ptCorrect + CsPoint( -3, -3 ) );
		else
			pSlotBgImg->SetPos( ptCorrect );
	}

	if( pRideFrameImg )
		pRideFrameImg->SetPos( ptCorrect + CsPoint( 1, 0 ) );

	if( pLockImg )
		pLockImg->SetPos( ptCorrect + CsPoint( 6, 5 ) );

	if( pDigimonImg )
		pDigimonImg->SetPos( ptCorrect + CsPoint( 6, 5 ) );

	if( pRideBtn )
		pRideBtn->SetPos( ptCorrect + CsPoint( 30, -10 ) );

	if( pEvolBtn )
		pEvolBtn->SetPos( ptCorrect + CsPoint( 6, 5 ) );

	if( pSelMaskImg )
	{
// 		if( bXAnti )
// 			pSelMaskImg->SetPos( ptCorrect + CsPoint( -3, -4 ) );
// 		else
			pSelMaskImg->SetPos( ptCorrect + CsPoint( 0, -1 ) );
	}

	bEnable = true;
}

CsPoint cDigimonStatusUI::sEvoInfoUI::GetPos() const
{
	if( pDigimonImg )
		return pDigimonImg->GetPos() + ( pDigimonImg->GetSize() * 0.5f ) - CsPoint( 3, 3 );
	return CsPoint::ZERO;
}
