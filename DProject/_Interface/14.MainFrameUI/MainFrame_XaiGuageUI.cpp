
#include "stdafx.h"
#include "MainFrame_XaiGuageUI.h"

void cXaiGuageUI::sCrystalUI::SetEnable(bool bEnable)
{
	bEmpty = !bEnable;
	if( pFullImg )
		pFullImg->SetVisible( bEnable );
}

bool cXaiGuageUI::sCrystalUI::IsEmpty() const
{
	return bEmpty;
}

//////////////////////////////////////////////////////////////////////////

cXaiGuageUI::cXaiGuageUI():m_pXaiGuage(NULL),m_pXaiDia(NULL)
{
}

cXaiGuageUI::~cXaiGuageUI()
{
}

cBaseWindow::eWINDOW_TYPE cXaiGuageUI::GetWindowType()
{
	return WT_XAIGUAGE_WINDOW;
}

void cXaiGuageUI::Destroy()
{
	m_listCrystalUI.clear();
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
	cBaseWindow::Delete();
}

void cXaiGuageUI::DeleteResource()
{
	DeleteScript();
}

void cXaiGuageUI::Create( int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	CsPoint pos(400, 575);
	if( g_pResist->m_CenterBar.s_bIsOpenQuick )//추가 퀵슬롯 오픈상태일 때
		pos.y -= 44;

	InitScript( NULL, pos, CsPoint( 376, 45 ), false, IFREGION_X::LEFT, IFREGION_Y::BOTTOM );

	m_pXaiGuage = NiNew cUI_ProgressBar;
	if( m_pXaiGuage )
	{
		m_pXaiGuage->Init( m_pRoot, CsPoint(0, 27), CsPoint(134, 16), false );		
		m_pXaiGuage->SetBackImage( "CommonUI\\Progressbar_bg_Center.tga" );
		m_pXaiGuage->SetGuageImage( "CommonUI\\Progressbar_Center.tga" );
		m_pXaiGuage->SetProgressRect( CsRect( 2, 0, 130, 16 ) );
		cText::sTEXTINFO ti;		
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_9, FONT_WHITE );					
		ti.s_bOutLine = false;		
		ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
		m_pXaiGuage->SetStringControl( &ti );
		m_pXaiGuage->SetRenderText(true);
		AddChildControl( m_pXaiGuage );
	}

	m_pXaiDia = NiNew cGridListBox;
	if( m_pXaiDia )
	{
		m_pXaiDia->Init( m_pRoot, CsPoint( 200, 0 ), CsPoint( 150, 45 ), CsPoint(5,0), CsPoint(20, 20), cGridListBox::LowRightUp, cGridListBox::LeftBottom, NULL, false, 0 );
		m_pXaiDia->SetMouseOverImg("Icon\\Mask_Over.tga");
		m_pXaiDia->SetBackOverAndSelectedImgRender(false);
		m_pXaiDia->SetAutoSelection(false);
		m_pXaiDia->AddEvent( cGridListBox::GRID_CLICKED_ITEM, this, &cXaiGuageUI::_ClickCrystal );
		int nMax = 150 / (20 + 5);
		for( int n = 0; n < nMax * 2; ++n )
		{
			cString * pString = NiNew cString;
			SAFE_POINTER_BEK( pString );

			sCrystalUI add;
			add.pBackImg = NiNew cSprite;
			if( add.pBackImg )
			{
				add.pBackImg->Init( NULL, CsPoint::ZERO, CsPoint( 20, 20), "XaiSystem\\xcrystal_empty.tga", false );		
				cString::sSPRITE* sSprite = pString->AddSprite( add.pBackImg );
				if( sSprite )
					sSprite->SetAutoPointerDelete(true);
			}
			
			add.pFullImg = NiNew cSprite;
			if( add.pFullImg )
			{
				add.pFullImg->Init( NULL, CsPoint(2,2), CsPoint( 16, 16), "XaiSystem\\xcrystal_Full.tga", false );		
				cString::sSPRITE* sSprite = pString->AddSprite( add.pFullImg );
				if( sSprite )
					sSprite->SetAutoPointerDelete(true);
			}

			m_listCrystalUI.push_back( add );
			cGridListBoxItem * addItem  = NiNew cGridListBoxItem(n, CsPoint( 20, 20 ));
			addItem->SetItem( pString );
			m_pXaiDia->AddItem( addItem );
		}

		AddChildControl( m_pXaiDia );
	}

	_UpdateMaxGuage( GetSystem()->GetMaxCrystal(), GetSystem()->GetMaxXGuage() );
	_UpdateGuage( GetSystem()->GetCurrentCrystal(), GetSystem()->GetCurrentXGuage() );
}

void cXaiGuageUI::Update(float const& fDeltaTime)
{	
	//_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE cXaiGuageUI::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;

	if(m_pXaiDia && m_pXaiDia->Update_ForMouse(CURSOR_ST.GetPos()) )
	{
		if( TOOLTIPMNG_STPTR && TOOLTIPMNG_STPTR->GetTooltip() )
		{
			std::wstring tooltipMsg = UISTRING_TEXT("TOOLTIP_XGUAGE_CRYSTAL_MSG");
			DmCS::StringFn::Replace( tooltipMsg, L"#value#", GetSystem()->GetConsumChargeValue() );
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( m_pXaiDia->GetClient(), CsPoint::ZERO, tooltipMsg.c_str() );
		}
		
		return muReturn;
	}

// 	// 위에 바 드래그해야만 이동 시키도록 하겠다.
// 	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( m_pRoot->GetSize().x, 32 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
// 	{
// 		_UpdateMoveWindow_ForMouse();
// 		return muReturn;
// 	}
	return MUT_OUT_WINDOW;
}

void cXaiGuageUI::Render()
{
	RenderScript();
}

void cXaiGuageUI::OnRButtonUp( CsPoint pos )
{
}

void cXaiGuageUI::OnLButtonUp( CsPoint pos )
{
}

void cXaiGuageUI::ResetDevice()
{
	ResetDeviceScript();
}

void cXaiGuageUI::_UpdateGuage( unsigned short const& nCrystal, unsigned int const& nGuage )
{
	if( m_pXaiGuage )
		m_pXaiGuage->SetProgressPos( nGuage * 1.0f );

	std::list<sCrystalUI>::iterator it = m_listCrystalUI.begin();
	for( int n  =0; it != m_listCrystalUI.end(); ++it, ++n )
	{
		if( n < nCrystal )
			(*it).SetEnable( true );
		else
			(*it).SetEnable( false );
	}
}

void cXaiGuageUI::_UpdateMaxGuage( unsigned short const& nMaxCrystal, unsigned int const& nMaxGuage )
{
	if( m_pXaiGuage )
		m_pXaiGuage->SetRange( nMaxGuage * 1.0f );

	if( m_pXaiDia )
		m_pXaiDia->SetVisibleItemCount(nMaxCrystal);
}

void cXaiGuageUI::_ClickCrystal(void* pSender, void* pdata)
{
	SAFE_POINTER_RET( pdata );
	cGridListBoxItem* pSelItem = static_cast<cGridListBoxItem*>(pdata);

	std::list<sCrystalUI>::iterator it = m_listCrystalUI.begin();
	for( int n = 0; it != m_listCrystalUI.end(); ++it, ++n )
	{
		if( n != pSelItem->getID() )
			continue;

		if( (*it).IsEmpty() )
			GetSystem()->SendXGToCrystal();
		else
			GetSystem()->SendCrystalToXG();
	}
}
//////////////////////////////////////////////////////////////////////////

bool cXaiGuageUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eUpdate_Current_XaiGuage, this );
	GetSystem()->Register( SystemType::eUpdate_Max_XaiGuage, this );
	GetSystem()->Register( SystemType::eChange_QuickSlot_Expand, this );
	return true;
}

void cXaiGuageUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eUpdate_Current_XaiGuage:
		{
			u2 nCrystal = 0;			u4 nGuage = 0;
			kStream >> nCrystal >> nGuage;
			_UpdateGuage( nCrystal, nGuage );
		}
		break;
	case SystemType::eUpdate_Max_XaiGuage:
		{
			u2 nMaxCrystal = 0;			u4 nMaxGuage = 0;
			kStream >> nMaxCrystal >> nMaxGuage;
			_UpdateMaxGuage( nMaxCrystal, nMaxGuage );
		}
		break;
	case SystemType::eChange_QuickSlot_Expand:
		{
			bool bOpend = false;
			kStream >> bOpend;
			if( bOpend )		SetPos( CsPoint(400, 575 - 44) );
			else				SetPos( CsPoint(400, 575) );

			ResetDeviceScript();
		}
		break;
	}
}