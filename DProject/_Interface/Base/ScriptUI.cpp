
#include "stdafx.h"
#include "ScriptUI.h"

cScriptUI::cScriptUI():m_pRoot(NULL), m_ptOldScreenSize(CsPoint::ZERO), __m_pDisableMask(NULL), __m_bEnableWindow(true)
,m_pWindowTitle(NULL),m_bVisible(true),m_pRootScript(NULL)
{
}

cScriptUI::~cScriptUI()
{
}

void cScriptUI::SetEnableWindow( bool bEnable )
{ 
	__m_bEnableWindow = bEnable; 
}

void cScriptUI::SetVisible( bool bValue )
{
	m_bVisible = bValue;
}

bool cScriptUI::IsVisible() const
{
	return m_bVisible;
}

CsPoint cScriptUI::GetRootSize() const
{
	return m_ptRootSize;
}


void cScriptUI::SetRootScript( cScriptUI* pRoot )
{
	m_pRootScript = pRoot;
}

void cScriptUI::InitScript( char* cRoot, CsPoint pos, CsPoint size, bool bMoveWindow,
						   IFREGION_X::eTYPE xType /*=IFREGION_X::LEFT*/, IFREGION_Y::eTYPE yType /*=IFREGION_Y::TOP*/, bool bApplyWindowSize /*=true*/ )
{
	m_bMoveWindow	= bMoveWindow;
	m_bLimitScreen	= true;
	m_eXRegionType	= xType;
	m_eYRegionType	= yType;

	SetPosSize( pos, size, bApplyWindowSize );

	// 루트 이미지
	assert_cs( m_pRoot == NULL );
	if( cRoot != NULL )
	{
		m_pRoot = NiNew cSprite;
		( (cSprite*)m_pRoot )->Init( NULL, m_ptRootClient, m_ptRootSize, cRoot, false );
	}
	else
	{
		m_pRoot = NiNew cWindow;
		m_pRoot->Init( NULL, m_ptRootClient, m_ptRootSize, false );
	}

	__m_pDisableMask = NiNew cSprite;
	__m_pDisableMask->Init( GetRoot(), CsPoint::ZERO, size, NiColorA( 0.2f, 0.2f, 0.2f, 0.5f ), true );
}

void cScriptUI::InitScript( cWindow* pParent, char* cRoot, CsPoint pos, CsPoint size, bool bMoveWindow,
						   IFREGION_X::eTYPE xType /*=IFREGION_X::LEFT*/, IFREGION_Y::eTYPE yType /*=IFREGION_Y::TOP*/, bool bApplyWindowSize /*=true*/ )
{	
	m_bMoveWindow	= bMoveWindow;
	m_bLimitScreen	= true;
	m_eXRegionType	= xType;
	m_eYRegionType	= yType;

	SetPosSize( pos, size, bApplyWindowSize );

	// 루트 이미지
	assert_cs( m_pRoot == NULL );
	if( cRoot != NULL )
	{
		m_pRoot = NiNew cSprite;
		( (cSprite*)m_pRoot )->Init( pParent, m_ptRootClient, m_ptRootSize, cRoot, false );
	}
	else
	{
		m_pRoot = NiNew cWindow;
		m_pRoot->Init( pParent, m_ptRootClient, m_ptRootSize, false );
	}

	__m_pDisableMask = NiNew cSprite;
	__m_pDisableMask->Init( GetRoot(), CsPoint::ZERO, size, NiColorA( 0.2f, 0.2f, 0.2f, 0.5f ), true );
}

void cScriptUI::SetRegionType( IFREGION_X::eTYPE xType , IFREGION_Y::eTYPE yType )
{
	m_eXRegionType	= xType;
	m_eYRegionType	= yType;
}

void cScriptUI::SetPosSize( CsPoint pos, CsPoint size, bool bApplyWindowSize /*=true*/ )
{
	// 상태
	m_ptRootClient	= pos;
	_CalRootDelte( true );
	m_ptRootSize = size;

	if( bApplyWindowSize == true )
	{
		if( ( m_ptOldScreenSize.x != g_nScreenWidth )||( m_ptOldScreenSize.y != g_nScreenHeight ) )
		{
			ApplyCurrentWindowSize( m_eXRegionType, m_eYRegionType, m_ptRootClient, m_ptRootSize, m_ptRootDelta, m_bMoveWindow, m_bLimitScreen );
			m_ptOldScreenSize = CsPoint( g_nScreenWidth, g_nScreenHeight );
		}
	}

// 	if( m_pRoot )
// 	{
// 		m_pRoot->SetPos( m_ptRootClient );
// 		m_pRoot->SetSize( m_ptRootSize );
// 	}
}

void cScriptUI::SetWindowPosition( CsPoint ptPos )
{
	if( m_ptRootClient == ptPos )
		return;

	SetPos( ptPos );
	ResetDeviceScript();
}

void cScriptUI::SetPos( CsPoint pos )
{
	// 상태
	m_ptRootClient = pos;
	_CalRootDelte( true );

	if( ( m_ptOldScreenSize.x != g_nScreenWidth )||( m_ptOldScreenSize.y != g_nScreenHeight ) )
	{
		ApplyCurrentWindowSize( m_eXRegionType, m_eYRegionType, m_ptRootClient, m_ptRootSize, m_ptRootDelta, m_bMoveWindow, m_bLimitScreen );
		m_ptOldScreenSize = CsPoint( g_nScreenWidth, g_nScreenHeight );
	}

// 	if( m_pRoot )
// 		m_pRoot->SetPos( m_ptRootClient );
}

void cScriptUI::_CalRootDelte( bool bInit )
{
	switch( m_eXRegionType )
	{
	case IFREGION_X::LEFT:
		m_ptRootDelta.x = 0;
		break;
	case IFREGION_X::CENTER:
		m_ptRootDelta.x = m_ptRootClient.x;
		break;
	case IFREGION_X::RIGHT:
		if( bInit == true )
		{
			assert_cs( m_bMoveWindow == false );
			m_ptRootDelta.x = DEFAULT_SCREEN_WIDTH - m_ptRootClient.x;
		}		
		break;
	case IFREGION_X::JUST_CENTER:
		if( bInit == true )
		{
			assert_cs( m_bMoveWindow == false );
			m_ptRootDelta.x = m_ptRootClient.x - DEFAULT_SCREEN_WIDTH/2;
		}
		break;
	default:
		assert_cs( false );
	}

	switch( m_eYRegionType )
	{
	case IFREGION_Y::TOP:
		m_ptRootDelta.y = 0;
		break;
	case IFREGION_Y::CENTER:
		m_ptRootDelta.y = m_ptRootClient.y;
		break;
	case IFREGION_Y::BOTTOM:
		if( bInit == true )
		{
			assert_cs( m_bMoveWindow == false );
			m_ptRootDelta.y = DEFAULT_SCREEN_HEIGHT - m_ptRootClient.y;
		}
		break;
	case IFREGION_Y::JUST_CENTER:
		if( bInit == true )
		{
			assert_cs( m_bMoveWindow == false );
			m_ptRootDelta.y = m_ptRootClient.y - DEFAULT_SCREEN_HEIGHT/2;
		}
		break;
	default:
		assert_cs( false );
	}
}


void cScriptUI::OnMouseOutWindow()
{
	std::list<cWindow*>::iterator it = m_vpChildControls.begin();
	for( ; it != m_vpChildControls.end(); ++it )
	{
		SAFE_POINTER_CON( (*it) );
		(*it)->OnMouseLeave();
	}

	std::list<cScriptUI*>::iterator sUI = m_vpChildScript.begin();
	for( ; sUI != m_vpChildScript.end(); ++sUI )
	{
		SAFE_POINTER_CON( (*sUI) );
		(*sUI)->OnMouseOutWindow();
	}
}

void cScriptUI::DeleteScript()
{
	std::list<cScriptUI*>::iterator sUI = m_vpChildScript.begin();
	for( ; sUI != m_vpChildScript.end(); ++sUI )
	{
		SAFE_POINTER_CON( (*sUI) );
		(*sUI)->DeleteScript();
		SAFE_NIDELETE( (*sUI) );
	}

	m_vpChildScript.clear();

	m_pWindowTitle = NULL;
	m_ptOldScreenSize = CsPoint::ZERO;

	SAFE_NIDELETE( m_pRoot );

	std::list<cWindow*>::iterator it = m_vpChildControls.begin();
	for( ; it != m_vpChildControls.end(); ++it )
		SAFE_NIDELETE( (*it) );

	m_vpChildControls.clear();

	std::list<cText*>::iterator tIt = m_vpText.begin();
	for( ; tIt != m_vpText.end(); ++tIt )
		SAFE_NIDELETE( (*tIt) );

	m_vpText.clear();	
	
	SAFE_NIDELETE( __m_pDisableMask );
	__m_bEnableWindow = true;
}

void cScriptUI::UpdateScript(float fDeltaTime)
{
	std::list<cWindow*>::iterator it = m_vpChildControls.begin();
	for( ; it != m_vpChildControls.end(); ++it )
	{
		SAFE_POINTER_CON( (*it) );
		(*it)->Update( fDeltaTime );
	}

	std::list<cScriptUI*>::iterator sUI = m_vpChildScript.begin();
	for( ; sUI != m_vpChildScript.end(); ++sUI )
	{
		SAFE_POINTER_CON( (*sUI) );
		(*sUI)->UpdateScript( fDeltaTime );
	}
}

void cScriptUI::RenderScript()
{
	if( !m_bVisible )
		return;

	if( g_pEngine )
		g_pEngine->ScreenSpace();

	if( m_pRoot )
		m_pRoot->Render();

	std::list<cWindow*>::iterator it = m_vpChildControls.begin();
	for( ; it != m_vpChildControls.end(); ++it )
	{
		SAFE_POINTER_CON( (*it) );
		if( (*it)->GetVisible() )
			(*it)->Render();
	}

	std::list<cText*>::iterator tIt = m_vpText.begin();
	for( ; tIt != m_vpText.end(); ++tIt )
	{
		SAFE_POINTER_CON( (*tIt) );
		if( (*tIt)->GetVisible() )
			(*tIt)->Render();
	}

	std::list<cScriptUI*>::iterator sUI = m_vpChildScript.begin();
	for( ; sUI != m_vpChildScript.end(); ++sUI )
	{
		SAFE_POINTER_CON( (*sUI) );
		(*sUI)->RenderScript();
	}
}


//======================================================================================
//
//	Control
//
//======================================================================================
void cScriptUI::AddChildScriptUI( cScriptUI* pAddUI )
{
	SAFE_POINTER_RET( pAddUI );

	pAddUI->SetRootScript( this );
	m_vpChildScript.push_back( pAddUI );
}

cText* cScriptUI::AddTitle( TCHAR const* szTitle, CsPoint ptDeltaPos, CFont::eFACE_SIZE eFontSize, NiColor fontColor )
{
	if( m_pWindowTitle == NULL )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, eFontSize, fontColor );
		ti.SetText( szTitle );
		ti.s_eTextAlign = DT_CENTER;

		CsPoint pos;
		pos.x = m_ptRootSize.x / 2 + ptDeltaPos.x;
		pos.y = 7 + ptDeltaPos.y;
		m_pWindowTitle = AddText( &ti, pos );
	}
	else
	{
		m_pWindowTitle->SetText( szTitle );
	}
	
	return m_pWindowTitle;
}

cButton* cScriptUI::AddButton( CsPoint pos, CsPoint size, CsPoint tex, char const* cPath, cWindow::eSOUND sound /*=cWindow::SD_NONE*/, bool bUseParent /*=true*/ )
{
	cButton* pButton = NiNew cButton;
	pButton->Init( bUseParent == true ? m_pRoot : NULL, pos, size, cPath, bUseParent == false );
	pButton->SetTexToken( tex );
	pButton->SetSound( sound );
	m_vpChildControls.push_back( pButton );
	//m_vpChildControls.PushBack( pButton );
	//m_vpButton.PushBack( pButton );

	return pButton;
}

cButton* cScriptUI::AddButton( CsPoint pos, CsPoint size, cButton::eIMAGE_TYPE it, TCHAR const* bs, cWindow::eSOUND sound /* = cWindow::SD_NONE */, bool bUseParent /* = true  */, CsPoint ptDeltaPos /*= CsPoint::ZERO*/)
{
	cButton* pButton = NiNew cButton;
	pButton->Init( bUseParent == true ? m_pRoot : NULL, pos, size, it, bs, bUseParent == false, ptDeltaPos );
	pButton->SetSound( sound );
	m_vpChildControls.push_back( pButton );
	//m_vpChildControls.PushBack( pButton );
	//m_vpButton.PushBack( pButton );

	return pButton;
}

cButton* cScriptUI::AddButton( CsPoint pos, CsPoint size, cButton::eIMAGE_TYPE it, cText::sTEXTINFO* ti, cWindow::eSOUND sound /* = cWindow::SD_NONE */, bool bUseParent /* = true  */, CsPoint ptDeltaPos /*= CsPoint::ZERO*/)
{
	cButton* pButton = NiNew cButton;
	pButton->Init( bUseParent == true ? m_pRoot : NULL, pos, size, it, ti, bUseParent == false, ptDeltaPos );
	pButton->SetSound( sound );
	m_vpChildControls.push_back( pButton );
	//m_vpChildControls.PushBack( pButton );
	//m_vpButton.PushBack( pButton );

	return pButton;
}

cAniButton* cScriptUI::AddAniButton( CsPoint pos, CsPoint size, CsPoint tex, char const* cPath, cWindow::eSOUND sound /*=cWindow::SD_NONE*/, bool bUseParent /*=true*/ )
{
	cAniButton* pButton = NiNew cAniButton;
	pButton->Init( bUseParent == true ? m_pRoot : NULL, pos, size, cPath, bUseParent == false );
	pButton->SetTexToken( tex );
	pButton->SetSound( sound );
	m_vpChildControls.push_back( pButton );
	//m_vpChildControls.PushBack( pButton );
	//m_vpButton.PushBack( pButton );

	return pButton;
}

cAniButton* cScriptUI::AddAniButton( CsPoint pos, CsPoint size, TCHAR const* bs, cWindow::eSOUND sound /* = cWindow::SD_NONE */, bool bUseParent /* = true  */, CsPoint ptDeltaPos /*= CsPoint::ZERO*/)
{
	cAniButton* pButton = NiNew cAniButton;
	pButton->Init( bUseParent == true ? m_pRoot : NULL, pos, size, bs, bUseParent == false, ptDeltaPos );
	pButton->SetSound( sound );
	m_vpChildControls.push_back( pButton );
	//m_vpChildControls.PushBack( pButton );
	//m_vpButton.PushBack( pButton );

	return pButton;
}

cAniButton* cScriptUI::AddAniButton( CsPoint pos, CsPoint size, cText::sTEXTINFO* ti, cWindow::eSOUND sound /* = cWindow::SD_NONE */, bool bUseParent /* = true  */, CsPoint ptDeltaPos /*= CsPoint::ZERO*/)
{
	cAniButton* pButton = NiNew cAniButton;
	pButton->Init( bUseParent == true ? m_pRoot : NULL, pos, size, ti, bUseParent == false, ptDeltaPos );
	pButton->SetSound( sound );
	m_vpChildControls.push_back( pButton );
	//m_vpChildControls.PushBack( pButton );
	//m_vpButton.PushBack( pButton );

	return pButton;
}

cSprite* cScriptUI::AddSprite( CsPoint pos, CsPoint size, char const* cPath,bool bUseParent /*=true*/ )
{
	cSprite* pSprite = NiNew cSprite;
	pSprite->Init( bUseParent == true ? m_pRoot : NULL, pos, size, cPath, bUseParent == false );
	m_vpChildControls.push_back( pSprite );
	//m_vpChildControls.PushBack( pSprite );
	//m_vpSprite.PushBack( pSprite );

	return pSprite;
}

cSprite* cScriptUI::AddSprite( CsPoint pos, CsPoint size, char const* cImagePath, bool bUseParent, NiColor color, bool bUseWorkingFolder )
{
	cSprite* pSprite = NiNew cSprite;
	pSprite->Init( bUseParent == true ? m_pRoot : NULL, pos, size, cImagePath, bUseParent == false, color, bUseWorkingFolder );
	m_vpChildControls.push_back( pSprite );
	//m_vpChildControls.PushBack( pSprite );
	//m_vpSprite.PushBack( pSprite );

	return pSprite;
}

cCheckBox* cScriptUI::AddCheckBox( CsPoint pos, CsPoint size, CsPoint tex, char const* cPath, cWindow::eSOUND sound /*=cWindow::SD_NONE*/, bool bUseParent /*=true*/ )
{
	cCheckBox* pCheck = NiNew cCheckBox;
	pCheck->Init( bUseParent == true ? m_pRoot : NULL, pos, size, cPath, bUseParent == false );
	pCheck->SetTexToken( tex );
	pCheck->SetSound( sound );
	m_vpChildControls.push_back( pCheck );
	//m_vpChildControls.PushBack( pCheck );
	//m_vpCheckBox.PushBack( pCheck );

	return pCheck;
}

cCheckBox* cScriptUI::AddCheckBox( CsPoint pos, CsPoint size, cCheckBox::eIMAGE_TYPE it, TCHAR const* cs, cWindow::eSOUND sound /*=cWindow::SD_NONE*/, bool bUseParent /*=true*/  )
{
	
	cCheckBox* pCheck = NiNew cCheckBox;
	pCheck->Init( bUseParent == true ? m_pRoot : NULL, pos, size, it, cs, bUseParent == false );
	pCheck->SetSound( sound );
	m_vpChildControls.push_back( pCheck );
	//m_vpChildControls.PushBack( pCheck );

	return pCheck;
}

cCheckBox* cScriptUI::AddCheckBox( CsPoint pos, CsPoint size, cCheckBox::eIMAGE_TYPE it, cText::sTEXTINFO* ti, cWindow::eSOUND sound /*=cWindow::SD_NONE*/, bool bUseParent /*=true*/ )
{
	cCheckBox* pCheck = NiNew cCheckBox;
	pCheck->Init( bUseParent == true ? m_pRoot : NULL, pos, size, it, ti, bUseParent == false );
	pCheck->SetSound( sound );
	m_vpChildControls.push_back( pCheck );
	//m_vpChildControls.PushBack( pCheck );
	return pCheck;
}

cText* cScriptUI::AddText( cText::sTEXTINFO* pTextInfo, CsPoint pos, bool bUseParent /*=true*/ )
{
	cText* pText = NiNew cText;	
	pText->Init( bUseParent == true ? m_pRoot : NULL, pos, pTextInfo, bUseParent == false );	

	m_vpText.push_back( pText );
	return pText;
}

cScrollBar* cScriptUI::AddScrollBar( cScrollBar::eSCROLL_TYPE type, CsPoint pos, CsPoint CtrlSize, CsPoint btnSize, CsRect rcRegion, int nRenderCount, bool bUseParent /*=true*/ )
{
	cScrollBar* pBar = NiNew cScrollBar;
	pBar->Init( type, bUseParent == true ? m_pRoot : NULL, pos, CtrlSize, btnSize, rcRegion, nRenderCount, bUseParent == false );
	m_vpChildControls.push_back( pBar );
//	m_vpChildControls.PushBack( pBar );
	//m_vpScrollBar.PushBack( pBar );

	return pBar;
}
cRadioButton*	cScriptUI::AddRadioButton( CsPoint pos, CsPoint size, CsPoint tex, char* cImagePath, bool bUseParent )
{
	cRadioButton* pButton = NiNew cRadioButton;
	pButton->Init( bUseParent == true ? m_pRoot : NULL, pos, size, tex, cImagePath, bUseParent == false );
	m_vpChildControls.push_back( pButton );
	//m_vpChildControls.PushBack( pButton );

	return pButton;
}
cRadioButton*	cScriptUI::AddRadioButton( CsPoint pos, CsPoint size, cCheckBox::eIMAGE_TYPE it, cText::sTEXTINFO* ti, bool bUseParent )
{
	cRadioButton* pButton = NiNew cRadioButton;
	pButton->Init( bUseParent == true ? m_pRoot : NULL, pos, size, it, ti, bUseParent == false );
	m_vpChildControls.push_back( pButton );
	//m_vpChildControls.PushBack( pButton );

	return pButton;

}
cRadioButton*	cScriptUI::AddRadioButton( CsPoint pos, CsPoint size, cCheckBox::eIMAGE_TYPE it, TCHAR const* cs, bool bUseParent )
{
	cRadioButton* pButton = NiNew cRadioButton;
	pButton->Init( bUseParent == true ? m_pRoot : NULL, pos, size, it, cs, bUseParent == false );
	m_vpChildControls.push_back( pButton );
	//m_vpChildControls.PushBack( pButton );

	return pButton;
}

cPopUp*	cScriptUI::AddPopup( CsPoint pos, CsPoint size, CsPoint patch, char* cPathName, char* cExpName, bool bUseParent)
{
	cPopUp* pPopup = NiNew cPopUp;
	pPopup->Init( bUseParent == true ? m_pRoot : NULL, pos, size, patch, cPathName, cExpName, bUseParent == false );
	m_vpChildControls.push_back( pPopup );
	//m_vpChildControls.PushBack( pPopup );

	return pPopup;
}

bool cScriptUI::AddChildControl( cWindow* pAddControl )
{
	SAFE_POINTER_RETVAL( pAddControl, false );
	pAddControl->SetParentWindow( m_pRoot );
	m_vpChildControls.push_back( pAddControl );
	//m_vpChildControls.PushBack( pAddControl );
	return true;
}

bool cScriptUI::RemoveChildControl( cWindow* pRemoveControl )
{
	SAFE_POINTER_RETVAL( pRemoveControl, false );

	std::list<cWindow*>::iterator it = m_vpChildControls.begin();
	for( ; it != m_vpChildControls.end(); ++it )
	{
		SAFE_POINTER_CON( (*it) );
		if( *it == pRemoveControl )
		{
			SAFE_NIDELETE( *it );
			m_vpChildControls.erase(it);
			return true;
		}
	}
	//m_vpChildControls.DeleteElement_FromData(pRemoveControl);	
	return false;
}

bool cScriptUI::RemoveChildText( cText* pRemoveText )
{
	SAFE_POINTER_RETVAL( pRemoveText, false );

	std::list<cText*>::iterator it = m_vpText.begin();
	for( ; it != m_vpText.end(); ++it )
	{
		SAFE_POINTER_CON( (*it) );
		if( *it == pRemoveText )
		{
			SAFE_NIDELETE( *it );
			m_vpText.erase(it);
			return true;
		}
	}
	return true;
}


//======================================================================================
//
//	ResetDevice
//
//======================================================================================


void cScriptUI::ApplyCurrentWindowSize( IFREGION_X::eTYPE ifx, IFREGION_Y::eTYPE ify, CsPoint& pos, CsPoint size, CsPoint& delta, bool bMoveWindow, bool bLimitScreen )
{
	/*if( ( g_pEngine->m_nOldScreenWidth == DEFAULT_SCREEN_WIDTH )&&
		( g_pEngine->m_nOldScreenHeight == DEFAULT_SCREEN_HEIGHT )&&
		( g_nScreenWidth == DEFAULT_SCREEN_WIDTH )&&
		( g_nScreenHeight == DEFAULT_SCREEN_HEIGHT ) )
	{
		return;
	}*/

	int nTempX = g_pEngine->m_nOldScreenWidth;
	int nTempY = g_pEngine->m_nOldScreenHeight;
	g_pEngine->m_nOldScreenWidth = DEFAULT_SCREEN_WIDTH;
	g_pEngine->m_nOldScreenHeight = DEFAULT_SCREEN_HEIGHT;

	ResetDevice( ifx, ify, pos, size, delta, bMoveWindow, bLimitScreen );

	g_pEngine->m_nOldScreenWidth = nTempX;
	g_pEngine->m_nOldScreenHeight = nTempY;
}

void cScriptUI::ResetDeviceScript()
{
	_CalRootDelte( false );
	ResetDevice( m_eXRegionType, m_eYRegionType, m_ptRootClient, m_ptRootSize, m_ptRootDelta, m_bMoveWindow, m_bLimitScreen );

	m_ptOldScreenSize = CsPoint( g_nScreenWidth, g_nScreenHeight );

	if( m_pRoot != NULL )
	{
		m_pRoot->SetSize(m_ptRootSize);
		m_pRoot->ResetDevice( m_ptRootClient );
	}

	std::list<cWindow*>::iterator it = m_vpChildControls.begin();
	for( ; it != m_vpChildControls.end(); ++it )
	{
		SAFE_POINTER_CON( (*it) );
		(*it)->ResetDevice();
	}

	std::list<cText*>::iterator tIt = m_vpText.begin();
	for( ; tIt != m_vpText.end(); ++tIt )
	{
		SAFE_POINTER_CON( (*tIt) );
		(*tIt)->ResetDevice();
	}

	if( __m_pDisableMask )
		__m_pDisableMask->ResetDevice();

	std::list<cScriptUI*>::iterator sUI = m_vpChildScript.begin();
	for( ; sUI != m_vpChildScript.end(); ++sUI )
	{
		SAFE_POINTER_CON( (*sUI) );
		(*sUI)->ResetDeviceScript();
	}
}

bool cScriptUI::SetMove( CsPoint& ptDelta )
{
	if( ptDelta == CsPoint::ZERO )
		return false;

	assert_cs( m_bMoveWindow == true );
	CsPoint ptOld = m_ptRootClient;
	m_ptRootClient += ptDelta;	

	if( m_bLimitScreen == true )
	{
		if( m_ptRootClient.x < 0 )
		{
			ptDelta.x = ptDelta.x - m_ptRootClient.x;
			m_ptRootClient.x = 0;
		}
		else if( m_ptRootClient.x+m_ptRootSize.x > g_nScreenWidth )
		{
			int nOrg = m_ptRootClient.x;
			m_ptRootClient.x = g_nScreenWidth - m_ptRootSize.x;
			ptDelta.x = ptDelta.x - ( nOrg - m_ptRootClient.x );
		}
		if( m_ptRootClient.y < 0 )
		{
			ptDelta.y = ptDelta.y - m_ptRootClient.y;
			m_ptRootClient.y = 0;
		}
		else if( m_ptRootClient.y+m_ptRootSize.y > g_nScreenHeight )
		{
			int nOrg = m_ptRootClient.y;
			m_ptRootClient.y = g_nScreenHeight - m_ptRootSize.y;
			ptDelta.y = ptDelta.y - ( nOrg - m_ptRootClient.y );
		}
	}	

	m_pRoot->SetMove( m_ptRootClient - ptOld );	

	return true;
}

void cScriptUI::_OnMoveWindowScript()
{
	m_pRoot->ResetDevice();

	std::list<cWindow*>::iterator it = m_vpChildControls.begin();
	for( ; it != m_vpChildControls.end(); ++it )
	{
		SAFE_POINTER_CON( (*it) );
		(*it)->ResetDevice();
	}

	std::list<cText*>::iterator tIt = m_vpText.begin();
	for( ; tIt != m_vpText.end(); ++tIt )
	{
		SAFE_POINTER_CON( (*tIt) );
		(*tIt)->ResetDevice();
	}

	if( __m_pDisableMask )
		__m_pDisableMask->ResetDevice();


	std::list<cScriptUI*>::iterator sUI = m_vpChildScript.begin();
	for( ; sUI != m_vpChildScript.end(); ++sUI )
	{
		SAFE_POINTER_CON( (*sUI) );
		(*sUI)->_OnMoveWindowScript();
	}
}

void cScriptUI::_OnSetFocusScript()
{

}

void cScriptUI::_OnKillFocusScript()
{
	std::list<cWindow*>::iterator it = m_vpChildControls.begin();
	for( ; it != m_vpChildControls.end(); ++it )
	{
		SAFE_POINTER_CON( (*it) );
		if( cWindow::T_Scrollbar == (*it)->GetType() )
			static_cast<cScrollBar*>(*it)->OnKillFocus();
	}

	std::list<cScriptUI*>::iterator sUI = m_vpChildScript.begin();
	for( ; sUI != m_vpChildScript.end(); ++sUI )
	{
		SAFE_POINTER_CON( (*sUI) );
		(*sUI)->_OnKillFocusScript();
	}
}

void cScriptUI::ResetDevice( IFREGION_X::eTYPE ifx, IFREGION_Y::eTYPE ify, CsPoint& pos, CsPoint size, CsPoint& delta, bool bMoveWindow, bool bLimitScreen )
{
	switch( ifx )
	{
	case IFREGION_X::LEFT:
		break;
	case IFREGION_X::CENTER:
		{
			if( delta.x+size.x/2 < g_pEngine->m_nOldScreenWidth/2 )
			{
				float fRate = (float)delta.x/(float)g_pEngine->m_nOldScreenWidth;
				delta.x = (int)( fRate*g_nScreenWidth );
				pos.x = delta.x;
			}
			else
			{
				float fRate = (float)(delta.x+size.x)/(float)g_pEngine->m_nOldScreenWidth;
				delta.x = (int)( fRate*g_nScreenWidth ) - size.x;
				pos.x = delta.x;
			}
		}
		break;
	case IFREGION_X::RIGHT:
		pos.x = g_nScreenWidth - delta.x;
		break;
	case IFREGION_X::JUST_CENTER:
		pos.x = g_nScreenWidth/2 + delta.x;
		break;
	default:
		assert_cs( false );
	}

	switch( ify )
	{
	case IFREGION_Y::TOP:
		break;
	case IFREGION_Y::CENTER:
		{
			if( delta.y+size.y/2 < g_pEngine->m_nOldScreenHeight/2 )
			{
				float fRate = (float)delta.y/(float)g_pEngine->m_nOldScreenHeight;
				delta.y = (int)( fRate*g_nScreenHeight );
				pos.y = delta.y;
			}
			else
			{
				float fRate = (float)(delta.y+size.y)/(float)g_pEngine->m_nOldScreenHeight;
				delta.y = (int)( fRate*g_nScreenHeight ) - size.y;
				pos.y = delta.y;
			}
		}
		break;
	case IFREGION_Y::BOTTOM:
		pos.y = g_nScreenHeight - delta.y;
		break;
	case IFREGION_Y::JUST_CENTER:
		pos.y = g_nScreenHeight/2 + delta.y;
		break;
	default:
		assert_cs( false );
	}

	if( (bMoveWindow == true )&&( bLimitScreen == true ) )
	{
		if( pos.x < 0 )
		{			
			pos.x = 0;
		}
		else if( pos.x+size.x > g_nScreenWidth )
		{
			pos.x = g_nScreenWidth - size.x;
		}
		if( pos.y < 0 )
		{			
			pos.y = 0;
		}
		else if( pos.y+size.y > g_nScreenHeight )
		{
			pos.y = g_nScreenHeight - size.y;
		}
	}
}

bool cScriptUI::IsInWindow( CsPoint pos )
{ 
	return CsRect( m_ptRootClient, m_ptRootClient + m_ptRootSize ).PtInRect( pos ) == TRUE; 
}

void cScriptUI::SetRootClient( CsPoint pos )
{ 
	m_ptRootClient = pos; 
}

void cScriptUI::__SetRootSize( CsPoint size )
{ 
	m_ptRootSize = size; 
	if( m_pRoot )
		m_pRoot->SetSize( size );
}

CsPoint cScriptUI::GetRootClient()
{ 
	return m_ptRootClient; 
}

CsPoint cScriptUI::GetTotalPosition()
{
	CsPoint totalPos;
	if( m_pRootScript )
		totalPos = m_pRootScript->GetTotalPosition();
	totalPos += m_ptRootClient;
	return totalPos;
}

cWindow* cScriptUI::GetRoot()
{ 
	return m_pRoot; 
}

void cScriptUI::SetLimitScreen( bool bLimit )
{ 
	m_bLimitScreen = bLimit; 
}

CsPoint cScriptUI::MousePosToWindowPos( CsPoint ptMousePos )
{ 
	return ptMousePos - GetTotalPosition(); 
}

cWindow* cScriptUI::GetChildControl( std::string const& sControlName )
{
	std::list<cWindow*>::iterator it = m_vpChildControls.begin();
	for( ; it != m_vpChildControls.end(); ++it )
	{
		if( 0 == sControlName.compare( (*it)->GetControlName() ) )
			return (*it);
	}

	return NULL;
}