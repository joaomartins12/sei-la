#include "stdafx.h"
#include "SkillWindow.h"

void cSkillWin::Destroy()
{	
	cBaseWindow::Delete();	
}

void cSkillWin::DeleteResource()
{	
	DeleteScript();	
	SAFE_NIDELETE( m_pTabMenu );
	SAFE_NIDELETE( m_pSkillMark );

	_Delete_BSkill();
	_Delete_DSkill();
}

void cSkillWin::Create(int nValue /* = 0  */)
{	
	SetRootClient( CsPoint( 500, 50 ) );		
	
	cBaseWindow::Init();
	InitScript( NULL, m_ptRootClient, CsPoint( 431, 604 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );

	_Create_BSkill();
	_Create_DSkill();

	m_pCancelButton = AddButton( CsPoint( 411, 6 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	
	
	m_pTabMenu = NiNew cRadioButton;
	m_pTabMenu->Init( m_pRoot, CsPoint( 1, 3 ), CsPoint( 70, 23 ), cCheckBox::IMAGE_RADIO_1, _LAN( "기본" ), false );
	m_pTabMenu->AddNode( CsPoint( 71, 3 ), CsPoint( 70, 23 ), cCheckBox::IMAGE_RADIO_1, _LAN( "디지몬" ) );
	m_pTabMenu->SetCheckIndex( 1 );

	m_pSkillMark = NiNew cSprite;
	m_pSkillMark->Init( NULL, CsPoint::ZERO, CsPoint( 24, 24 ), "Skill\\Skill_Mark.tga", false );

	Update();
}

cBaseWindow::eMU_TYPE
cSkillWin::Update_ForMouse()
{	
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		Close();
		return muReturn;	
	}

	int nRadioAction;
	if( (nRadioAction = m_pTabMenu->Update_ForMouse()) != cRadioButton::INVALIDE_RADIO_CLICK )
	{
		if( nRadioAction == cRadioButton::ACTION_CLICK )
		{
			cWindow::PlaySound( cWindow::SD_At2 );		
		}
		return muReturn;
	}

	switch( m_pTabMenu->GetCheckIndex() )
	{
	case 0:
		if( _Update_ForMouse_BSkill() == true )
			return muReturn;
		break;
	case 1: 
		if( _Update_ForMouse_DSkill() == true )
			return muReturn;
		break;
	}

	if( g_Cursor.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 317, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();	

	return muReturn;
}

void cSkillWin::Update()
{	
	_UpdateMoveWindow();	

	switch( m_pTabMenu->GetCheckIndex() )
	{
	case 0:
		_Update_BSkill();
		break;
	case 1:
		_Update_DSkill();
		break;
	}
}

void cSkillWin::Render()
{
	switch( m_pTabMenu->GetCheckIndex() )
	{
	case 0:
		_Render_BSkill();
		break;
	case 1:
		_Render_DSkill();
		break;
	}

	RenderScript();		

	m_pTabMenu->Render();	
}

void cSkillWin::ResetDevice()
{	
	ResetDeviceScript();	

	if( m_pTabMenu )
		m_pTabMenu->ResetDevice();
	if( m_pSkillMark )
		m_pSkillMark->ResetDevice();

	_ResetDevice_BSkill();
	_ResetDevice_DSkill();
}

void cSkillWin::OnLButtonUp( CsPoint pos )
{	
	switch( m_pTabMenu->GetCheckIndex() )
	{
	case 0:
		_OnLButtonUp_BSkill( pos );
		break;
	case 1:
		_OnLButtonUp_DSkill( pos );
		break;
	}
}

void cSkillWin::OnRButtonUp( CsPoint pos )
{
	switch( m_pTabMenu->GetCheckIndex() )
	{
	case 0:
		_OnRButtonUp_BSkill( pos );
		break;
	case 1:
		_OnRButtonUp_DSkill( pos );
		break;
	}
}

bool cSkillWin::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	switch( m_pTabMenu->GetCheckIndex() )
	{
	case 0:
		return _CursorIconLBtnUp_BSkill( eIconType, nIconSlot, nIconCount );
		break;
	case 1:
		return _CursorIconLBtnUp_DSkill( eIconType, nIconSlot, nIconCount );
		break;
	}
	return false;
}

void cSkillWin::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();

	if( m_pTabMenu )
		m_pTabMenu->ResetDevice();

	_ResetDevice_BSkill();
	_ResetDevice_DSkill();
}