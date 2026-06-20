#include "stdafx.h"
#include "CCursor.h"
#include "resource.h"

CCursor::CCursor() : 
m_nButtonState(CURSOR::BUTTON_NONE),
m_nWheel(INVALIDE_WHEEL),
m_eIconType(CURSOR_ICON::CI_NONE),
m_nIconSlot(0),
m_nIconCount(0),
m_pIFIcon(NULL),
m_bIconDropMessageBox(false),
m_nIconSubSlot1(0),
m_nIconSubSlot2(0),
m_nIconSubSlot3(0),
m_eCurCursor(CURSOR_RES::NONE),
m_bShowCursor(true)
{
	for( int i=0; i<CURSOR_RES::MAX_COUNT; ++i )
		m_hCursor[ i ] = NULL;
}

CCursor::~CCursor()
{
}

void CCursor::_LoadCursor( int const& nType, std::wstring const& wsFileName, LPCWSTR defultCursor, HINSTANCE hIns )
{
	if( CURSOR_RES::MAX_COUNT <= nType )
		return;

	std::wstring curFile = wsFileName + L".Cur";
	m_hCursor[ nType ] = LoadCursorFromFile( curFile.c_str() );
	if( m_hCursor[ nType ] )
		return;

	std::wstring aniFile = wsFileName + L".Ani";
	m_hCursor[ nType ] = LoadCursorFromFile( aniFile.c_str() );

	if( m_hCursor[ nType ] )
		return;
	
	m_hCursor[ nType ] = LoadCursor( hIns, defultCursor );
}

void CCursor::Init(HINSTANCE hIns)
{	
	_LoadCursor( CURSOR_RES::NORMAL,	L"Data\\NP\\Cursor\\DEFAULT_CUR",	(LPCWSTR)IDC_CURSOR,		hIns );
	_LoadCursor( CURSOR_RES::HAND,		L"Data\\NP\\Cursor\\HAND_CUR",		(LPCWSTR)IDC_HAND_CUR,		hIns );
	_LoadCursor( CURSOR_RES::ATTACK,	L"Data\\NP\\Cursor\\ATTACK_CUR",	(LPCWSTR)IDC_ATTACK_CUR,	hIns );
	_LoadCursor( CURSOR_RES::DIGITAMA,	L"Data\\NP\\Cursor\\DIGITAMA_CUR",	(LPCWSTR)IDC_DIGITAMA_CUR,	hIns );
	_LoadCursor( CURSOR_RES::STORE,		L"Data\\NP\\Cursor\\STORE_CUR",		(LPCWSTR)IDC_STORE_CUR,		hIns );
	_LoadCursor( CURSOR_RES::TALK,		L"Data\\NP\\Cursor\\TALK_CUR",		(LPCWSTR)IDC_TALK_CUR,		hIns );
		
	SetCursorRes( CURSOR_RES::NORMAL );

	m_pWarehouseAdapt = new AdaptWareHouse;

	if( m_pWarehouseAdapt )
	{
		m_pWarehouseAdapt->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CT_WAREHOUSE ) );
	}

	m_eIconType = CURSOR_ICON::CI_NONE;
	m_bIconDropMessageBox = false;
	m_pIFIcon = NULL;
}

bool CCursor::CheckClickBox( int x, int y, int sx, int sy, int type )
{ 
	return ( m_nButtonState == type )&&IsInCursor( x, y, x+sx, y+sy ); 
}

CURSOR::eTYPE CCursor::CheckClickBox( int x, int y, int sx, int sy )
{ 
	return IsInCursor( x, y, x+sx, y+sy ) ? GetButtonState() : CURSOR::BUTTON_OUTWINDOW; 
}

CURSOR::eTYPE CCursor::CheckClickBox( CsRect rc )
{ 
	return rc.PtInRect( m_ptCursorPos ) == TRUE ? GetButtonState() : CURSOR::BUTTON_OUTWINDOW; 
}

bool CCursor::IsInCursor( int l, int t, int r, int b )
{ 
	return ( (m_ptCursorPos.x >= l) && (m_ptCursorPos.y >= t ) && (m_ptCursorPos.x <= r) && (m_ptCursorPos.y <= b) ); 
}

void CCursor::Input(const MSG& p_kMsg)
{
	switch(p_kMsg.message)	//2016-01-11-nova 윈도우 영역 벗어나도 마우스값을 입력받기 위해서
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		{
			SetCapture(GAMEAPP_ST.GetHWnd());

		}
		break;

	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT        mouseEvent;		//	2016-08-18(eiless84)	WM_MOUSELEAVE 이벤트 추가.
			mouseEvent.cbSize = sizeof(mouseEvent);
			mouseEvent.dwFlags = TME_LEAVE;
			mouseEvent.hwndTrack = GAMEAPP_ST.GetHWnd();
			TrackMouseEvent(&mouseEvent);
		}
		break;
	

	case WM_MOUSELEAVE:
		GLOBALINPUT_ST.ReleaseCameraRotation();		// 게임 화면 벗어나면.	우클릭 마우스 회전 취소.
		ShowCursorRes();
		break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_CAPTURECHANGED:	
		ReleaseCapture();
		break;

	//case WM_KILLFOCUS:									// #3891 이동중 포커스 잃으면 계속 이동하는 버그에 대한 처리. 추후에 처리.
	//	{
	//		if(g_pCharMng && g_pCharMng->GetTamerUser())
	//			g_pCharMng->GetTamerUser()->DeletePath();	// 이동중인 패스를 삭제.

	//		if(g_pServerMoveOwner)
	//			g_pServerMoveOwner->KeyReset();
	//	}
	//	break;
	case WM_SETFOCUS:
		{
			GLOBALINPUT_ST.ResetModifier();
		}	
		break;
	}

	GetCursorPos( &m_ptCursorPos );
	ScreenToClient( GAMEAPP_ST.GetHWnd(), &m_ptCursorPos );	

	switch(p_kMsg.message)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		{
			if( g_pResist->m_Global.s_bRightHandMouse )
			{
				if( _IconLBtnDown() == false )
				{
					SetButtonState( CURSOR::LBUTTON_DOWN );

					if( g_pGameIF != NULL )
					{
						if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_CARD_EVENT ) == true )
							g_pGameIF->OnLButtonDown( m_ptCursorPos );
					}
				}
			}

			else
			{
				if( _IconCancel() == false ) 
				{
					SetButtonState( CURSOR::RBUTTON_DOWN );
				}
			}
		}
		break;

	case WM_LBUTTONUP:
		{
			if( g_pResist->m_Global.s_bRightHandMouse )
			{
				SetButtonState( CURSOR::LBUTTON_UP );

				if( _IconLBtnUp() == false )
				{
					if( g_pGameIF != NULL )
						g_pGameIF->OnLButtonUp( m_ptCursorPos );
				}
			}

			else
			{
				SetButtonState( CURSOR::RBUTTON_UP );

				if( GetButtonState() == CURSOR::RBUTTON_UP )
				{
					if( g_pGameIF != NULL )
						g_pGameIF->OnRButtonUp( m_ptCursorPos );
				}
			}
		}
		break;

	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
		{
			if( g_pResist->m_Global.s_bRightHandMouse )
			{
				if( _IconCancel() == false ) 
				{
					SetButtonState( CURSOR::RBUTTON_DOWN );
				}
			}

			else
			{
				if( _IconLBtnDown() == false )
				{
					SetButtonState( CURSOR::LBUTTON_DOWN );
				}
			}
		}
		break;

	case WM_RBUTTONUP:
		{
			if( g_pResist->m_Global.s_bRightHandMouse )
			{
				SetButtonState( CURSOR::RBUTTON_UP );
				if( GetButtonState() == CURSOR::RBUTTON_UP )
				{
					if( g_pGameIF != NULL )
						g_pGameIF->OnRButtonUp( m_ptCursorPos );
				}
			}

			else
			{
				if( _IconLBtnUp() == false )
				{
					SetButtonState( CURSOR::LBUTTON_UP );
					if( g_pGameIF != NULL )
						g_pGameIF->OnLButtonUp( m_ptCursorPos );
				}
			}
		}
		break;

	case WM_MBUTTONDOWN:
		{
			m_nButtonState = CURSOR::MBUTTON_DOWN;
		}
		break;

	case WM_MBUTTONUP:
		{
			m_nButtonState = CURSOR::MBUTTON_UP;
		}
		break;

	case WM_MOUSEMOVE:
		{
			if(p_kMsg.wParam == MK_LBUTTON)
			{
				if( g_pResist->m_Global.s_bRightHandMouse )
				{
					if(m_nButtonState != CURSOR::LBUTTON_PRESS)
					{
						m_nButtonState = CURSOR::LBUTTON_DOWN;	
					}
				}
				else
				{
					if(m_nButtonState != CURSOR::RBUTTON_PRESS)
					{
						m_nButtonState = CURSOR::RBUTTON_DOWN;		
					}
				}
			}
			else if(p_kMsg.wParam == MK_RBUTTON)
			{
				if( g_pResist->m_Global.s_bRightHandMouse )
				{
					if(m_nButtonState != CURSOR::RBUTTON_PRESS)
					{
						m_nButtonState = CURSOR::RBUTTON_DOWN;
					}
				}
				else 
				{
					if(m_nButtonState != CURSOR::LBUTTON_PRESS)
					{
						m_nButtonState = CURSOR::LBUTTON_DOWN;
					}
				}
			}
			else if(p_kMsg.wParam == MK_MBUTTON && m_nButtonState != CURSOR::MBUTTON_PRESS)
			{
				m_nButtonState = CURSOR::MBUTTON_DOWN;
			}			
		}
		break;

	case WM_MOUSEWHEEL:
		{
			m_nWheel = GET_WHEEL_DELTA_WPARAM(p_kMsg.wParam);
		}
		break;
	}
}

void CCursor::SetButtonState( CURSOR::eTYPE type )
{
	if( ( type != CURSOR::RBUTTON_DOWN )&&( type != CURSOR::RBUTTON_PRESS )
#ifdef KEYBOARD_MOVE
		&& ( type != CURSOR::LBUTTON_DOWN )&&( type != CURSOR::LBUTTON_PRESS )
#endif
		)
	{
		if( GLOBALINPUT_ST.ReleaseCameraRotation() == true )
		{
			ShowCursorRes();
			m_nButtonState = CURSOR::BUTTON_NONE;
			return;
		}
	}
	m_nButtonState = type;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
void CCursor::Render()
{
	switch( m_eIconType )
	{
	case CURSOR_ICON::CI_NONE:
		break;
	case CURSOR_ICON::CI_INVEN:
		{
			if( g_pDataMng->GetInven()->GetData( TO_ID( m_nIconSlot ) )->IsEnable() == true )
			{
				g_pIconMng->RenderCursorIconItem( g_pDataMng->GetInven()->GetData( TO_ID( m_nIconSlot ) ), m_ptCursorPos );
			}
			else
			{
				ReleaseIcon();
			}
		}
		break;
	case CURSOR_ICON::CI_WAREHOUSE:
		{
/*			if( g_pDataMng->GetWarehouse()->GetData( TO_ID( m_nIconSlot ) )->IsEnable() == true )
			{
				g_pIconMng->RenderCursorIconItem( g_pDataMng->GetWarehouse()->GetData( TO_ID( m_nIconSlot ) ), m_ptCursorPos );
			}*/

			if( m_pWarehouseAdapt->GetData( TO_ID(m_nIconSlot))->IsEnable() == true )
			{
				g_pIconMng->RenderCursorIconItem( m_pWarehouseAdapt->GetData( TO_ID( m_nIconSlot ) ), m_ptCursorPos );
			}
			else
			{
				ReleaseIcon();
			}
		}
		break;
#ifdef NEW_SHARESTASH
	case CURSOR_ICON::CI_SHARESTASH:
		{	
/*			if( g_pDataMng->GetWarehouse()->GetShareData( TO_ID( m_nIconSlot ) )->IsEnable() == true )
			{
				g_pIconMng->RenderCursorIconItem( g_pDataMng->GetWarehouse()->GetShareData( TO_ID( m_nIconSlot ) ), m_ptCursorPos );
			}*/
			if( m_pWarehouseAdapt->GetShareData( TO_ID(m_nIconSlot))->IsEnable() == true )
			{
				g_pIconMng->RenderCursorIconItem( m_pWarehouseAdapt->GetShareData( TO_ID( m_nIconSlot ) ), m_ptCursorPos );
			}
			else
			{
				ReleaseIcon();
			}
		}
		break;
#endif
	case CURSOR_ICON::CI_TEQUIP:
		{
			if( TO_CONSTANT( m_nIconSlot ) == SERVER_DATA_TEQUIP_CONSTANT )
			{
				if( g_pDataMng->GetTEquip()->GetData( TO_ID( m_nIconSlot ) )->IsEnable() == true )
				{
					g_pIconMng->RenderCursorIconItem( g_pDataMng->GetTEquip()->GetData( TO_ID( m_nIconSlot ) ), m_ptCursorPos );
				}
				else
				{
					ReleaseIcon();
				}
			}
			else
			{
				assert_cs( TO_CONSTANT( m_nIconSlot ) == SERVER_DATA_DIGIVICE_CONSTANT );
				if( g_pDataMng->GetTEquip()->GetDigiviceItem()->IsEnable() == true )
				{
					g_pIconMng->RenderCursorIconItem( g_pDataMng->GetTEquip()->GetDigiviceItem(), m_ptCursorPos );
				}
				else
				{
					ReleaseIcon();
				}
			}
		}
		break;
	case CURSOR_ICON::CI_CHIPSET:
		{
			if( g_pDataMng->GetDigivice()->GetChipset( TO_ID( m_nIconSlot ) )->IsEnable() == true )
				g_pIconMng->RenderCursorIconItem( g_pDataMng->GetDigivice()->GetChipset( TO_ID( m_nIconSlot ) ), m_ptCursorPos );
			else
				ReleaseIcon();
		}
		break;	
	case CURSOR_ICON::CI_TSKILL:
		{
			cSkill::sINFO* pInfo = g_pCharMng->GetTamerUser()->GetSkillMng()->GetSkill( m_nIconSlot );
			if( pInfo != NULL )
				g_pIconMng->RenderCursorIconSkill( pInfo, m_ptCursorPos );
			else
				ReleaseIcon();
		}
		break;
	case CURSOR_ICON::CI_DSKILL:
		{
			cSkill::sINFO* pInfo = g_pCharMng->GetDigimonUser( m_nIconSubSlot1 )->GetSkillMng()->GetSkill( m_nIconSlot );
			if( pInfo != NULL )
				g_pIconMng->RenderCursorIconSkill( pInfo, m_ptCursorPos );
			else
				ReleaseIcon();
		}
		break;
	case CURSOR_ICON::CI_QUICKSLOT:
		{
			switch( m_nIconSubSlot2 )
			{
			case cResist::ITEM:
				{
					int nIcon = nsCsFileTable::g_pItemMng->GetItem( m_nIconSubSlot3 )->GetInfo()->s_nIcon;
					g_pIconMng->RenderCursorIconItem( nIcon, m_ptCursorPos );
				}
				break;
			case cResist::SKILL:
				{
					cSkill* pSkillMng = NULL;
					switch( m_nIconSubSlot1 )
					{
					case cQuickSlot::Q_TAMER:
					case cQuickSlot::Q_ADDSLOT:
						pSkillMng = g_pCharMng->GetTamerUser()->GetSkillMng();
						break;
					case cQuickSlot::Q_DIGIMON:
						pSkillMng = g_pCharMng->GetDigimonUser( m_nIconSubSlot1 - 1 )->GetSkillMng();
						break;
					}

					if( pSkillMng && pSkillMng->GetFTSkill( m_nIconSubSlot3 ) != NULL )
						g_pIconMng->RenderCursorIconSkill( pSkillMng->GetSkill( m_nIconSubSlot3 )/*->GetFTSkill( m_nIconSubSlot3 )->GetInfo()->s_nIcon*/, m_ptCursorPos );
				}				
				break;
			default:
				assert_cs( false );
			}
		}
		break;
	case CURSOR_ICON::CI_CENTERBAR:
		{
			if( m_nIconSubSlot2 == cResist::ITEM )
			{
				int nIcon = nsCsFileTable::g_pItemMng->GetItem( m_nIconSubSlot3 )->GetInfo()->s_nIcon;
				g_pIconMng->RenderCursorIconItem( nIcon, m_ptCursorPos );
			}			
		}
		break;
	case CURSOR_ICON::CI_TACTICSHOUSE:
		{
#pragma warning( disable : 4312 )
			sCHAR_IMAGE* pCharImage = (sCHAR_IMAGE*)m_nIconSubSlot1;
#pragma warning( default : 4312 )
			g_pIconMng->RenderCursorIconCharImage( pCharImage, m_ptCursorPos );
		}
		break;
#ifdef CROSSWARS_SYSTEM
	case CURSOR_ICON::CI_CROSS:
		{
			if( g_pDataMng->GetInvenCross()->GetData( TO_ID( m_nIconSlot ) )->IsEnable() == true )
			{
				g_pIconMng->RenderCursorIconItem( g_pDataMng->GetInvenCross()->GetData( TO_ID( m_nIconSlot ) ), m_ptCursorPos );
			}
			else
			{
				ReleaseIcon();
			}		
		}
		break;
#endif
	case CURSOR_ICON::CI_INCUBATOR:
		{
			if( m_nIconSubSlot2 == 0 )//디지몬
			{
#pragma warning( disable : 4312 )
				sCHAR_IMAGE* pCharImage = (sCHAR_IMAGE*)m_nIconSubSlot1;
#pragma warning( default : 4312 )
				g_pIconMng->RenderCursorIconCharImage( pCharImage, m_ptCursorPos );
			}
			else if( m_nIconSubSlot2 == 1 )//아이템
			{
				int nIcon = nsCsFileTable::g_pItemMng->GetItem( m_nIconSubSlot1 )->GetInfo()->s_nIcon;
				g_pIconMng->RenderCursorIconItem( nIcon, m_ptCursorPos );
			}
		}
		break;
	default:
		assert_cs( false );
	}
}



void CCursor::IconDropMessageBegin()
{
	// 아이콘 이미 설정 되어 있어야 하며
	// 윈도우안에 이미 존재 하지 않는다.
	assert_cs( m_eIconType != CURSOR_ICON::CI_NONE );
	assert_cs( g_pGameIF != NULL );
	assert_cs( g_pGameIF->IsInWindow( m_ptCursorPos ) == false );
	m_bIconDropMessageBox = true;
}

bool CCursor::IconDropMessageTry()
{
	// 아이콘 이미 설정 되어 있어야 하며
	// 윈도우안에 이미 존재 하지 않는다.
	assert_cs( m_eIconType != CURSOR_ICON::CI_NONE );
	assert_cs( g_pGameIF != NULL );
	assert_cs( g_pGameIF->IsInWindow( m_ptCursorPos ) == false );

	if( m_bIconDropMessageBox == false )
		return false;

	switch( m_eIconType )
	{
	case CURSOR_ICON::CI_INVEN:				
		{			
			CsItem::sINFO* pFTITem = nsCsFileTable::g_pItemMng->GetItem( g_pDataMng->GetInven()->GetData( TO_ID( m_nIconSlot ) )->GetType() )->GetInfo();

			cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_DELETE, pFTITem->s_szName );

			// 버릴려는 아이템의 등급 검사
			if( pFTITem->s_nClass >= 2 )
			{
				cDigimonTalk::Print( 11011 );
			}					
		}		
		break;
	case CURSOR_ICON::CI_TEQUIP:
	case CURSOR_ICON::CI_CHIPSET:
		{
			if( g_pDataMng->IsItemUse( m_nIconSlot ) == true )
			{
				g_pDataMng->SendItemMoveInven( m_nIconSlot );
			}
			ReleaseIcon();
		}		
		break;
	case CURSOR_ICON::CI_TSKILL:
	case CURSOR_ICON::CI_DSKILL:
	case CURSOR_ICON::CI_QUICKSLOT:
	case CURSOR_ICON::CI_TACTICSHOUSE:
	case CURSOR_ICON::CI_CENTERBAR:
		ReleaseIcon();
		break;
	case CURSOR_ICON::CI_WAREHOUSE:
		break;
#ifdef NEW_SHARESTASH
	case CURSOR_ICON::CI_SHARESTASH:
		break;
#endif
#ifdef CROSSWARS_SYSTEM
	case CURSOR_ICON::CI_CROSS:
		break;
#endif
	default:
		assert_cs( false );
	}	
	return true;
}

void CCursor::IconDropOk()
{
	switch( m_eIconType )
	{
	case CURSOR_ICON::CI_INVEN:
		g_pDataMng->GetInven()->ItemDrop( m_nIconSlot, m_nIconCount );
		break;
	default:
		assert_cs( false );
	}
	ReleaseIcon();
}

void CCursor::IconDropCancel()
{
	ReleaseIcon();
}

bool CCursor::_IconCancel()
{
	if( m_eIconType == CURSOR_ICON::CI_NONE )
		return false;

	if( cMessageBox::IsMessageBox() )
		return true;

	ReleaseIcon();
	return true;
}

bool CCursor::_IconLBtnDown()
{
	if( m_eIconType == CURSOR_ICON::CI_NONE )
		return false;

	assert_cs( g_pGameIF );
	return g_pGameIF->CursorIconLBtnDown( m_ptCursorPos, m_eIconType, m_nIconSlot, m_nIconCount );
}

bool CCursor::_IconLBtnUp()
{
	if( m_eIconType == CURSOR_ICON::CI_NONE )
		return false;

	assert_cs( g_pGameIF );
	return g_pGameIF->CursorIconLBtnUp( m_ptCursorPos, m_eIconType, m_nIconSlot, m_nIconCount );
}


bool CCursor::IsNotUseItem( CURSOR_ICON::eTYPE eType, int nItemIndex )
{
	switch( m_eIconType )
	{	
	case CURSOR_ICON::CI_INVEN:				
		{			
			CsItem::sINFO* pFTITem = nsCsFileTable::g_pItemMng->GetItem( g_pDataMng->GetInven()->GetData( TO_ID( m_nIconSlot ) )->GetType() )->GetInfo();

			if(pFTITem->s_dwItemID == nItemIndex)
				return true;
		}		
		break;
	}
	return false;
}

void CCursor::SetIcon( CURSOR_ICON::eTYPE eType, int nSlot, int nCount, void* pIFIcon )
{
	if( m_eIconType != CURSOR_ICON::CI_NONE )
		ReleaseIcon();

	m_eIconType = eType;	

	switch( m_eIconType )
	{
	case CURSOR_ICON::CI_TSKILL:
	case CURSOR_ICON::CI_DSKILL:
	case CURSOR_ICON::CI_QUICKSLOT:
	case CURSOR_ICON::CI_TACTICSHOUSE:
	case CURSOR_ICON::CI_CENTERBAR:
	case CURSOR_ICON::CI_INCUBATOR:
		m_nIconSlot = nSlot;
		break;
	case CURSOR_ICON::CI_INVEN:		m_nIconSlot = TO_INVEN_SID( nSlot );			break;	
	case CURSOR_ICON::CI_CHIPSET:	m_nIconSlot = TO_CHIPSET_SID( nSlot );			break;
	case CURSOR_ICON::CI_WAREHOUSE:	m_nIconSlot = TO_WAREHOUSE_SID( nSlot );		break;
#ifdef NEW_SHARESTASH
	case CURSOR_ICON::CI_SHARESTASH:	m_nIconSlot = TO_SHARESTASH_SID( nSlot );		break;
#endif
	case CURSOR_ICON::CI_TEQUIP:
		{
			if( nSlot != SERVER_DATA_DIGIVICE_CONSTANT )
				m_nIconSlot = TO_TEQUIP_SID( nSlot );
			else
				m_nIconSlot = nSlot;
		}
		break;
#ifdef CROSSWARS_SYSTEM
	case CURSOR_ICON::CI_CROSS:		m_nIconSlot = TO_INVENCROSS_SID( nSlot );			break;	
#endif
	default:
		assert_cs( false );
	}	
	m_nIconCount = nCount;
	if(pIFIcon)
	{
		m_pIFIcon = pIFIcon;
		( (sIFIcon*)m_pIFIcon )->s_eState = sIFIcon::MOUSE_DRAG;
	}

	HideCursorRes();
}

void CCursor::SetIconSubInfo( int nSubSlot1, int nSubSlot2, int nSubSlot3 )
{
	m_nIconSubSlot1 = nSubSlot1;
	m_nIconSubSlot2 = nSubSlot2;
	m_nIconSubSlot3 = nSubSlot3;
}

void CCursor::ReleaseIcon()
{
	// 커서 해제 해야 하는데 당연히... 해당 포인터가 존재 해야 한다.
	switch( m_eIconType )
	{
	case CURSOR_ICON::CI_WAREHOUSE:
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_WAREHOUSE ) == true )
		{
			assert_cs( m_pIFIcon != NULL );
			( (sIFIcon*)m_pIFIcon )->ReleaseState( true );
		}
		break;
 	case CURSOR_ICON::CI_CARDINVEN:
 		break;
	default:
		if( m_pIFIcon != NULL )
		{
			( (sIFIcon*)m_pIFIcon )->ReleaseState( true );
		}
	}

	m_eIconType = CURSOR_ICON::CI_NONE;
	m_bIconDropMessageBox = false;
	m_pIFIcon = NULL;
	m_nIconSlot = 0;

	ShowCursorRes();
}

void CCursor::SetCursorRes( CURSOR_RES::eTYPE type )
{
	if( m_bShowCursor == false )
		return;

	assert_cs( m_hCursor[ type ] != NULL );
	if( m_eCurCursor == type )
		return;

	m_eCurCursor = type;

#pragma warning( disable : 4311 )
	SetClassLong( GAMEAPP_ST.GetHWnd(), GCL_HCURSOR, (LONG)m_hCursor[ m_eCurCursor ] );
	::SetCursor( m_hCursor[ m_eCurCursor ] );
#pragma warning( default : 4311 )
}

void CCursor::SetCursorRes( CsC_AvObject* pObject )
{
	switch( pObject->GetLeafRTTI() )
	{
	case RTTI_TUTORIAL_MONSTER:
	case RTTI_MONSTER:
		SetCursorRes( CURSOR_RES::ATTACK );
		return;
	case RTTI_ITEM:
		SetCursorRes( CURSOR_RES::HAND );
		return;
	case RTTI_TAMER:
		{
			CTamer* pTamer = (CTamer*)pObject;
			cCondition* pCon = pTamer->GetCondition();
			if( pCon && pCon->IsCondition( nSync::Shop ) )
			{
				SetCursorRes( CURSOR_RES::STORE );
				return;
			}
		}
		break;
	case RTTI_NPC:
		{
			switch( ( (CNpc*)pObject )->GetFTNpc()->GetInfo()->s_eType )
			{
			case nsCsNpcTable::NT_TRADE:
				SetCursorRes( CURSOR_RES::STORE );
				return;
			case nsCsNpcTable::NT_DIGITAMA_TRADE:
				SetCursorRes( CURSOR_RES::DIGITAMA );
				return;
			default:
				SetCursorRes( CURSOR_RES::TALK );
				return;
			}
		}
		break;
	case RTTI_EMPLOYMENT:
		SetCursorRes( CURSOR_RES::STORE );
		return;
	}

	SetCursorRes( CURSOR_RES::NORMAL );
}

void CCursor::HideCursorRes()
{
	if( m_bShowCursor == false )
		return;

	m_eCurCursor = CURSOR_RES::NONE;
	m_bShowCursor = false;

	SetClassLong( GAMEAPP_ST.GetHWnd(), GCL_HCURSOR, (LONG)NULL );
	::SetCursor( NULL );
}

void CCursor::ShowCursorRes()
{
	if( m_bShowCursor == true )
		return;

	m_bShowCursor = true;
	SetCursorRes( CURSOR_RES::NORMAL );
}

void CCursor::InitCursor()
{
	ReleaseIcon();
	SetCursorRes( CURSOR_RES::NORMAL );
}

void CCursor::ResetWheel()
{
	m_nWheel = INVALIDE_WHEEL;
}

void CCursor::LoopReset()
{
	if( GetButtonState() == CURSOR::LBUTTON_DOWN )
	{
		SetButtonState( CURSOR::LBUTTON_PRESS );
	}

	if( GetButtonState() == CURSOR::RBUTTON_DOWN )
	{
		SetButtonState( CURSOR::RBUTTON_PRESS );
	}

	if( GetButtonState() != CURSOR::LBUTTON_PRESS && GetButtonState() != CURSOR::RBUTTON_PRESS )
	{
		SetButtonState( CURSOR::BUTTON_NONE );
	}	
}