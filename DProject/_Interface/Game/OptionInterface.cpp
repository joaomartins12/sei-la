

#include "stdafx.h"
#include "OptionInterface.h"

#pragma warning( disable : 4800 )

void cOptionInterface::Destroy()
{
	cOptionBase::Delete();
}

void cOptionInterface::DeleteResource()
{
	cOptionBase::DeleteResource();
}

void cOptionInterface::Create(int nValue /* = 0  */)
{
	if( cOptionBase::CreateBase( UISTRING_TEXT( "OPTION_INTERFACE_TITLE_INTERFACE_SETTING" ).c_str(), nValue ) == false )
		return;
	m_pDefaultBtn = AddButton( CsPoint( 14, 556 ), CsPoint( 101, 31 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_DEFAULT" ).c_str() );	

	_InitInterfaceControl();
}

void cOptionInterface::_InitInterfaceControl()
{
	TCHAR sz[ IF_OP_COMBO_STR_MAX_COUNT ][ IF_OP_COMBO_STR_MAX_LEN ];
	NiColor color[ IF_OP_COMBO_STR_MAX_COUNT ];
	int nData[ IF_OP_COMBO_STR_MAX_COUNT ];

	DWORD dwServerOption = g_pCharMng->GetTamerUser()->GetSyncOption();

#ifdef KEYBOARD_MOVE
	_AddControl_Title( UISTRING_TEXT( "COMMON_TXT_MOVE" ).c_str() );

#ifdef VERSION_USA
	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "COMMON_TXT_TAMER" ).c_str() ); 
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "COMMON_TXT_DIGIMON" ).c_str() );
#else
	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_TAMER_MOVE" ).c_str() ); 
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_DIGIMON_MOVE" ).c_str() );
#endif
	nData[ 0 ] = (int)(!g_pResist->m_Global.s_bMoveDigimon);
	nData[ 1 ] = (int)g_pResist->m_Global.s_bMoveDigimon;
	_AddControl_CheckBox( UISTRING_TEXT( "OPTION_INTERFACE_CHECKBOX_MOVE_TARGET" ).c_str(), 2,  sz, nData, false, true, MOVE_MAIN );

	bool bIsTutorialPlay = false;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_IS_TUTORIAL_PLAYING, &bIsTutorialPlay );
	if( bIsTutorialPlay )
	{
		sOP_CHECK_BOX* pCheckBox = FindCheckBox( MOVE_MAIN );
		if( pCheckBox )
		{
// 			if( pCheckBox->s_pCheckText[0] )
// 				pCheckBox->s_pCheckText[0]->SetVisible(false);
// 			if( pCheckBox->s_pCheckText[1] )
// 				pCheckBox->s_pCheckText[1]->SetVisible(false);
			if( pCheckBox->s_pCheck[0] )
				pCheckBox->s_pCheck[0]->SetVisible(false);
			if( pCheckBox->s_pCheck[1] )
				pCheckBox->s_pCheck[1]->SetVisible(false);
		}
	}

	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_MOUSE" ).c_str() ); 
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_KEYBOARD" ).c_str() );
	nData[ 0 ] = (int)g_pResist->m_Global.s_bMove_Mouse;
	nData[ 1 ] = (int)g_pResist->m_Global.s_bMove_KeyBoard;
	_AddControl_CheckBox( UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_INPUT_METHOD" ).c_str(), 2,  sz, nData, false, false, MOVE_INPUT );
#endif
	// 카메라
	_AddControl_Title( UISTRING_TEXT( "COMMON_TXT_CAMERA" ).c_str() );

	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_ROTATE_FREE_VIEW" ).c_str() ); 
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_ROTATE_FIXED_VIEW" ).c_str() ); 
	color[ 0 ] = FONT_WHITE;
	color[ 1 ] = FONT_WHITE;
	nData[ 0 ] = 0;
	nData[ 1 ] = 1;
	_AddControl_Combo( UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_ROTATION" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Character.s_bStaticCamera, CAMERA_MODE );

	std::wstring wsLevel = UISTRING_TEXT( "COMMON_TXT_NUM_LEVEL" );
	DmCS::StringFn::Replace( wsLevel, L"#Num#", 1 );
	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, wsLevel.c_str() ); 

	wsLevel = UISTRING_TEXT( "COMMON_TXT_NUM_LEVEL" );
	DmCS::StringFn::Replace( wsLevel, L"#Num#", 2 );
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, wsLevel.c_str() ); 

	wsLevel = UISTRING_TEXT( "COMMON_TXT_NUM_LEVEL" );
	DmCS::StringFn::Replace( wsLevel, L"#Num#", 3 );
	_tcscpy_s( sz[ 2 ], IF_OP_COMBO_STR_MAX_LEN, wsLevel.c_str() ); 

	wsLevel = UISTRING_TEXT( "COMMON_TXT_NUM_LEVEL" );
	DmCS::StringFn::Replace( wsLevel, L"#Num#", 4 );
	_tcscpy_s( sz[ 3 ], IF_OP_COMBO_STR_MAX_LEN, wsLevel.c_str() ); 

	wsLevel = UISTRING_TEXT( "COMMON_TXT_NUM_LEVEL" );
	DmCS::StringFn::Replace( wsLevel, L"#Num#", 5 );
	_tcscpy_s( sz[ 4 ], IF_OP_COMBO_STR_MAX_LEN, wsLevel.c_str() ); 

	color[ 0 ] = FONT_WHITE;
	color[ 1 ] = FONT_WHITE;
	color[ 2 ] = FONT_WHITE;
	color[ 3 ] = FONT_WHITE;
	color[ 4 ] = FONT_WHITE;
	nData[ 0 ] = 1;
	nData[ 1 ] = 2;
	nData[ 2 ] = 3;
	nData[ 3 ] = 4;
	nData[ 4 ] = 5;
	_AddControl_Combo( UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_DISTANCE" ).c_str(), 5, sz, color, nData, (int)g_pResist->m_Character.s_nCameraMaxDistLevel, CAMERA_DIST );

	_AddControl_Slider( UISTRING_TEXT( "OPTION_GRAPHIC_SLIDER_AUTO_ROTATING_SPEED" ).c_str(), CsPoint( 20, 230 ), 10, (int)( g_pResist->m_Character.s_fStaticCameraRotSpeed*100.0f ), 
		UISTRING_TEXT( "OPTION_GRAPHIC_SLIDER_SLOW" ).c_str(), UISTRING_TEXT( "OPTION_GRAPHIC_SLIDER_FAST" ).c_str(), CAMERA_AUTO_ROT_SPEED );

	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_OFF" ).c_str() ); 
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_ON" ).c_str() ); 
	color[ 0 ] = NiColor( 240/255.0f, 0/255.0f, 51/255.0f );
	color[ 1 ] = FONT_WHITE;
	nData[ 0 ] = 0;
	nData[ 1 ] = 1;
	_AddControl_Combo( UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_CRITICAL_EFFECT_ENEMY" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bCameraEnemyCritical, CAMERA_ENEMY_CRITICAL );	
	// 모델
	_AddControl_Title( UISTRING_TEXT( "OPTION_INTERFACE_TITLE_MODEL" ).c_str() );		

	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_SEE" ).c_str() ); 
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_HIDE" ).c_str() );
	color[ 0 ] = FONT_WHITE;
	color[ 1 ] = NiColor( 240/255.0f, 0/255.0f, 51/255.0f );	
	nData[ 0 ] = 1;
	nData[ 1 ] = 0;
	
	int nSel = ( dwServerOption & nTamer::Option::CostumeOff ) ? nData[ 1 ] : nData[ 0 ] ;
	_AddControl_Combo( UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_COSTUME" ).c_str(), 2, sz, color, nData, nSel, VIEW_COSTUME );		

	// 이름표시
	_AddControl_Title( UISTRING_TEXT( "OPTION_INTERFACE_TITLE_NAME_DISPLAY" ).c_str() );

	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_HIDE" ).c_str() ); 
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_SEE" ).c_str() );	
	color[ 0 ] = NiColor( 240/255.0f, 0/255.0f, 51/255.0f );
	color[ 1 ] = FONT_WHITE;
	nData[ 0 ] = 0;
	nData[ 1 ] = 1;

	_AddControl_Combo( UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_SELF" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bMyRenderName, NAMEVIEW_SELF );
	_AddControl_Combo( UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_OTHER_PLAYERS" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bOtherRenderName, NAMEVIEW_OTHER );
	_AddControl_Combo( UISTRING_TEXT( "COMMON_TXT_MONSTER" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bMonsterRenderName, NAMEVIEW_MONSTER );
	_AddControl_Combo( UISTRING_TEXT( "COMMON_TXT_NPC" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bNpcRenderName, NAMEVIEW_NPC );

	// 게이지
	_AddControl_Title( UISTRING_TEXT( "OPTION_INTERFACE_TITLE_GAUGE" ).c_str() );
	_AddControl_Combo( UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_DISPLAY" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bViewHpBar, HPVIEW );

	// 커뮤니티
	_AddControl_Title( UISTRING_TEXT( "COMMON_TXT_COMMUNITY" ).c_str() ); 

	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_AUTO_REJECT" ).c_str() );
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_ACTIVATION" ).c_str() );
	color[ 0 ] = NiColor( 240/255.0f, 0/255.0f, 51/255.0f );
	color[ 1 ] = FONT_WHITE;
	_AddControl_Combo( UISTRING_TEXT( "COMMON_TXT_TRADE" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bTradeActive, TRADE_ACTIVE );
	_AddControl_Combo( UISTRING_TEXT( "COMMON_TXT_PARTY" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bPartyActive, PARTY_ACTIVE );

#ifdef GUILD_INVITATION
	_AddControl_Combo( UISTRING_TEXT( "COMMON_TXT_INVITE_TO_GUILD" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bGuildInvitation, GUILD_INVITATE );
#endif

	// 말풍선
	_AddControl_Title( UISTRING_TEXT( "OPTION_INTERFACE_TITLE_SPEECH_BUBBLE" ).c_str() );

	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_HIDE" ).c_str() );
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_SEE" ).c_str() );
	color[ 0 ] = NiColor( 240/255.0f, 0/255.0f, 51/255.0f );
	color[ 1 ] = FONT_WHITE;
	_AddControl_Combo( UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_GENERAL_DIALOGUE" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bChatBalloone, BALLOONE_CHAT );
	_AddControl_Combo( UISTRING_TEXT( "COMMON_TXT_DIGIMON_TALK" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bDigimonBalloone, BALLOONE_DIGIMON );
	_AddControl_Combo( UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_DIGIMON_HELP" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bTipBalloone, BALLOONE_TIP );

	// 이펙트
	_AddControl_Title( UISTRING_TEXT( "OPTION_INTERFACE_TITLE_EFFECT" ).c_str() );

	_AddControl_Combo( UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_DIGIMON_EFFECT" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bViewLevelEffect, VIEW_LEVELEFFECT );
	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_OFF_ALL" ).c_str() );
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_HIDE_ATTRIBUTE_EFFECT" ).c_str() );
	_tcscpy_s( sz[ 2 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_ON_ALL" ).c_str() );
	color[ 0 ] = NiColor( 240/255.0f, 0/255.0f, 51/255.0f );
	color[ 1 ] = FONT_WHITE;
	color[ 2 ] = FONT_WHITE;
	nData[ 0 ] = cResist::sGLOBAL::AEM_ALL_OFF;
	nData[ 1 ] = cResist::sGLOBAL::AEM_ONLY_NORATT_ON;
	nData[ 2 ] = cResist::sGLOBAL::AEM_ALL_ON;
	_AddControl_Combo( UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_BLOWING_EFFECT" ).c_str(), 3, sz, color, nData, (int)g_pResist->m_Global.s_eAttackEffectViewMode, VIEW_ATTACK_EFFECT );	
	
#ifdef USE_CENTER_BAR
	// 센터바
	_AddControl_Title( UISTRING_TEXT( "OPTION_INTERFACE_CENTER_BAR" ).c_str() );

	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_OFF" ).c_str() ); 
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_ON" ).c_str() ); 
	color[ 0 ] = NiColor( 240/255.0f, 0/255.0f, 51/255.0f );
	color[ 1 ] = FONT_WHITE;
	nData[ 0 ] = 0;
	nData[ 1 ] = 1;
	_AddControl_Combo( UISTRING_TEXT( "OPTION_INTERFACE_CENTER_BAR" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_CenterBar.s_bInitial, CENTER_BAR );
	_AddControl_Combo( UISTRING_TEXT( "OPTION_ADDITIONAL_QUICKSLOT" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_CenterBar.s_bIsOpenQuick, QUICK_OPEN );
	_AddControl_Slider( UISTRING_TEXT( "OPTION_GRAPHIC_SLIDER_ALPHA" ).c_str(), CsPoint( 0, 100 ), 10, (int)( g_pResist->m_CenterBar.s_fAlpha*100.0f ), 
		UISTRING_TEXT( "OPTION_GRAPHIC_SLIDER_LOW" ).c_str(), UISTRING_TEXT( "OPTION_GRAPHIC_SLIDER_HIGH" ).c_str(), CENTER_BAR_ALPHA );
	FindSlider( CENTER_BAR_ALPHA )->s_pSlider->Enable( g_pResist->m_CenterBar.s_bInitial );
#endif
	// 추가기능 설정
	_AddControl_Title( UISTRING_TEXT( "OPTION_INTERFACE_TITLE_ADDITIONAL_CONFIGURE" ).c_str() );
	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_OFF" ).c_str() ); 
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_ON" ).c_str() ); 
	color[ 0 ] = NiColor( 240/255.0f, 0/255.0f, 51/255.0f );
	color[ 1 ] = FONT_WHITE;
	nData[ 0 ] = 0;
	nData[ 1 ] = 1;
	_AddControl_Combo( UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_BOSS_ALERT" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bBossWarning, BOSS_WARNING );

	if( g_pResist->m_Global.s_bRightToolTip != FALSE )
	{
		g_pResist->m_Global.s_bRightToolTip = TRUE;
	}
	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_OFF" ).c_str() ); 
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_ON" ).c_str() ); 
	color[ 0 ] = NiColor( 240/255.0f, 0/255.0f, 51/255.0f );
	color[ 1 ] = FONT_WHITE;
	nData[ 0 ] = 0;
	nData[ 1 ] = 1;
	_AddControl_Combo( UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_RIGHT_TOOLTIP" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bRightToolTip, RIGHT_TOOLTIP );

	// 기타
	_AddControl_Title( UISTRING_TEXT( "OPTION_TITLE_OTHERS" ).c_str() );

#ifdef LANGAUGE_SELECT
	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_KOREAN_TRANSLATION" ).c_str() );
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_KOREAN_ORIGINAL" ).c_str() );
	color[ 0 ] = FONT_WHITE;
	color[ 1 ] = FONT_WHITE;
	nData[ 0 ] = nsCsFileTable::KOREA_TRANSLATION;
	nData[ 1 ] = nsCsFileTable::KOREA_ORIGINAL;
	_AddControl_Combo( UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_LANGUAGE" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_eFTLanguage, LANGUAGE );
#endif

#ifdef USE_BARCODE_REDER
	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_OFF" ).c_str() ); 
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_ON" ).c_str() ); 
	color[ 0 ] = NiColor( 240/255.0f, 0/255.0f, 51/255.0f );
	color[ 1 ] = FONT_WHITE;
	nData[ 0 ] = 0;
	nData[ 1 ] = 1;
	_AddControl_Combo( UISTRING_TEXT( "COMMON_TXT_GAME_SCANNER" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bUseBarcode, USE_BARCODE );
#endif


	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_LEFT" ).c_str() ); 
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_RIGHT" ).c_str() );
	color[ 0 ] = FONT_WHITE;
	color[ 1 ] = FONT_WHITE;
	nData[ 0 ] = 0;
	nData[ 1 ] = 1;
	_AddControl_Combo( UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_MOUSE_CONTROL" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bRightHandMouse, MOUSE_HAND );	

	// 간략화
	_AddControl_Title( UISTRING_TEXT( "OPTION_INTERFACE_TITLE_SIMPLIFY" ).c_str() );

	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_OFF" ).c_str() ); 
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_ON" ).c_str() ); 
	color[ 0 ] = FONT_WHITE;
	color[ 1 ] = NiColor( 240/255.0f, 0/255.0f, 51/255.0f );
	_AddControl_Combo( UISTRING_TEXT( "COMMON_TXT_TAMER" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bFigureTamer, FIGURE_TAMER );
	_AddControl_Combo( UISTRING_TEXT( "COMMON_TXT_DIGIMON" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bFigureDigimon, FIGURE_DIGIMON );
	_AddControl_Combo( UISTRING_TEXT( "OPTION_INTERFACE_COMBOBOX_PERSONAL_SHOP" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bFigureEmployment, FIGURE_EMPLOYMENT );
	_ResizeScrollBar();
}

void cOptionInterface::Update(float const& fDeltaTime)
{
	cOptionBase::Update(fDeltaTime);
}

cBaseWindow::eMU_TYPE
cOptionInterface::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	//============================================================================================================
	// 일반
	//============================================================================================================
	switch( m_pDefaultBtn->Update_ForMouse() )
	{
	case cCheckBox::ACTION_CLICK:
		_OnDefault();
	case cCheckBox::ACTION_DOWN:
		return muReturn;		
	}

	return cOptionBase::Update_ForMouse();
}

void cOptionInterface::Render()
{
	cOptionBase::Render();
}

void cOptionInterface::ResetDevice()
{
	cOptionBase::ResetDevice();
}

void cOptionInterface::OnMoveWindow()
{
	cOptionBase::OnMoveWindow();
}

void cOptionInterface::_OnOk()
{
	g_pResist->m_Global.s_bTradeActive = (bool)FindCombo( TRADE_ACTIVE )->s_pCombo->GetCurSel_Data();
	g_pResist->m_Global.s_bPartyActive = (bool)FindCombo( PARTY_ACTIVE )->s_pCombo->GetCurSel_Data();

#ifdef GUILD_INVITATION
	g_pResist->m_Global.s_bGuildInvitation = (bool)FindCombo ( GUILD_INVITATE )->s_pCombo->GetCurSel_Data();
#endif

	g_pResist->m_Global.s_bRightHandMouse = (bool)FindCombo( MOUSE_HAND )->s_pCombo->GetCurSel_Data();
	g_pResist->m_Global.s_bCameraEnemyCritical = (BOOL)FindCombo( CAMERA_ENEMY_CRITICAL )->s_pCombo->GetCurSel_Data();
	g_pResist->m_Global.s_bRightToolTip = (BOOL)FindCombo( RIGHT_TOOLTIP )->s_pCombo->GetCurSel_Data();
	g_pResist->m_Global.s_bBossWarning = (BOOL)FindCombo( BOSS_WARNING )->s_pCombo->GetCurSel_Data();	

#ifdef USE_CENTER_BAR
	g_pResist->m_CenterBar.s_bInitial = (BOOL)FindCombo( CENTER_BAR )->s_pCombo->GetCurSel_Data();	
	g_pResist->m_CenterBar.s_fAlpha = FindSlider( CENTER_BAR_ALPHA )->s_pSlider->GetCurPosIndex()*0.01f;	
#endif

	g_pResist->m_Character.s_fStaticCameraRotSpeed = FindSlider( CAMERA_AUTO_ROT_SPEED )->s_pSlider->GetCurPosIndex()*0.01f;
//	if( g_pResist->m_Character.s_nCameraMaxDistLevel != FindCombo( CAMERA_DIST )->s_pCombo->GetCurSel_Data() )	2015-12-03-nova 인터페이스 -> 테이머와의 거리 설정 변경 후 기본값으로 적용되지 않는 현상
	{
		g_pResist->m_Character.s_nCameraMaxDistLevel = FindCombo( CAMERA_DIST )->s_pCombo->GetCurSel_Data();
		int nCamMaxLv = 0;
		if( nsCsGBTerrain::g_pCurRoot != NULL )
			nCamMaxLv = nsCsMapTable::g_pMapListMng->GetList( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID )->GetInfo()->s_nCamera_Max_Level;
		if( nCamMaxLv == 0 )
			nCamMaxLv = g_pResist->m_Character.s_nCameraMaxDistLevel;
		CAMERA_ST.SetDistLevel( nCamMaxLv, true );
	}

	if( g_pResist->m_Character.s_bStaticCamera != (BOOL)FindCombo( CAMERA_MODE )->s_pCombo->GetCurSel_Data() )
	{
		g_pResist->m_Character.s_bStaticCamera = (BOOL)FindCombo( CAMERA_MODE )->s_pCombo->GetCurSel_Data();
		if( g_pResist->m_Character.s_bStaticCamera == TRUE )
		{
			CAMERA_ST.SetStaticCamera( true );			
		}
		else
		{
			CAMERA_ST.SetStaticCamera( false );
		}
	}

	if( g_pResist->m_Global.s_bViewLevelEffect != (BOOL)FindCombo( VIEW_LEVELEFFECT )->s_pCombo->GetCurSel_Data() )
	{
		g_pResist->m_Global.s_bViewLevelEffect = (BOOL)FindCombo( VIEW_LEVELEFFECT )->s_pCombo->GetCurSel_Data();

		std::map<DWORD, CDigimon*>* pVector = g_pCharMng->GetDigimonMap();
		std::map<DWORD, CDigimon*>::iterator it = pVector->begin();
		for( ; it != pVector->end(); ++it )
		{
			CDigimon* pDigimon = it->second;
			SAFE_POINTER_CON( pDigimon );
			pDigimon->CheckEnchantEffect();
			pDigimon->CheckLevelEffect();
			pDigimon->CheckSizeEffect();

#ifdef  SDM_TAMER_XGUAGE_20180628
			pDigimon->CheckingXAntiEffect();
#endif

		}
	}

	if( g_pResist->m_Global.s_eAttackEffectViewMode != (cResist::sGLOBAL::eATT_EFF_MODE)FindCombo( VIEW_ATTACK_EFFECT )->s_pCombo->GetCurSel_Data() )
	{
		g_pResist->m_Global.s_eAttackEffectViewMode = (cResist::sGLOBAL::eATT_EFF_MODE)FindCombo( VIEW_ATTACK_EFFECT )->s_pCombo->GetCurSel_Data();
	}

#ifdef LANGAUGE_SELECT
	if( g_pResist->m_Global.s_eFTLanguage != FindCombo( LANGUAGE )->s_pCombo->GetCurSel_Data() )
	{
		char cPath[ MAX_PATH ];
		g_pResist->m_Global.s_eFTLanguage = (nsCsFileTable::eLANGUAGE)FindCombo( LANGUAGE )->s_pCombo->GetCurSel_Data();
		nsCsFileTable::g_eLanguage = g_pResist->m_Global.s_eFTLanguage;
		nsCsFileTable::g_pSkillMng->Reload( nsCsFileTable::g_FileTableMng.GetLanguagePath( g_pResist->m_Global.s_eFTLanguage, cPath ) );
	}
#endif

#ifdef USE_BARCODE_REDER
	// 바코드
	if( g_pResist->m_Global.s_bUseBarcode != FindCombo( USE_BARCODE )->s_pCombo->GetCurSel_Data() )
	{
		if( nsBARCODE::IsInstallDrive() == false )
		{
			g_pResist->m_Global.s_bUseBarcode = false;
		}
		else
		{
			g_pResist->m_Global.s_bUseBarcode = FindCombo( USE_BARCODE )->s_pCombo->GetCurSel_Data();
		}
		GAME_EVENT_ST.OnEvent( EVENT_CODE::USE_GAMESCANNER, &g_pResist->m_Global.s_bUseBarcode );
	}
#endif

	// 모델
	bool bViewCostum = ( g_pCharMng->GetTamerUser()->IsSyncOption( nTamer::Option::CostumeOff ) == false );
	if( bViewCostum != (bool)FindCombo( VIEW_COSTUME )->s_pCombo->GetCurSel_Data() )
	{
		DWORD dwServerOption = g_pCharMng->GetTamerUser()->GetSyncOption();
		if( FindCombo( VIEW_COSTUME )->s_pCombo->GetCurSel_Data() )
			dwServerOption &= ~nTamer::Option::CostumeOff;
		else
			dwServerOption |= nTamer::Option::CostumeOff;

		if( net::game )
		{
			net::game->SendTamerOption( dwServerOption );
		}
		else
		{
			g_pCharMng->GetTamerUser()->ApplySyncOption( dwServerOption );
		}
	}
	
#ifdef KEYBOARD_MOVE
	// 0 : 테이머 1 : 디지몬, 둘 중 하나만 선택 가능
// 	BOOL bBefore = g_pResist->m_Global.s_bMoveDigimon;
// 	g_pResist->m_Global.s_bMoveDigimon = (BOOL)FindCheckBox( MOVE_MAIN )->s_pCheck[1]->IsCheck();
// 	if( bBefore != g_pResist->m_Global.s_bMoveDigimon )
// 		g_pServerMoveOwner->SetMoveOption( g_pResist->m_Global.s_bMoveDigimon );

	g_pResist->m_Global.s_bMoveDigimon = (BOOL)FindCheckBox( MOVE_MAIN )->s_pCheck[1]->IsCheck();
	g_pServerMoveOwner->SetMoveOption( g_pResist->m_Global.s_bMoveDigimon );

	g_pResist->m_Global.s_bMove_Mouse = (BOOL)FindCheckBox( MOVE_INPUT )->s_pCheck[0]->IsCheck();
	g_pResist->m_Global.s_bMove_KeyBoard = (BOOL)FindCheckBox( MOVE_INPUT )->s_pCheck[1]->IsCheck();
#endif
	cOptionBase::_OnOk();
}

void cOptionInterface::_OnCancel()
{
	g_pResist->m_Global.s_bMyRenderName = (bool)FindCombo( NAMEVIEW_SELF )->s_nBackupData;
	g_pResist->m_Global.s_bOtherRenderName = (bool)FindCombo( NAMEVIEW_OTHER )->s_nBackupData;
	g_pResist->m_Global.s_bMonsterRenderName = (bool)FindCombo( NAMEVIEW_MONSTER )->s_nBackupData;
	g_pResist->m_Global.s_bNpcRenderName = (bool)FindCombo( NAMEVIEW_NPC )->s_nBackupData;		

	g_pResist->m_Global.s_bViewHpBar = (bool)FindCombo( HPVIEW )->s_nBackupData;

	g_pResist->m_Global.s_bFigureTamer = (bool)FindCombo( FIGURE_TAMER )->s_nBackupData;
	g_pResist->m_Global.s_bFigureDigimon = (bool)FindCombo( FIGURE_DIGIMON )->s_nBackupData;
	g_pResist->m_Global.s_bFigureEmployment = (bool)FindCombo( FIGURE_EMPLOYMENT )->s_nBackupData;

	g_pResist->m_Global.s_bChatBalloone = (bool)FindCombo( BALLOONE_CHAT )->s_nBackupData;
	g_pResist->m_Global.s_bTipBalloone = (bool)FindCombo( BALLOONE_TIP )->s_nBackupData;;
	g_pResist->m_Global.s_bDigimonBalloone = (bool)FindCombo( BALLOONE_DIGIMON )->s_nBackupData;

	cOptionBase::_OnCancel();
}

void cOptionInterface::_OnDefault()
{
	g_pResist->m_Global.ResetInterface();

	_DeleteControl();
	_InitInterfaceControl();
}


void cOptionInterface::Combo_SelChange( sOP_COMBO* pCR )
{
	switch( pCR->s_nValue )
	{
	case NAMEVIEW_SELF:			g_pResist->m_Global.s_bMyRenderName = (bool)pCR->s_pCombo->GetCurSel_Data();			break;
	case NAMEVIEW_OTHER:		g_pResist->m_Global.s_bOtherRenderName = (bool)pCR->s_pCombo->GetCurSel_Data();			break;		
	case NAMEVIEW_MONSTER:		g_pResist->m_Global.s_bMonsterRenderName = (bool)pCR->s_pCombo->GetCurSel_Data();		break;
	case NAMEVIEW_NPC:			g_pResist->m_Global.s_bNpcRenderName = (bool)pCR->s_pCombo->GetCurSel_Data();			break;	
	case HPVIEW:				g_pResist->m_Global.s_bViewHpBar = (bool)pCR->s_pCombo->GetCurSel_Data();				break;
	case FIGURE_TAMER:			g_pResist->m_Global.s_bFigureTamer = (bool)pCR->s_pCombo->GetCurSel_Data();				break;
	case FIGURE_DIGIMON:		g_pResist->m_Global.s_bFigureDigimon = (bool)pCR->s_pCombo->GetCurSel_Data();			break;
	case FIGURE_EMPLOYMENT:		g_pResist->m_Global.s_bFigureEmployment = (bool)pCR->s_pCombo->GetCurSel_Data();		break;
	case BALLOONE_CHAT:			g_pResist->m_Global.s_bChatBalloone = (bool)pCR->s_pCombo->GetCurSel_Data();			break;
	case BALLOONE_DIGIMON:		g_pResist->m_Global.s_bDigimonBalloone = (bool)pCR->s_pCombo->GetCurSel_Data();			break;
	case BALLOONE_TIP:			g_pResist->m_Global.s_bTipBalloone = (bool)pCR->s_pCombo->GetCurSel_Data();				break;

#ifdef USE_BARCODE_REDER
	case USE_BARCODE:
		if( nsBARCODE::IsInstallDrive() == false )
		{
			cPrintMsg::PrintMsg( 20038 );
			g_pResist->m_Global.s_bUseBarcode = false;
			FindCombo( USE_BARCODE )->s_pCombo->SetCurSel_FromData( (int)g_pResist->m_Global.s_bUseBarcode );
			GAME_EVENT_ST.OnEvent( EVENT_CODE::USE_GAMESCANNER, &g_pResist->m_Global.s_bUseBarcode );
		}
		break;
#endif

	case CENTER_BAR:
		g_pResist->m_CenterBar.s_bInitial = (BOOL)FindCombo( CENTER_BAR )->s_pCombo->GetCurSel_Data();
		FindSlider( CENTER_BAR_ALPHA )->s_pSlider->Enable( g_pResist->m_CenterBar.s_bInitial );		
		break;
	case QUICK_OPEN:
		{
			bool bOpend = (bool)FindCombo( QUICK_OPEN )->s_pCombo->GetCurSel_Data();
			g_pResist->m_CenterBar.SetQuickSlotOpenClose( bOpend );
			//g_pResist->m_CenterBar.s_bIsOpenQuick = (BOOL)FindCombo( QUICK_OPEN )->s_pCombo->GetCurSel_Data();
		}
		break;

		// 종료시 처리
#ifdef LANGAUGE_SELECT
	case LANGUAGE:
#endif
	case MOUSE_HAND:
	case CAMERA_ENEMY_CRITICAL:
	case TRADE_ACTIVE:
	case PARTY_ACTIVE:
	case DUAL_ACTIVE:
	case VIEW_LEVELEFFECT:
	case VIEW_COSTUME:
	case VIEW_ATTACK_EFFECT:
	case RIGHT_TOOLTIP:
	case BOSS_WARNING:	
	case CAMERA_MODE:
	case CAMERA_DIST:
#ifdef GUILD_INVITATION
	case GUILD_INVITATE:
#endif
		break;
	default:
		assert_cs( false );
	}
}


#pragma warning( default : 4800 )