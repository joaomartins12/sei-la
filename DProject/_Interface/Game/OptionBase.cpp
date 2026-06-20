

#include "stdafx.h"
#include "OptionBase.h"



void cOptionBase::Destroy()
{
	cBaseWindow::Delete();	
}

void cOptionBase::DeleteResource()
{
	DeleteScript();
	_DeleteControl();
}

void cOptionBase::_DeleteControl()
{
	std::list< sOP_BASE* >::iterator it = m_listControl.begin();
	std::list< sOP_BASE* >::iterator itEnd = m_listControl.end();
	for( ; it!=itEnd; ++it )
	{
		sOP_BASE::DeleteInstance( *it );
	}
	m_listControl.clear();
}

bool cOptionBase::CreateBase( TCHAR const* szWinName, int nValue /* = 0  */)
{	
	if( cBaseWindow::Init() == false )
		return false;

	InitScript( "MainOption\\Base\\Win.tga", CsPoint( g_nScreenWidth/2 - 446/2, g_nScreenHeight/2 - 595/2 ), CsPoint( 446, 595 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );
	m_pCancelButton = AddButton( CsPoint( 424, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	
	m_pOkBtn = AddButton( CsPoint( 365, 556 ), CsPoint( 70, 31 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_OK" ).c_str() );
	m_pScrollBar = AddScrollBar( cScrollBar::TYPE_1, CsPoint( 419, 70 ), CsPoint( 16, 470 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint::ZERO, CsPoint( 446, 595 ) ), IF_OP_MACRO_SEL_COUNT );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( szWinName );
	m_pWinName = AddText( &ti, CsPoint( 215, 7 ) );

	m_pCheckRadio = NULL;
	m_pNextFocusCombo = m_pFocusCombo = NULL;

	return true;
}

void cOptionBase::_AddControl_Title( TCHAR const* sz, int nValue /*=0*/ )
{
	sOP_TITLE* pOP = (sOP_TITLE*)sOP_BASE::NewInstance( this, sOP_BASE::TITLE );
	pOP->Init( sz, nValue );

	m_listControl.push_back( pOP );
}

void cOptionBase::_AddControl_CheckRadio( TCHAR const* szName, TCHAR const* szOver, int nRadioID, int nValue /*=0*/ )
{
	sOP_CHECK_RADIO* pOP = (sOP_CHECK_RADIO*)sOP_BASE::NewInstance( this, sOP_BASE::CHECK_RADIO );
	pOP->Init( szName, szOver, nRadioID, nValue );

	m_listControl.push_back( pOP );
}

void cOptionBase::_AddControl_Combo( TCHAR const* szName, int nArrayCount, TCHAR (*strArray)[ IF_OP_COMBO_STR_MAX_LEN ], NiColor* pColorArray, int* pDataArray, int nSelData, int nValue /*=0*/ )
{
	assert_cs( nArrayCount <= IF_OP_COMBO_STR_MAX_COUNT );
	sOP_COMBO* pOP = (sOP_COMBO*)sOP_BASE::NewInstance( this, sOP_BASE::COMBO );
	pOP->Init( szName, nArrayCount, strArray, pColorArray, pDataArray, nSelData, nValue );

	m_listControl.push_back( pOP );
}

void cOptionBase::_AddControl_Slider( TCHAR const* szName, CsPoint ptRange, int nBtnMoveRange, int nStartPos, TCHAR const* szLow, TCHAR const* szHigh, int nValue /*= 0*/ )
{
	sOP_SLIDER* pOP = (sOP_SLIDER*)sOP_BASE::NewInstance( this, sOP_BASE::SLIDER );
	pOP->Init( szName, ptRange, nBtnMoveRange, nStartPos, nValue, szLow, szHigh );

	m_listControl.push_back( pOP );
}

void cOptionBase::_AddControl_CheckBox( TCHAR const* szName, int nCheckBoxCnt, TCHAR (*strArray)[ IF_OP_COMBO_STR_MAX_LEN ], int* pDataArray, bool bBattle, bool bOnly /*= true*/, int nValue /*= 0 */ )
{
	sOP_CHECK_BOX* pOP = (sOP_CHECK_BOX*)sOP_BASE::NewInstance( this, sOP_BASE::CHECK_BOX );
	pOP->Init( szName, nCheckBoxCnt, strArray, pDataArray, bBattle, bOnly, nValue );

	m_listControl.push_back( pOP );
}

cOptionBase::sOP_COMBO* cOptionBase::FindCombo( int nComboValue )
{
	std::list< sOP_BASE* >::iterator it = m_listControl.begin();
	std::list< sOP_BASE* >::iterator itEnd = m_listControl.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->GetType() != sOP_BASE::COMBO )
			continue;

		if( (*it)->s_nValue == nComboValue )
			return (sOP_COMBO*)(*it);
	}
	return NULL;
}

cOptionBase::sOP_SLIDER* cOptionBase::FindSlider( int nSliderValue )
{
	std::list< sOP_BASE* >::iterator it = m_listControl.begin();
	std::list< sOP_BASE* >::iterator itEnd = m_listControl.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->GetType() != sOP_BASE::SLIDER )
			continue;

		if( (*it)->s_nValue == nSliderValue )
			return (sOP_SLIDER*)(*it);
	}
	return NULL;
}


cOptionBase::sOP_CHECK_BOX* cOptionBase::FindCheckBox( int nCheckBoxValue )
{
	std::list< sOP_BASE* >::iterator it = m_listControl.begin();
	std::list< sOP_BASE* >::iterator itEnd = m_listControl.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->GetType() != sOP_BASE::CHECK_BOX )
			continue;

		if( (*it)->s_nValue == nCheckBoxValue )
			return (sOP_CHECK_BOX*)(*it);
	}
	return NULL;
}

void cOptionBase::Update(float const& fDeltaTime)
{
	_UpdateMoveWindow();

	std::list< sOP_BASE* >::iterator it = m_listControl.begin();
	std::list< sOP_BASE* >::iterator itEnd = m_listControl.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->Update(fDeltaTime);
	}
}

cBaseWindow::eMU_TYPE
cOptionBase::Update_ForMouse()
{
	if(m_bActiveMoveWindow)	//2016-01-14-nova 윈도우 이동시 버튼 선택이 안되도록
		return MUT_NONE;

	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if( m_pScrollBar->Update_ForMouse() != cScrollBar::ACTION_NONE )
		return muReturn;

	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_CLICK:
		_OnCancel();
	case cButton::ACTION_DOWN:
		return muReturn;
	}

	switch( m_pOkBtn->Update_ForMouse() )
	{
	case cButton::ACTION_CLICK:
		_OnOk();
	case cButton::ACTION_DOWN:
		return muReturn;
	}

	int nRemainPos = m_pScrollBar->GetCurPosIndex();
	int nRender = m_pScrollBar->GetRenderCount();
	std::list< sOP_BASE* >::iterator it = m_listControl.begin();
	std::list< sOP_BASE* >::iterator itEnd = m_listControl.end();
	for( ; it!=itEnd; ++it )
	{
		if( nRemainPos > 0 )
		{
			--nRemainPos;
			continue;
		}

		(*it)->Update_ForMouse();

		if( --nRender < 1 )
			break;
	}

	// 포커스 콤보 박스
	m_pFocusCombo = m_pNextFocusCombo;

	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 446, 27 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cOptionBase::Render()
{
	RenderScript();
	CsPoint pos( 35, 87 );
	pos += GetRootClient();

	int nRenderIndex = 0;
	int nRemainPos = m_pScrollBar->GetCurPosIndex();
	int nRender = m_pScrollBar->GetRenderCount();

	std::list< sOP_BASE* >::iterator it = m_listControl.begin();
	std::list< sOP_BASE* >::iterator itEnd = m_listControl.end();
	for( ; it!=itEnd; ++it )
	{
		if( nRemainPos > 0 )
		{
			--nRemainPos;
			continue;
		}

		(*it)->Render( pos );

		pos.y += 29;
		if( --nRender < 1 )
			break;
	}

	if( m_pFocusCombo != NULL )
	{
		sOP_COMBO* pBackup = m_pFocusCombo;
		m_pFocusCombo = NULL;
		pBackup->Render( m_ptFocusComboPos );
		m_pFocusCombo = pBackup;
	}
}

void cOptionBase::ResetDevice()
{
	if( IsLive() == false )
		return;

	ResetDeviceScript();

	std::list< sOP_BASE* >::iterator it = m_listControl.begin();
	std::list< sOP_BASE* >::iterator itEnd = m_listControl.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->ResetDevice();
	}
}

void cOptionBase::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();

	std::list< sOP_BASE* >::iterator it = m_listControl.begin();
	std::list< sOP_BASE* >::iterator itEnd = m_listControl.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->ResetDevice();
	}
}

void cOptionBase::_OnOk()
{
	if( g_pResist )
		g_pResist->SaveGlobal();

	Close( false );
	g_pGameIF->GetDynamicIF( cBaseWindow::WT_MAINOPTION );
}

void cOptionBase::_OnCancel()
{
	Close( false );
	g_pGameIF->GetDynamicIF( cBaseWindow::WT_MAINOPTION );
}