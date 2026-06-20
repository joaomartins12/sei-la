

#include "stdafx.h"
#include "OptionGraphic.h"

#pragma warning( disable : 4800 )

#define IF_OG_SIGHT_RATE		0.3f

void cOptionGraphic::Destroy()
{
	cOptionBase::Delete();
}

void cOptionGraphic::DeleteResource()
{
	cOptionBase::DeleteResource();

	SAFE_NIDELETE( m_pDefaultSet );
}

void cOptionGraphic::Create(int nValue /* = 0  */)
{
	if( cOptionBase::CreateBase( UISTRING_TEXT( "OPTION_GRAPHIC_TITLE_GRAPHIC_SETTING" ).c_str(), nValue ) == false )
		return;

	CsPoint ptGraphicSpecPos( 60, 44 );
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	ti.s_eTextAlign = DT_RIGHT;
	ti.SetText( UISTRING_TEXT( "OPTION_GRAPHIC_LOW" ).c_str() );
	AddText( &ti, ptGraphicSpecPos );
	ti.SetText( UISTRING_TEXT( "OPTION_GRAPHIC_GENERAL" ).c_str() );
	AddText( &ti, ptGraphicSpecPos + CsPoint( 100, 0 ) );
	ti.SetText( UISTRING_TEXT( "OPTION_GRAPHIC_ADVANCED" ).c_str() );
	AddText( &ti, ptGraphicSpecPos + CsPoint( 200, 0 ) );

	m_pDefaultSet = NiNew cRadioButton;
	m_pDefaultSet->Init( m_pRoot, ptGraphicSpecPos + CsPoint( 5, -1 ), CsPoint( 17, 17 ), CsPoint( 17, 0 ), "MainOption\\Graphic\\\\Option_Graphic_Check.tga", false );
	m_pDefaultSet->AddNode( ptGraphicSpecPos + CsPoint( 105, -1 ), CsPoint( 17, 17 ), CsPoint( 17, 0 ), "MainOption\\Graphic\\\\Option_Graphic_Check.tga" );
	m_pDefaultSet->AddNode( ptGraphicSpecPos + CsPoint( 205, -1 ), CsPoint( 17, 17 ), CsPoint( 17, 0 ), "MainOption\\Graphic\\\\Option_Graphic_Check.tga" );
	m_nOptionDefIndex = g_pResist->m_Global.s_nOptionDefIndex;
	m_pDefaultSet->SetCheckIndex( g_pResist->m_Global.s_nOptionDefIndex );


	TCHAR sz[ IF_OP_COMBO_STR_MAX_COUNT ][ IF_OP_COMBO_STR_MAX_LEN ];
	NiColor color[ IF_OP_COMBO_STR_MAX_COUNT ];
	int nData[ IF_OP_COMBO_STR_MAX_COUNT ];

	//=============================================================================================================
	//		화면
	//=============================================================================================================
	_AddControl_Title( UISTRING_TEXT( "OPTION_GRAPHIC_TITLE_DISPLAY" ).c_str() );

	// 윈도우 모드
	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_WINDOW" ).c_str() );
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_FULL_SCREEN" ).c_str() );
	color[ 0 ] = FONT_WHITE;
	color[ 1 ] = FONT_WHITE;
	nData[ 0 ] = 0;
	nData[ 1 ] = 1;
	_AddControl_Combo( UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_MODE" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bFullScreen, VIEW_MODE );

	// 해상도
	int nCount = g_pResolutionInfo->GetResolutionCount();
	if( nCount > IF_OP_COMBO_STR_MAX_COUNT )
		nCount = IF_OP_COMBO_STR_MAX_COUNT;

	D3DDISPLAYMODE* pMode = NULL;
	for( int i=0; i<nCount; ++i )
	{
		pMode = g_pResolutionInfo->GetResolution( i );
		_stprintf_s( sz[ i ], MAX_FILENAME, _T( "%d x %d" ), pMode->Width, pMode->Height );
		color[ i ] = FONT_WHITE;
		nData[ i ] = i;
	}
	int nSelData = g_pResolutionInfo->FindResolutionIndex( g_pResist->m_Global.s_nResolutionWidth, g_pResist->m_Global.s_nResolutionHeight );
	_AddControl_Combo( UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_RESOLUTION" ).c_str(), nCount, sz, color, nData, nSelData, VIEW_RESOLUTION );

	//=============================================================================================================
	//		밝기
	//=============================================================================================================	

	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_OFF" ).c_str() );
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_GRAPHIC_SPECIFY_USER" ).c_str() );
	nData[ 0 ] = 0;
	nData[ 1 ] = 1;
	_AddControl_Combo( UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_BRIGHTNESS" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bUseBright, USE_BRIGHT );

	// 케릭터
	_AddControl_Slider( UISTRING_TEXT( "OPTION_GRAPHIC_SLIDER_DISPLAY_BRIGHTNESS" ).c_str(), CsPoint( 0, 200 ), 6, (int)(g_pResist->m_Global.s_fBright*100.0f), 
		UISTRING_TEXT( "OPTION_GRAPHIC_SLIDER_DARK" ).c_str(), UISTRING_TEXT( "OPTION_GRAPHIC_SLIDER_BRIGHT" ).c_str(), GAME_BRIGHT );
	FindSlider( GAME_BRIGHT )->s_pSlider->Enable( (int)g_pResist->m_Global.s_bUseBright );

	//=============================================================================================================
	// 포스트이펙트
	//=============================================================================================================
	_AddControl_Title( UISTRING_TEXT( "OPTION_GRAPHIC_TITLE_POST_EFFECT" ).c_str() );
	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_OFF" ).c_str() );
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_ON" ).c_str() );
	nData[ 0 ] = 0;
	nData[ 1 ] = 1;
	_AddControl_Combo( UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_BLUR_EFFECT" ).c_str(), 2, sz, color, nData, g_pResist->m_Global.s_nPostEffect, POST_EFFECT_BLUR );

	//=============================================================================================================
	// 설정
	//=============================================================================================================
	_AddControl_Title( UISTRING_TEXT( "OPTION_GRAPHIC_TITLE_CONFIG" ).c_str() );

	// 텍스쳐 디테일
	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_MAX" ).c_str() );
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_HIGH" ).c_str() );
	_tcscpy_s( sz[ 2 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_MIDDLE" ).c_str() );
	_tcscpy_s( sz[ 3 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_MIDDLE_LOW" ).c_str() );
	_tcscpy_s( sz[ 4 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_LOW" ).c_str() );
	_tcscpy_s( sz[ 5 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_MIN" ).c_str() );

	color[ 0 ] = FONT_WHITE;
	color[ 1 ] = FONT_WHITE;
	color[ 2 ] = FONT_WHITE;
	color[ 3 ] = FONT_WHITE;
	color[ 4 ] = FONT_WHITE;
	color[ 5 ] = FONT_WHITE;

	nData[ 0 ] = NiTexturingProperty::FILTER_TRILERP;
	nData[ 1 ] = NiTexturingProperty::FILTER_NEAREST_MIPLERP;
	nData[ 2 ] = NiTexturingProperty::FILTER_BILERP_MIPNEAREST;
	nData[ 3 ] = NiTexturingProperty::FILTER_NEAREST_MIPNEAREST;
	nData[ 4 ] = NiTexturingProperty::FILTER_BILERP;
	nData[ 5 ] = NiTexturingProperty::FILTER_NEAREST;

	_AddControl_Combo( UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_TEXTURE_DETAIL" ).c_str(), 5, sz, color, nData, g_pResist->m_Global.s_eTexFilter, TEX_DETAIL );

	// 그림자
	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_HIDE" ).c_str() );
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_SEE" ).c_str() );
	color[ 0 ] = NiColor( 240/255.0f, 0/255.0f, 51/255.0f );
	color[ 1 ] = FONT_WHITE;
	nData[ 0 ] = 0;
	nData[ 1 ] = 1;
	_AddControl_Combo( UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_SHADOW" ).c_str(), 2, sz, color, nData, g_pResist->m_Global.s_nShadowType, SHADOW );

	// 날씨효과
	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_OFF" ).c_str() );
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_LOW" ).c_str() );
	_tcscpy_s( sz[ 2 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_MIDDLE" ).c_str() );
	_tcscpy_s( sz[ 3 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_HIGH" ).c_str() );	
	color[ 0 ] = NiColor( 240/255.0f, 0/255.0f, 51/255.0f );
	color[ 1 ] = FONT_WHITE;
	color[ 2 ] = FONT_WHITE;
	color[ 3 ] = FONT_WHITE;
	nData[ 0 ] = 3;
	nData[ 1 ] = 0;
	nData[ 2 ] = 1;
	nData[ 3 ] = 2;	
	_AddControl_Combo( UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_WEATHER" ).c_str(), 4, sz, color, nData, g_pResist->m_Global.s_nWeather, WEATHER );

	// 안티 얼라이싱
	nCount = 1;
	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_OFF" ).c_str() );
	color[ 0 ] = NiColor( 240/255.0f, 0/255.0f, 51/255.0f );
	nData[ 0 ] = NiDX9Renderer::FBMODE_DEFAULT;
	if( g_pResolutionInfo->IsMultisample( NiDX9Renderer::FBMODE_MULTISAMPLES_2 ) )
	{
		std::wstring wsLevel = UISTRING_TEXT( "COMMON_TXT_NUM_LEVEL" );
		DmCS::StringFn::Replace( wsLevel, L"#Num#", 2 );
		_tcscpy_s( sz[ nCount ], IF_OP_COMBO_STR_MAX_LEN, wsLevel.c_str() );
		color[ nCount ] = FONT_WHITE;
		nData[ nCount ] = NiDX9Renderer::FBMODE_MULTISAMPLES_2;
		++nCount;
	}
	if( g_pResolutionInfo->IsMultisample( NiDX9Renderer::FBMODE_MULTISAMPLES_4 ) )
	{
		std::wstring wsLevel = UISTRING_TEXT( "COMMON_TXT_NUM_LEVEL" );
		DmCS::StringFn::Replace( wsLevel, L"#Num#", 4 );
		_tcscpy_s( sz[ nCount ], IF_OP_COMBO_STR_MAX_LEN, wsLevel.c_str() );
		color[ nCount ] = FONT_WHITE;
		nData[ nCount ] = NiDX9Renderer::FBMODE_MULTISAMPLES_4;
		++nCount;
	}
	if( g_pResolutionInfo->IsMultisample( NiDX9Renderer::FBMODE_MULTISAMPLES_6 ) )
	{
		std::wstring wsLevel = UISTRING_TEXT( "COMMON_TXT_NUM_LEVEL" );
		DmCS::StringFn::Replace( wsLevel, L"#Num#", 6 );
		_tcscpy_s( sz[ nCount ], IF_OP_COMBO_STR_MAX_LEN, wsLevel.c_str() );
		color[ nCount ] = FONT_WHITE;
		nData[ nCount ] = NiDX9Renderer::FBMODE_MULTISAMPLES_6;
		++nCount;
	}
	if( g_pResolutionInfo->IsMultisample( NiDX9Renderer::FBMODE_MULTISAMPLES_8 ) )
	{
		std::wstring wsLevel = UISTRING_TEXT( "COMMON_TXT_NUM_LEVEL" );
		DmCS::StringFn::Replace( wsLevel, L"#Num#", 8 );
		_tcscpy_s( sz[ nCount ], IF_OP_COMBO_STR_MAX_LEN, wsLevel.c_str() );
		color[ nCount ] = FONT_WHITE;
		nData[ nCount ] = NiDX9Renderer::FBMODE_MULTISAMPLES_8;
		++nCount;
	}
	_AddControl_Combo( UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_ANTI_ALIASING" ).c_str(), nCount, sz, color, nData, g_pResist->m_Global.s_nAntialicing, ANTIALICING );

	// 케릭터 아웃라인
	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_OFF" ).c_str() );
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_ON" ).c_str() );
	color[ 0 ] = NiColor( 240/255.0f, 0/255.0f, 51/255.0f );
	color[ 1 ] = FONT_WHITE;
	nData[ 0 ] = 0;
	nData[ 1 ] = 1;
	_AddControl_Combo( UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_CARTOON_RENDERING" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bCharOutLine, CATOON );

	// 풀 표시
	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_GRASS_INVISIBLE" ).c_str() );
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_GRASS_VISIBLE" ).c_str() );
	color[ 0 ] = NiColor( 240/255.0f, 0/255.0f, 51/255.0f );
	color[ 1 ] = FONT_WHITE;
	_AddControl_Combo( UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_CONFIGURE_GRASS" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bCell, CELL_OBJECT );

	// 수직 동기화
	_tcscpy_s( sz[ 0 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_OFF" ).c_str() );
	_tcscpy_s( sz[ 1 ], IF_OP_COMBO_STR_MAX_LEN, UISTRING_TEXT( "OPTION_SETTING_ON" ).c_str() );
	color[ 0 ] = NiColor( 240/255.0f, 0/255.0f, 51/255.0f );
	color[ 1 ] = FONT_WHITE;
	_AddControl_Combo( UISTRING_TEXT( "OPTION_GRAPHIC_COMBOBOX_VERTICAL_SYNC" ).c_str(), 2, sz, color, nData, (int)g_pResist->m_Global.s_bHSync, H_SYNC );

// 	// 확장 쉐이더 사용
// 	color[ 0 ] = NiColor( 240/255.0f, 0/255.0f, 51/255.0f );
// 	color[ 1 ] = FONT_WHITE;
// 	_AddControl_Combo( _LAN( "확장 쉐이더 사용" ), 2, sz, color, nData, (int)g_pResist->m_Global.s_bUseMyShader, USE_MY_SHADER );


	//=============================================================================================================
	//		시야
	//=============================================================================================================
	_AddControl_Title( UISTRING_TEXT( "OPTION_GRAPHIC_TITLE_VIEW" ).c_str() );
	CsPoint ptSliderRange = CsPoint( (int)( ( 1.0f - IF_OG_SIGHT_RATE )*100.0f ), 100 );

	// 배경
	_AddControl_Slider( UISTRING_TEXT( "OPTION_GRAPHIC_SLIDER_BG_VIEW" ).c_str(), ptSliderRange, 6, (int)(g_pResist->m_Global.s_fObjectSightRate*100.0f), 
		UISTRING_TEXT( "OPTION_GRAPHIC_SLIDER_LOW" ).c_str(), UISTRING_TEXT( "OPTION_GRAPHIC_SLIDER_HIGH" ).c_str(), TR_SIGHT );

	_ResizeScrollBar();
}

void cOptionGraphic::Update(float const& fDeltaTime)
{
	cOptionBase::Update(fDeltaTime);

	g_pResist->m_Global.s_fObjectSightRate = CsMin( 1.0f, FindSlider( TR_SIGHT )->s_pSlider->GetCurPosIndex()*0.01f );
	float fRate = g_pResist->m_Global.s_fObjectSightRate;
	nsCsGBTerrain::g_pCurRoot->GetInfo()->s_fClipObject_Far = fRate*nsCsGBTerrain::g_pCurRoot->GetOrgClipObject_Far();
	nsCsGBTerrain::g_pCurRoot->GetInfo()->s_fFogStart = fRate*nsCsGBTerrain::g_pCurRoot->GetOrgFogObject_Start();
	nsCsGBTerrain::g_pCurRoot->GetInfo()->s_fFogEndObject = fRate*nsCsGBTerrain::g_pCurRoot->GetOrgFogObject_End();
	nsCsGBTerrain::g_pCurRoot->CalFogDateValue();	

	if( g_pResist->m_Global.s_bUseBright )
	{
		g_pResist->m_Global.s_fBright = CsMin( 2.0f, FindSlider( GAME_BRIGHT )->s_pSlider->GetCurPosIndex()*0.01f );	
		g_pGameIF->SetBright();
	}	
}

cBaseWindow::eMU_TYPE
cOptionGraphic::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	switch( m_pDefaultSet->Update_ForMouse() )
	{
	case cCheckBox::ACTION_CLICK:
		_OnDefault( m_pDefaultSet->GetCheckIndex() );
	case cCheckBox::ACTION_DOWN:
	case cRadioButton::ANI_RADIO_PRESS:
		return muReturn;
	}

	return cOptionBase::Update_ForMouse();
}

void cOptionGraphic::Render()
{
	cOptionBase::Render();
	m_pDefaultSet->Render();
}

void cOptionGraphic::ResetDevice()
{
	cOptionBase::ResetDevice();
	m_pDefaultSet->ResetDevice();
}

void cOptionGraphic::OnMoveWindow()
{
	cOptionBase::OnMoveWindow();
	m_pDefaultSet->ResetDevice();
}

void cOptionGraphic::_OnOk()
{
	//==================================================================================================
	// 해상도
	//==================================================================================================
	// 해상도 변경 되었으면 변경 적용
	if( FindCombo( VIEW_RESOLUTION ) && FindCombo( VIEW_RESOLUTION )->IsChange() )
	{
		// 풀스크린 모드도 받아 놓는다.
		g_pResist->m_Global.s_bFullScreen = (bool)FindCombo( VIEW_MODE )->s_pCombo->GetCurSel_Data();

		D3DDISPLAYMODE* pMode = g_pResolutionInfo->GetResolution( FindCombo( VIEW_RESOLUTION )->s_pCombo->GetCurSel_Data() );
		g_pResist->m_Global.s_nResolutionWidth = pMode->Width;
		g_pResist->m_Global.s_nResolutionHeight = pMode->Height;		
		g_pEngine->ChangeResolution();

		for (int i = 0; i < cQuickSlot::QUICKSLOT_MAXCNT; i++)
		{
			if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_QUICKSLOT, i))
			{
				g_pGameIF->CloseDynamicIF(cBaseWindow::WT_QUICKSLOT, i);
				g_pGameIF->GetDynamicIF(cBaseWindow::WT_QUICKSLOT, i, i);
				g_pGameIF->GetQuickSlot(i)->ChangeMacroKey();
			}
		}

		if(g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUICKEVOL ) )
		{
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUICKEVOL );
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_QUICKEVOL, 0, 2 );
			g_pGameIF->GetQuickEvol()->SetTargetObject( g_pCharMng->GetDigimonUser(0) );
		}
#ifdef SIMPLEFICATION
		if(g_pGameIF->IsActiveWindow( cBaseWindow::WT_SIMPLE_BTN ) )
		{
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_SIMPLE_BTN );
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_SIMPLE_BTN );
		}
#endif

		if(g_pGameIF->IsActiveWindow(WT_OPTION_USERGRADE))
		{
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_OPTION_USERGRADE );
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_OPTION_USERGRADE );
		}
		if(g_pGameIF->IsActiveWindow( cBaseWindow::WT_SMALLMAP))
		{
			//[DM_KOR] #405
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_SMALLMAP );
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_SMALLMAP );
		}

		for( int i=0; i<2; ++i )
		{
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_CENTERBAR, i );
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_CENTERBAR, i, i );
		}
	}
	// 풀스크린만 변경되었으면 토글 시켜준다.
	else if( FindCombo( VIEW_MODE ) && FindCombo( VIEW_MODE )->IsChange() )
	{
		g_pResist->m_Global.s_bFullScreen = (bool)FindCombo( VIEW_MODE )->s_pCombo->GetCurSel_Data();
		g_pEngine->ToggleWindow( g_pResist->m_Global.s_bFullScreen );

		for( int i = 0 ; i < cQuickSlot::QUICKSLOT_MAXCNT ; i++ )
		{
			if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_QUICKSLOT, i))
			{
				g_pGameIF->CloseDynamicIF(cBaseWindow::WT_QUICKSLOT, i);
				g_pGameIF->GetDynamicIF(cBaseWindow::WT_QUICKSLOT, i, i);
				g_pGameIF->GetQuickSlot(i)->ChangeMacroKey();
			}
		}
		if(g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUICKEVOL ) )
		{
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUICKEVOL );
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_QUICKEVOL, 0, 2 );
			g_pGameIF->GetQuickEvol()->SetTargetObject( g_pCharMng->GetDigimonUser(0) );
		}
#ifdef SIMPLEFICATION
		if(g_pGameIF->IsActiveWindow( cBaseWindow::WT_SIMPLE_BTN ) )
		{
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_SIMPLE_BTN );
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_SIMPLE_BTN );
		}
#endif
		if(g_pGameIF->IsActiveWindow(WT_OPTION_USERGRADE))
		{
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_OPTION_USERGRADE );
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_OPTION_USERGRADE );
		}

		if(g_pGameIF->IsActiveWindow( cBaseWindow::WT_SMALLMAP))
		{
			//[DM_KOR] #405
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_SMALLMAP );
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_SMALLMAP );
		}
	}

	//==================================================================================================
	// 포스트 이펙트
	//==================================================================================================
	if( FindCombo( POST_EFFECT_BLUR ) && FindCombo( POST_EFFECT_BLUR )->IsChange() )
	{
		g_pResist->m_Global.s_nPostEffect = FindCombo( POST_EFFECT_BLUR )->s_pCombo->GetCurSel_Data();
		switch( g_pResist->m_Global.s_nPostEffect )
		{
		case 0:		g_pEngine->SetPostEffect( CEngine::PE_NONE );				break;
		case 1:		g_pEngine->SetPostEffect( CEngine::PE_GAUSSIAN_BLUR );		break;
		}
	}

	//==================================================================================================
	// 텍스쳐 디테일
	//==================================================================================================
	if( FindCombo( TEX_DETAIL ) && FindCombo( TEX_DETAIL )->IsChange() )
	{
		g_pResist->m_Global.s_eTexFilter = FindCombo( TEX_DETAIL )->s_pCombo->GetCurSel_Data();	

		nsCsGBTerrain::g_eTexFilter = (NiTexturingProperty::FilterMode)g_pResist->m_Global.s_eTexFilter;
		nsCsGBTerrain::g_pCurRoot->ResetTextureFilterMode();
	}

	//==================================================================================================
	// 안티 얼라이싱 & 수직 동기화
	//==================================================================================================
	if( FindCombo( ANTIALICING ) && FindCombo( ANTIALICING )->IsChange() || FindCombo( H_SYNC )->IsChange() )
	{
		g_pResist->m_Global.s_nAntialicing = FindCombo( ANTIALICING )->s_pCombo->GetCurSel_Data();
		g_pResist->m_Global.s_bHSync = (bool)FindCombo( H_SYNC )->s_pCombo->GetCurSel_Data();
		g_pEngine->ResetRenderer();
	}

	//==================================================================================================
	// 케릭터 아웃라인
	//==================================================================================================
	if( nsCsGBTerrain::g_Device.g_bUseMyShader == false )
	{
		if( FindCombo( CATOON ))
			FindCombo( CATOON )->s_pCombo->SetCurSel_FromData( (int)false );
	}
	if( FindCombo( CATOON ) && FindCombo( CATOON )->IsChange() )
	{
		g_pResist->m_Global.s_bCharOutLine = (bool)FindCombo( CATOON )->s_pCombo->GetCurSel_Data();
		nsCsGBTerrain::g_bCharOutLine = g_pResist->m_Global.s_bCharOutLine;
		g_pMngCollector->ResetObjectShader();
	}

	//==================================================================================================
	// 그림자
	//==================================================================================================
	if( nsCsGBTerrain::g_Device.g_bUseMyShader == false )
	{
		if( FindCombo( SHADOW ) )
			FindCombo( SHADOW )->s_pCombo->SetCurSel_FromData( cResist::sGLOBAL::SHADOW_OFF );
	}
	if( FindCombo( SHADOW ) && FindCombo( SHADOW )->IsChange() )
	{
		g_pResist->m_Global.s_nShadowType = FindCombo( SHADOW )->s_pCombo->GetCurSel_Data();
		nsCsGBTerrain::g_bShadowRender = ( g_pResist->m_Global.s_nShadowType == cResist::sGLOBAL::SHADOW_ON );
		nsCsGBTerrain::g_pCurRoot->ResetShader();
	}

	if( FindCombo( WEATHER ) && FindCombo( WEATHER )->IsChange() )
	{
		g_pResist->m_Global.s_nWeather = FindCombo( WEATHER )->s_pCombo->GetCurSel_Data();		
		g_pWeather->SetPerformance( g_pResist->m_Global.s_nWeather );
	}

	cOptionBase::_OnOk();
}

void cOptionGraphic::_OnCancel()
{
	// 풀표시
	g_pResist->m_Global.s_bCell = (bool)FindCombo( CELL_OBJECT )->s_nBackupData;
	nsCsGBTerrain::g_bSpeedCellRender = g_pResist->m_Global.s_bCell;

	// 오브젝트 시야
	g_pResist->m_Global.s_fObjectSightRate = CsMin( 1.0f, FindSlider( TR_SIGHT )->s_nBackupPos*0.01f );
	nsCsGBTerrain::g_pCurRoot->GetInfo()->s_fClipObject_Far = g_pResist->m_Global.s_fObjectSightRate*nsCsGBTerrain::g_pCurRoot->GetOrgClipObject_Far();
	nsCsGBTerrain::g_pCurRoot->GetInfo()->s_fFogStart = g_pResist->m_Global.s_fObjectSightRate*nsCsGBTerrain::g_pCurRoot->GetOrgFogObject_Start();
	nsCsGBTerrain::g_pCurRoot->GetInfo()->s_fFogEndObject = g_pResist->m_Global.s_fObjectSightRate*nsCsGBTerrain::g_pCurRoot->GetOrgFogObject_End();
	nsCsGBTerrain::g_pCurRoot->CalFogDateValue();

	// 확장 쉐이더
//	g_pResist->m_Global.s_bUseMyShader = (bool)FindCombo( CELL_OBJECT )->s_nBackupData;
	g_pResist->m_Global.s_bUseMyShader = true;

	// 옵션 디폴트 키
	g_pResist->m_Global.s_nOptionDefIndex = m_nOptionDefIndex;
	
	cOptionBase::_OnCancel();
}

void cOptionGraphic::_OnDefault( int nDefault )
{
	g_pResist->m_Global.s_nOptionDefIndex = nDefault;
	if( nDefault == -1 )
	{
		m_pDefaultSet->SetCheckIndex( -1 );
		return;
	}

	switch( nDefault )
	{
	case 0: // 저사양
		FindCombo( TEX_DETAIL )->s_pCombo->SetCurSel_FromData( (int)NiTexturingProperty::FILTER_BILERP_MIPNEAREST );
		FindCombo( SHADOW )->s_pCombo->SetCurSel_FromData( cResist::sGLOBAL::SHADOW_OFF );
		FindCombo( ANTIALICING )->s_pCombo->SetCurSel_FromData( FindCombo( ANTIALICING )->s_pCombo->GetData( 0 ) );
		FindCombo( CATOON )->s_pCombo->SetCurSel_FromData( (int)false );
		FindCombo( CELL_OBJECT )->s_pCombo->SetCurSel_FromData( (int)false );
		FindCombo( H_SYNC )->s_pCombo->SetCurSel_FromData( (int)false );
		FindCombo( WEATHER )->s_pCombo->SetCurSel_FromData( FindCombo( WEATHER )->s_pCombo->GetData( 1 ) );
		FindCombo( POST_EFFECT_BLUR )->s_pCombo->SetCurSel_FromData( 0 );

		FindSlider( TR_SIGHT )->s_pSlider->SetCurPosIndex( (int)( (1.0f - IF_OG_SIGHT_RATE)*100.0f ) );
		break;

	case 1: // 일반사양
		FindCombo( TEX_DETAIL )->s_pCombo->SetCurSel_FromData( (int)NiTexturingProperty::FILTER_NEAREST_MIPLERP );
		FindCombo( SHADOW )->s_pCombo->SetCurSel_FromData( cResist::sGLOBAL::SHADOW_ON );
		FindCombo( ANTIALICING )->s_pCombo->SetCurSel_FromData( FindCombo( ANTIALICING )->s_pCombo->GetData( 1 ) );
		FindCombo( CATOON )->s_pCombo->SetCurSel_FromData( (int)true );
		FindCombo( CELL_OBJECT )->s_pCombo->SetCurSel_FromData( (int)true );
		FindCombo( H_SYNC )->s_pCombo->SetCurSel_FromData( (int)true );
		FindCombo( WEATHER )->s_pCombo->SetCurSel_FromData( FindCombo( WEATHER )->s_pCombo->GetData( 2 ) );

		FindSlider( TR_SIGHT )->s_pSlider->SetCurPosIndex( (int)( (1.0f - IF_OG_SIGHT_RATE*0.3f)*100.0f ) );
		break;

	case 2: // 고사양
		FindCombo( TEX_DETAIL )->s_pCombo->SetCurSel_FromData( (int)NiTexturingProperty::FILTER_TRILERP );
		FindCombo( SHADOW )->s_pCombo->SetCurSel_FromData( cResist::sGLOBAL::SHADOW_ON );
		FindCombo( ANTIALICING )->s_pCombo->SetCurSel_FromData( FindCombo( ANTIALICING )->s_pCombo->GetData( 2 ) );
		FindCombo( CATOON )->s_pCombo->SetCurSel_FromData( (int)true );
		FindCombo( CELL_OBJECT )->s_pCombo->SetCurSel_FromData( (int)true );
		FindCombo( H_SYNC )->s_pCombo->SetCurSel_FromData( (int)true );
		FindCombo( WEATHER )->s_pCombo->SetCurSel_FromData( FindCombo( WEATHER )->s_pCombo->GetData( 3 ) );

		FindSlider( TR_SIGHT )->s_pSlider->SetCurPosIndex( 100 );
		break;
	}

	if( ( nsCsGBTerrain::g_Device.g_bPixelShader2 == false ) || ( nsCsGBTerrain::g_Device.g_bUseMyShader == false ) )
	{
		nsCsGBTerrain::g_Device.g_bUseMyShader = false;
		FindCombo( USE_MY_SHADER )->s_pCombo->SetCurSel_FromData( (int)false );
		FindCombo( CATOON )->s_pCombo->SetCurSel_FromData( (int)false );
		FindCombo( SHADOW )->s_pCombo->SetCurSel_FromData( cResist::sGLOBAL::SHADOW_OFF );
		FindSlider( GAME_BRIGHT )->s_pSlider->Enable( false );
	}

	g_pResist->m_Global.s_bCell = FindCombo( CELL_OBJECT )->s_pCombo->GetCurSel_Data();
	nsCsGBTerrain::g_bSpeedCellRender = g_pResist->m_Global.s_bCell;
}

void cOptionGraphic::Combo_SelChange( sOP_COMBO* pCR )
{
	switch( pCR->s_nValue )
	{
	case CELL_OBJECT:
		{
			g_pResist->m_Global.s_bCell = (bool)pCR->s_pCombo->GetCurSel_Data();
			nsCsGBTerrain::g_bSpeedCellRender = g_pResist->m_Global.s_bCell;
		}
		break;

		// 종료시 처리
	case VIEW_MODE:
	case VIEW_RESOLUTION:
	case POST_EFFECT_BLUR:
		return;

	case SHADOW:
		if( nsCsGBTerrain::g_Device.g_bUseMyShader == false )
		{
			FindCombo( SHADOW )->s_pCombo->SetCurSel_FromData( cResist::sGLOBAL::SHADOW_OFF );

/*			if( g_pResist->m_Global.s_bUseMyShader == false )
				cPrintMsg::PrintMsg( 20065, (TCHAR*)FindCombo( SHADOW )->s_pOptionName->GetText() );
			else*/
				cPrintMsg::PrintMsg( 20023 );

			return;
		}
		break;

	case CATOON:
		if( nsCsGBTerrain::g_Device.g_bUseMyShader == false )
		{
			FindCombo( CATOON )->s_pCombo->SetCurSel_FromData( (int)false );

/*			if( g_pResist->m_Global.s_bUseMyShader == false )
				cPrintMsg::PrintMsg( 20065, (TCHAR*)FindCombo( CATOON )->s_pOptionName->GetText() );
			else*/
				cPrintMsg::PrintMsg( 20023 );

			return;
		}
		break;

	case USE_MY_SHADER:
		g_pResist->m_Global.s_bUseMyShader = true;
//		g_pResist->m_Global.s_bUseMyShader = (bool)FindCombo( USE_MY_SHADER )->s_pCombo->GetCurSel_Data();
		cPrintMsg::PrintMsg( 20023 );
		return;

	case USE_BRIGHT:			
		{	
			g_pResist->m_Global.s_bUseBright = (bool)FindCombo( USE_BRIGHT )->s_pCombo->GetCurSel_Data();				
			FindSlider( GAME_BRIGHT )->s_pSlider->Enable( g_pResist->m_Global.s_bUseBright );			
		}
		break;

	case TEX_DETAIL:	
	case ANTIALICING:
	case H_SYNC:	
	case WEATHER:
		break;	

	default:
		assert_cs( false );
	}

	_OnDefault( -1 );	
}


#pragma warning( default : 4800 )