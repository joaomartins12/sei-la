
#include "stdafx.h"
#include "WebWin.h"
#include "WebControl.h"
#include "nlib/cURL.h"

bool GetTamerRankURL(char *szBuffer, int nBufferSize, char *szTamerName) // szBuffer is result URL
{
	// szTamerName으로 제공된 테이머 이름으로 해당 넘의 랭킹 정보창 URL을 돌려준다.
	// 결과값은 szBuffer
	// nSvrNo  100:루체, 200:리바, 300:리리스

	char szResTamerName[222];
	cURL::Encode(szTamerName, szResTamerName);

	switch(net::cluster)
	{
		/*
	case 100 : strcpy_s(szBuffer, nBufferSize, "http://www.digimonmasters.com/ranking/game_pop.aspx?v=ruche&o=");	break;
	case 200 : strcpy_s(szBuffer, nBufferSize, "http://www.digimonmasters.com/ranking/game_pop.aspx?v=riva&o=");	break;
	case 300 : strcpy_s(szBuffer, nBufferSize, "http://www.digimonmasters.com/ranking/game_pop.aspx?v=riri&o=");	break;
	case 400 : strcpy_s(szBuffer, nBufferSize, "http://www.digimonmasters.com/ranking/game_pop.aspx?v=barba&o=");	break;
	*/

	default  : return false;
	}	

	strcat_s(szBuffer, nBufferSize, szResTamerName);
	return true;
}

void cWebWin::SetWebControl( eWEBWIN eType, CsPoint ptPos, CsPoint ptSize, void* pValue /* = NULL */ )
{
	m_eType = eType;
	m_pCancelButton = AddButton( CsPoint( ptPos.x + ptSize.x - 25,  ptPos.y + 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	

	m_pBGWindow = NiNew cPopUp;
	m_pBGWindow->Init( m_pRoot, ptPos, ptSize, CsPoint( 32, 32 ), "Control_G\\Popup\\WebDlg", "tga", false );														

	TCHAR szWebName[ 256 ] = {0,};
	switch( m_eType )
	{
	case eRANK:
		{
#ifdef TAIWAN_NAME_LENGTH
			if( _tcslen( (TCHAR*)pValue ) > NAME_MAX_LEN )	// 중문 이름 길이는 8자를 넘을 수 없다.
			{
				cPrintMsg::PrintMsg( 10021 );
				return;
			}
#else
			if( _tcslen( (TCHAR*)pValue ) > Language::pLength::name )
			{
				cPrintMsg::PrintMsg( 10021 );
				return;
			}
#endif

			char szTempName[ Language::pLength::name+1 ];	
			W2M( szTempName, (TCHAR*)pValue, Language::pLength::name+1 );							

			char szTempWebName[ 256 ];
			if( GetTamerRankURL( szTempWebName, 256, szTempName )  == false )
			{
				cPrintMsg::PrintMsg( 10021 );
				return;
			}

			M2W( szWebName, szTempWebName, 256 );
		}
		break;
	case eCASH:
		{
			return;
// 			char szTempWebName[ 256 ];
// 			char szPassEnCode[ Language::pLength::pass ];
// 
// 			EnCodeA( net::pass, szPassEnCode );
// 			//_EnCode( "skatkdqo", szPassEnCode );
// 
// 			sprintf_s( szTempWebName, 256, "https://www.joymax.com/billing/Property_Front.aspx?WorkUrl=https://billingcp.joymax.com/ChargeSilk/pgList.aspx",
// 				net::id, szPassEnCode, net::ip );
// 			
// 			//sprintf_s( szTempWebName, 256, "http://bill.wemade.net/index.asp?loginid=%s&pwd=%ssw3xdffe&userip=%s&fromcode=G&gamecode=307",
// 			//	"nangbae", szPassEnCode, net::ip );	 		
// 
// 			M2W( szWebName, szTempWebName, 256 );
		}
		break;
	default:
		return;
	}	

	m_hWndwebRank = CreateWebControl( szWebName, WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER , ptPos.x + 8, ptPos.y + 27, 
		ptSize.x - 16, ptSize.y - 35, GAMEAPP_ST.GetHWnd(), NULL );			

	::ShowWindow( m_hWndwebRank, SW_SHOW );
	::SetFocus( m_hWndwebRank );	
}

void cWebWin::Destroy()
{	
	cBaseWindow::Delete();	
}

void cWebWin::ResetMap()
{
	Close( false );
}

void cWebWin::DeleteResource()
{		
	SAFE_NIDELETE( m_pBGWindow );
	::DestroyWindow( m_hWndwebRank );
	::SetFocus( GAMEAPP_ST.GetHWnd() );

	DeleteScript();
}

void cWebWin::Create(int nValue /* = 0  */)
{	
	cBaseWindow::Init();
	InitScript( NULL, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), false, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );	
}

cBaseWindow::eMU_TYPE
cWebWin::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;	

	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		{
			Close();

			switch( m_eType )
			{
			case eRANK:
				break;
			case eCASH:
				break;
			}
		}		
		return muReturn;
	default:
		return muReturn;
	}	

	return muReturn;
}

void cWebWin::Update(float const& fDeltaTime)
{
	_UpdateMoveWindow();	
}

void cWebWin::Render()
{
	m_pBGWindow->Render();
	RenderScript();		
}

void cWebWin::ResetDevice()
{	
	if( m_pBGWindow )
		m_pBGWindow->ResetDevice();
	ResetDeviceScript();		
}


