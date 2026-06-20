
#include "stdafx.h"
#include "Publicity.h"


cPublicity::cPublicity():m_fShowTime(6.0f),m_bShow(false),m_fCurrentTime(0.0f),m_pMessageBox(NULL),m_fAlpha(0.0f),m_bFadeIn(true)
,m_pTextBack(NULL)
{

}

cPublicity::~cPublicity()
{
}

cBaseWindow::eWINDOW_TYPE	cPublicity::GetWindowType()
{ 
	return WT_PUBLICITY; 
}

void cPublicity::Destroy()
{	
	m_pControls.clear();
	cBaseWindow::Delete();	
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void cPublicity::DeleteResource()
{
	DeleteScript();
}

void cPublicity::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( NULL, CsPoint( 242, 90 ), CsPoint( (g_nScreenWidth - 448), 60 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::TOP );
	m_pTextBack = AddSprite( CsPoint::ZERO, CsPoint( (g_nScreenWidth - 448), 60 ), "CommonUI\\bg_notice.tga", true );

	m_pMessageBox = NiNew cListBox();
	if( m_pMessageBox )
	{
		m_pMessageBox->Init( GetRoot(), CsPoint(0, 25), CsPoint((g_nScreenWidth - 448), 100), NULL, false );
		AddChildControl( m_pMessageBox );
	}

}

void cPublicity::Update(float const& fDeltaTime)
{	
	if( !m_bShow )
		return;

	//_UpdateTextAlpha( fDeltaTime );
	m_fCurrentTime += fDeltaTime;
	if( m_fCurrentTime > m_fShowTime )
	{
		_InitData();
		_SetNextMessage();
	}
}

void cPublicity::_UpdateTextAlpha(float const& fDeltaTime)
{
	if( m_bFadeIn )
	{
		m_fAlpha += fDeltaTime;
		if( m_fAlpha > 1.0f )
		{
			m_bFadeIn = false;
			m_fAlpha = 1.0f;
		}
	}
	else
	{
		m_fAlpha -= fDeltaTime;
		if( m_fAlpha < 0.3f )
		{
			m_bFadeIn = true;
			m_fAlpha = 0.3f;
		}
	}

	std::list<cString::sTEXT*>::iterator it = m_pControls.begin();
	for( ; it != m_pControls.end(); ++it )
		(*it)->SetAlpha( m_fAlpha );
}

cBaseWindow::eMU_TYPE cPublicity::Update_ForMouse()
{
	return MUT_OUT_WINDOW;
}

void cPublicity::Render()
{
	RenderScript();	
}

void cPublicity::ResetDevice()
{	
	ResetDeviceScript();
	if( !m_wsShowMsg.empty() )
		_InsertMessage( m_wsShowMsg, false );
}

void cPublicity::_SetNextMessage()
{
	if( m_pMessageBox )
		m_pMessageBox->RemoveAllItem();

	m_wsShowMsg = GetSystem()->GetShowMessageAndPop();
	if( !m_wsShowMsg.empty() )
	{
		_InsertMessage( m_wsShowMsg );
		m_bShow = true;
	}

	if( !m_bShow )// Ã¢ ´Ý±â
		GetSystem()->closeDisplayWindow();
}

void cPublicity::_InitData()
{
	m_fAlpha = 0.0f;
	m_bFadeIn = true;
	m_fCurrentTime = 0.0f;
	m_bShow = false;
	m_wsShowMsg.clear();
}

void cPublicity::_InsertMessage( std::wstring const& wsMsg, bool bSound )
{	
	SAFE_POINTER_RET( m_pMessageBox );
	m_pMessageBox->RemoveAllItem();
	m_pControls.clear();
	cText::sTEXTINFO ti;
	ti.Init(CFont::FS_16 );
	
	cStringList pkDesc;
	g_pStringAnalysis->Cut_Parcing( &pkDesc,(g_nScreenWidth - 448),wsMsg.c_str(), &ti );

	CsPoint StringMaxSize = pkDesc.CalMaxSize( 0 );
	if( m_pTextBack )
		m_pTextBack->SetPosSize( CsPoint( -212, 0), CsPoint(1000, StringMaxSize.y + 50 ) );


	std::list< cString* >* pkDestList = pkDesc.GetList();
	std::list< cString* >::iterator kDescIter = pkDestList->begin();
	for(; kDescIter != pkDestList->end(); ++kDescIter)
	{
		cString * pkViewer = NiNew cString;
		CsPoint maxSize = (*kDescIter)->GetMaxSize();

		CsPoint csPos(576/2 - maxSize.x/2,0);
		for(int i= 0 ; i < (*kDescIter)->GetListSize(); ++i)
		{
			cString::sTEXT* pText = (cString::sTEXT*)(*kDescIter)->GetElement(i);
			cString::sTEXT* pAddData = pkViewer->AddText(pText->s_Text.GetTextInfo(), csPos);	
			//m_pControls.push_back( pAddData );
			csPos.x += pText->s_ptSize.x;
		}
		cListBoxItem * addItem  = NiNew cListBoxItem;
		addItem->SetItem( pkViewer );
		m_pMessageBox->AddItem( addItem );
	}

// 	if( bSound )
// 		cWindow::PlaySound( ".\\System\\lotto.wav" );
}
//////////////////////////////////////////////////////////////////////////

void cPublicity::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eAddNeonDisplay:
		_SetNextMessage();
		break;	 
	}
}

bool cPublicity::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eAddNeonDisplay, this );

	return true;
}
