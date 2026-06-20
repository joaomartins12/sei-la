#include "stdafx.h"
#include "Division.h"
#include "common_vs2019/pParty.h"

cDivision::cDivision():m_pSelMask(NULL),m_pBGWindow(NULL)
{

}

cDivision::~cDivision()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void cDivision::Destroy()
{	
	cBaseWindow::Delete();		
}

void cDivision::DeleteResource()
{	
	DeleteScript();

	SAFE_NIDELETE( m_pSelMask );
	SAFE_NIDELETE( m_pBGWindow );
	m_strDivision.Delete();		
}

void cDivision::Create(int nValue /* = 0  */)
{
	cBaseWindow::Init();					

	if( nValue == 0 )
	{
		_SetBooty();
		CsPoint ptMaxSize = m_strDivision.CalMaxSize( 5 ) + CsPoint( 20, 20 );

		InitScript( NULL, CsPoint( 255, 25 ), ptMaxSize, false, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );					

		m_pBGWindow = NiNew cPopUp;
		m_pBGWindow->Init( m_pRoot, CsPoint::ZERO, ptMaxSize, CsPoint( 3, 3 ), "Control_G\\Popup\\NDlg", "tga", false );

		m_pSelMask = NiNew cSprite;
		m_pSelMask->Init( NULL, CsPoint::ZERO, CsPoint( ptMaxSize.x - 10, 22 ), "Talk\\Common_Seletbar.tga", false );
		m_pSelMask->SetAlpha( 0.5f );		
	}
	else
	{
		_SetRareRate();
		CsPoint ptMaxSize = m_strDivision.CalMaxSize( 5 ) + CsPoint( 20, 20 );

		InitScript( NULL, CsPoint( 255, 25 ), ptMaxSize, false, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );			

		m_pBGWindow = NiNew cPopUp;
		m_pBGWindow->Init( m_pRoot, CsPoint::ZERO, ptMaxSize, CsPoint( 3, 3 ), "Control_G\\Popup\\NDlg", "tga", false );

		m_pSelMask = NiNew cSprite;
		m_pSelMask->Init( NULL, CsPoint::ZERO, CsPoint( ptMaxSize.x - 5, 22 ), "Talk\\Common_Seletbar.tga", false );
		m_pSelMask->SetAlpha( 0.5f ); 		
	}		
}

void cDivision::Update(float const& fDeltaTime)
{
	
}	


cBaseWindow::eMU_TYPE
cDivision::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();

	m_bRenderSelMask = false;

	int nActive1, nActive2;	
	m_ptSelMask = CsPoint( 5, 7 ) + GetRootClient();
	switch( m_strDivision.Update_ForMouse( nActive1, nActive2, m_ptSelMask, 5, CsPoint( 255, 18 ) ) )
	{
	case CURSOR::LBUTTON_DOWN:
	case CURSOR::RBUTTON_DOWN:
		{
			switch( nActive1 )
			{
			case CROP_BASE:	GetSystem()->Send_LootingRuleChange( nCrop::Base, 0, nCrop::RareRate::Lv1 );	break;
			case CROP_FREE:	GetSystem()->Send_LootingRuleChange( nCrop::Free, 0, nCrop::RareRate::Lv1 );	break;
			case CROP_ORDER:GetSystem()->Send_LootingRuleChange( nCrop::Order, 0, nCrop::RareRate::Lv1 );	break;
			case CROP_DICE:	GetSystem()->Send_LootingRuleChange( nCrop::Free, 1, nCrop::RareRate::Lv1 );	break;
			case CLASS_LV1:	GetSystem()->Send_LootingItemRateChange( nCrop::RareRate::Lv1 );	break;
			case CLASS_LV2:	GetSystem()->Send_LootingItemRateChange( nCrop::RareRate::Lv2 );	break;
			case CLASS_LV3:	GetSystem()->Send_LootingItemRateChange( nCrop::RareRate::Lv3 );	break;
			case CLASS_LV4:	GetSystem()->Send_LootingItemRateChange( nCrop::RareRate::Lv4 );	break;			
			default:				
				break;
			}

			Close();
		}	
	}

	switch( nActive1 )
	{
	case CROP_BASE:	
	case CROP_FREE:	
	case CROP_ORDER:	
	case CROP_DICE:
	case CROP_END:
	case CLASS_LV1:
	case CLASS_LV2:
	case CLASS_LV3:
	case CLASS_LV4:
		m_bRenderSelMask = true;
		break;
	}

	return muReturn;
}

void cDivision::Render()
{
	RenderScript();	

	m_pBGWindow->Render();
	
	m_strDivision.Render( GetRootClient() + CsPoint( 10, 10 ), 5 );
	if( m_bRenderSelMask == true )
		m_pSelMask->Render( m_ptSelMask );	
}

void cDivision::ResetDevice()
{
	ResetDeviceScript();
	
	m_strDivision.ResetDevice();
	m_pBGWindow->ResetDevice();
	m_pSelMask->ResetDevice();
}

void cDivision::_SetBooty()
{
	m_strDivision.Delete();

	cString* pString;
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );

	ICONITEM::eTYPE eIconType = ICONITEM::Community;
	int nIconIdx[4] = { 14, 16, 17, 15 };

	pString = NiNew cString;
	ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_BASE" ).c_str() );
	pString->AddIcon( CsPoint( 15, 15 ), eIconType, nIconIdx[0] );
	pString->TailAddSizeX( 3 );
	pString->AddText( &ti );
	pString->SetValue1( CROP_BASE );	
	m_strDivision.AddTail( pString );	

	pString = NiNew cString;	
	ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_ORDER" ).c_str() );
	pString->AddIcon( CsPoint( 15, 15 ), eIconType, nIconIdx[1] );
	pString->TailAddSizeX( 3 );
	pString->AddText( &ti );
	pString->SetValue1( CROP_ORDER );
	m_strDivision.AddTail( pString );

	pString = NiNew cString;	
	ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_FREE" ).c_str() );
	pString->AddIcon( CsPoint( 15, 15 ), eIconType, nIconIdx[2] );
	pString->TailAddSizeX( 3 );
	pString->AddText( &ti );
	pString->SetValue1( CROP_FREE );
	m_strDivision.AddTail( pString );

	pString = NiNew cString;	
	ti.SetText(UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_RANDOM" ).c_str() );
	pString->AddIcon( CsPoint( 15, 15 ), eIconType, nIconIdx[3] );
	pString->TailAddSizeX( 3 );
	pString->AddText( &ti );
	pString->SetValue1( CROP_DICE );
	m_strDivision.AddTail( pString );

	pString = NiNew cString;	
	ti.SetText( UISTRING_TEXT( "COMMON_TXT_CANCEL" ).c_str() );		
	pString->AddText( &ti );
	pString->SetValue1( CROP_END );
	m_strDivision.AddTail( pString );
}

void cDivision::_SetRareRate()
{
	m_strDivision.Delete();
	
	cString* pString;
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );

	ti.s_Color = g_pDataMng->GetItemColor( 2 );
	pString = NiNew cString;
	ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_ITEM_RARE_ALL" ).c_str() );	
	pString->AddText( &ti );
	pString->SetValue1( CLASS_LV1 );	
	m_strDivision.AddTail( pString );	

	ti.s_Color = g_pDataMng->GetItemColor( 3 );
	pString = NiNew cString;	
	ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_ITEM_RARE_MAGIC" ).c_str() );	
	pString->AddText( &ti );
	pString->SetValue1( CLASS_LV2 );	
	m_strDivision.AddTail( pString );	

	ti.s_Color = g_pDataMng->GetItemColor( 4 );
	pString = NiNew cString;	
	ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_ITEM_RARE_REARE" ).c_str() );	
	pString->AddText( &ti );
	pString->SetValue1( CLASS_LV3 );
	m_strDivision.AddTail( pString );

	ti.s_Color = g_pDataMng->GetItemColor( 5 );
	pString = NiNew cString;	
	ti.SetText( UISTRING_TEXT( "PARTY_ITEM_LOOTING_RULE_ITEM_RARE_EPIC" ).c_str() );	
	pString->AddText( &ti );
	pString->SetValue1( CLASS_LV4 );
	m_strDivision.AddTail( pString );	

	ti.s_Color = FONT_WHITE;
	pString = NiNew cString;	
	ti.SetText( UISTRING_TEXT( "COMMON_TXT_CANCEL" ).c_str() );		
	pString->AddText( &ti );
	pString->SetValue1( CROP_END );
	m_strDivision.AddTail( pString );
}



bool cDivision::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void cDivision::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}