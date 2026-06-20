#include "stdafx.h"
#include "MakeTactics.h"

cMakeTactics::sBAR::sBAR()
{
	s_nRenderDelta[ 0 ] = ( 37 + 33*4 );
	s_nRenderDelta[ 1 ] = ( 37 + 33*3 );
	s_nRenderDelta[ 2 ] = ( 37 + 33*2 );
	s_nRenderDelta[ 3 ] = ( 37 + 33 );
	s_nRenderDelta[ 4 ] = ( 37 );
	s_nRenderDelta[ 5 ] = 0;

	s_eChange = CN_NONE;
	s_pBar = NULL;
	s_bColorUpdate = false;

	// [2/23/2016 hyun] 게이지바 활성화상태
	SetActivateState(false);
}

void cMakeTactics::sBAR::InitLevel( int nLevel )
{
	s_nCurLevel = s_nDestLevel = nLevel;
	s_fCurPos = (float)s_nRenderDelta[ s_nCurLevel ];
}

void cMakeTactics::sBAR::ChangeLevel( eCHANGE type )
{
	s_eChange = type;

	switch( s_eChange )
	{
	case CN_DESTROY:
	case CN_INCREASE:
	case CN_KEEP:
		assert_cs( s_nCurLevel < 5 );
		s_nDestLevel = s_nCurLevel + 1;
		SetChangeColor( IF_MT_BAR_COLOR_DEST );
		s_fDelayTime = 0.0f;
		break;
	case CN_DECREASE:
		assert_cs( s_nCurLevel > 0 );
		s_nDestLevel = s_nCurLevel - 1;
		s_fDelayTime = 0.6f;
		break;
	case CN_DESTROY_DECREASE:
		assert_cs( s_nCurLevel > 0 );
		s_nDestLevel = 0;
		s_fDelayTime = 1.6f;
		break;
	}

	s_fCurPos = (float)s_nRenderDelta[ s_nCurLevel ];
	s_fDestPos = (float)s_nRenderDelta[ s_nDestLevel ];

	s_nDeltaPos = (int)( s_fDestPos - s_fCurPos );
	// [2/23/2016 hyun] 게이지바 활성화	
	SetActivateState(true);
}

void cMakeTactics::sBAR::SetChangeColor( NiColorA destColor )
{
	s_colorDest = destColor;
	s_pBar->GetScreen()->GetColor( 0, 0, s_colorSrc );
	if( s_colorDest == s_colorSrc )
	{
		s_bColorUpdate = false;
		return;
	}

	s_colorSetting = s_colorSrc;
	s_colorDelta = s_colorDest - s_colorSrc;
	if( s_colorDelta.r != 0.0f )
	{
		s_bColorCheckPlus = ( s_colorDelta.r > 0 );
		s_nColorCheckIndex = 0;
	}
	else if( s_colorDelta.g != 0.0f )
	{
		s_bColorCheckPlus = ( s_colorDelta.g > 0 );
		s_nColorCheckIndex = 1;
	}
	else
	{
		s_bColorCheckPlus = ( s_colorDelta.b > 0 );
		s_nColorCheckIndex = 2;
	}
	s_bColorUpdate = true;
}

bool cMakeTactics::sBAR::Update(float const& fDeltaTime)
{
	CsPoint pos = CsPoint( 17, 46 );
	pos.y += (int)s_fCurPos;
	CsPoint size = CsPoint( 35, IF_MT_MAX_BAR_LEN );
	size.y -= (int)s_fCurPos;
	s_pBar->SetPos(pos);
	s_pBar->SetSize(size);

	if( s_bColorUpdate )
	{
		s_colorSetting += s_colorDelta * g_fDeltaTime;		

		float fSrc, fDest;
		switch( s_nColorCheckIndex )
		{
		case 0:
			fSrc = s_colorSetting.r;
			fDest = s_colorDest.r;
			break;
		case 1:
			fSrc = s_colorSetting.g;
			fDest = s_colorDest.g;
			break;
		case 2:
			fSrc = s_colorSetting.b;
			fDest = s_colorDest.b;
			break;
		}

		if( s_bColorCheckPlus )
		{
			if( fSrc >= fDest )
			{
				s_bColorUpdate = false;
				s_colorSetting = s_colorDest;
			}
		}
		else
		{
			if( fSrc <= fDest )
			{
				s_bColorUpdate = false;
				s_colorSetting = s_colorDest;
			}
		}
		s_pBar->SetColorA( s_colorSetting );
	}

	if( s_eChange != CN_NONE )
	{		
		switch( s_eChange )
		{
		case CN_INCREASE:
			s_fCurPos += s_nDeltaPos*g_fDeltaTime;
			if( s_fCurPos < s_fDestPos )
			{
				s_fCurPos = s_fDestPos;
				s_nCurLevel = s_nDestLevel;
				s_eChange = CN_NONE;
				SetChangeColor( IF_MT_BAR_COLOR_SRC );
			}
			break;
		case CN_KEEP:
			s_fCurPos += s_nDeltaPos*g_fDeltaTime;
			if( s_fCurPos < s_fDestPos )
			{
				s_fCurPos = s_fDestPos;
				s_nCurLevel = s_nDestLevel;
				ChangeLevel( CN_DECREASE );
				SetChangeColor( IF_MT_BAR_COLOR_SRC );
			}
			break;
		case CN_DESTROY:
			s_fCurPos += s_nDeltaPos*g_fDeltaTime;
			if( s_fCurPos < s_fDestPos )
			{
				s_fCurPos = s_fDestPos;
				s_nCurLevel = s_nDestLevel;
				s_eChange = CN_NONE;
				ChangeLevel( CN_DESTROY_DECREASE );
				SetChangeColor( IF_MT_BAR_COLOR_END );
			}
			break;
		case CN_DECREASE:
			if( ( s_fDelayTime -= g_fDeltaTime ) < 0.0f )
			{
				s_fCurPos += s_nDeltaPos*g_fDeltaTime*7.0f;
				if( s_fCurPos > s_fDestPos )
				{
					s_fCurPos = s_fDestPos;
					s_nCurLevel = s_nDestLevel;
					s_eChange = CN_NONE;
				}
			}			
			break;
		case CN_DESTROY_DECREASE:
			if( ( s_fDelayTime -= g_fDeltaTime ) < 0.0f )
			{
				s_fCurPos += s_nDeltaPos*g_fDeltaTime*1.2f;
				if( s_fCurPos > s_fDestPos )
				{
					s_fCurPos = s_fDestPos;
					s_nCurLevel = s_nDestLevel;
					s_eChange = CN_NONE;

					InitLevel( 0 );
				}
			}			
			break;
		}
	}

	return ( s_eChange == CN_NONE )&&( s_bColorUpdate == false );
}

void cMakeTactics::sBAR::SetActivateState( bool bActivate )
{
	s_bActiveBar = bActivate;
}

bool cMakeTactics::sBAR::GetActivateState() const
{
	return s_bActiveBar;
}

////////////////////////////////////////////////////////////////////////////////////////
//
//		중급 / 고급 데이터 사용 시 UI 위치 변경
//		EGG_BACKUP 과 MINIGAME 전처리기가 추가된 이후 MAKE_TACTICS_UI_UPGRADE 사용
//
////////////////////////////////////////////////////////////////////////////////////////

cMakeTactics::cMakeTactics():iPotableIndex(-1)
#ifdef MINIGAME
,m_pMiniGameBtn(NULL)
#endif
{
	bBackDiskBtnStatus = false;
	bAutoResistBtnStatus = false;
	bCancelBtnStatus = false;
	m_bPortable = false;
	m_bCompleateChangeBar = false;

	m_pBackupDiskBtn = NULL;
	m_pAutoResistBtn = NULL;
	m_pCancelBtn = NULL;
	m_pCloseBtn = NULL;
	m_pRequiteBtn = NULL;


	m_pRenderTex = NULL;
	m_pTarget = NULL;
	m_pNPCObejct = NULL;	

	m_eResult = NONE;	
	
	m_StringBody.Delete();	
}

cMakeTactics::~cMakeTactics()
{	

}

bool cMakeTactics::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;
#ifdef MINIGAME
	GetSystem()->Register( SystemType::EventMakeTactics::_CloseMiniGame, this );
#endif
	return true;
}
void cMakeTactics::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{ 
	switch(iNotifiedEvt)
	{
#ifdef MINIGAME
	case SystemType::EventMakeTactics::_CloseMiniGame:
		OnEndMiniGame();
		break;
#endif
	default:
		break;
	}
}

void cMakeTactics::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	cBaseWindow::Delete();
	
	g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_NONE );
	g_pNpcMng->DelNpc( HATCH_NPC_ID );	// NPC 삭제
}

bool cMakeTactics::Close( bool bSound )
{ 
	if( !GetSystem()->GetWaitRecvServer() )
	{ 
#ifdef MINIGAME
		GetSystem()->HideMinigame();
#endif
		return cBaseWindow::Close( bSound );
	}
	return false;
}
void cMakeTactics::DeleteResource()
{
	cCreateName::DeleteInstance( GetWindowType(), 0 );

	// 처리가되기전에 인터페이스 닫아줄시
	ProcessResult();	
	DeleteScript();	

	g_pSoundMgr->Set_BGM_FadeVolume( m_fBackupMusic );

	// 연결된 NPC 닫힘애니실행O
	if( m_pTarget )
	{
		( (CNpc*)m_pTarget )->SetCloseAnimation();
	}
	else if( m_pNPCObejct )
	{
		( (CNpc*)m_pNPCObejct )->SetCloseAnimation();		
	}

	m_pTarget = NULL;
	m_pNPCObejct = NULL;

	// 케쉬 용병알 4업 이상 확인 메세지
	cMessageBox::DelMsg( 30054, false );

	SAFE_NIDELETE( m_pRenderTex );
	// 아이템 언롹킹
	g_pDataMng->GetInven()->ItemUnlock_ItemType( nItem::Portable_Item, eHATCH);
}

void cMakeTactics::PreResetMap()
{ 
	Close( false );
}
void cMakeTactics::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	iPotableIndex = nValue;

	cButton::eIMAGE_TYPE eBtnType  = cButton::IMAGE_NOR_1;
	cButton::eIMAGE_TYPE eBtnType2 = cButton::IMAGE_NOR_1;

	CsPoint ptReqPos = CsPoint( 44, 330 );			//지급받기
	CsPoint ptRegDigiPos = AUTOREGIST_BUTTON_POS;	//자동등록, Data 입력
	CsPoint ptCancelPos = CsPoint( 139, 330 );		//취소, 제거

	CsPoint ptReqSize = CsPoint( 84, 33 );			//지급받기	
	CsPoint ptRegDigiSize = BACKUPDISK_BUTTON_SIZE;	//자동등록, Data 입력
	CsPoint ptCancelSize = CsPoint( 84, 33 );		//취소, 제거	

	CsPoint ptTexSize = CsPoint( 132, 145 );		//부화기 모델
	CsPoint ptDigiIconPos = CsPoint( 18, 231 );		//등록된 알 아이콘

	//부화기 바탕 이미지 
	InitScript( "MakeTactics\\DiGi_Born-Win.tga", CsPoint( 150, 190 ), CsPoint( 237, 380 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );

	m_pBackupDiskBtn = AddButton(BACKUPDISK_BUTTON_POS, BACKUPDISK_BUTTON_SIZE, eBtnType, UISTRING_TEXT( "MAKETACTICS_BTN_REGIST_BACKUP_DISK" ).c_str() );
	m_pBackupDiskBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cMakeTactics::PressBackupDiskBtn);

	AddTitle( UISTRING_TEXT( "MAKETACTICS_TITLE_DIGIEGG_HATCH" ).c_str() );

	m_pAutoResistBtn = AddButton( ptRegDigiPos, ptRegDigiSize, eBtnType, UISTRING_TEXT( "COMMON_TXT_AUTO_REGISTER" ).c_str() );
	if(m_pAutoResistBtn)
	{
		m_pAutoResistBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cMakeTactics::PressAutoResistBtn);
		m_pAutoResistBtn->SetControlName( "RegistItem" );
	}

	m_pCancelBtn = AddButton( ptCancelPos, ptCancelSize, eBtnType2, UISTRING_TEXT( "COMMON_TXT_CANCEL" ).c_str() );
	m_pCancelBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cMakeTactics::PressCancelBtn);
	m_pCancelBtn->SetControlName( "TacticeCancel" );

	m_pCloseBtn = AddButton( CsPoint( 213, 8 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );
	m_pCloseBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cMakeTactics::PressCloseBtn);

	m_pRequiteBtn = AddButton( ptReqPos, ptReqSize, eBtnType2, UISTRING_TEXT( "MAKETACTICS_BTN_HATCH" ).c_str() );
	if( m_pRequiteBtn )
	{
		m_pRequiteBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cMakeTactics::PressRequiteBtn);
		m_pRequiteBtn->SetControlName("RequiteBtn");
	}

#ifdef MAKE_TACTICS_UI_UPGRADE
	m_pDataChangeBtn = AddButton( DATACHANGE_BUTTON_POS, DATACHANGE_BUTTON_SIZE, eBtnType, UISTRING_TEXT( "MAKETACTICS_BTN_CHANGE" ).c_str() );
	m_pDataChangeBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cMakeTactics::PressDataChangeBtn);	
	_RefreshGradeString();
#endif

#ifdef MINIGAME
	m_pMiniGameBtn = AddButton( MINIGAME_BUTTON_POS, CsPoint( 34, 34 ), CsPoint( 0, 46 ), "MiniGame\\chancebutton_small.tga" );	
	m_pMiniGameBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cMakeTactics::PressMiniGameBtn);
	SetEnableMinigameBt(false);
#endif

	// 바
	m_CurBar.s_pBar = AddSprite(CsPoint::ZERO, CsPoint( 35, 0 ), "MakeTactics\\DiGi_Bar.tga");
	m_CurBar.s_pBar->SetColorA( IF_MT_BAR_COLOR_SRC );
	m_CurBar.s_pBar->GetAlphaProp()->SetAlphaBlending( true );
	m_CurBar.s_pBar->GetAlphaProp()->SetSrcBlendMode( NiAlphaProperty::ALPHA_ONE );
	m_CurBar.s_pBar->GetAlphaProp()->SetDestBlendMode( NiAlphaProperty::ALPHA_ONE );

	m_pRenderTex = NiNew cRenderTex;

	if( m_pRenderTex->Init( ptTexSize ) == false )
	{
		SAFE_NIDELETE( m_pRenderTex );
	}

	m_rcBackupDisk.SetRect( CsPoint( 18, 279 ), (CsSIZE)IF_MT_ICONSIZE );
	m_rcResiDigitama.SetRect( ptDigiIconPos, (CsSIZE)IF_MT_ICONSIZE );	//용병알 이미지 좌표
	m_rcDataChange.SetRect( DATACHANGE_BUTTON_POS, (CsSIZE)DATACHANGE_BUTTON_SIZE );
	
	//SIMPLE_TOOLTIP
	m_rcAutoReg.SetRect( ptRegDigiPos, (CsSIZE)ptRegDigiSize );
	m_rcChance.SetRect( MINIGAME_BUTTON_POS, CsSIZE( 34, 34 ) );	
	m_rcBackupReg.SetRect( CsPoint( 97, 280 ), CsSIZE( 84, 33 ) );
	m_rcRecv.SetRect( ptReqPos, (CsSIZE)ptReqSize );
	m_rcCancel.SetRect( ptCancelPos, (CsSIZE)ptCancelSize );

	m_fBackupMusic = g_pResist->m_Global.s_fMusic;
	g_pSoundMgr->Set_BGM_Volume( CsMin( m_fBackupMusic, 0.1f ) );	



	CreateData();	

	// 아이템 롹킹
	g_pDataMng->GetInven()->ItemLock_ItemType( nItem::Portable_Item, eHATCH);
}

void cMakeTactics::Update(float const& fDeltaTime)
{
	g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_MAKE_TACTICS );

	cCreateName::UpdateInstance( GetWindowType(), 0 );

	if(m_pTarget && !m_bPortable)	// 휴대용이 아닐경우만 실행
	{
		// 테이머와의 거리 체크
		assert_cs( m_pTarget->GetLeafRTTI() == RTTI_NPC );
		if( ( (CNpc*)m_pTarget )->CheckTamerDist() == false )
		{		
			Close();	
			return;
		}
	}
	else if(m_pNPCObejct)	// 휴대용인 경우
	{
		// 테이머와의 거리 체크
		assert_cs( m_pNPCObejct->GetLeafRTTI() == RTTI_NPC );
		if( ( (CNpc*)m_pNPCObejct )->CheckTamerDist(PORTABLE_DIST) == false )
		{		
			Close();
			return;
		}
	}

	m_bCompleateChangeBar = m_CurBar.Update(fDeltaTime);	
	
	if( m_bCompleateChangeBar &&( m_eResult == NONE ) && m_CurBar.GetActivateState())
	{
		m_pAutoResistBtn->SetEnable(true);
		m_pCancelBtn->SetEnable(true);
		m_pBackupDiskBtn->SetEnable(true);
		m_pRequiteBtn->SetEnable( ( GetSystem()->GetDigimonEggCount() >= IF_MAKE_TACTICS_REQUITE_LEVEL ) );
		m_pDataChangeBtn->SetEnable( true );

		m_CurBar.SetActivateState(false);	// [2/23/2016 hyun] 비활성화
#ifdef MINIGAME
		//m_pMiniGameBtn->SetEnable(true);
		SendMiniGameCheck();
#endif

	}

	_UpdateMoveWindow();	
}

cBaseWindow::eMU_TYPE cMakeTactics::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn;

	muReturn = cCreateName::Update_ForMouseInstance( GetWindowType(), 0 );
	if( muReturn != MUT_OUT_WINDOW )	
		return muReturn;

	muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if(m_pBackupDiskBtn && m_pBackupDiskBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}

	if(m_pAutoResistBtn && m_pAutoResistBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}

	if(m_pCancelBtn && m_pCancelBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}

	if(m_pCloseBtn && m_pCloseBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}

	if(m_pRequiteBtn && m_pRequiteBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}	

	if(m_pDataChangeBtn && m_pDataChangeBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}	
#ifdef MINIGAME
	if(m_pMiniGameBtn && m_pMiniGameBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		return muReturn;
	}
#endif

	//ToolTip	
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	if( m_rcResiDigitama.PtInRect( ptMouseLocalPos ) == TRUE )
	{
		if( GetSystem()->GetDigimonEggType() != 0 )
		{
			CsPoint pos = GetRootClient() + CsPoint( 44, 280 );
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_MT_ICONSIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, GetSystem()->GetDigimonEggType(), cBaseWindow::WT_MAKE_TACTICS, GetSystem()->GetDataFlag());			
		}	
		else
		{
			#ifdef SIMPLE_TOOLTIP
			// 툴팁 1001			
			CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1001 );
			#endif
		}
		return muReturn;	
	}

#ifdef SIMPLE_TOOLTIP
	else if( m_rcAutoReg.PtInRect( ptMouseLocalPos ) == TRUE )
	{
		// 현재 버튼이 자동등록인지 데이터 입력인지 판단해서 툴팁을 바꿔서 보여주도록 수정합니다.

		if( bAutoResistBtnStatus )	// 자동 등록 버튼일 때,
		{
			CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1002 );			
		}
		else						// 데이버 입력 버튼 일 때,
		{
			// 툴팁 1008
			CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1008 );
		}
		return muReturn;
	}

	else if( m_rcChance.PtInRect( ptMouseLocalPos ) == TRUE )	// 미니게임 버튼 비활성화 시 툴팁
	{
		// 툴팁 1003
		CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1003 );
		return muReturn;
	}

	#ifdef MAKE_TACTICS_UI_UPGRADE	
	else if( m_rcDataChange.PtInRect( ptMouseLocalPos ) == TRUE )	// 변경 버튼 툴팁
	{
		CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1010 );
		return muReturn;
	}
	#endif
#endif
	//백업디스크의 툴팁 띄워준다.
	else if( m_rcBackupDisk.PtInRect( ptMouseLocalPos ) == TRUE )
	{
		if( GetSystem()->GetBackupDiskType() != 0 )
		{
			CsPoint pos = GetRootClient() + CsPoint( 44, 350 );
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_MT_ICONSIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, GetSystem()->GetBackupDiskType(), cBaseWindow::WT_MAKE_TACTICS );
		}

		else
		{
		#ifdef SIMPLE_TOOLTIP
			// 툴팁 1004
			CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1004 );
		#endif				
		}
		return muReturn;
	}

	#ifdef SIMPLE_TOOLTIP
	else if(m_rcBackupReg.PtInRect( ptMouseLocalPos ) == TRUE )
	{
		if( GetSystem()->GetBackupDiskType() == 0 )
		{
			// 툴팁 1005
			CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1005 );
			return muReturn;
		}
	}
	else if(m_rcRecv.PtInRect( ptMouseLocalPos ) == TRUE )
	{		
		// 툴팁 1006
		CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1006 );
		return muReturn;
	}
	else if(m_rcCancel.PtInRect( ptMouseLocalPos ) == TRUE )
	{
		// 툴팁 1007
		if( GetSystem()->GetDigimonEggCount() == 0 )
		{			
			CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1007 );				
		}
		else
		{		
			// 툴팁 1009
			CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1009 );
		}
		return muReturn;
	}
	#endif

	_UpdateMoveWindow_ForMouse();		

	return muReturn;
}

void cMakeTactics::Render()
{
	RenderScript();

	if( GetSystem()->GetDigimonEggType() != 0 )
	{
		g_pIconMng->RenderItem( GetSystem()->GetDigimonEgg(), GetRootClient() + m_rcResiDigitama.GetPos(), false );		
	}	

	if( GetSystem()->GetBackupDiskType() != 0 )
	{
		g_pIconMng->RenderItem( GetSystem()->GetBackupDisk(), GetRootClient() + m_rcBackupDisk.GetPos(), false );		
	}

	if( m_pRenderTex != NULL )
	{		
		m_pRenderTex->SetHeightConstant( 0.5f );
		m_pRenderTex->SetDistConstant( 0.28f );

		m_pRenderTex->BeginRender();		
		
		if( m_pTarget )
			m_pRenderTex->RenderObject( GET_SUBCAMERA(CAMERA_05), m_pTarget );		//2015-11-30-nova
		else if(m_pNPCObejct)
			m_pRenderTex->RenderObject( GET_SUBCAMERA(CAMERA_05), m_pNPCObejct );	//2016-07-18-eiless84

		m_pRenderTex->EndRender( GET_SUBCAMERA(CAMERA_05), CsPoint( 73, 49 ) + GetRootClient() );
	}

#ifdef MINIGAME
	CsPoint ptStringPos = CsPoint( 17, 120 );	
	
	#ifdef VERSION_TH
		ptStringPos = CsPoint( 19, 120 );
	#endif
	
	m_StringBody.Render( ptStringPos + GetRootClient(), 4 );
#endif

	cCreateName::RenderInstance( GetWindowType(), 0 );
}

bool cMakeTactics::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	CsPoint ptLocal = MousePosToWindowPos( CURSOR_ST.GetPos() );
	switch( eIconType )
	{
	case CURSOR_ICON::CI_INVEN:
		{
			if( m_rcResiDigitama.PtInRect( ptLocal ) == TRUE )
			{
				Resist( nIconSlot );
			}
			if(m_rcBackupDisk.PtInRect( ptLocal ) == TRUE )
			{
				Resist( nIconSlot );
			}
			CURSOR_ST.ReleaseIcon();
			return true;
		}
		break;
	}

	return false;
}

void cMakeTactics::ResetDevice()
{	
	ResetDeviceScript();

#ifdef MINIGAME
	m_StringBody.ResetDevice();
#endif

	if( m_pRenderTex )
	{
		m_pRenderTex->ResetDevice();
	}
}

bool cMakeTactics::OnEscapeKey()
{
#ifdef MINIGAME
	if( g_pGameIF->IsActiveWindow(cBaseWindow::WT_MINIGAME) ) 
		return false;
#endif
	Close(); 
	return true; 
}

void cMakeTactics::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();

	if( m_pRenderTex )
	{
		m_pRenderTex->ResetDevice();
	}
}

void cMakeTactics::UserDie()
{
	Close( false );
}

void cMakeTactics::RecvDiedAlready()
{
	GetSystem()->SetWaitRecvServer(false);
	Close( false );
}

void cMakeTactics::CreateData()
{
	m_CurBar.InitLevel( GetSystem()->GetDigimonEggCount() );	

	// 알이 이미 등록 되어 있다면
	if( GetSystem()->GetDigimonEgg()->IsEnable() == true )
	{
		_ResistDigitamaEnableControl();
	}
	else
	{
		_ResistDigitamaDisableControl();
	}

	//백업 디스크가 등록 되어 있다면
	if( GetSystem()->GetBackupDisk()->IsEnable() == true )
	{
		_ResistBackupDiskEnableControl();
	}
	else
	{
		_ResistBackupDiskDisableControl();
	}

#ifdef MINIGAME
	SendMiniGameCheck();
#endif
}

void cMakeTactics::SetNpcAnimation(DWORD dwAniID)
{
	if(m_pTarget)
	{
		assert_csm( (m_pTarget->GetModelID() == 95001), _T( "부화기 모델 번호가 인큐베이터가 아니에요.." ) );

		m_pTarget->SetAnimation( dwAniID );
	}
	else if(m_pNPCObejct)
	{
		m_pNPCObejct->SetAnimation( dwAniID );
	}
}

n4	 cMakeTactics::GetNpcID() const
{
	SAFE_POINTER_RETVAL(m_pTarget, 0);

	CsNpc* pFTNpc = ( (CNpc*)m_pTarget )->GetFTNpc();

	SAFE_POINTER_RETVAL(pFTNpc, 0);

	return	pFTNpc->GetInfo()->s_dwNpcID;

}



void cMakeTactics::_RefreshGradeString()
{	
#ifndef MAKE_TACTICS_UI_UPGRADE
	return;
#endif

	assert_cs(m_pAutoResistBtn);

	if( GetSystem()->GetDataFlag() >= GetSystem()->GetDataFlagMax())
		GetSystem()->SetDataFlag(CsTactics::DATA_LV1);


	std::wstring wsGrade;

	switch(GetSystem()->GetDataFlag())
	{
	case CsTactics::DATA_LV1:
		wsGrade = UISTRING_TEXT( "MAKETACTICS_DATA_TYPE_LOW" );
		break;

#ifdef TACTICS_USE_DATA_LV2
	case CsTactics::DATA_LV2:
		wsGrade = UISTRING_TEXT( "MAKETACTICS_DATA_TYPE_MID" );
		break;
#endif
#ifdef TACTICS_USE_DATA_LV3 
	case CsTactics::DATA_LV3:
		wsprintf(szGrade, _T("고급"));		// 사용 시 uitext 에 추가하여 사용할 것
		break;
#endif
#ifdef TACTICS_USE_DATA_LV4 
	case CsTactics::DATA_LV4:
		wsprintf(szGrade, _T("최고급"));	// 사용 시 uitext 에 추가하여 사용할 것
		break;
#endif
#ifdef TACTICS_USE_DATA_LV5 
	case CsTactics::DATA_LV5:
		wsprintf(szGrade, _T("돈내놔"));	// 사용 시 uitext 에 추가하여 사용할 것
		break;
#endif
	default:
		assert_csm1(false, "데이터 등급 : (%d) 숫자가 (1~5)인지 다시 확인해주시거나 클라에게 추가 요청해주세요.", GetSystem()->GetDataFlag());
		break;
	}

	if( m_pAutoResistBtn )
	{
		std::wstring wsBuff;
		DmCS::StringFn::Format( wsBuff, L"%s %s", wsGrade.c_str(), UISTRING_TEXT( "MAKETACTICS_BTN_DATA_INPUT" ).c_str() );
		m_pAutoResistBtn->SetText( wsBuff.c_str() );
		m_pAutoResistBtn->SetControlName( "InsertDataBtn" );
	}
}

void cMakeTactics::PressBackupDiskBtn( void* pSender, void* pData )
{
	if(bBackDiskBtnStatus)
	{
		_CancelBackupDisk(); //백업디스크 등록 취소		
	}
	else
	{
		_ResistBackupDisk(); //백업디스크 등록
	}
}

void cMakeTactics::PressAutoResistBtn( void* pSender, void* pData )
{
	if(bAutoResistBtnStatus)
	{
		_ResistDigitama();
	}
	else
	{
		_TryLevelUp();	
	}
}

void cMakeTactics::PressCancelBtn( void* pSender, void* pData )
{
	if(bCancelBtnStatus){	
		_CancelDigitama();
	}
	else{
		_DeleteDigitama();		
	}
}

void cMakeTactics::PressCloseBtn( void* pSender, void* pData )
{
	Close();
}

void cMakeTactics::PressRequiteBtn( void* pSender, void* pData )
{
	assert_cs( GetSystem()->GetDigimonEggType() != 0 );
	assert_cs( GetSystem()->GetDigimonEggCount() >= 3 );

	if( nsCsFileTable::g_pTacticsMng->IsTactics( GetSystem()->GetDigimonEggType() ) == false )
	{
		CsMessageBox( MB_OK, _T( "현재는 부화 할 수 없는 용병알입니다." ) );
		return;
	}

	// 용병 추가할 공간이 있는가
	if( g_pDataMng->GetTactics()->GetEmptyTactics() == NULL )
	{
		cPrintMsg::PrintMsg( 20007 );
		return;
	}

	cCreateName::NewInstance( GetWindowType(), 0, m_pRequiteBtn->GetClient() );	
}

void cMakeTactics::PressDataChangeBtn( void* pSender, void* pData )
{
#ifdef MAKE_TACTICS_UI_UPGRADE
	GetSystem()->AddDataFlag();	// 데이터 등급 토글
#endif
	_RefreshGradeString();
}

void cMakeTactics::PressMiniGameBtn( void* pSender, void* pData )
{
#ifdef MINIGAME
	if(GetWindowType() != WT_MINIGAME)	//미니게임이 안떠있을 때만 물어봐야지
	{
		CsTactics::sINFO* pFTTactics = nsCsFileTable::g_pTacticsMng->GetTactics( GetSystem()->GetDigimonEggType() )->GetInfo();

		if( ( pFTTactics->s_nViewWarning[CsTactics::DATA_LV1] )&&( GetSystem()->GetDigimonEggCount() < pFTTactics->s_nViewWarning[CsTactics::DATA_LV1]) )
		{
			//캐쉬 용병알일 땐 찬스 쓰면 안되(데이터 주입 확률이 100%이기 때문에)
			cPrintMsg::PrintMsg( 30542 );
		}
		else
		{
			g_pCharMng->GetTamerUser()->DeletePath();//BG 밖이라 움직이네;; 멈추기;;
			g_pCharMng->GetDigimonUser(0)->DeletePath();

			sMONEY iNeedMoney;
			int nCount = 0;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_MAKETACTICS_DIGITAMA_COUNT, &nCount);
			iNeedMoney = (u8)(5000 + 5000*nCount);
			cPrintMsg::PrintMsg( 30540, &iNeedMoney );
			assert_cs( cMessageBox::GetFirstMessageBox() );
		}
	}		
#endif
}

void cMakeTactics::_ResistBackupDiskEnableControl()
{
	bBackDiskBtnStatus= true;
	m_pBackupDiskBtn->SetText( UISTRING_TEXT( "MAKETACTICS_BTN_REGIST_CANCEL" ).c_str() );
}

void cMakeTactics::_ResistBackupDiskDisableControl()
{
	bBackDiskBtnStatus = false;
	m_pBackupDiskBtn->SetText( UISTRING_TEXT( "MAKETACTICS_BTN_REGIST_BACKUP_DISK" ).c_str() );
}

void cMakeTactics::_ResistBackupDisk()
{
	if(GetSystem()->AutoResisitBackupDisk())
	{
		ResistBackupDisk(GetSystem()->GetBackupDiskIndex() - 1);
	}	
}

void cMakeTactics::_CancelBackupDisk()
{
	if( GetSystem()->GetBackupDiskType() == 0 )
	{
		Close();
		return;
	}

	assert_cs( GetSystem()->GetBackupDiskCount() == 0 );

	// 인벤에 빈공간이 없다면
	if( g_pDataMng->GetInven()->GetEmptySlotCount() == 0 )
	{
		cPrintMsg::PrintMsg( 11015 );
		return;
	}

	GetSystem()->Cancel_BackupDisk(GetNpcID(),(iPotableIndex == -1)?true : false);

	SetNpcAnimation( ANI::NPC_OPEN_IDLE2 );		

	_ResistBackupDiskDisableControl();
}

void cMakeTactics::_ResistDigitamaEnableControl()
{
	bAutoResistBtnStatus = false;
	m_pAutoResistBtn->SetText( UISTRING_TEXT( "MAKETACTICS_BTN_DATA_INPUT" ).c_str() );	

	// 한번도 시도 하지 않았다면
	if( GetSystem()->GetDigimonEggCount() == 0 )
	{
		// 돌려 받을수 있다.
		_CancelEnableControl();

	}
	else
	{
		// 지우거나 보상받아야 한다.
		_CancelDisableControl();
	}	

	// 지급 받을수 있는 기준 레벨을 넘어 갔다면
	m_pRequiteBtn->SetEnable( ( GetSystem()->GetDigimonEggCount() >= IF_MAKE_TACTICS_REQUITE_LEVEL ) );

#ifdef MAKE_TACTICS_UI_UPGRADE
	m_pDataChangeBtn->SetEnable( true );	// 알 등록 후 변경 버튼 활성화

	// 사용 가능 데이터 초기화
	GetSystem()->SetDataFlagMax(CsTactics::DATA_LV1);
	GetSystem()->SetDataFlag(CsTactics::DATA_LV1);
	_RefreshGradeString();

	// 최대 사용 가능 DATA 등급 책정
	CsTactics::sINFO* pFTTactics = nsCsFileTable::g_pTacticsMng->GetTactics( GetSystem()->GetDigimonEggType() )->GetInfo();

	SAFE_POINTER_RET(pFTTactics);

	for(int i = CsTactics::DATA_LV1; i < CsTactics::DATA_MAX; ++i)
	{
		if( pFTTactics->s_nLimitLevel[i] == 0 )
			break;

		if( pFTTactics->s_nLimitLevel[i] != 0)
		{
			GetSystem()->SetDataFlagMax(i+1);

			// DATA 사용이 가능하다면 해당 디지타마의 부화 가능한 MAX단수를 갱신해준다.
			GetSystem()->SetEnableGradeMax(pFTTactics->s_nLimitLevel[i]);
		}
	}
#endif
}

void cMakeTactics::_ResistDigitamaDisableControl()
{
	bAutoResistBtnStatus = true;
	if( m_pAutoResistBtn )
	{
		m_pAutoResistBtn->SetText( UISTRING_TEXT( "COMMON_TXT_AUTO_REGISTER" ).c_str() );	
		m_pAutoResistBtn->SetControlName( "RegistItem" );
	}

	_CancelEnableControl();

	m_pRequiteBtn->SetEnable( false );		


	m_StringBody.Delete();
#ifdef MAKE_TACTICS_UI_UPGRADE
	m_pDataChangeBtn->SetEnable( false );	// 알 등록 버튼일 때는 변경 비활성화
#endif
#ifdef MINIGAME
	SetEnableMinigameBt(false);
#endif
}


void cMakeTactics::_CancelEnableControl()
{
	bCancelBtnStatus= true;
	m_pCancelBtn->SetText( UISTRING_TEXT( "COMMON_TXT_CANCEL" ).c_str() );	
	m_pCancelBtn->SetControlName( "TacticeCancel" );
}

void cMakeTactics::_CancelDisableControl()
{
	bCancelBtnStatus = false;
	m_pCancelBtn->SetText( UISTRING_TEXT( "MAKETACTICS_BTN_REMOVE" ).c_str() );
	m_pCancelBtn->SetControlName( "ItemRemove" );
}

void cMakeTactics::_ResistDigitama()
{
	if( m_pAutoResistBtn->IsEnable() == false)	// 디지타마 부화중이면 리턴
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}

	if(GetSystem()->AutoResistDigiMonEgg())
	{
		ResistDigitama(GetSystem()->GetDigiMonEggIndex() - 1);
	}		
}

void cMakeTactics::_DeleteBackupDisk()
{	
	GetSystem()->ResetBackupDisk();
	_ResistBackupDiskDisableControl();	
}

void cMakeTactics::_CancelDigitama()
{
	if( GetSystem()->GetDigimonEggType() == 0 )
	{
		Close();
		return;
	}

	assert_cs( GetSystem()->GetDigimonEggCount() == 0 );

	// 인벤에 빈공간이 없다면
	if( g_pDataMng->GetInven()->GetEmptySlotCount() == 0 )
	{
		cPrintMsg::PrintMsg( 11015 );
		return;
	}

	GetSystem()->Cancel_Digitama(GetNpcID(), (iPotableIndex == -1)?true:false);

	SetNpcAnimation(ANI::NPC_OPEN_IDLE2);	

#ifdef MINIGAME
	GetSystem()->SetMinigameComp(false);
#endif

	// 등록되어 있는거 초기화
	m_CurBar.InitLevel( 0 );
	_ResistDigitamaDisableControl();
}

void cMakeTactics::_DeleteDigitama()
{
	cPrintMsg::PrintMsg( 20004 );	
}

void cMakeTactics::_TryLevelUp()
{
	if(GetSystem()->TryLevelUp1())
	{
		return;
	}

	if(GetSystem()->TryLevelUp2())
	{
		_DeleteBackupDisk();
		SendServer();
	}
}

void cMakeTactics::SetTarget( CsC_AvObject* pTarget, bool	bPortable )
{
	m_bPortable = bPortable;

	if(pTarget)						// 기존 부화 인큐베이터 일 경우
		m_pTarget = pTarget;

	else if(m_pNPCObejct == NULL )	// 휴대용 인큐배이터 일 경우
	{
		CTamerUser*	pTamerUser = g_pCharMng->GetTamerUser();
		SAFE_POINTER_RET( pTamerUser );

		if( !g_pNpcMng->IsNpc( HATCH_NPC_ID ) )	// NPC 정보가 없으면 생성.
			g_pNpcMng->AddNpc(HATCH_NPC_ID%NPC_FT_MASK, HATCH_NPC_ID);	// NPC 추가.

		cType type;	// 애니메이션 실행 불가..
		type.SetIDX(HATCH_NPC_ID%NPC_FT_MASK, nClass::Npc);
		m_pNPCObejct = g_pMngCollector->GetObject(type);

		SAFE_POINTER_RET(m_pNPCObejct);

		// 부화기 회전 방향 설정. 카메라 바라보기.
		NiPoint3 CamPos = CAMERA_ST.GetWorldTranslate();
		NiPoint3 ObjDir = CamPos - pTamerUser->GetPos();
		m_pNPCObejct->SetRotation_AniDir( ObjDir );

		// 부화기 위치 설정. 태이머 위치.
		m_pNPCObejct->SetPos( pTamerUser->GetPos() );
	}
}

CsC_AvObject* cMakeTactics::GetTarget() 
{ 
	return m_pTarget; 
}	

void cMakeTactics::SendServer()
{
	if(GetSystem()->Send_Server(GetNpcID(),(iPotableIndex == -1)?true:false))
	{
		RecvResult( SUCCESS );
	}
}

void cMakeTactics::RecvServer()
{ 
	GetSystem()->SetWaitRecvServer(false);
}

void cMakeTactics::ResistDigitama( int nInvenIndex )
{
	int ret = GetSystem()->Resist_Digitama(nInvenIndex, GetNpcID(), (iPotableIndex == -1)?true:false);

	if(ret == 1)
	{
		return;
	}

	if(ret == 2)
	{
		SetNpcAnimation(ANI::NPC_OPEN_IDLE1);
	}
	_ResistDigitamaEnableControl();
#ifdef MINIGAME
	GetSystem()->SetMinigameComp(false);
	SendMiniGameCheck();
#endif



}

void cMakeTactics::Resist( int nInvenIndex )
{
	if( m_pAutoResistBtn->IsEnable() == false)	// 디지타마 부화중이면 리턴
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}

	switch( GetSystem()->IsDigimonEggResist( nInvenIndex ) )
	{
	case IF_DIGITAMA_MAKE_TACTICS_ITEM_TYPE:
		ResistDigitama( nInvenIndex );
		break;
	case nItem::BackupDisk:
		ResistBackupDisk( nInvenIndex );
		break;
	}
}

void cMakeTactics::ResistBackupDisk( int nInvenIndex )
{
	int ret = GetSystem()->Resist_BackupDisk(nInvenIndex, GetNpcID(), (iPotableIndex == -1)?true:false);

	if(ret == 1)
	{
		return;
	}

	if(ret == 2)
	{
		SetNpcAnimation(ANI::NPC_OPEN_IDLE1);
	}

	_ResistBackupDiskEnableControl();	
}

void cMakeTactics::RequiteTactics( TCHAR* szName )
{
	assert_cs( GetSystem()->GetDigimonEggType() != 0 );
	assert_cs( GetSystem()->GetDigimonEggCount() >= 3 );

	g_pTacticsAni->Set_bAniProcess(false);	//2016-03-02-nova 디지몬부화시 카메라 이동 애니메이션 작동

	if(m_bPortable)	// 휴대용 아이템일 경우 NPC 애니 스킵
	{
		g_pTacticsAni->Skip( GetSystem()->GetDigimonEggType(), m_pNPCObejct);
	}
	else if(m_pTarget)
	{
		g_pTacticsAni->Start( GetSystem()->GetDigimonEggType(), m_pTarget );
	}
	else			// 예외 처리. 버그로 인한 NPC 타겟이 없을지도 모를 경우
	{
		g_pTacticsAni->Skip( GetSystem()->GetDigimonEggType(), m_pNPCObejct);
	}


	m_CurBar.InitLevel( 0 );	

	_ResistDigitamaDisableControl();

	GetSystem()->Requite_Tactics(szName, GetNpcID(), iPotableIndex );	
}

void cMakeTactics::DestroyResistDigitama( bool bNetwork )
{
	if( GetSystem()->IsTutorialPlaying() )
	{
		cPrintMsg::PrintMsg( 30412 );
		return;
	}

	// 동기화
	if( bNetwork == true )
	{
		GetSystem()->DestroyResist_Digitama(GetNpcID(), (iPotableIndex == -1)?true:false);

		m_CurBar.InitLevel( 0 );

		SetNpcAnimation( ANI::NPC_OPEN_IDLE2 );	
#ifdef MINIGAME
		GetSystem()->SetMinigameComp(false);
		OnUpdateMinigameInfo();
#endif
	}

	GetSystem()->ResetDigimonEgg();
	_ResistDigitamaDisableControl();
}

void cMakeTactics::RecvResult( eRESULT result )
{
	m_eResult = result;

	switch( m_eResult )
	{
	case DEL_DIGITAMA:
		SetNpcAnimation( ANI::NPC_TACTICS_FAIL2 );
		m_CurBar.ChangeLevel( sBAR::CN_DESTROY );
		break;
	case DEL_DATA:
		SetNpcAnimation( ANI::NPC_TACTICS_FAIL1 );	
		m_CurBar.ChangeLevel( sBAR::CN_KEEP );
		break;
	case SUCCESS:		
		SetNpcAnimation( ANI::NPC_TACTICS_SUCCESS );
		m_CurBar.ChangeLevel( sBAR::CN_INCREASE );
		break;
	case DEL_BACKUP:
		SetNpcAnimation( ANI::NPC_TACTICS_FAIL1 );	
		m_CurBar.ChangeLevel( sBAR::CN_KEEP );
		break;
	default:
		assert_cs( false );
	}

	m_pAutoResistBtn->SetEnable(false);
	m_pCancelBtn->SetEnable(false);

	m_pRequiteBtn->SetEnable( false );
	m_pDataChangeBtn->SetEnable( false );	


	if(m_pBackupDiskBtn)
		m_pBackupDiskBtn->SetEnable(false);

#ifdef MINIGAME
	GetSystem()->SetMinigameComp(false);
	OnUpdateMinigameInfo();
	m_pMiniGameBtn->SetEnable(false);
#endif
}

void cMakeTactics::ProcessResult()
{
	if( m_eResult == NONE )
		return;

	switch( m_eResult )
	{
	case DEL_DIGITAMA:
		{
#ifdef MINIGAME
			GetSystem()->SetMinigameComp(false);
#endif
			g_pCharMng->GetTamerUser()->SetAnimation( ANI::NPC_NO );

			SetNpcAnimation( ANI::NPC_OPEN_IDLE2 );

			cPrintMsg::PrintMsg( 20015 );

			// 용병 부화용 데이타 입력 결과 - 실패
			GetSystem()->DataInputFailed();

			DestroyResistDigitama( false );	
		}		
		break;
	case DEL_DATA:
		{
#ifdef MINIGAME
			GetSystem()->SetMinigameComp(false);
#endif
			SetNpcAnimation( ANI::NPC_OPEN_IDLE1 );
			cPrintMsg::PrintMsg( 20016 );
		}
		break;
	case SUCCESS:
		{
#ifdef MINIGAME
			GetSystem()->SetMinigameComp(false);
#endif			
			assert_cs( GetSystem()->GetDigimonEgg()->IsEnable() == true );
			SetNpcAnimation( ANI::NPC_OPEN_IDLE1 );	

			GetSystem()->SetSuccessLevel();

			_CancelDisableControl();

			// 지급 받을수 있는 기준 레벨을 넘어 갔다면
			m_pRequiteBtn->SetEnable( ( GetSystem()->GetDigimonEggCount() >= IF_MAKE_TACTICS_REQUITE_LEVEL ) );
		
			int nCurLevel = GetSystem()->GetDigimonEggCount();
			switch( nCurLevel )
			{
			case 1:
				cPrintMsg::PrintMsg( 20008 );
				break;
			case 2:
				cPrintMsg::PrintMsg( 20009 );
				break;
			case 3:
				cPrintMsg::PrintMsg( 20010 );
				cWindow::PlaySound( "System\\Tactics_success.wav" );
				break;
			case 4:
				cPrintMsg::PrintMsg( 20011 );
				cWindow::PlaySound( "System\\Tactics_success.wav" );
				break;
			case 5:
				cPrintMsg::PrintMsg( 20012 );
				cWindow::PlaySound( "System\\Tactics_success.wav" );
				break;
			default:	assert_cs( false );
			}

			if( nCurLevel > 2 )
			{
				if( g_pCharMng && g_pCharMng->GetTamerUser() )
					g_pCharMng->GetTamerUser()->SetAnimation( ANI::NPC_YES );	
			}
		}
		break;

	case DEL_BACKUP:
		{
			SetNpcAnimation( ANI::NPC_OPEN_IDLE1 ); 
			cPrintMsg::PrintMsg( 30503 );
			#ifdef MINIGAME
			GetSystem()->SetMinigameComp(false);
			#endif
		}
		break;

	default:
		assert_cs( false );
	}

	m_eResult = NONE;	
}

bool cMakeTactics::GetAutoResistBtnStatus()
{
	return bAutoResistBtnStatus;
}

uint cMakeTactics::_GetBackupDiskType()
{
	return GetSystem()->GetBackupDiskType();
}

cButton* const cMakeTactics::GetCurResiBtn() const
{
	return m_pAutoResistBtn;
};

bool cMakeTactics::GetDeleteBtnState()	const
{
	SAFE_POINTER_RETVAL(m_pCancelBtn,FALSE);
	return m_pCancelBtn->IsEnable();
}

void cMakeTactics::SetDeleteBtnState(bool bEnable)
{
	SAFE_POINTER_RET(m_pCancelBtn);
	m_pCancelBtn->SetEnable(bEnable);
}

cItemInfo* cMakeTactics::GetResistItem()
{
	return GetSystem()->GetDigimonEgg(); 
}





#ifdef	MINIGAME
void cMakeTactics::SendMiniGameCheck()
{
	if(GetSystem()->GetDigimonEggType() == 0)
		return;
	OnUpdateMinigameInfo();
}
void cMakeTactics::OnEndMiniGame(void)
{
	OnUpdateMinigameInfo();
}

void cMakeTactics::OnUpdateMinigameInfo(void)
{
	int nCount = GetSystem()->GetMinigameSuccessCnt();//성공 횟수
	bool isCompleted = GetSystem()->GetMinigameComp();//성공 횟수
	int DigiLv = GetSystem()->GetDigimonEggCount();
	UpdateMiniGameInfoText(isCompleted, nCount, DigiLv);
	UpdateMinigameInfoStartBt(isCompleted, nCount, DigiLv);
}

void cMakeTactics::UpdateMiniGameInfoText(const bool isCompleted, const int& nCount, const int& DigitamaLv)
{
	m_StringBody.Delete();
	if(isCompleted == true && DigitamaLv  < 4)
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );	
		cString* pString = NiNew cString;
		TCHAR nPrib[10] ;
		ti.s_eTextAlign = DT_CENTER;
#ifdef VERSION_TH
		ti.s_eFontSize = CFont::FS_10;
		memcpy(nPrib,_T("LUCK"),10);
		switch(nCount)
		{
		case 0:
			ti.s_Color = NiColor( 25/255.0f, 25/255.0f, 25/255.0f );//검정;	
			break;
		case 1:
			ti.s_Color = NiColor( 93/255.0f, 93/255.0f, 93/255.0f );//진한회색
			break;
		case 2:
			ti.s_Color = NiColor( 189/255.0f, 189/255.0f, 189/255.0f );//회색;
			break;
		case 3:
			ti.s_Color = NiColor( 255/255.0f, 228/255.0f, 0/255.0f );//노랑;
			break;
		case 4:
			ti.s_Color = NiColor( 29/255.0f, 219/255.0f, 22/255.0f );//녹색;
			break;
		case 5:
			ti.s_Color = NiColor( 1/255.0f, 0/255.0f, 255/255.0f );//파랑
			break;
		case 6:
			ti.s_Color = NiColor( 131/255.0f, 36/255.0f, 255/255.0f );//보라
			break;
		case 7:
			ti.s_Color = NiColor( 255/255.0f, 0/255.0f, 0/255.0f );//빨강;
			break;
		}
#else
		ti.s_Color = FONT_RED;

		switch(nCount)
		{
		case 0:
			memcpy(nPrib,_T("-4%"),10);
			break;
		case 1:
			memcpy(nPrib,_T("-3%"),10);
			break;
		case 2:
			memcpy(nPrib,_T("-2%"),10);
			break;
		case 3:
			memcpy(nPrib,_T("-1%"),10);
			break;
		case 4:
			memcpy(nPrib,_T("+1%"),10);
			break;
		case 5:
			memcpy(nPrib,_T("+2%"),10);
			break;
		case 6:
			memcpy(nPrib,_T("+3%"),10);
			break;
		case 7:
			memcpy(nPrib,_T("+4%"),10);
			break;
		}
#endif


		ti.SetText( nPrib );
		pString->AddText( &ti );
		m_StringBody.AddTail( pString );
	}
}
void cMakeTactics::UpdateMinigameInfoStartBt(const bool isCompleted, const int& Count, const int& DigitamaLv)
{
	if(isCompleted == false && DigitamaLv < 4)
		SetEnableMinigameBt(true);
	else
		SetEnableMinigameBt(false);

}
void cMakeTactics::SetEnableMinigameBt(bool bOn)
{
	if(m_pMiniGameBtn)
		m_pMiniGameBtn->SetEnable(bOn);
}
#endif