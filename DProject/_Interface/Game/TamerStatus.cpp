
#include "stdafx.h"
#include "TamerStatus.h"

cTamerStatus::cTamerStatus()
{
#ifdef USE_BARCODE_REDER
	m_pBtnBarcodeResist = NULL;
	m_pBtnBarcodeUnResist = NULL;
	m_pCurBtnBarcode = NULL;
	m_pBarcodeBG = NULL;
	m_BarcodeState = 0;
#endif

	m_pCheckDeleteDigimon = NULL;
	m_pEditResistNumber = NULL;
	m_pDeleteDigimonCloseBtn = NULL;
	m_pDeleteDigimonOkBtn = NULL;
#ifdef CROSSWARS_SYSTEM
	m_pSelectInfo = NULL;
	m_pCrossInfoArray = NULL;
	m_pRenderDigimon = NULL;
#endif

}

void cTamerStatus::Destroy()
{
	cBaseWindow::Delete();	
}

void cTamerStatus::DeleteResource()
{
	DeleteScript();

	SAFE_NIDELETE( m_pTabMenu );	

#ifdef USE_BARCODE_REDER
	_DeleteBarcode();
#endif
	_DeleteEquip();
	_DeleteDigivice();
	_DeleteSkill();
#ifdef CROSSWARS_SYSTEM
	_DeleteCross();
	cCreateName::DeleteInstance( cBaseWindow::WT_MAKE_CROSSTACTICS, 0 );
#endif
}

void cTamerStatus::Create(int nValue /* = 0  */)
{
	SetRootClient( CsPoint( 100, 100 ) );
	_CreateEquip();
	_CreateDigivice();
	_CreateSkill();
#ifdef CROSSWARS_SYSTEM
	_CreateCross();
#endif

	cBaseWindow::Init();
	cCheckBox::eIMAGE_TYPE eCheckBoxType;//탭 타입 (UI리뉴얼 관련)
	InitScript( NULL, m_ptRootClient, CsPoint( 255, 424 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );
	m_pCancelButton = AddButton( CsPoint( 235, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	

	eCheckBoxType = cCheckBox::IMAGE_RADIO_1;
	CsPoint ptTabStart = CsPoint( 1, 3);

	_InitEquip();
	_InitDigivice();
	_InitSkill();
#ifdef CROSSWARS_SYSTEM
	_InitCross();
	bool bCrossLoader = g_pDataMng->GetInvenCross()->IsCrossLoader();
#else
	bool bCrossLoader = false;
#endif
#ifdef USE_BARCODE_REDER
	_InitBarcode();
#endif

	m_pTabMenu = NiNew cRadioButton;

	if(bCrossLoader)
	{
		m_pTabMenu->Init( m_pRoot, ptTabStart, CsPoint( 55, 23 ), eCheckBoxType, UISTRING_TEXT( "COMMON_TXT_TAMER" ).c_str(), false );
		m_pTabMenu->AddNode( CsPoint( ptTabStart.x + 55, ptTabStart.y ), CsPoint( 65, 23 ), eCheckBoxType, UISTRING_TEXT( "COMMON_TXT_DIGIVICE" ).c_str() );
		m_pTabMenu->AddNode( CsPoint( ptTabStart.x + 120, ptTabStart.y ), CsPoint( 45, 23 ), eCheckBoxType, UISTRING_TEXT( "COMMON_TXT_SKILL" ).c_str() );
		m_pTabMenu->AddNode( CsPoint( ptTabStart.x + 165, ptTabStart.y ), CsPoint( 65, 23 ), eCheckBoxType, UISTRING_TEXT( "COMMON_TXT_CROSSLOADER" ).c_str() );
	}
	else
	{
		m_pTabMenu->Init( m_pRoot, ptTabStart, CsPoint( 70, 23 ), eCheckBoxType, UISTRING_TEXT( "COMMON_TXT_TAMER" ).c_str(), false );
		m_pTabMenu->AddNode( CsPoint( ptTabStart.x + 70, ptTabStart.y ), CsPoint( 70, 23 ), eCheckBoxType, UISTRING_TEXT( "COMMON_TXT_DIGIVICE" ).c_str() );
		m_pTabMenu->AddNode( CsPoint( ptTabStart.x + 140, ptTabStart.y ), CsPoint( 70, 23 ), eCheckBoxType, UISTRING_TEXT( "COMMON_TXT_SKILL" ).c_str() );
	}
	m_pTabMenu->SetCheckIndex( 0 );
}

void cTamerStatus::ResetName()
{
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	m_pNameText->SetText( pUser->GetName() );
	cCommunityContent::sGUILD_INFO guildInfo;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDINFO, &guildInfo );
	m_pGuildText->SetText( guildInfo.s_szGuildName.c_str() );
}

void cTamerStatus::Update(float const& fDeltaTime)
{
	switch( m_pTabMenu->GetCheckIndex() )
	{
	case 0:
		_Update_Equip();
		break;
	case 1:		
		break;
	case 2:
		break;
#ifdef CROSSWARS_SYSTEM
	case 3:
		_Update_Cross();
		break;
#endif
	}

#ifdef USE_BARCODE_REDER
	_UpdateBarcode();
#endif
	_UpdateMoveWindow();
#ifdef CROSSWARS_SYSTEM
	if(3 == m_pTabMenu->GetCheckIndex() && m_bViewHouse){
		__SetRootSize(CsPoint(IF_CROSSLOADER_WIDTH,IF_CROSSLOADER_HEIGHT)+CsPoint( IF_CROSSINVEN_WIDTH, 0 ));
	}

	cCreateName::UpdateInstance( cBaseWindow::WT_MAKE_CROSSTACTICS, 0 );
#endif

}

void cTamerStatus::SetTab( int nTab )
{
	m_pTabMenu->SetCheckIndex( nTab ); 
}

cBaseWindow::eMU_TYPE
cTamerStatus::Update_ForMouse()
{
	if( _UpdateDigimonDel() == true )
		return cBaseWindow::MUT_CLICK_FOCUS;

#ifdef CROSSWARS_SYSTEM
	cBaseWindow::eMU_TYPE muReturn;

	muReturn = cCreateName::Update_ForMouseInstance( cBaseWindow::WT_MAKE_CROSSTACTICS, 0 );
	if( muReturn != MUT_OUT_WINDOW )
		return muReturn;

	muReturn = cBaseWindow::Update_ForMouse();
#else
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;	
#endif

	int nRadioAction;
	if( (nRadioAction = m_pTabMenu->Update_ForMouse()) != cRadioButton::INVALIDE_RADIO_CLICK )
	{
		if( nRadioAction == cRadioButton::ACTION_CLICK )
		{
			cWindow::PlaySound( cWindow::SD_At2 );		
		}
		return muReturn;
	}

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

	switch( m_pTabMenu->GetCheckIndex() )
	{
	case 0:
		if( _UpdateIcon_ForMouse_Equip() == true )
			return muReturn;
		break;
	case 1:
		if( _UpdateIcon_ForMouse_Digivice() == true )
			return muReturn;
		break;
	case 2:
		if( _UpdateIcon_ForMouse_Skill() == true )
			return muReturn;
		break;
#ifdef CROSSWARS_SYSTEM
	case 3://크로스로더 마우스업데이트
		if( m_bViewHouse == true )
		{
			CsPoint ptLocal = MousePosToWindowPos( CURSOR_ST.GetPos() );
			if( ptLocal.x <= _GetCrossLoaderSize().x )
			{
				if( muReturn == MUT_OUT_WINDOW )	
					return muReturn;
				if( _UpdateIcon_ForMouse_CrossLoader() == true )
					return muReturn;		
			}
			else
			{
				return _UpdateIcon_ForMouse_CrossInven();
			}		
		}
		else
			if( _UpdateIcon_ForMouse_CrossLoader() == true )
				return muReturn;		
		break;
#endif
	}	

#ifdef USE_BARCODE_REDER
	if( _UpdateBarcode_ForMouse() == true )
		return muReturn;
#endif

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 255, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();
	return muReturn;
}

void cTamerStatus::Render()
{
	switch( m_pTabMenu->GetCheckIndex() )
	{
	case 0:
		_RenderEquip();
		break;
	case 1:
		_RenderDigivice();
		break;
	case 2:
		_RenderSkill();
		break;
#ifdef CROSSWARS_SYSTEM
	case 3:
		_RenderCross();
		break;
#endif
	}

	RenderScript();
	m_pTabMenu->Render();
#ifdef USE_BARCODE_REDER
	_RenderBarcode();
#endif

	_RenderDeleteDigimon();
#ifdef CROSSWARS_SYSTEM
	cCreateName::RenderInstance( cBaseWindow::WT_MAKE_CROSSTACTICS, 0 );
#endif
}

void cTamerStatus::OnMouseOutWindow()
{
	cBaseWindow::OnMouseOutWindow();

	switch( m_pTabMenu->GetCheckIndex() )
	{
	case 0:
		_OnMouseOutWindow_Equip();
		return;
	case 1:
		_OnMouseOutWindow_Digivice();
		return;
	case 2:
		_OnMouseOutWindow_Skill();
		return;
#ifdef CROSSWARS_SYSTEM
	case 3:
		_OnMouseOutWindow_Cross();
		return;
#endif
	}
}

void cTamerStatus::OnLButtonUp( CsPoint pos )
{	
	switch( m_pTabMenu->GetCheckIndex() )
	{
	case 0:
		_OnLButtonUp_Equip( pos );
		return;
	case 1:
		_OnLButtonUp_Digivice( pos );
		return;
	case 2:
		_OnLButtonUp_Skill( pos );
		return;
#ifdef CROSSWARS_SYSTEM
	case 3:
		_OnLButtonUp_Cross( pos );
		return;
#endif
	}
	assert_cs( false );
}

void cTamerStatus::OnRButtonUp( CsPoint pos )
{
	switch( m_pTabMenu->GetCheckIndex() )
	{
	case 0:
		_OnRButtonUp_Equip( pos );
		return;
	case 1:
		_OnRButtonUp_Digivice( pos );
		return;
	case 2:
		_OnRButtonUp_Skill( pos );
		return;
#ifdef CROSSWARS_SYSTEM
	case 3:
		_OnRButtonUp_Cross( pos );
		return;
#endif
	}
	assert_cs( false );
}

bool cTamerStatus::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	switch( m_pTabMenu->GetCheckIndex() )
	{
	case 0:
		return _CursorIconLBtnUp_Equip( eIconType, nIconSlot );
	case 1:
		return _CursorIconLBtnUp_Digivice( eIconType, nIconSlot );
	case 2:
		return _CursorIconLBtnUp_Skill( eIconType, nIconSlot );
#ifdef CROSSWARS_SYSTEM
	case 3:
		return _CursorIconLBtnUp_Cross( eIconType, nIconSlot, nIconCount);
#endif
	}

	assert_cs( false );
	return false;
}

void cTamerStatus::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();

	if( m_pTabMenu )
		m_pTabMenu->ResetDevice();

#ifdef USE_BARCODE_REDER
	_ResetDevice_Barcode();
#endif
	_ResetDevice_Equip();
	_ResetDevice_Digivice();
	_ResetDevice_Skill();
#ifdef CROSSWARS_SYSTEM
	_ResetDevice_Cross();
#endif
}

void cTamerStatus::ResetDevice()
{	
	ResetDeviceScript();

	if( m_pTabMenu )
		m_pTabMenu->ResetDevice();

#ifdef USE_BARCODE_REDER
	_ResetDevice_Barcode();
#endif
	_ResetDevice_Equip();
	_ResetDevice_Digivice();
	_ResetDevice_Skill();
#ifdef CROSSWARS_SYSTEM
	_ResetDevice_Cross();
#endif
}