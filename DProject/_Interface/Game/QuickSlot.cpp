
#include "stdafx.h"
#include "QuickSlot.h"
#include "../Adapt/AdaptTutorialQuest.h"

#define IF_QSLOT_ICON_SIZE CsPoint( 32, 32 )
#define ICON_HALF		16

cQuickSlot::cQuickSlot() : m_pInfoBtn(NULL), m_pTargetObject(NULL), m_eQ_Type(NONE), m_pAdaptTutorial(NULL)
{
	m_pAdaptTutorial = new AdaptTutorialQuest;
	if( m_pAdaptTutorial )
		m_pAdaptTutorial->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CT_TUTORIAL_CONTENTS ) );
}

cQuickSlot::~cQuickSlot()
{
	m_lLockSlot.clear();
	m_pAdaptTutorial = NULL;
}

void cQuickSlot::Destroy()
{
	cBaseWindow::Delete();
}

void cQuickSlot::DeleteResource()
{
	DeleteScript();
	SAFE_NIDELETE( m_pInfoBtn );
	SAFE_NIDELETE( m_pBox );
	SAFE_NIDELETE( m_pBox2 );
	SAFE_NIDELETE( m_pAddImage );
	if( m_eQ_Type == Q_ADDSLOT )
	{
		SAFE_NIDELETE( m_pSlotCloseBtn );
		SAFE_NIDELETE( m_pSlotOpenBtn );
		SAFE_NIDELETE( m_pCenterBarCloseBtn );
		SAFE_NIDELETE( m_pCenterBarOpenBtn );
	}
	for( int i = 0 ; i < nLimit::QuickSlot ; i++ )
		SAFE_NIDELETE( m_pMacroText[ i ] );
}

void cQuickSlot::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	m_nQuickSlotIndex = nValue;
	m_eQ_Type = (eQUICK_TYPE)m_nQuickSlotIndex;	// 0=테이머, 1=디지몬, 2이상=추가퀵슬롯

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_8 );
	ti.s_eTextAlign = DT_CENTER;

	switch( m_eQ_Type )
	{
	case Q_TAMER:
		{
			CsPoint ptInitPos = CsPoint( 397, g_nScreenHeight - 44 - Q_TAMER*44 - 27 );
			InitScript( NULL, ptInitPos, CsPoint( 376, 44 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP );

			m_pInfoBtn = NiNew cButton;
			m_pInfoBtn->Init( m_pRoot, CsPoint::ZERO, CsPoint( 28, 44 ), "QuickSlot\\Info.tga", false );
			m_pInfoBtn->SetTexToken( CsPoint( 28, 0 ) );
			m_pAddImage = NiNew cImage;
			m_pAddImage->Init( m_pRoot, CsPoint( 28, 0 ), CsPoint( 44, 44 ), "QuickSlot\\Characterbox.tga", false );
		}
		break;
	case Q_DIGIMON:
		{
			CsPoint ptInitPos = CsPoint( 397, g_nScreenHeight - 44 - Q_DIGIMON*44 - 27 );
			InitScript( NULL, ptInitPos, CsPoint( 376, 44 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP );
			m_ptRootPos = GetRootClient();
			m_pInfoBtn = NiNew cButton;
			m_pInfoBtn->Init( m_pRoot, CsPoint::ZERO, CsPoint( 28, 44 ), "QuickSlot\\Info_Digi.tga", false );
			m_pInfoBtn->SetTexToken( CsPoint( 28, 0 ) );
			m_pAddImage = NiNew cImage;
			m_pAddImage->Init( m_pRoot, CsPoint( 28, 0 ), CsPoint( 44, 44 ), "QuickSlot\\Characterbox.tga", false );
		}
		break;
	case Q_ADDSLOT:	// 추가 퀵슬롯 가장 상위 줄
		{
			CsPoint ptInitPos = CsPoint( 397, g_nScreenHeight - 44 - Q_DIGIMON*44 - 27 );
			InitScript( NULL, ptInitPos, CsPoint( 376, 44 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP );

			//	열기 버튼
			m_pSlotOpenBtn = NiNew cButton;
			m_pSlotOpenBtn->Init( m_pRoot, CsPoint::ZERO, CsPoint( 36, 18 ), "QuickSlot\\Quick_OpenBtn.tga", false );
			m_pSlotOpenBtn->SetTexToken( CsPoint( 0, 18 ) );
			//	닫기 버튼
			m_pSlotCloseBtn = NiNew cButton;
			m_pSlotCloseBtn->Init( m_pRoot, CsPoint::ZERO, CsPoint( 36, 18 ), "QuickSlot\\Quick_CloseBtn.tga", false );
			m_pSlotCloseBtn->SetTexToken( CsPoint( 0, 18 ) );

			//	센터바 열기 버튼
			m_pCenterBarOpenBtn = NiNew cButton;
			m_pCenterBarOpenBtn->Init( m_pRoot, CsPoint::ZERO, CsPoint( 77, 24 ), "QuickSlot\\Centerbar_On.tga", false );
			m_pCenterBarOpenBtn->SetTexToken( CsPoint( 0, 24 ) );

			//	센터바 닫기 버튼
			m_pCenterBarCloseBtn = NiNew cButton;
			m_pCenterBarCloseBtn->Init( m_pRoot, CsPoint::ZERO, CsPoint( 77, 24 ), "QuickSlot\\Centerbar_Off.tga", false );
			m_pCenterBarCloseBtn->SetTexToken( CsPoint( 0, 24 ) );

			//초상화위치 이미지
			m_pAddImage = NiNew cImage;
			m_pAddImage->Init( m_pRoot, CsPoint( 0, 0 ), CsPoint( 28, 43 ), "QuickSlot\\Info_Plus.tga", false );
			m_pAddImage->SetTexToken( CsPoint( 28, 0 ) );
			m_pAddImage->SetState( cImage::NORMAL0 );

			m_pInfoBtn = NiNew cButton;
		}
		break;
	default:	// 추가 퀵슬롯
		{

		}
		break;
	}

	CsPoint start( 72, 0 );
	for( int i=0; i<nLimit::QuickSlot; ++i )
	{		
		m_IFIcon[ i ].Init( CsRect( start + CsPoint( 3, 2 ), (CsSIZE)IF_QSLOT_ICON_SIZE ) );
		m_pMacroText[ i ] = NiNew cText;
		m_pMacroText[ i ]->Init( m_pRoot, start + CsPoint( 18, 33 ), &ti, false );

		start.x += 38;
	}

	m_pBox = NiNew cSprite;
	m_pBox->Init( NULL, CsPoint::ZERO, CsPoint( 38, 44 ), "QuickSlot\\Skillbox.tga", false );

	m_pBox2 = NiNew cImage;
	m_pBox2->Init( NULL, CsPoint::ZERO, CsPoint( 38, 44 ), "QuickSlot\\Skillbox2.tga", false );
	m_pBox2->SetTexToken( CsPoint( 38, 0 ) );
	m_pBox2->SetState( cImage::NORMAL0 );

}

void cQuickSlot::ChangeMacroKey()
{
	TCHAR sz[ MAX_FILENAME ];
	for( int i=0; i<nLimit::QuickSlot; ++i )
	{
		bool bSuccess;
		switch( m_eQ_Type )
		{
		case Q_TAMER:	bSuccess= GLOBALINPUT_ST.GetHotKeyPtr()->GetText( (DMKEY::eTYPE)( i ), sz, MAX_FILENAME );									break;
		case Q_DIGIMON:	bSuccess= GLOBALINPUT_ST.GetHotKeyPtr()->GetText( (DMKEY::eTYPE)( nLimit::QuickSlot + i ), sz, MAX_FILENAME );		break;
		case Q_ADDSLOT:	bSuccess= GLOBALINPUT_ST.GetHotKeyPtr()->GetText( (DMKEY::eTYPE)( DMKEY::QU_2_0 + i ), sz, MAX_FILENAME );			break;
		}
		assert_cs( bSuccess == true );
		m_pMacroText[ i ]->SetText( sz );
	}	
}

void cQuickSlot::Update(float const& fDeltaTime)
{	
	if( m_eQ_Type == Q_ADDSLOT )
	{
		if( g_pResist->m_CenterBar.s_bIsOpenQuick )	// 퀵슬롯이 열려있을 때
		{
			//버튼 위치
			m_pSlotCloseBtn->SetPos( CsPoint( 155, -96 ) );
			if( g_pResist->m_CenterBar.s_bInitial )
			{
				m_pCenterBarCloseBtn->SetPos( CsPoint( 300, -68 ) );
			}
			else
			{
				m_pCenterBarOpenBtn->SetPos( CsPoint( 300, -68 ) );
			}
		}
		else	// 추가 퀵슬롯이 닫혀있을 때
		{
			//버튼 위치
			m_pSlotOpenBtn->SetPos( CsPoint( 155, -52 ) );
			
			if( g_pResist->m_CenterBar.s_bInitial )
			{
				m_pCenterBarCloseBtn->SetPos( CsPoint( 300, -24 ) );
			}
			else
			{
				m_pCenterBarOpenBtn->SetPos( CsPoint( 300, -24 ) );
			}
		}
	}
	else if( m_eQ_Type == Q_DIGIMON )
	{
		CsPoint start( 72, 0 );
		CsPoint ptBtnPos( 0, 0 );

		if( g_pResist->m_CenterBar.s_bIsOpenQuick/*g_pGameIF->GetQuickSlot( Q_ADDSLOT )->m_bIsOpen*/ )//추가 퀵슬롯 오픈상태일 때
		{
			while( GetRootClient().y > ( m_ptRootPos.y - 44 ) )
				SetMove( CsPoint( 0, -4 ) );

			start = start - CsPoint( 0, 44 );
			m_pRoot->SetPos( m_ptRootPos - CsPoint( 0, 44 ) );
		}
		else
		{
			while( GetRootClient().y < m_ptRootPos.y )
				SetMove( CsPoint( 0, 4 ) );

			m_pRoot->SetPos( m_ptRootPos);
		}

		m_pInfoBtn->SetPos( ptBtnPos );

		for( int i=0; i<nLimit::QuickSlot; ++i )
			m_pMacroText[ i ]->SetPos( CsPoint( 72 + 38 * i , 0 ) + CsPoint( 18, 33 ) );


	}
}

cBaseWindow::eMU_TYPE cQuickSlot::Update_ForMouse()
{
	if( m_eQ_Type == Q_ADDSLOT )	//	추가 퀵슬롯일 때
		return _Update_ForMouse_AddSlot();

	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
	{
		m_pInfoBtn->Update_ForMouse();	//2016-02-15-nova
		return muReturn;
	}

	if( m_pInfoBtn->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		// 테이머 거라면
		if( m_eQ_Type == Q_TAMER )
		{
			bool bSetTab = true;
			int nTabNo = 0;
			ContentsStream kTmp;
			kTmp << bSetTab << nTabNo;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_CLOSE_TAMERSTATUS, &kTmp );
		}
		else
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_CLOSE_DIGIMONSTATUS, NULL );
		}

		return muReturn;
	}
#ifdef SIMPLE_TOOLTIP
	CsRect rcTemp = CsRect( CsPoint::ZERO, CsSIZE( 28, 44 ) );
	if( m_eQ_Type == Q_DIGIMON )
	{
		if( g_pResist->m_CenterBar.s_bIsOpenQuick )
			rcTemp = CsRect( CsPoint( 0, -44 ), CsSIZE( 28, 44 ) );
	}

	if(rcTemp.PtInRect( MousePosToWindowPos( CURSOR_ST.GetPos() ) ) == TRUE)
	{
		if( m_nQuickSlotIndex == 0 ){

			CsTalk_Tooltip::sINFO* pInfo = nsCsFileTable::g_pTalkMng->GetTalk_Tooltip( 1055 )->GetInfo();
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 0, 0 ), 100, cTooltip::UIHELP, 1055 );	//2017-01-19-nova 마우스 좌표로 툴팁표시
		}
		else
		{
			CsTalk_Tooltip::sINFO* pInfo = nsCsFileTable::g_pTalkMng->GetTalk_Tooltip( 1056 )->GetInfo();
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 0, 0 ), 100, cTooltip::UIHELP, 1056 );	//2017-01-19-nova 마우스 좌표로 툴팁표시
		}
	}
#endif
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	CURSOR::eTYPE eCursor = CURSOR_ST.GetButtonState();

	switch( eCursor )
	{	
	case CURSOR::LBUTTON_DOWN:		
		{
			for( int i=0; i<nLimit::QuickSlot; ++i )
			{
				if( m_IFIcon[ i ].MouseLBtnClick( ptMouseLocalPos ) == false )
				{
					CURSOR_ST.SetButtonState(CURSOR::LBUTTON_PRESS);
					m_IFIcon[ i ].ReleaseState();
				}
			}
		}
		break;
	case CURSOR::RBUTTON_DOWN:		
		{
			for( int i=0; i<nLimit::QuickSlot; ++i )
			{
				if( m_IFIcon[ i ].MouseRBtnClick( ptMouseLocalPos ) == false )
				{
					CURSOR_ST.SetButtonState(CURSOR::RBUTTON_PRESS);
					m_IFIcon[ i ].ReleaseState();
				}
			}
		}
		break;
	case CURSOR::LBUTTON_PRESS:
		{
			OnMouseOutWindow();
			for( int i=0; i<nLimit::QuickSlot; ++i )
			{
				if( m_IFIcon[ i ].s_eState != sIFIcon::MOUSE_LCLICK )
				{
					if( m_IFIcon[ i ].MouseOn( ptMouseLocalPos ) == false )
						m_IFIcon[ i ].ReleaseState();
				}
			}
		}
		break;
	case CURSOR::RBUTTON_PRESS:
		{
			for( int i=0; i<nLimit::QuickSlot; ++i )
			{
				if( m_IFIcon[ i ].s_eState != sIFIcon::MOUSE_RCLICK )
				{
					if( m_IFIcon[ i ].MouseOn( ptMouseLocalPos ) == false )
						m_IFIcon[ i ].ReleaseState();
				}
			}
		}
		break;
	default:
		for( int i=0; i<nLimit::QuickSlot; ++i )
		{
			if( m_IFIcon[ i ].MouseOn( ptMouseLocalPos ) == false )				
				m_IFIcon[ i ].ReleaseState();
		}	
	}

	

	// 툴팁용
	{
		cResist::sCHAR_QUICKSLOT::sCHAR* pResistQuickSlot;
		cResist::sCHAR_QUICKSLOT::sCHAR::sINFO* pResistQuickSlotInfo;
		switch( m_pTargetObject->GetLeafRTTI() )
		{
		case RTTI_TUTORIAL_TAMER:
		case RTTI_TAMER_USER:
			pResistQuickSlot = ( (CTamerUser*)m_pTargetObject )->GetQuickSlot();
			break;
		case RTTI_TUTORIAL_DIGIMON:
		case RTTI_DIGIMON_USER:
			pResistQuickSlot = ( (CDigimonUser*)m_pTargetObject )->GetQuickSlot();
			break;
		default:
			assert_cs( false );
		}
		
		for( int i=0; i<nLimit::QuickSlot; ++i )
		{
			if( m_IFIcon[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == FALSE )
				continue;

// 			if( !GLOBALINPUT_ST.IsShift() )
// 				break;

			pResistQuickSlotInfo = &pResistQuickSlot->s_QuickSlotInfo[ i ];
			switch( pResistQuickSlotInfo->s_eType )
			{
			case cResist::NONE:
				break;
			case cResist::ITEM:
				{
					CsPoint pos = m_IFIcon[ i ].GetPos() + GetRootClient();
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_QSLOT_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, pResistQuickSlotInfo->s_nID, cBaseWindow::WT_QUICKSLOT );
				}				
				break;
			case cResist::SKILL:
				{
					CsPoint pos = m_IFIcon[ i ].GetPos() + GetRootClient();
					int nSize;
#ifdef VERSION_USA
					nSize = 360;
#else
					nSize = TOOLTIP_MAX_SIZE;
#endif
					switch( m_pTargetObject->GetLeafRTTI() )
					{
					case RTTI_TUTORIAL_TAMER:
					case RTTI_TAMER_USER:
						TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_QSLOT_ICON_SIZE, nSize, cTooltip::SKILL, pResistQuickSlotInfo->s_nID, m_nQuickSlotIndex, cBaseWindow::WT_QUICKSLOT );
						break;
					case RTTI_TUTORIAL_DIGIMON:
					case RTTI_DIGIMON_USER:
						TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_QSLOT_ICON_SIZE, nSize, cTooltip::SKILL, pResistQuickSlotInfo->s_nID, ( (CDigimonUser*)m_pTargetObject )->GetFTEvolCurObj()->m_nEvoSlot, cBaseWindow::WT_QUICKSLOT );
						break;
					}
				}				
				break;
			default:
				assert_cs( false );
			}
		}
	}	

	return muReturn;
}

void cQuickSlot::Render()
{
	switch( m_eQ_Type )
	{
	case Q_TAMER:	// 테이머 퀵슬롯
		{
			m_pInfoBtn->Render();
			// 얼굴 이미지
			if( m_pTargetObject == NULL )
				return;

			_RenderTamer();
		}
		break;
	case Q_DIGIMON:	// 디지몬 퀵슬롯
		{
			m_pInfoBtn->Render();
			// 얼굴 이미지
			if( m_pTargetObject == NULL )
				return;

			_RenderDigimon();
		}
		break;
	default:	//추가 퀵슬롯
		{
			_Render_AddSlot();
		}
		return;
	}

	for( int i = 0 ; i < nLimit::QuickSlot ; i++ )
		m_pMacroText[ i ]->Render();

	EndRenderScript();
}

void cQuickSlot::_RenderTamer()
{
	CTamerUser* pTamer = (CTamerUser*)m_pTargetObject;	

	// 퀵슬롯
	_RenderSlot( pTamer->GetQuickSlot(), pTamer->GetSkillMng() );

	RenderScript();

	// 얼굴 이미지
	sCHAR_IMAGE* pCharImage = pTamer->GetCharImage();
	if( pCharImage != NULL )
		pCharImage->Render_S( CsPoint( m_pRoot->GetClient().x + 28, m_pRoot->GetClient().y ), CsPoint( 44, 43 ) );
}

void cQuickSlot::_RenderDigimon()
{
	CDigimonUser* pDigimon = (CDigimonUser*)m_pTargetObject;

	// 퀵슬롯
	_RenderSlot( pDigimon->GetQuickSlot(), pDigimon->GetSkillMng() );

	RenderScript();

	// 얼굴 이미지
	sCHAR_IMAGE* pCharImage = pDigimon->GetCharImage();

	if( pCharImage != NULL )
		pCharImage->Render_S( CsPoint( m_pRoot->GetClient().x + 28, m_pRoot->GetClient().y ), CsPoint( 44, 43 ) );
}

void cQuickSlot::_RenderSlot( cResist::sCHAR_QUICKSLOT::sCHAR* pResistQuickSlot, cSkill* pSkillMng )
{
	SAFE_POINTER_RET( pResistQuickSlot );
	SAFE_POINTER_RET( pSkillMng );

	float fDist = 0.0f;
	if( ( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )&&( m_nQuickSlotIndex == 1 )&&g_pCharResMng->GetTargetMark() )
	{
		fDist = ( g_pCharResMng->GetTargetMark()->GetPos2D() - m_pTargetObject->GetPos2D() ).Length();
	}

	CsItem::sINFO* pFTItemInfo;

	cResist::sCHAR_QUICKSLOT::sCHAR::sINFO* pResistQuickSlotInfo = NULL;
	for( int i=0; i<nLimit::QuickSlot; ++i )
	{
		pResistQuickSlotInfo = &pResistQuickSlot->s_QuickSlotInfo[ i ];
		CsPoint pos = m_IFIcon[ i ].GetPos() + GetRootClient();
		if( fDist == 0.0f )
		{
			m_pBox->Render( pos - CsPoint( 3, 2 ) );
		}
		else
		{
			// 상대방의 몸사이즈 만큼 빼주자
			fDist -= g_pCharResMng->GetTargetMark()->GetAttackCharSize();
			int nImageIndex = BoxImage_FromDist( i, fDist );
			if( nImageIndex == -1 )
				m_pBox->Render( pos - CsPoint( 3, 2 ) );
			else
				m_pBox2->Render( pos - CsPoint( 3, 2 ), nImageIndex );
		}		

		switch( pResistQuickSlotInfo->s_eType )
		{
		case cResist::NONE:
			break;
		case cResist::ITEM:
			{
				cItemInfo Item;
				Item.m_nType = pResistQuickSlotInfo->s_nID;
				Item.m_nCount = pResistQuickSlotInfo->s_nCount;

				if( Item.m_nType == 0 || Item.m_nCount == 0 )
					break;

				g_pIconMng->RenderItem( &Item, pos, true );

				pFTItemInfo = nsCsFileTable::g_pItemMng->GetItem( pResistQuickSlotInfo->s_nID )->GetInfo();

				// 쿨타임 중이라면 쿨타임 랜더링
				g_pIconMng->RenderTime( pos, ICON_HALF, pFTItemInfo );

				// 전투중에 못먹는 아이템이라면 레드 마스크 랜더링
				if( ( pFTItemInfo->s_nUseMode == 1 )&&( g_pCharMng->GetDigimonUser( 0 )->IsBattle() ) )
				{
					g_pIconMng->RenderMask( ICONMASK::IF_ICON_RED, pos, IF_QSLOT_ICON_SIZE );
				}
				else if( IsLockSlot( i ) )
					g_pIconMng->RenderMask( ICONMASK::IF_ICON_RED, pos, IF_QSLOT_ICON_SIZE );
			}
			break;
		case cResist::SKILL:
			{
				cSkill::sINFO* pSkillInfo = pSkillMng->GetSkill( pResistQuickSlotInfo->s_nID );								
				if( pSkillInfo == NULL )	// 게임 종료 및 로그아웃 후 기간 만료로 스킬 삭제 되었을때 대비용
					break;
				
				g_pIconMng->RenderSkill( pSkillInfo, pos );

				// 쿨타임 중이라면 쿨타임 랜더링
				g_pIconMng->RenderTime( pos, ICON_HALF, &pSkillMng->GetSkill( pResistQuickSlotInfo->s_nID )->s_CoolTime );
				
				// ds 체크 ( ds 모자를때는 레드마스크 랜더링 )
				if( pSkillInfo->s_pFTSkill->GetInfo()->s_nUseDS > m_pTargetObject->GetBaseStat()->GetDS() )
				{
					g_pIconMng->RenderMask( ICONMASK::IF_ICON_RED, pos, IF_QSLOT_ICON_SIZE );
				}
				else if( IsLockSlot( i ) )
					g_pIconMng->RenderMask( ICONMASK::IF_ICON_RED, pos, IF_QSLOT_ICON_SIZE );

				//쿨타임 초기화 했을 경우 마스크 알파 바꿔 가면서 랜더
				if( pSkillMng->GetSkill( pResistQuickSlotInfo->s_nID )->s_CoolTime.IsCoolReset() )
				{
					u4 nDeltaTick = pSkillMng->GetSkill( pResistQuickSlotInfo->s_nID )->s_CoolTime.GetResetProcTick();
					float fAlpha = (float)(nDeltaTick % 500)/500.0f ;

					if( nDeltaTick > 1000 )
						pSkillMng->GetSkill( pResistQuickSlotInfo->s_nID )->s_CoolTime.SetCoolReset( false );

					g_pIconMng->RenderMaskGlitter( ICONMASK::IF_ICON_SELECT,  pos, IF_QSLOT_ICON_SIZE, fAlpha );
				}
			}			
			break;
		default:
			assert_cs( false );
		}

		if( m_IFIcon[ i ].s_eState != sIFIcon::NONE )
		{
			m_IFIcon[ i ].RenderMask( pos, IF_QSLOT_ICON_SIZE );
		}
	}
}


void cQuickSlot::OnMouseOutWindow()
{
	if( IsEnableWindow() == false )
		return;

	cBaseWindow::OnMouseOutWindow();

	if( CURSOR_ST.GetButtonState() != CURSOR::LBUTTON_PRESS )
	{
		for( int i=0; i<nLimit::QuickSlot; ++i )
		{
			m_IFIcon[ i ].ReleaseState( true );
		}
		return;
	}
	if( CURSOR_ST.IsIcon() == true )
		return;

#ifdef LJW_TUTORIAL_RENEWAL_20191128
	if( m_pAdaptTutorial && m_pAdaptTutorial->IsTutorialPlaying_ClientMode() )
		return;
#endif

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
	for( int i=0; i<nLimit::QuickSlot; ++i )
	{
		if( IsLockSlot( i ) )
			continue;

		if( m_IFIcon[ i ].s_eState == sIFIcon::MOUSE_LCLICK )
		{
			if( m_IFIcon[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == FALSE )
			{
				int nQuickSlotType, nID;
				cResist::sCHAR_QUICKSLOT::sCHAR::sINFO* pResistQuickSlotInfo =
					_CalQuickSlotConstant( nQuickSlotType, nID, i );				

				switch( nQuickSlotType )
				{
				case cResist::NONE:
					m_IFIcon[ i ].ReleaseState( true );
					break;
				case cResist::SKILL:
					{
						CURSOR_ST.SetIcon( CURSOR_ICON::CI_QUICKSLOT, i, 1, &m_IFIcon[ i ] );
						CURSOR_ST.SetIconSubInfo( m_nQuickSlotIndex, nQuickSlotType, nID );
						pResistQuickSlotInfo->Reset();
						m_IFIcon[ i ].ReleaseState( true );
					}
					break;
				case cResist::ITEM:
					{
						// 퀵슬롯에서의 아이콘 갯수는 의미가 없다
						CURSOR_ST.SetIcon( CURSOR_ICON::CI_QUICKSLOT, i, 1, &m_IFIcon[ i ] );
						CURSOR_ST.SetIconSubInfo( m_nQuickSlotIndex, nQuickSlotType, nID );
						pResistQuickSlotInfo->Reset();
						m_IFIcon[ i ].ReleaseState( true );
					}
					break;
				default:
					assert_cs( false );
				}
			}
			break;
		}
	}
}

void cQuickSlot::OnLButtonUp( CsPoint pos )
{
	cResist::sCHAR_QUICKSLOT::sCHAR* pResistQuickSlot;

	CsC_AvObject* pTarget;
	switch( m_eQ_Type )
	{
	case Q_TAMER:		
		pTarget = m_pTargetObject;
		pResistQuickSlot = ( (CTamerUser*)m_pTargetObject )->GetQuickSlot();	break;
	case Q_DIGIMON:
		pTarget = m_pTargetObject;
		pResistQuickSlot = ( (CDigimonUser*)m_pTargetObject )->GetQuickSlot();	break;
	case Q_ADDSLOT:	
		{
			if( g_pResist->m_CenterBar.s_bIsOpenQuick )
			{
				pTarget = g_pCharMng->GetTamerUser();
				pResistQuickSlot = &m_pAddQuickSlot->s_QuickSlotChar[ Q_ADDSLOT ];
			}
			else
				return;
		}
		break;
	}
	CsPoint ptLocal = MousePosToWindowPos( pos );
	for( int i=0; i<nLimit::QuickSlot; ++i )
	{
		if( IsLockSlot( i ) )
			continue;

		if( ( m_IFIcon[ i ].s_eState == sIFIcon::MOUSE_LCLICK )&&( m_IFIcon[ i ].s_Rect.PtInRect( ptLocal ) ) )
		{
			// 동작
			cData_QuickSlot::QuickSlotUse( pTarget, pResistQuickSlot, i );
			return;
		}
	}
}

void cQuickSlot::OnRButtonUp( CsPoint pos )
{
	cResist::sCHAR_QUICKSLOT::sCHAR* pResistQuickSlot;

	CsC_AvObject* pTarget;
	switch( m_eQ_Type )
	{
	case Q_TAMER:		
		pTarget = m_pTargetObject;
		pResistQuickSlot = ( (CTamerUser*)m_pTargetObject )->GetQuickSlot();	break;
	case Q_DIGIMON:
		pTarget = m_pTargetObject;
		pResistQuickSlot = ( (CDigimonUser*)m_pTargetObject )->GetQuickSlot();	break;
	case Q_ADDSLOT:	
		{
			if( g_pResist->m_CenterBar.s_bIsOpenQuick )
			{
				pTarget = g_pCharMng->GetTamerUser();
				pResistQuickSlot = &m_pAddQuickSlot->s_QuickSlotChar[ Q_ADDSLOT ];
			}
			else
				return;
		}
		break;
	}

	CsPoint ptLocal = MousePosToWindowPos( pos );
	for( int i=0; i<nLimit::QuickSlot; ++i )
	{
		if( IsLockSlot( i ) )
			continue;

		if( ( m_IFIcon[ i ].s_eState == sIFIcon::MOUSE_RCLICK )&&( m_IFIcon[ i ].s_Rect.PtInRect( ptLocal ) ) )
		{
			// 동작
			cData_QuickSlot::QuickSlotUse( pTarget, pResistQuickSlot, i );
			return;
		}
	}
}

bool cQuickSlot::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	CsPoint ptLocal = MousePosToWindowPos( CURSOR_ST.GetPos() );

	cResist::sCHAR_QUICKSLOT::sCHAR* pResistQuickSlot;

	switch( m_eQ_Type )
	{
	case Q_TAMER:		pResistQuickSlot = ( (CTamerUser*)m_pTargetObject )->GetQuickSlot();	break;
	case Q_DIGIMON:	pResistQuickSlot = ( (CDigimonUser*)m_pTargetObject )->GetQuickSlot();	break;
	case Q_ADDSLOT:	
		{
			if( g_pResist->m_CenterBar.s_bIsOpenQuick )
				pResistQuickSlot = &m_pAddQuickSlot->s_QuickSlotChar[ Q_ADDSLOT ];
			else
				return false;
		}
		break;
	}

	for( int i=0; i<nLimit::QuickSlot; ++i )
	{
		if( m_IFIcon[ i ].s_Rect.PtInRect( ptLocal ) )
		{
			int nInsertReturn = -1;
			switch( eIconType )
			{
			case CURSOR_ICON::CI_INVEN:
				nInsertReturn = cData_QuickSlot::InvenToQuickSlot( nIconSlot, m_nQuickSlotIndex, pResistQuickSlot, i );
				break;
			case CURSOR_ICON::CI_TSKILL:
				// 테이머 스킬이니 소스 인덱스 0
				nInsertReturn = cData_QuickSlot::SkillToQuickSlot( 0, nIconSlot, m_nQuickSlotIndex, pResistQuickSlot, i );				
				break;
			case CURSOR_ICON::CI_DSKILL:
				nInsertReturn = cData_QuickSlot::SkillToQuickSlot( CURSOR_ST.GetIconSubInfo1() + 1, nIconSlot, m_nQuickSlotIndex, pResistQuickSlot, i );				
				break;
			case CURSOR_ICON::CI_QUICKSLOT:
				{
					int nSlotIndex = i;
					if( IsLockSlot( i ) )
					{
						nSlotIndex = nIconSlot;
						m_nQuickSlotIndex = CURSOR_ST.GetIconSubInfo1();
						switch( CURSOR_ST.GetIconSubInfo1() )
						{
						case Q_TAMER:
							{
								SAFE_POINTER_RETVAL( g_pCharMng, false );
								CTamerUser* pTUser = g_pCharMng->GetTamerUser();
								SAFE_POINTER_RETVAL( pTUser, false );
								pResistQuickSlot = pTUser->GetQuickSlot();
							}
							break;
						case Q_DIGIMON:
							{
								SAFE_POINTER_RETVAL( g_pCharMng, false );
								CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );
								SAFE_POINTER_RETVAL( pDUser, false );
								pResistQuickSlot = pDUser->GetQuickSlot();
							}
							break;
						}
					}
					nInsertReturn = cData_QuickSlot::QuickSlotToQuickSlot(	CURSOR_ST.GetIconSubInfo1(), CURSOR_ST.GetIconSubInfo2(), CURSOR_ST.GetIconSubInfo3(),
						m_nQuickSlotIndex, pResistQuickSlot, nSlotIndex );
				}
				break;
			default:
				nInsertReturn = 0;
				cPrintMsg::PrintMsg( 11014 );
				break;
			}

			switch( nInsertReturn )
			{
			case 0:				// 커서 내비두기
				return true;
			case 1:				// 커서 교체
				CURSOR_ST.ReleaseIcon();
				CURSOR_ST.SetIcon( CURSOR_ICON::CI_QUICKSLOT, i, 1, &m_IFIcon[ i ] );
				m_IFIcon[ i ].ReleaseState( true );
				return true;
			case 2:				// 커서 해제
				CURSOR_ST.ReleaseIcon();
				m_IFIcon[ i ].ReleaseState( true );	//2016-03-10-nova tamer 스킬에서 클릭이동시 안되는 문제
				return true;
			}
		}
	}
	return false;
}


void cQuickSlot::SetTargetObject( CsC_AvObject* pTarget )
{
	SAFE_POINTER_RET(this);

	m_pTargetObject = pTarget;
	assert_cs( m_pTargetObject != NULL );
}

int cQuickSlot::BoxImage_FromDist( int nSlotIndex, float fDist )
{
	CDigimonUser* pDigimon = (CDigimonUser*)m_pTargetObject;
	cSkill* pSkillMng = pDigimon->GetSkillMng();
	cResist::sCHAR_QUICKSLOT::sCHAR::sINFO* pResistQuickSlotInfo = &pDigimon->GetQuickSlot()->s_QuickSlotInfo[ nSlotIndex ];
	switch( pResistQuickSlotInfo->s_eType )
	{
	case cResist::NONE:
	case cResist::ITEM:		
		return -1;

	case cResist::SKILL:
		{
			CsSkill::sINFO* pFTSkillInfo = pSkillMng->GetSkill( pResistQuickSlotInfo->s_nID )->s_pFTSkill->GetInfo();
			if( pFTSkillInfo->s_nAttType == 2 )
			{
				if( fDist < pFTSkillInfo->s_fAttRange_MinDmg )
					return 0;
				else if( fDist < pFTSkillInfo->s_fAttRange_NorDmg )
					return 1;
				else if( fDist < pFTSkillInfo->s_fAttRange_MaxDmg )
					return 2;
				else if( fDist < pFTSkillInfo->s_fAttRange )
					return 0;
				else
					return 3;
			}
			else
			{
				return -1;
			}
		}
		break;
	}

	assert_cs( false );
	return -1;
}

void cQuickSlot::ResetDevice()
{	
	ResetDeviceScript();
	m_pInfoBtn->ResetDevice();
	m_pBox->ResetDevice();
	m_pBox2->ResetDevice();
}

bool cQuickSlot::OnMacroKey(const MSG& p_kMsg)
{
	cResist::sCHAR_QUICKSLOT::sCHAR* pResistQuickSlot;
	switch( m_eQ_Type )
	{
	case Q_TAMER:
		{
			switch(p_kMsg.message)
			{
			case WM_KEYUP:
			case WM_SYSKEYUP:
				{
					for( int i = 0 ; i < nLimit::QuickSlot ; ++i )
					{
						if( IsLockSlot( i ) )
							continue;

						if( GLOBALINPUT_ST.GetEqualSnKey(p_kMsg.wParam, DMKEY::QU_0_0+i) && GLOBALINPUT_ST.GetEqualModifier(DMKEY::QU_0_0+i) )
						{
							pResistQuickSlot = ( (CTamerUser*)m_pTargetObject )->GetQuickSlot();
							cData_QuickSlot::QuickSlotUse( m_pTargetObject, pResistQuickSlot, i );
							return true;
						}
					}
				}
				break;
			}				
		}
		break;

	case Q_DIGIMON:
		{
			switch(p_kMsg.message)
			{
			case WM_KEYUP:
			case WM_SYSKEYUP:
				{
					for( int i = 0 ; i < nLimit::QuickSlot ; ++i )
					{
						if( IsLockSlot( i ) )
							continue;

						if( GLOBALINPUT_ST.GetEqualSnKey(p_kMsg.wParam, DMKEY::QU_1_0+i) && GLOBALINPUT_ST.GetEqualModifier(DMKEY::QU_1_0+i) )
						{
							pResistQuickSlot = ( (CDigimonUser*)m_pTargetObject )->GetQuickSlot();
							cData_QuickSlot::QuickSlotUse( m_pTargetObject, pResistQuickSlot, i );
							return true;
						}
					}
				}
				break;
			}
		}
		break;

	case Q_ADDSLOT:
		{
			switch(p_kMsg.message)
			{
			case WM_KEYUP:
			case WM_SYSKEYUP:
				{
					for( int i = 0 ; i < nLimit::QuickSlot ; ++i )
					{
						if( IsLockSlot( i ) )
							continue;

						if( GLOBALINPUT_ST.GetEqualSnKey(p_kMsg.wParam, DMKEY::QU_2_0+i) && GLOBALINPUT_ST.GetEqualModifier(DMKEY::QU_2_0+i) )
						{
							pResistQuickSlot = &m_pAddQuickSlot->s_QuickSlotChar[ Q_ADDSLOT ];
							cData_QuickSlot::QuickSlotUse( g_pCharMng->GetTamerUser() , pResistQuickSlot, i );
							return true;
						}
					}
				}
				break;
			}	
		}
		break;
	}	
	return false;
}

cResist::sCHAR_QUICKSLOT::sCHAR::sINFO*
cQuickSlot::_CalQuickSlotConstant( int& nQuickSlotType, int& nID, int nIndex )
{
	cResist::sCHAR_QUICKSLOT::sCHAR* pResistQuickSlot = NULL;

	switch( m_eQ_Type )
	{
	case Q_TAMER:		pResistQuickSlot = ( (CTamerUser*)m_pTargetObject )->GetQuickSlot();		break;
	case Q_DIGIMON:	pResistQuickSlot = ( (CDigimonUser*)m_pTargetObject )->GetQuickSlot();	break;
	case Q_ADDSLOT:	pResistQuickSlot = &m_pAddQuickSlot->s_QuickSlotChar[ Q_ADDSLOT ];							break;
	}
	nQuickSlotType = pResistQuickSlot->s_QuickSlotInfo[ nIndex ].s_eType;
	nID = pResistQuickSlot->s_QuickSlotInfo[ nIndex ].s_nID;
	return &pResistQuickSlot->s_QuickSlotInfo[ nIndex ];
}

void cQuickSlot::_InitSlot_AddQuick( cResist::sCHAR_QUICKSLOT* pQuickSlot )
{
	m_pAddQuickSlot = csnew cResist::sCHAR_QUICKSLOT;
	m_pAddQuickSlot = pQuickSlot;
}

cBaseWindow::eMU_TYPE cQuickSlot::_Update_ForMouse_AddSlot()
{
	if( g_pResist->m_CenterBar.s_bInitial )		// 센터바 오픈 중일 때
	{
		//센터바 닫기 버튼
		switch( m_pCenterBarCloseBtn->Update_ForMouse() )
		{
		case cButton::ACTION_NONE:
			break;
		case cButton::ACTION_CLICK:
			{
				//닫기
				g_pResist->m_CenterBar.s_bInitial = false;
				return MUT_CLICK_FOCUS;
			}
			break;
		case cButton::ACTION_PRESS:
		case cButton::ACTION_DOWN:
			return MUT_CLICK_FOCUS;
			break;
		default:
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( CURSOR_ST.GetPos() + CsPoint( 20, 0 ), CsPoint::ZERO, UISTRING_TEXT( "QUICKSLOT_CENTER_BAR_OFF" ).c_str() );
			break;
		}
	}
	else					// 닫혀 있을 때 ( 오픈 버튼만 업데이트 )
	{
		//센터바 열기 버튼
		switch( m_pCenterBarOpenBtn->Update_ForMouse() )
		{
		case cButton::ACTION_NONE:
			break;
		case cButton::ACTION_CLICK:
			{
				//닫기
				g_pResist->m_CenterBar.s_bInitial = true;
				return MUT_CLICK_FOCUS;
			}
			break;
		case cButton::ACTION_PRESS:
		case cButton::ACTION_DOWN:
			return MUT_CLICK_FOCUS;
			break;
		default:
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( CURSOR_ST.GetPos() + CsPoint( 20, 0 ), CsPoint::ZERO, UISTRING_TEXT( "QUICKSLOT_CENTER_BAR_ON" ).c_str() );
			break;
		}
	}

	if( g_pResist->m_CenterBar.s_bIsOpenQuick )	// 추가 퀵슬롯 오픈 중일 때
	{
		//열기/닫기 버튼
		switch( m_pSlotCloseBtn->Update_ForMouse() )
		{
		case cButton::ACTION_NONE:
			break;
		case cButton::ACTION_CLICK:
			{
				g_pResist->m_CenterBar.SetQuickSlotOpenClose( false );
				//g_pResist->m_CenterBar.s_bIsOpenQuick = false;
				g_pGameIF->GetBuffInfo( IF_BUFF_SYSTEM )->SetAddSystempBuffPosY(0);
				return MUT_CLICK_FOCUS;
			}
			break;
		case cButton::ACTION_PRESS:
		case cButton::ACTION_DOWN:
			return MUT_CLICK_FOCUS;
			break;
		default:
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( CURSOR_ST.GetPos() + CsPoint( 20, 0 ), CsPoint::ZERO, UISTRING_TEXT( "QUICKSLOT_ADDITIONAL_SLOT_OFF" ).c_str() );
			break;
		}
	}
	else // 닫혀 있을 때 ( 오픈 버튼만 업데이트 )
	{
		//열기/닫기 버튼
		switch( m_pSlotOpenBtn->Update_ForMouse() )
		{
		case cButton::ACTION_NONE:
			break;
		case cButton::ACTION_CLICK:
			{
				g_pResist->m_CenterBar.SetQuickSlotOpenClose( true );
				//g_pResist->m_CenterBar.s_bIsOpenQuick = true;
				g_pGameIF->GetBuffInfo( IF_BUFF_SYSTEM )->SetAddSystempBuffPosY(44);
				return MUT_CLICK_FOCUS;
			}
			break;
		case cButton::ACTION_PRESS:
		case cButton::ACTION_DOWN:
			return MUT_CLICK_FOCUS;
			break;
		default:
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( CURSOR_ST.GetPos() + CsPoint( 20, 0 ), CsPoint::ZERO, UISTRING_TEXT( "QUICKSLOT_ADDITIONAL_SLOT_ON" ).c_str() );
			break;
		}

		return cBaseWindow::MUT_OUT_WINDOW;
	}

	//////////////////////////////////////////////////////////////////

	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;

	if( g_pResist->m_CenterBar.s_bIsOpenQuick )	// 추가 퀵슬롯 오픈 중일 때
	{
		CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
		CURSOR::eTYPE eCursor = CURSOR_ST.GetButtonState();

		switch( eCursor )
		{	
		case CURSOR::LBUTTON_DOWN:		
			{
				for( int i=0; i<nLimit::QuickSlot; ++i )
				{
					if( m_IFIcon[ i ].MouseLBtnClick( ptMouseLocalPos ) == false )
					{
						CURSOR_ST.SetButtonState(CURSOR::LBUTTON_PRESS);
						m_IFIcon[ i ].ReleaseState();
					}
				}
			}
			break;
		case CURSOR::RBUTTON_DOWN:		
			{
				for( int i=0; i<nLimit::QuickSlot; ++i )
				{
					if( m_IFIcon[ i ].MouseRBtnClick( ptMouseLocalPos ) == false )
					{
						CURSOR_ST.SetButtonState(CURSOR::RBUTTON_PRESS);
						m_IFIcon[ i ].ReleaseState();
					}

				}
			}
			break;
		case CURSOR::LBUTTON_PRESS:
			{
				OnMouseOutWindow();
				for( int i=0; i<nLimit::QuickSlot; ++i )
				{
					if( m_IFIcon[ i ].s_eState != sIFIcon::MOUSE_LCLICK )
					{
						if( m_IFIcon[ i ].MouseOn( ptMouseLocalPos ) == false )
							m_IFIcon[ i ].ReleaseState();
					}
				}
			}
			break;
		case CURSOR::RBUTTON_PRESS:
			{
				for( int i=0; i<nLimit::QuickSlot; ++i )
				{
					if( m_IFIcon[ i ].s_eState != sIFIcon::MOUSE_RCLICK )
					{
						if( m_IFIcon[ i ].MouseOn( ptMouseLocalPos ) == false )
							m_IFIcon[ i ].ReleaseState();
					}
				}
			}
			break;
		default:
			for( int i=0; i<nLimit::QuickSlot; ++i )
			{
				if( m_IFIcon[ i ].MouseOn( ptMouseLocalPos ) == false )				
					m_IFIcon[ i ].ReleaseState();
			}	
		}

		// 툴팁용

		cResist::sCHAR_QUICKSLOT::sCHAR::sINFO* pResistQuickSlotInfo;

		for( int i=0; i<nLimit::QuickSlot; ++i )
		{
			if( m_IFIcon[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == FALSE )
				continue;

			pResistQuickSlotInfo = &m_pAddQuickSlot->s_QuickSlotChar[ Q_ADDSLOT ].s_QuickSlotInfo[ i ];

			switch( pResistQuickSlotInfo->s_eType )
			{
			case cResist::NONE:
				break;
			case cResist::ITEM:
				{
					CsPoint pos = m_IFIcon[ i ].GetPos() + GetRootClient();
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_QSLOT_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, pResistQuickSlotInfo->s_nID, cBaseWindow::WT_QUICKSLOT );
				}				
				break;
			case cResist::SKILL:
				{
					CsPoint pos = m_IFIcon[ i ].GetPos() + GetRootClient();
#ifdef VERSION_USA
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_QSLOT_ICON_SIZE, 360, cTooltip::SKILL, pResistQuickSlotInfo->s_nID, Q_TAMER, cBaseWindow::WT_QUICKSLOT );
#else
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_QSLOT_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::SKILL, pResistQuickSlotInfo->s_nID, Q_TAMER, cBaseWindow::WT_QUICKSLOT );
#endif
				}
				break;
			default:
				assert_cs( false );
			}
		}
	}	

	return muReturn;
}

void cQuickSlot::_Render_AddSlot()
{
	if( g_pResist->m_CenterBar.s_bIsOpenQuick )//퀵슬롯 열려있을 때
	{
		m_pSlotCloseBtn->Render();

		if( g_pResist->m_CenterBar.s_bInitial )//센터바 열려 있을 때
			m_pCenterBarCloseBtn->Render();
		else
			m_pCenterBarOpenBtn->Render();

		m_pAddImage->Render();

		// 얼굴 이미지
		sCHAR_IMAGE* pCharImage = g_pCharMng->GetTamerUser()->GetCharImage();
		if( pCharImage != NULL )
			pCharImage->Render_S( CsPoint( m_pRoot->GetClient().x + 28, m_pRoot->GetClient().y ), CsPoint( 44, 43 ) );

		float fDist = 0.0f;
		if( ( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )&&( m_eQ_Type == Q_DIGIMON )&&g_pCharResMng->GetTargetMark() )
		{
			fDist = ( g_pCharResMng->GetTargetMark()->GetPos2D() - m_pTargetObject->GetPos2D() ).Length();
		}

		CsItem::sINFO* pFTItemInfo;

		cResist::sCHAR_QUICKSLOT::sCHAR::sINFO* pResistQuickSlotInfo = NULL;
		for( int i=0; i<nLimit::QuickSlot; ++i )
		{
			pResistQuickSlotInfo = &m_pAddQuickSlot->s_QuickSlotChar[ Q_ADDSLOT ].s_QuickSlotInfo[ i ];
			CsPoint pos = m_IFIcon[ i ].GetPos() + GetRootClient();

			if( fDist == 0.0f )
			{
				m_pBox->Render( pos - CsPoint( 3, 2 ) );
			}
			else
			{
				// 상대방의 몸사이즈 만큼 빼주자
				fDist -= g_pCharResMng->GetTargetMark()->GetAttackCharSize();
				int nImageIndex = BoxImage_FromDist( i, fDist );
				if( nImageIndex == -1 )
					m_pBox->Render( pos - CsPoint( 3, 2 ) );
				else
					m_pBox2->Render( pos - CsPoint( 3, 2 ), nImageIndex );
			}		

			switch( pResistQuickSlotInfo->s_eType )
			{
			case cResist::NONE:
				break;
			case cResist::ITEM:
				{
					cItemInfo Item;
					Item.m_nType = pResistQuickSlotInfo->s_nID;
					Item.m_nCount = pResistQuickSlotInfo->s_nCount;
					g_pIconMng->RenderItem( &Item, pos, true );

					pFTItemInfo = nsCsFileTable::g_pItemMng->GetItem( pResistQuickSlotInfo->s_nID )->GetInfo();

					// 쿨타임 중이라면 쿨타임 랜더링
					g_pIconMng->RenderTime( pos, ICON_HALF, pFTItemInfo );

					// 전투중에 못먹는 아이템이라면 레드 마스크 랜더링
					if( ( pFTItemInfo->s_nUseMode == 1 )&&( g_pCharMng->GetDigimonUser( 0 )->IsBattle() ) )
					{
						g_pIconMng->RenderMask( ICONMASK::IF_ICON_RED, pos, IF_QSLOT_ICON_SIZE );
					}
				}
				break;
			case cResist::SKILL:
				{
					cSkill* pSkillMng = g_pCharMng->GetTamerUser()->GetSkillMng();
					cSkill::sINFO* pSkillInfo =  pSkillMng->GetSkill( pResistQuickSlotInfo->s_nID );								
					if( pSkillInfo == NULL )	// 게임 종료 및 로그아웃 후 기간 만료로 스킬 삭제 되었을때 대비용
						break;
					g_pIconMng->RenderSkill( pSkillInfo, pos );

					// 쿨타임 중이라면 쿨타임 랜더링
					g_pIconMng->RenderTime( pos, ICON_HALF, &pSkillMng->GetSkill( pResistQuickSlotInfo->s_nID )->s_CoolTime );

					// ds 체크 ( ds 모자를때는 레드마스크 랜더링 )
					if( pSkillInfo->s_pFTSkill->GetInfo()->s_nUseDS > g_pCharMng->GetDigimonUser( 0 )->GetBaseStat()->GetDS() )
					{
						g_pIconMng->RenderMask( ICONMASK::IF_ICON_RED, pos, IF_QSLOT_ICON_SIZE );
					}
				}			
				break;
			default:
				assert_cs( false );
			}

			if( m_IFIcon[ i ].s_eState != sIFIcon::NONE )
			{
				m_IFIcon[ i ].RenderMask( pos, IF_QSLOT_ICON_SIZE );
			}
		}
		RenderScript();
		EndRenderScript();
		for( int i = 0 ; i < nLimit::QuickSlot ; i++ )
			m_pMacroText[ i ]->Render();
	}
	else	//퀵슬롯 닫혀있을 때
	{
		m_pSlotOpenBtn->Render();

		if( g_pResist->m_CenterBar.s_bInitial )//센터바 열려 있을 때
			m_pCenterBarCloseBtn->Render();
		else
			m_pCenterBarOpenBtn->Render();
		RenderScript();
		EndRenderScript();
	}
}

void cQuickSlot::_DeleteSkill_AddSlot( int nType )	//	( Type : RTTI_테이머 or RTTI_디지몬 )
{
	for( int i = 0 ; i < nLimit::QuickSlot ; i++ )
	{
		switch( nType )
		{
		case RTTI_TAMER_USER:	//	테이머 변경 시
			{
				if( m_pAddQuickSlot->s_QuickSlotChar[ Q_ADDSLOT ].s_QuickSlotInfo[ i ].s_eType == cResist::SKILL )	// 등록되 있는놈이 테이머 스킬이면 
				{
					//스킬이면 빼주자
					m_pAddQuickSlot->s_QuickSlotChar[ Q_ADDSLOT ].s_QuickSlotInfo[ i ].s_eType = cResist::NONE;
					m_pAddQuickSlot->s_QuickSlotChar[ Q_ADDSLOT ].s_QuickSlotInfo[ i ].s_nID = (uint)-1;
				}
			}
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void cQuickSlot::LockSlot(int const& nSlotIndex)
{
	std::list< int >::iterator it = m_lLockSlot.begin();
	for( ; it != m_lLockSlot.end(); ++it )
	{
		if( *it == nSlotIndex )
			return;
	}

	m_lLockSlot.push_back( nSlotIndex );
}

void cQuickSlot::UnlockSlot(int const& nSlotIndex)
{
	std::list< int >::iterator it = m_lLockSlot.begin();
	for( ; it != m_lLockSlot.end(); )
	{
		if( *it == nSlotIndex )
			it = m_lLockSlot.erase( it );
		else
			++it;
	}
}

void cQuickSlot::LockSlotAll()
{
	for( int i = 0; i < nLimit::QuickSlot; ++i )
		m_lLockSlot.push_back( i );
}

void cQuickSlot::UnlockSlotAll()
{
	m_lLockSlot.clear();
}

bool cQuickSlot::IsLockSlot(int const& nSlotIndex) const
{
	std::list< int >::const_iterator it = m_lLockSlot.begin();
	for( ; it != m_lLockSlot.end(); ++it )
	{
		if( *it == nSlotIndex )
			return true;
	}

	return false;
}

int cQuickSlot::GetSlotIndex(DWORD const& dwSlotType, DWORD const& dwSlotValue)
{
	SAFE_POINTER_RETVAL( m_pTargetObject, -1 );
	cResist::sCHAR_QUICKSLOT::sCHAR* pResistQuickSlot = NULL;
	switch( m_eQ_Type )
	{
	case Q_TAMER:
		pResistQuickSlot = ( (CTamerUser*)m_pTargetObject )->GetQuickSlot();
		break;
	case Q_DIGIMON:
		pResistQuickSlot = ( (CDigimonUser*)m_pTargetObject )->GetQuickSlot();
		break;
	}

	SAFE_POINTER_RETVAL( pResistQuickSlot, -1 );

	for( int i = 0; i < nLimit::QuickSlot; ++i )
	{
		if( pResistQuickSlot->s_QuickSlotInfo[i].s_eType != dwSlotType )
			continue;

		if( pResistQuickSlot->s_QuickSlotInfo[i].s_nID == dwSlotValue )
			return i;
	}

	return -1;
}

CsPoint cQuickSlot::GetSlotPos(int const& nSlotIndex)
{
	return ( m_IFIcon[ nSlotIndex ].GetPos() + GetRootClient() - CsPoint( 3, 2 ) );
}
