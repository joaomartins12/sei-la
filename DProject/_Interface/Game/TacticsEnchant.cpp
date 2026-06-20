
#include "stdafx.h"
#include "TacticsEnchant.h"

#define IF_ENCHANT_ICON_SIZE	CsPoint( 32, 32 )

#define MAX_ENCHANT 15		// 스탯 별 강화 수치 15


cEnchantTactics::cEnchantTactics():m_pHpStatimg(NULL)
{
	m_pMoney = NULL;
	m_pOverMask = NULL;
	m_pNonSelectMask = NULL;
	m_pSelectMask = NULL;	
	m_pRadio = NULL;
	m_pSprEnchant = NULL;
	m_pSprWeak = NULL;

	m_pDigiClonCancel = NULL;
	m_pSubItemCancel = NULL;

	m_pText = NULL;
	m_pExplain = NULL;
}

cEnchantTactics::~cEnchantTactics()
{

}

void cEnchantTactics::Destroy()
{
	cBaseWindow::Delete();
	if( g_pDatsCenter )
		g_pDatsCenter->EnchantEnd();
}

void cEnchantTactics::DeleteResource()
{
	DeleteScript();		

	SAFE_NIDELETE( m_pMoney );
	SAFE_NIDELETE( m_pOverMask );
	SAFE_NIDELETE( m_pNonSelectMask );
	SAFE_NIDELETE( m_pSelectMask );	
	SAFE_NIDELETE( m_pRadio );
	SAFE_NIDELETE( m_pSprEnchant );
	SAFE_NIDELETE( m_pSprWeak );
	SAFE_NIDELETE( m_pDigiClonCancel );
	SAFE_NIDELETE( m_pSubItemCancel );
	SAFE_NIDELETE( m_pText );
	SAFE_NIDELETE( m_pExplain );

	if( m_DigicloneItem && m_DigicloneItem->GetType() != 0 )
	{		
		g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nDigicloneSlot ) );
	}

	if( m_SubItem && m_SubItem->GetType() != 0 )
	{	
		g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nSubItemSlot ) );
	}	
}

void cEnchantTactics::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;
	InitScript( "EnchantTactics\\Win.tga", CsPoint( 150, 65 ), CsPoint( 460, 262 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );
	AddTitle( UISTRING_TEXT( "ENCHANTTACTICS_TITLE_DIGIMON_ENCHANT" ).c_str(), CsPoint( 0, 7 ) );	

	m_pSprEnchant = NiNew cSprite;
	m_pSprEnchant->Init( m_pRoot, CsPoint( 20, 75 ), CsPoint( 207, 113 ), "EnchantTactics\\Enchant_Win.tga", false );

	m_pSprWeak = NiNew cSprite;
	m_pSprWeak->Init( m_pRoot, CsPoint( 20, 75 ), CsPoint( 207, 113 ), "EnchantTactics\\Weak_Win.tga", false );

	CsSIZE sizeRC( 63, 20 );
	CsPoint startRC;
	for( int i=0; i < MAX_ENCHANT_STAT_COUNT; ++i )
	{		
		startRC.x = 257 + ( 101 * ( i % 2 ) );
		startRC.y = 86 + ( 34 * ( i / 2 ) );
		m_rcRect[ i ] = CsRect( startRC, sizeRC );				
	}

#ifdef SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507
	m_pHpStatimg = AddSprite( CsPoint( 257, 156 ), CsPoint( 63, 16 ), "EnchantTactics\\Enchant_HP.tga" );
#endif
	m_pCancelButton = AddButton( CsPoint( 433, 13 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	

	// 디지클론 장착 영역
	m_rcRectItem[ 0 ] = CsRect( CsPoint( 54, 98 ), CsPoint( 53, 98 ) + IF_ENCHANT_ICON_SIZE ); 
	m_nDigicloneSlot = cData_Inven::INVALIDE_INVEN_INDEX;

	// 보조제 영역
	m_rcRectItem[ 1 ] = CsRect( CsPoint( 171, 98 ), CsPoint( 170 , 98 ) + IF_ENCHANT_ICON_SIZE );
	m_nSubItemSlot = cData_Inven::INVALIDE_INVEN_INDEX;	

	// 약화 아이템 영역
	m_rcRectItem[ 2 ] = CsRect( CsPoint( 107, 98 ), CsPoint( 107 , 98 ) + IF_ENCHANT_ICON_SIZE );
	m_nSubItemSlot = cData_Inven::INVALIDE_INVEN_INDEX;

#ifdef SIMPLE_TOOLTIP
	m_rcReqCost.SetRect(CsPoint( 55, 98 + 32 + 20 ), CsSIZE( 160, 24 ));
#endif

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( g_pCharMng->GetDigimonUser( 0 )->GetName() );

	AddText( &ti, CsPoint( 337, 46 ) );		

	ti.s_eTextAlign = DT_LEFT;

	m_pMoney = NiNew cMoneyIF;
	m_pMoney->Init( cMoneyIF::DISP_FULL, sMONEY( (u8)0 ), false );

	m_pOverMask = NiNew cSprite;
	m_pOverMask->Init( NULL, CsPoint( 0, 0 ), CsPoint( 63, 20 ), "EnchantTactics\\Power_Mask_02.tga", false );
	m_pOverMask->GetAlphaProp()->SetDestBlendMode( NiAlphaProperty::ALPHA_ONE );

	m_pNonSelectMask = NiNew cSprite;
	m_pNonSelectMask->Init( NULL, CsPoint( 0, 0 ), CsPoint( 63, 20 ), "EnchantTactics\\Power_Mask_01.tga", false );	

	m_pSelectMask = NiNew cSprite;
	m_pSelectMask->Init( NULL, CsPoint( 0, 0 ), CsPoint( 63, 20 ), "EnchantTactics\\Power_Mask_03.tga", false );	

	m_pEnchantButton = AddButton( CsPoint( 355, 220 ), CsPoint( 75, 32 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "ENCHANTTACTICS_BTN_ENCHANT" ).c_str(), cWindow::SD_Ba2 );
	if( m_pEnchantButton )
		m_pEnchantButton->SetControlName("EnchantStartBtn");
	m_pResistButton = AddButton( CsPoint( 250, 220 ), CsPoint( 85, 32 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "ENCHANTTACTICS_BTN_DIGICLONE" ).c_str(), cWindow::SD_Ba2 );

	std::wstring wsSuccess;
	DmCS::StringFn::Format( wsSuccess, L"%s :", UISTRING_TEXT( "ENCHANTTACTICS_SUCCESS_ABILITY" ).c_str() );
	ti.SetText( wsSuccess.c_str() );
	m_pText = NiNew cText;
	m_pText->Init( m_pRoot, CsPoint( 240, 180 ), &ti, false );
	ti.SetText( L" " );
	ti.s_Color = FONT_YELLOW;
	m_pExplain = NiNew cText; 
	m_pExplain->Init( m_pRoot, CsPoint( 343, 180 ), &ti, false );

	m_pRadio = NiNew cRadioButton;
	ti.s_Color = FONT_WHITE;
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( UISTRING_TEXT( "ENCHANTTACTICS_BTN_ENCHANT" ).c_str() );
	m_pRadio->Init( m_pRoot, CsPoint( 45, 50 ), CsPoint( 75, 21 ), cCheckBox::IMAGE_RADIO_3, &ti, false );
	ti.SetText( UISTRING_TEXT( "ENCHANTTACTICS_BTN_RESET" ).c_str() );
	m_pRadio->AddNode( CsPoint( 45 + 76, 50 ), CsPoint( 75, 21 ), cCheckBox::IMAGE_RADIO_3, &ti );
	m_pRadio->SetCheckIndex( 0 );

	m_pDigiClonCancel = NiNew cButton;
	m_pDigiClonCancel->Init( m_pRoot, CsPoint( 86, 132 ), CsPoint( 16, 16 ), "System\\Ch_Close.tga", false );	
	m_pDigiClonCancel->SetTexToken( CsPoint( 0, 16 ) );

	m_pSubItemCancel = NiNew cButton;
	m_pSubItemCancel->Init( m_pRoot, CsPoint( 204, 132 ), CsPoint( 16, 16 ), "System\\Ch_Close.tga", false );
	m_pSubItemCancel->SetTexToken( CsPoint( 0, 16 ) );

	m_DigicloneItem = NULL;
	m_SubItem = NULL;	


	_InitEnchant();
}

void cEnchantTactics::_InitEnchant()
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );	

	m_nSelectAbility = -1;
	m_nOverAbility = -1;
	m_bWaitRecvServer = false;		

	g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nDigicloneSlot ) );
	m_DigicloneItem = NULL;
	m_nDigicloneSlot = cData_Inven::INVALIDE_INVEN_INDEX;		

	m_pMoney->SetMoney( (u8)0 );	

	g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nSubItemSlot ) );
	m_SubItem = NULL;
	m_nSubItemSlot = cData_Inven::INVALIDE_INVEN_INDEX;		

	_SetResistButtonText();

	m_pEnchantButton->SetText( UISTRING_TEXT( "ENCHANTTACTICS_BTN_ENCHANT" ).c_str() );	
	m_pEnchantButton->SetControlName("EnchantStartBtn");
	m_pSubItemCancel->SetPos( CsPoint( 204, 132 ) );

}

void cEnchantTactics::_InitWeak()
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );	

	m_nSelectAbility = -1;
	m_nOverAbility = -1;
	m_bWaitRecvServer = false;

	g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nDigicloneSlot ) );
	m_DigicloneItem = NULL;
	m_nDigicloneSlot = cData_Inven::INVALIDE_INVEN_INDEX;		

	m_pMoney->SetMoney( (u8)0 );	

	g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nSubItemSlot ) );
	m_SubItem = NULL;
	m_nSubItemSlot = cData_Inven::INVALIDE_INVEN_INDEX;		

	_SetResistButtonText();

	m_pEnchantButton->SetText( UISTRING_TEXT( "ENCHANTTACTICS_BTN_RESET" ).c_str() );
	m_pEnchantButton->SetControlName("EnchantResetBtn");
	m_pSubItemCancel->SetPos( CsPoint( 141, 132 ) );

}

void cEnchantTactics::Update(float const& fDeltaTime)
{	
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE
cEnchantTactics::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn;	

	m_nOverAbility = -1;

	muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;	

	// 나가기 버튼
	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		Close();
		return muReturn;
	default:
		return muReturn;
	}

	// 탭 변경
	if( m_pRadio->Update_ForMouse() == cRadioButton::ACTION_CLICK )
	{		
		switch( m_pRadio->GetCheckIndex() )
		{
		case 0:
			_InitEnchant();
			break;
		case 1:
			_InitWeak();
			break;
		}
	}	

	// 강화 버튼
	switch( m_pEnchantButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		{
			CDigimon::sENCHENT_STAT* pEStat = ((CDigimon*)g_pCharMng->GetDigimonUser( 0 ))->GetEnchantStat();						

			if( m_nSelectAbility == -1 ) //m_pRadio->GetCheckIndex()에 따라 Message구분
			{
				int iCommentID = 30409;
#ifdef VERSION_KOR
				switch( m_pRadio->GetCheckIndex() )
				{
				case 0:
					iCommentID = 30409;
					break;
				case 1:
					iCommentID = 30425;
					break;
				}
#endif
				cPrintMsg::PrintMsg( iCommentID );
			}		
			else
			{
				switch( m_pRadio->GetCheckIndex() )
				{
				case 0:
					{	
						if( _GetSelectStat_EnchantLV() + 1 > MAX_ENCHANT )
						{
							cPrintMsg::PrintMsg( 30407 );								
						}						

						if( m_nDigicloneSlot == cData_Inven::INVALIDE_INVEN_INDEX ) 
						{
							cPrintMsg::PrintMsg( 30408 );
						}
						else if( g_pDataMng->GetInven()->GetMoney() < m_pMoney->GetMoney() )			
						{
							cPrintMsg::PrintMsg( 11003 );							
						}
						else
							g_pDatsCenter->StartSelectCard();	
					}					
					break;
				case 1:
					{
						if( _GetSelectStat_EnchantLV() < 1 )
						{
							cPrintMsg::PrintMsg( 30428 );
							return muReturn;
						}

						if( m_nSubItemSlot == cData_Inven::INVALIDE_INVEN_INDEX ) 
						{
							cPrintMsg::PrintMsg( 30432 );
						}
						else
						{							
							if( nsCsFileTable::g_pItemMng->GetItem( m_SubItem->GetType() )->GetInfo()->s_nType_S == 1 )
							{								
								switch( m_nSelectAbility )
								{
								case 0: cPrintMsg::PrintMsg( 30426, L"AT" );										break;
								case 1: cPrintMsg::PrintMsg( 30426, L"BL" );										break;
								case 2: cPrintMsg::PrintMsg( 30426, L"CT" );										break;
								case 3: cPrintMsg::PrintMsg( 30426, L"EV" );										break;
								case 4: cPrintMsg::PrintMsg( 30426, L"HP" );										break;
								}
							}
							else
							{
								switch( m_nSelectAbility )
								{
								case 0: cPrintMsg::PrintMsg( 30429, L"AT" );										break;
								case 1: cPrintMsg::PrintMsg( 30429, L"BL" );										break;
								case 2: cPrintMsg::PrintMsg( 30429, L"CT" );										break;
								case 3: cPrintMsg::PrintMsg( 30429, L"EV" );										break;
								case 4: cPrintMsg::PrintMsg( 30429, L"HP" );										break;
								}															
							}							
						}							
					}						
					break;
				}				
			}
		}		
		return muReturn;
#ifdef SIMPLE_TOOLTIP
	case cButton::ACTION_ON:		
		switch( m_pRadio->GetCheckIndex() )
		{
		case 0:
			{		
				CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1025 );
			}
			break;
		case 1:
			{		

				CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1028 );
			}
			break;
		}


		return muReturn;
#endif
	default:
		return muReturn;
	}

	//  등록 버튼
	switch( m_pResistButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:	
		SetAutoResist();
		return muReturn;
#ifdef SIMPLE_TOOLTIP
	case cButton::ACTION_ON:

		switch( m_pRadio->GetCheckIndex() )
		{
		case 0:
			{			

				if( m_DigicloneItem == NULL )
				{
					CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1024 );
				}
				else
				{
					CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1029 );
				}
			}
			break;
		case 1:
			{


				CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1027 );
			}
			break;
		}

		return muReturn;
#endif
	default:
		return muReturn;
	}	

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );		
	for( int i=0; i < MAX_ENCHANT_STAT_COUNT; ++i )
	{
		if( m_rcRect[ i ].PtInRect( ptMouseLocalPos ) == FALSE )
			continue;

#ifdef SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507
		int nStat = GetSelAbilityType( i );
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( m_rcRect[ i ].GetPos() + GetRootClient(), CsPoint( 64, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ENCHANT_EXPLAIN, nStat );
#else
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( m_rcRect[ i ].GetPos() + GetRootClient(), CsPoint( 64, 32 ), TOOLTIP_MAX_SIZE, cTooltip::STAT_EXPLAIN, i );
#endif
		if( i == m_nSelectAbility )
			continue;

		m_nOverAbility = i;				
	}

	// 장착아이템 빼기 버튼
	switch( m_pSubItemCancel->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		if( m_nSubItemSlot != cData_Inven::INVALIDE_INVEN_INDEX )
		{
			g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nSubItemSlot ) );
			m_SubItem = NULL;
			m_nSubItemSlot = cData_Inven::INVALIDE_INVEN_INDEX;		

			_SetResistButtonText();
		}
		return muReturn;
	default:
		return muReturn;
	}

	switch( m_pRadio->GetCheckIndex() )
	{
	case 0:
		{
			// 장착아이템 빼기 버튼
			switch( m_pDigiClonCancel->Update_ForMouse() )
			{
			case cButton::ACTION_NONE:
				break;
			case cButton::ACTION_CLICK:
				if( m_nDigicloneSlot != cData_Inven::INVALIDE_INVEN_INDEX )
				{
					g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nDigicloneSlot ) );
					m_DigicloneItem = NULL;
					m_nDigicloneSlot = cData_Inven::INVALIDE_INVEN_INDEX;		

					m_pMoney->SetMoney( (u8)0 );
					_SetResistButtonText();
					SetNextEnchantText();
				}
				return muReturn;
			default:
				return muReturn;
			}

			if( m_rcRectItem[ 0 ].PtInRect( ptMouseLocalPos ) == TRUE )
			{
				if( m_DigicloneItem )
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( m_rcRectItem[ 0 ].GetPos() + GetRootClient(), IF_ENCHANT_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, m_DigicloneItem->GetType(), cBaseWindow::WT_STORE );
#ifdef SIMPLE_TOOLTIP
				else
				{
					CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1021 );
				}
#endif
			}			

			if( m_rcRectItem[ 1 ].PtInRect( ptMouseLocalPos ) == TRUE )
			{
				if( m_SubItem )
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( m_rcRectItem[ 1 ].GetPos() + GetRootClient(), IF_ENCHANT_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, m_SubItem->GetType(), cBaseWindow::WT_STORE );
#ifdef SIMPLE_TOOLTIP
				else
				{
					CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1022 );
				}
#endif
			}
#ifdef SIMPLE_TOOLTIP
			if(m_rcReqCost.PtInRect( ptMouseLocalPos) )
			{
				CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1023 );
			}		
#endif	
		}
		break;
	case 1:
		{
			if( m_rcRectItem[ 2 ].PtInRect( ptMouseLocalPos ) == TRUE )
			{
				if( m_SubItem )
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( m_rcRectItem[ 2 ].GetPos() + GetRootClient(), IF_ENCHANT_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, m_SubItem->GetType(), cBaseWindow::WT_STORE );
#ifdef SIMPLE_TOOLTIP
				else
				{
					CsPoint pos = CURSOR_ST.GetPos() + CsPoint( 20, 0 );
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1026 );
				}
#endif
			}			
		}
		break;
	}		

	_UpdateMoveWindow_ForMouse();		

	return muReturn;
}

void cEnchantTactics::Render()
{
	RenderScript();		
	switch( m_pRadio->GetCheckIndex() )
	{
	case 0:
		m_pSprEnchant->Render();		
		break;
	case 1:
		m_pSprWeak->Render();		
		break;
	}

	if( m_pRadio )	
		m_pRadio->Render();

	m_pMoney->Render( GetRootClient() + CsPoint( 52, 157 ) );

	for( int i = 0; i < MAX_ENCHANT_STAT_COUNT; i++ )
	{
		if( m_nSelectAbility == -1 )
			break;

		if( m_nSelectAbility == i || m_nOverAbility == i )
			continue;		

		m_pNonSelectMask->Render( m_rcRect[ i ].GetPos() + GetRootClient() );
	}	

	if( m_nOverAbility != -1 )
		m_pOverMask->Render( m_rcRect[ m_nOverAbility ].GetPos() + GetRootClient() );

	if( m_nSelectAbility != -1 )
		m_pSelectMask->Render( m_rcRect[ m_nSelectAbility ].GetPos() + GetRootClient() );	

	switch( m_pRadio->GetCheckIndex() )
	{
	case 0:		
		{
			if( m_nSelectAbility != -1 )
			{
				m_pText->Render();	
				m_pExplain->Render();
			}

			if( m_DigicloneItem != NULL )
				g_pIconMng->RenderItem( m_DigicloneItem, GetRootClient() + m_rcRectItem[ 0 ].GetPos(), false );		
			if( m_SubItem != NULL )
				g_pIconMng->RenderItem( m_SubItem, GetRootClient() + m_rcRectItem[ 1 ].GetPos(), false );	

			m_pDigiClonCancel->Render();
			m_pSubItemCancel->Render();
		}
		break;
	case 1:		
		{
			if( m_SubItem != NULL )
				g_pIconMng->RenderItem( m_SubItem, GetRootClient() + m_rcRectItem[ 2 ].GetPos(), false );	

			m_pSubItemCancel->Render();
		}
		break;
	}

	CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
	CDigimon::sENCHENT_STAT* pEStat = ( (CDigimon*)pDigimonUser )->GetEnchantStat();

	CsPoint RootPos;
	RootPos = GetRootClient() - CsPoint( 23, -2 );
	for( int n = 0; n < MAX_ENCHANT_STAT_COUNT; ++n )
	{
		int nValue = 0;
		switch( n )
		{
		case 0:	nValue = pEStat->GetEnchantLv( ET_AT );		break;
		case 1: nValue = pEStat->GetEnchantLv( ET_BL );		break;
		case 2: nValue = pEStat->GetEnchantLv( ET_CR );		break;
		case 3: nValue = pEStat->GetEnchantLv( ET_EV );		break;
		case 4: nValue = pEStat->GetEnchantLv( ET_HP );		break;
		}
		g_pIconMng->RenderIcon( ICONITEM::Enchant_Lv, m_rcRect[ n ].GetPos() + RootPos, nValue );
	}
		
	EndRenderScript();
}

void cEnchantTactics::OnLButtonUp( CsPoint pos )
{
	CsPoint ptLocal = MousePosToWindowPos( CURSOR_ST.GetPos() );

	for( int i=0; i<MAX_ENCHANT_STAT_COUNT; ++i )
	{
		if( i == m_nSelectAbility )
			continue;

		if( m_rcRect[ i ].PtInRect( ptLocal ) == TRUE )
		{
			m_nSelectAbility = i;

			switch( m_pRadio->GetCheckIndex() )
			{
			case 0:
				{					
					if( m_DigicloneItem )
					{				
						g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nDigicloneSlot ) );
					}

					m_DigicloneItem = NULL;
					m_nDigicloneSlot = cData_Inven::INVALIDE_INVEN_INDEX;

					SetNextEnchantText();
				}
				break;
			case 1:
				break;
			}
			break;
		}
	}	

	//[DM_KOR] #402
	_SetResistButtonText();

}

bool cEnchantTactics::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	CsPoint ptLocal = MousePosToWindowPos( CURSOR_ST.GetPos() );

	switch( eIconType )
	{
	case CURSOR_ICON::CI_INVEN:
		{
			switch( m_pRadio->GetCheckIndex() )
			{
			case 0:
				{
					if( m_rcRectItem[ 0 ].PtInRect( ptLocal ) == TRUE )
					{
						SetResist( nIconSlot );
					}			

					if( m_rcRectItem[ 1 ].PtInRect( ptLocal ) == TRUE )
					{
						SetResist( nIconSlot );
					}			
				}
				break;
			case 1:
				{
					if( m_rcRectItem[ 2 ].PtInRect( ptLocal ) == TRUE )
					{
						SetResist( nIconSlot );
					}			
				}
				break;
			}

			CURSOR_ST.ReleaseIcon();
			return true;
		}
		break;
	}

	return false;
}

void cEnchantTactics::ResetDevice()
{	
	ResetDeviceScript();

	m_pMoney->ResetDevice();
	m_pNonSelectMask->ResetDevice();
	m_pSelectMask->ResetDevice();
	m_pOverMask->ResetDevice();	
	if( m_pSubItemCancel )
		m_pSubItemCancel->ResetDevice();
	if( m_pDigiClonCancel )
		m_pDigiClonCancel->ResetDevice();
	if( m_pText )
		m_pText->ResetDevice();	
	if( m_pExplain )
		m_pExplain->ResetDevice();	

	if( m_pRadio )	
		m_pRadio->ResetDevice();
	if( m_pSprEnchant )
		m_pSprEnchant->ResetDevice();
	if( m_pSprWeak )
		m_pSprWeak->ResetDevice();
}

void cEnchantTactics::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();

	m_pMoney->ResetDevice();
	m_pNonSelectMask->ResetDevice();
	m_pSelectMask->ResetDevice();
	m_pOverMask->ResetDevice();	
	if( m_pSubItemCancel )
		m_pSubItemCancel->ResetDevice();
	if( m_pDigiClonCancel )
		m_pDigiClonCancel->ResetDevice();
	if( m_pText )
		m_pText->ResetDevice();	
	if( m_pExplain )
		m_pExplain->ResetDevice();	

	if( m_pRadio )	
		m_pRadio->ResetDevice();
	if( m_pSprEnchant )
		m_pSprEnchant->ResetDevice();
	if( m_pSprWeak )
		m_pSprWeak->ResetDevice();
}

bool cEnchantTactics::_IsDigiClonInsert()
{
	if( m_nSelectAbility == -1 )
	{
		cPrintMsg::PrintMsg( 30409 );
		return false;
	}

	CDigimon* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );

	CDigimon::sENCHENT_STAT* pEState = pDigimonUser->GetEnchantStat();	
	if( pEState->GetEnchantLevel() == nLimit::MaxInchantLevel )
	{
		cPrintMsg::PrintMsg( 30407 );
		return false;
	}	

	int nEnchantLv = _GetSelectStat_EnchantLV();

	if( nEnchantLv + 1 > MAX_ENCHANT )
	{
		cPrintMsg::PrintMsg( 30407 );
		return false;
	}

	cData_PostLoad::sDATA* pkDigimonData = g_pDataMng->GetPostLoad()->GetDigimonData();
	if( NULL == pkDigimonData )
		return false;

	int nHatchLv = pkDigimonData->s_HatchLevel;
	int nStatIdx = GetSelAbilityType( m_nSelectAbility );
	if( !nsCsFileTable::g_pTacticsMng->IsEnableEnchantHatchLv( nStatIdx + 1, nEnchantLv + 1, nHatchLv ) )
	{
		cPrintMsg::PrintMsg( 30398 );//초월을 완료한 디지몬만이 해당 강화를 진행할 수 있습니다
		return false;
	}
	return true;
}

int cEnchantTactics::_GetDigiClonType( int nNextEnchantLV )
{
	m_nUsableClone.clear();

	MAP_ENCHANT_ITEM_INFO const& enchantItems = nsCsFileTable::g_pTacticsMng->GetEnchantItemData();
	MAP_ENCHANT_ITEM_INFO_CIT it = enchantItems.begin();
	for( ; it != enchantItems.end(); ++it )
	{
		if( it->second.nLowLevel > nNextEnchantLV )
			continue;

		if( it->second.nHighLevel < nNextEnchantLV )
			continue;

		m_nUsableClone.push_back(it->first);
	}

	return (int)m_nUsableClone.size();
}

int cEnchantTactics::_GetSelectStat_EnchantLV()
{
	CDigimon::sENCHENT_STAT* pEState = ( (CDigimon*)g_pCharMng->GetDigimonUser( 0 ) )->GetEnchantStat();	
	switch( m_nSelectAbility )
	{
	case 0:		return pEState->GetEnchantLv( ET_AT );			
	case 1:		return pEState->GetEnchantLv( ET_BL );			
	case 2:		return pEState->GetEnchantLv( ET_CR );			
	case 3:		return pEState->GetEnchantLv( ET_EV );			
	case 4:		return pEState->GetEnchantLv( ET_HP );
	default:
		assert_csm( false, _T( "잘못된 능력치 선택" ) );
	}

	return -1;
}

void cEnchantTactics::SetAutoResist()
{
	if( m_bWaitRecvServer == true )
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}

	int nSlot = cData_Inven::INVALIDE_INVEN_INDEX;
	switch( m_pRadio->GetCheckIndex() )
	{
	case 0:
		{
			if( m_DigicloneItem != NULL && m_SubItem != NULL )
				return;

			if( m_DigicloneItem == NULL )
			{	
				if( _IsDigiClonInsert() == true )
				{
					int nCnt = _GetDigiClonType( _GetSelectStat_EnchantLV() + 1 );

					for( int i = 0 ; i < nCnt ; i++ )
					{
						nSlot = g_pDataMng->GetInven()->GetFirstSlot_Item_TypeLS( m_nUsableClone[ i ] / 100, m_nUsableClone[ i ] % 100 );

						if( nSlot != cData_Inven::INVALIDE_INVEN_INDEX )
							break;
					}
					if( nSlot == cData_Inven::INVALIDE_INVEN_INDEX )
						cPrintMsg::PrintMsg( 30399 );
				}
			}
			else
			{
				nSlot = g_pDataMng->GetInven()->GetFirstSlot_ItemType( nItem::TacticsEnchantSub, 0 );

				//if( nSlot == cData_Inven::INVALIDE_INVEN_INDEX )
				//	cPrintMsg::PrintMsg( 30399 );
			}

			if( nSlot != cData_Inven::INVALIDE_INVEN_INDEX )
			{
				SetResist( nSlot );	
			}	
		}
		break;
	case 1:
		{
			if( m_SubItem != NULL )
			{
				CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_SubItem->GetType() )->GetInfo();			

				if( pFTItem->s_nType_S == 1 )
				{
					nSlot = g_pDataMng->GetInven()->GetFirstSlot_ItemType( nItem::TacticsEnchantReset, 2 );	
				}
				if( pFTItem->s_nType_S == 2 )
				{
					nSlot = g_pDataMng->GetInven()->GetFirstSlot_ItemType( nItem::TacticsEnchantReset, 1 );	
				}
			}
			else
			{
				nSlot = g_pDataMng->GetInven()->GetFirstSlot_ItemType( nItem::TacticsEnchantReset, 1 );
				if( nSlot == cData_Inven::INVALIDE_INVEN_INDEX )
					nSlot = g_pDataMng->GetInven()->GetFirstSlot_ItemType( nItem::TacticsEnchantReset, 2 );

				if( nSlot == cData_Inven::INVALIDE_INVEN_INDEX )
					cPrintMsg::PrintMsg( 30430 );
			}									

			if( nSlot != cData_Inven::INVALIDE_INVEN_INDEX )
			{
				SetResist( nSlot );	
			}	
		}
		break;
	}

}

void cEnchantTactics::_SetResistButtonText()
{
	switch( m_pRadio->GetCheckIndex() )
	{
	case 0:
		{	//[DM_KOR] #402
			if( m_DigicloneItem == NULL)
				m_pResistButton->SetText( UISTRING_TEXT( "ENCHANTTACTICS_BTN_DIGICLONE" ).c_str() );
			else
				m_pResistButton->SetText( UISTRING_TEXT( "ENCHANTTACTICS_BTN_BACKUP_DISK" ).c_str() );
		}
		break;
	case 1:
		{
			m_pResistButton->SetText( UISTRING_TEXT( "ENCHANTTACTICS_BTN_RESET_ITEM" ).c_str() );
		}
		break;
	}
}

void cEnchantTactics::SetNextEnchantText()
{
	bool bEnchant = true;
	int nNextEnchantLv = _GetSelectStat_EnchantLV() + 1;
	if( nNextEnchantLv > MAX_ENCHANT )
		bEnchant = false;

	if( bEnchant == true )
	{
		TCHAR sz[ 32 ];
		CDigimonUser::sUSER_STAT* pUserStat = (CDigimonUser::sUSER_STAT*)g_pCharMng->GetDigimonUser( 0 )->GetBaseStat();
		FMDigimon::sDINFO info;
		info.s_nDigimonID		= g_pCharMng->GetDigimonUser( 0 )->GetFTID();
		info.s_nLevel			= pUserStat->GetLevel();
		info.s_fGameOrgScale	= g_pCharMng->GetDigimonUser( 0 )->GetOrgGameScale();

		int nIndex = 0;
		int nCnt = _GetDigiClonType( nNextEnchantLv );
		if( m_DigicloneItem != NULL )
		{
			int nItem_S = nsCsFileTable::g_pItemMng->GetItem( m_DigicloneItem->GetType() )->GetInfo()->s_nType_S; 

			while( nIndex < nCnt )
			{
				if( nItem_S == m_nUsableClone[ nIndex ] % 100  )
				{
					break;
				}
				nIndex++;
			}
		}

		bool  bPersent = true;
		bool  bFloat = true;
		float fMinValue;
		float fMaxValue;

		int nMinValue = 0;
		int nMaxValue = 0;

		CDigimon::sENCHENT_STAT* pEState = ( (CDigimon*)g_pCharMng->GetDigimonUser( 0 ) )->GetEnchantStat();	
		int nHatchLv = 0;
		cData_PostLoad::sDATA* pkDigimonData = g_pDataMng->GetPostLoad()->GetDigimonData();
		if( pkDigimonData )
			nHatchLv = pkDigimonData->s_HatchLevel;
		_GetEnchantMinMaxValue( m_nUsableClone[ nIndex ], m_nSelectAbility, nNextEnchantLv, nHatchLv, nMinValue, nMaxValue );

		switch( m_nSelectAbility )
		{
		case 0:
			{
				int nBase = FMDigimon::GetBaseAtt( &info ) + FMDigimon::GetEvolAtt( &info );				

				fMinValue = (float)( pEState->GetEnchantValue( ET_AT ) + nMinValue );
				fMaxValue = (float)( pEState->GetEnchantValue( ET_AT ) + nMaxValue );	

				fMinValue = ( nBase * ( fMinValue / 100.0f ) );
				fMaxValue = ( nBase * ( fMaxValue / 100.0f ) );

				bPersent = false;
			}
			break;
		case 1:
			{
				fMinValue = (float)( pEState->GetEnchantValue( ET_BL ) + nMinValue );
				fMaxValue = (float)( pEState->GetEnchantValue( ET_BL ) + nMaxValue );	
				bFloat = false;
			}
			break;
		case 2:
			{
				fMinValue = FMDigimon::GetBaseCr( &info ) * ((pEState->GetEnchantValue( ET_CR ) + nMinValue) / 10000.0f);
				fMaxValue = FMDigimon::GetBaseCr( &info ) * ((pEState->GetEnchantValue( ET_CR ) + nMaxValue) / 10000.0f);	
			}	
			break;
		case 3:
			{
				fMinValue = FMDigimon::GetBaseEv( &info ) * ((pEState->GetEnchantValue( ET_EV ) + nMinValue) / 10000.0f);
				fMaxValue = FMDigimon::GetBaseEv( &info ) * ((pEState->GetEnchantValue( ET_EV ) + nMaxValue) / 10000.0f);		
			}
			break;
		case 4:
			{
				int nBase = FMDigimon::GetBaseMaxHP( &info ) + FMDigimon::GetEvolMaxHP( &info );	

				fMinValue = (float)( pEState->GetEnchantValue( ET_HP ) + nMinValue );
				fMaxValue = (float)( pEState->GetEnchantValue( ET_HP ) + nMaxValue );	

				fMinValue = ( nBase * ( fMinValue / 100.0f ) );
				fMaxValue = ( nBase * ( fMaxValue / 100.0f ) );

				bPersent = false;
			}
			break;
		/*	case 5:
			nAbility = ET_AS;
			break;*/
		default:
			assert_csm( false, _T( "잘못된 능력치 선택" ) );
		}		

		if( bPersent == true )
		{
			if( bFloat == true )
				_stprintf_s( sz, 32, L"%.2f%% ~ %.2f%%", fMinValue, fMaxValue);	
			else
				_stprintf_s( sz, 32, L"%d%% ~ %d%%", (int)fMinValue, (int)fMaxValue );	
		}
		else
			_stprintf_s( sz, 32, L" %d ~ %d", (int)(fMinValue), (int)(fMaxValue) );

		m_pExplain->SetText( sz );
	}
	else
	{
		m_pExplain->SetText( UISTRING_TEXT( "ENCHANTTACTICS_CANNOT_ENCHANT" ).c_str() );
	}
}

void cEnchantTactics::_GetEnchantMinMaxValue( DWORD const& dwItemType, int const& nEnchantType, int const& nEnchantLv, int const& nGrowLv, int& nMin, int& nMax )
{
	nMin = 0;
	nMax = 0;
	bool bMaxValue = false;
	switch( dwItemType )
	{
	case nItem::AdvancedDigiCloneS:
	case nItem::AdvancedDigiCloneA:
	case nItem::AdvancedDigiCloneB:
	case nItem::AdvancedDigiCloneC:
	case nItem::AdvancedDigiCloneD:
	case nItem::UltimateDigiCloneA:
	case nItem::UltimateDigiCloneB:
	case nItem::UltimateDigiCloneC:
	case nItem::UltimateDigiCloneD:
	case nItem::UltimateDigiCloneS://궁극및 강화 디지클론은 Max 강화
		bMaxValue = true;
		break;
	}

	int nStateType = GetSelAbilityType( nEnchantType ) + 1;
	
	sEnchantStatInfo const* pEnchantStatInfo = nsCsFileTable::g_pTacticsMng->GetEnchantStatInfo( nStateType, nEnchantLv );
	SAFE_POINTER_RET( pEnchantStatInfo );

// 	if( pEnchantStatInfo->nDigimonGrowMinLv > nGrowLv || pEnchantStatInfo->nDigimonGrowMaxLv < nGrowLv )
// 	{ // 현재 디지몬 단수로는 강화할 수 없는 스텟이다.
// 		cPrintMsg::PrintMsg( 10004 );
// 		return;
// 	}

	if( bMaxValue )
	{
		nMin = pEnchantStatInfo->nSpecialEnchantValue;
		nMax = pEnchantStatInfo->nSpecialEnchantValue;
	}
	else
	{
		nMin = pEnchantStatInfo->nNormalEnchantMinValue;
		nMax = pEnchantStatInfo->nNormalEnchantMaxValue;
	}	
}

void cEnchantTactics::SetResist( int nInvenIndex )
{
	if( m_bWaitRecvServer == true )
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}

	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	cItemInfo* pInvenItem = pInven->GetData( nInvenIndex );
	SAFE_POINTER_RET( pInvenItem );

	switch( m_pRadio->GetCheckIndex() )
	{
	case 0:
		{			
			int nItem_L = nsCsFileTable::g_pItemMng->GetItem( pInvenItem->GetType() )->GetInfo()->s_nType_L; 
			int nItem_S = nsCsFileTable::g_pItemMng->GetItem( pInvenItem->GetType() )->GetInfo()->s_nType_S; 
			if( nItem_L == nItem::TacticsEnchant )
			{

				sEnchantItemInfo const* pInfo = nsCsFileTable::g_pTacticsMng->GetEnchantItemInfo( (nItem_L * 100) + nItem_S );

				CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
				CDigimonUser::sUSER_STAT* pUserStat = (CDigimonUser::sUSER_STAT*)pDigimonUser->GetBaseStat();						

				if( _IsDigiClonInsert() == true )
				{
					int nCnt = _GetDigiClonType( _GetSelectStat_EnchantLV() + 1 );
					bool bUsable = false;

					for( int i = 0 ; i < nCnt ; i++ )
					{
						if( nItem_S == m_nUsableClone[ i ] % 100  )
						{
							bUsable = true;
							break;
						}
					}

					if( bUsable )
					{
#ifdef GUILD_RENEWAL
						//강화 비용 감소
						std::pair<int, int> usingSkillValue;
						usingSkillValue.first = GUILD_INCHAPAYREDUCE;
						GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_USINGSKILLVALUE, &usingSkillValue );
						int nVal = usingSkillValue.second;

						u8 nSaleMoney = 0;
						if( nVal != 0 )
						{		
							nSaleMoney = (u8)( ( (u8)pInfo->dwNeedMoney * nVal ) / 10000 );
						}

						m_pMoney->SetMoney( (u8)pInfo->dwNeedMoney - nSaleMoney );
#else
						m_pMoney->SetMoney( (u8)pInfo->dwNeedMoney );			
#endif
						if( m_DigicloneItem )
						{				
							g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nDigicloneSlot ) );
						}

						m_nDigicloneSlot = nInvenIndex;
						g_pDataMng->ItemLock( TO_INVEN_SID( m_nDigicloneSlot ) );
						m_DigicloneItem = g_pDataMng->GetInven()->GetData( m_nDigicloneSlot );

						_SetResistButtonText();
						SetNextEnchantText();
					}
					else // 해당 슬롯에 사용못하는 디지클론아이템		
					{
						cPrintMsg::PrintMsg( 30403 );
					}
				}		
			}
			else if( nItem_L == nItem::TacticsEnchantSub )
			{		
				if( m_SubItem )
				{				
					g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nSubItemSlot ) );
				}

				m_nSubItemSlot = nInvenIndex;
				g_pDataMng->ItemLock( TO_INVEN_SID( m_nSubItemSlot ) );
				m_SubItem = g_pDataMng->GetInven()->GetData( m_nSubItemSlot );				
			}
			else
			{
				cPrintMsg::PrintMsg( 30404 );
			}
		}
		break;
	case 1:
		{			
			if( g_pCharMng->GetDigimonUser( 0 )->GetEnchantStat()->GetEnchantLevel() < 1 )
				cPrintMsg::PrintMsg( 30428 );
			else
			{
				int nItemID = g_pDataMng->GetInven()->GetData( nInvenIndex )->GetType();
				if( nsCsFileTable::g_pItemMng->GetItem( nItemID )->GetInfo()->s_nType_L == nItem::TacticsEnchantReset )
				{		
					if( m_SubItem )
					{				
						g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nSubItemSlot ) );
					}

					m_nSubItemSlot = nInvenIndex;
					g_pDataMng->ItemLock( TO_INVEN_SID( m_nSubItemSlot ) );
					m_SubItem = g_pDataMng->GetInven()->GetData( m_nSubItemSlot );				
				}
				else
				{
					int iCommentID = 30404;
#ifdef VERSION_KOR
					iCommentID = 30424; //국내에서만 멘트 변경
#endif
					cPrintMsg::PrintMsg( iCommentID );
				}
			}	
		}
		break;
	}		

	cWindow::PlaySound( "System\\interface_person_01.wav" );
}

int cEnchantTactics::GetSelAbilityType( int const& nSelIdx )
{
	switch( nSelIdx )
	{
	case 0:	return ET_AT;	// 공격
	case 1: return ET_BL;	// 블럭 - 이름이 안 맞아도 어쩔 수가 없음
	case 2: return ET_CR;	// 크리
	case 3: return ET_EV;	// 회피
	case 4: return ET_HP;	// 체력
	}

	return -1;
}

bool cEnchantTactics::SendEnchantServer()
{
	if( m_bWaitRecvServer == true )
	{		
		return false;
	}	

	if( m_nDigicloneSlot == cData_Inven::INVALIDE_INVEN_INDEX )
	{
		return false;
	}

	int nAbility;
	switch( m_nSelectAbility )
	{
	case 0:
		nAbility = ET_AT;
		break;
	case 1:
		nAbility = ET_BL;
		break;
	case 2:
		nAbility = ET_CR;
		break;	
	case 3:
		nAbility = ET_EV;
		break;		
	case 4:
		nAbility = ET_HP;
		break;
	/*	case 5:
		nAbility = ET_AS;
		break;*/
	default:
		assert_csm( false, _T( "잘못된 능력치 선택" ) );
	}			

	if( m_bWaitRecvServer == false )
	{	
		m_bWaitRecvServer = true;
		net::game->SendDigimonInchant( nAbility + 1, m_nDigicloneSlot, m_nSubItemSlot );
		SetEnableWindow( false );
	}	

	return true;
}

void cEnchantTactics::RecvEnchantServer()
{
	SetEnableWindow( true );
	m_bWaitRecvServer = false;	

	if( m_DigicloneItem )
	{				
		g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nDigicloneSlot ) );
	}
	if( m_SubItem )
	{				
		g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nSubItemSlot ) );
	}

	m_pMoney->SetMoney( (u8)0 );

	int nItem_L = nsCsFileTable::g_pItemMng->GetItem( g_pDataMng->GetInven()->GetData( m_nDigicloneSlot )->GetType() )->GetInfo()->s_nType_L; 
	int nItem_S = nsCsFileTable::g_pItemMng->GetItem( g_pDataMng->GetInven()->GetData( m_nDigicloneSlot )->GetType() )->GetInfo()->s_nType_S; 

	sEnchantItemInfo const* pInfo = nsCsFileTable::g_pTacticsMng->GetEnchantItemInfo( (nItem_L * 100) + nItem_S );

#ifdef GUILD_RENEWAL

	u8 nTotalMoney = pInfo->dwNeedMoney;
	// 강화 비용 감소
	std::pair<int, int> usingSkillValue;
	usingSkillValue.first = GUILD_INCHAPAYREDUCE;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_USINGSKILLVALUE, &usingSkillValue );
	int nVal = usingSkillValue.second;

	if( nVal > 0 )
	{
		u8 nSaleMoney = (u8)( ( nTotalMoney * nVal ) / 10000 );
		nTotalMoney -= nSaleMoney;
		cPrintMsg::PrintMsg( 30223, &sMONEY(nTotalMoney), &sMONEY(pInfo->dwNeedMoney), &sMONEY(nSaleMoney) );// “길드스킬로 인해 소모되는 비용이 [Money]만큼 감소합니다.”
	}
	else
		cPrintMsg::PrintMsg( 30213, &sMONEY(nTotalMoney) );	// “강화 비용 [Money]가 소모되었습니다.”

	g_pDataMng->GetInven()->DecreaseMoney( nTotalMoney );
#else
	g_pDataMng->GetInven()->DecreaseMoney( pInfo->dwNeedMoney );
#endif	// GUILD_RENEWAL
	g_pDataMng->GetInven()->GetData( m_nDigicloneSlot )->DecreaseCount( 1 );
	m_DigicloneItem = NULL;
	m_nDigicloneSlot = cData_Inven::INVALIDE_INVEN_INDEX;

	if( m_nSubItemSlot != cData_Inven::INVALIDE_INVEN_INDEX )
	{
		g_pDataMng->GetInven()->GetData( m_nSubItemSlot )->DecreaseCount( 1 );
		m_SubItem = NULL;
		m_nSubItemSlot = cData_Inven::INVALIDE_INVEN_INDEX;
	}		

	_SetResistButtonText();
	net::game->SendQueryMoneyInven();

	//GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_ENCHANT_COMPLETE, NULL );
}

void cEnchantTactics::SendWeakServer()
{
	if( m_bWaitRecvServer == true )
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}	

	int nAbility;
	switch( m_nSelectAbility )
	{
	case 0:
		nAbility = ET_AT;
		break;
	case 1:
		nAbility = ET_BL;
		break;
	case 2:
		nAbility = ET_CR;
		break;	
	case 3:
		nAbility = ET_EV;
		break;		
	case 4:
		nAbility = ET_HP;
		break;
	/*	case 5:
		nAbility = ET_AS;
		break;*/
	default:
		assert_csm( false, _T( "잘못된 능력치 선택" ) );
	}		

	if( m_bWaitRecvServer == false )
	{	
		m_bWaitRecvServer = true;
		net::game->SendDecreaseDigimonInchant( nAbility + 1, m_nSubItemSlot );
		SetEnableWindow( false );
	}	
}

bool	cEnchantTactics::IsSubItem() 
{
	return ( m_SubItem != NULL );
}

bool	cEnchantTactics::IsDigiClon()
{ 
	return ( m_DigicloneItem != NULL );
}

void cEnchantTactics::RecvWeakServer()
{
	CDigimon::sENCHENT_STAT* pEState;
	pEState = ((CDigimon*)g_pCharMng->GetDigimonUser( 0 ) )->GetEnchantStat();

	if( nsCsFileTable::g_pItemMng->GetItem( m_SubItem->GetType() )->GetInfo()->s_nType_S == 1 )
	{
		std::wstring wsLevel = UISTRING_TEXT( "ENCHANTTACTICS_ENCHANT_LEVEL" );
		std::wstring wsMsg;
		switch( m_nSelectAbility )
		{
		case 0:
			{
				DmCS::StringFn::Format( wsMsg, L"%s (%d%s -> %d%s)", L"AT", pEState->GetEnchantLv( ET_AT ), wsLevel.c_str(), pEState->GetEnchantLv( ET_AT ) - 1, wsLevel.c_str() );
				cPrintMsg::PrintMsg( 30427, const_cast<wchar_t*>(wsMsg.c_str()) );	
			}
			break;
		case 1:
			{
				DmCS::StringFn::Format( wsMsg, L"%s (%d%s -> %d%s)", L"BL", pEState->GetEnchantLv( ET_BL ), wsLevel.c_str(), pEState->GetEnchantLv( ET_BL ) - 1, wsLevel.c_str() );
				cPrintMsg::PrintMsg( 30427, const_cast<wchar_t*>(wsMsg.c_str()) );	
			}
			break;
		case 2:
			{
				DmCS::StringFn::Format( wsMsg, L"%s (%d%s -> %d%s)", L"CT", pEState->GetEnchantLv( ET_CR ), wsLevel.c_str(), pEState->GetEnchantLv( ET_CR ) - 1, wsLevel.c_str() );
				cPrintMsg::PrintMsg( 30427, const_cast<wchar_t*>(wsMsg.c_str()) );
			}
			break;	
		case 3:
			{
				DmCS::StringFn::Format( wsMsg, L"%s (%d%s -> %d%s)", L"EV", pEState->GetEnchantLv( ET_EV ), wsLevel.c_str(), pEState->GetEnchantLv( ET_EV ) - 1, wsLevel.c_str() );
				cPrintMsg::PrintMsg( 30427, const_cast<wchar_t*>(wsMsg.c_str()) );
			}
			break;		
		case 4:
			{
				DmCS::StringFn::Format( wsMsg, L"%s (%d%s -> %d%s)", L"HP", pEState->GetEnchantLv( ET_HP ), wsLevel.c_str(), pEState->GetEnchantLv( ET_HP ) - 1, wsLevel.c_str() );
				cPrintMsg::PrintMsg( 30427, const_cast<wchar_t*>(wsMsg.c_str()) );
			}
			break;		
			break;
			/*	case 5:
			nAbility = ET_AS;
			break;*/
		}	
	}
	else // 전체 초기화 성공
	{
		switch( m_nSelectAbility )
		{
		case 0:		cPrintMsg::PrintMsg( 30431, _ONLY_ENG( "AT" ) );			break;
		case 1:		cPrintMsg::PrintMsg( 30431, _ONLY_ENG( "BL" ) );			break;
		case 2:		cPrintMsg::PrintMsg( 30431, _ONLY_ENG( "CT" ) );			break;	
		case 3:		cPrintMsg::PrintMsg( 30431, _ONLY_ENG( "EV" ) );			break;	
		case 4:		cPrintMsg::PrintMsg( 30431, _ONLY_ENG( "HP" ) );			break;		
			/*case 4:
			nAbility = ET_HT;
			break;
			case 5:
			nAbility = ET_AS;
			break;*/	
		}	
	}

	SetEnableWindow( true );
	m_bWaitRecvServer = false;	

	g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nSubItemSlot ) );
	g_pDataMng->GetInven()->GetData( m_nSubItemSlot )->DecreaseCount( 1 );
	m_SubItem = NULL;
	m_nSubItemSlot = cData_Inven::INVALIDE_INVEN_INDEX;

	net::game->SendQueryMoneyInven();

	_SetResistButtonText();
}

bool cEnchantTactics::IsCheckedAbility() const
{
	return m_nSelectAbility > 0 && m_nSelectAbility < 3 ? true : false;
}

cWindow* cEnchantTactics::GetChildControl( std::string const& sControlName )
{
	if( 0 == sControlName.compare("State_Selected_AT") )
	{
		if( -1 == m_nSelectAbility )
			return NULL;
		if( 0 == m_nSelectAbility )
			return m_pSelectMask;
		return NULL;
	}
		
	return cScriptUI::GetChildControl( sControlName );
}

CsPoint cEnchantTactics::StateControlPos( std::string const& sControlName )
{
	CsPoint pos;
	if( 0 == sControlName.compare("State_AT") )
		pos = m_rcRect[ 0 ].GetPos() + GetRootClient();
	else if( 0 == sControlName.compare("State_BL") )
		pos = m_rcRect[ 1 ].GetPos() + GetRootClient();
	else if( 0 == sControlName.compare("State_BL") )
		pos = m_rcRect[ 2 ].GetPos() + GetRootClient();
	else if( 0 == sControlName.compare("State_BL") )
		pos = m_rcRect[ 3 ].GetPos() + GetRootClient();

	return pos;
}

cItemInfo* cEnchantTactics::GetRegistDigiClone()
{
	return m_DigicloneItem;
}

cItemInfo* cEnchantTactics::GetRegistSubItem()
{
	return m_SubItem;
}
