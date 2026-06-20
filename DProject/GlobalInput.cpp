
#include "stdafx.h"
#include "GlobalInput.h"

cGlobalInput::cGlobalInput()
{
	m_bCameraRotation = false;

	m_SameClickCoolTime.SetEndTime( 1.0f );

	mHotKey = NULL;

	iModifier = DMKEYBOARD::KMOD_NONE;

	for(int i=0; i<4; ++i)
	{
		bKBCamera[i] = false;
	}
}

void cGlobalInput::_Keyboard_Input( const MSG& p_kMsg )
{
	m_Msg = p_kMsg;

	bool bAbsoluteMacro = false;
	bool bEdit = ( GetFocus() != GAMEAPP_ST.GetHWnd() ) || g_IME.GetEdit();

	Modified(p_kMsg);

	// 메세지 박스 떠있다면 이 이후 키는 무시
	if( ( bEdit == false )||( (bAbsoluteMacro = _IsAbsoluteMacroKey(p_kMsg)) == true ) )
		_MacroKey( p_kMsg );
}

bool cGlobalInput::_MacroKey( const MSG& p_kMsg )
{
	//2017-04-12-nova	포탈이동시 키보드입력안되도록
	if(g_pResist->IsMovePortal())
	{
		return false;
	}

// 	if( p_kMsg.wParam == '[' )
// 	{
// 
// 		static bool bWireframe = false;
// 		bWireframe = !bWireframe;
// 		if( nsCsGBTerrain::g_pCurRoot )
// 			nsCsGBTerrain::g_pCurRoot->SetWireframe( bWireframe );
// 
// 		return true;
// 	}
// 	else if( p_kMsg.wParam == ']' )
// 	{
// 		static bool bOcclusionCull = true;
// 		bOcclusionCull = !bOcclusionCull;
// 		if( CAMERA_STPTR )
// 			CAMERA_STPTR->SetUseOcclusionCulling( bOcclusionCull );
// 		return true;
// 	}



	if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_DATS )
	{
		if( g_pDatsCenter->GetState() > CDatsCenter::eSTATE_ENCHENT )
			return false;
	}

	if( g_pTacticsAni->IsPlay() == true )
		return false;

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_WEBWIN ) )
	{
		return false;
	}

	if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_EVENT )
	{
		return KBEventScene(p_kMsg);
	}

	if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_DATA_TRADE )
		return true;

	if( KBChattingEsc(p_kMsg) )
		return true;

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_NEW_CASHSHOP ) ) 
		return KBCashShop(p_kMsg);

	KeyBoardMenu(p_kMsg);

	KeyBoardETC(p_kMsg);	

#ifdef KEYBOARD_MOVE	
	KeyBoardMove(p_kMsg);	
#endif	// KEYBOARD_MOVE

#ifdef ENABLE_KEYBOARD_CAMERA 
	KeyBoardCamera(p_kMsg);	
#endif

	if( g_pGameIF->OnMacroKey(p_kMsg) == true )
		return true;

	return false;
}

bool cGlobalInput::_IsAbsoluteMacroKey(const MSG& p_kMsg)
{
	if(p_kMsg.message == WM_KEYUP)
	{
		switch(p_kMsg.wParam)
		{
		case VK_ESCAPE:
		case VK_RETURN:
		case VK_NUMLOCK:
		case VK_UP:
		case VK_DOWN:
			{
				return true;
			}
		}
	}

	return false;
}

bool cGlobalInput::ScreenShot()
{
	// 폴더가 존재 하지 않는다면 폴더 생성
	nsCSFILE::CreateDirectory( _T( ".\\ScreenShot" ) );

	// 파일 이름 생성
	TCHAR szFileName[ MAX_FILENAME ];
	SYSTEMTIME st;
	GetLocalTime( &st );

	TCHAR* pcScreenshotSuffix = _T("");
#ifdef _GIVE
	pcScreenshotSuffix = _T(".jpg");
#else
	pcScreenshotSuffix =  _T(".tga");
#endif 
	_stprintf_s( szFileName, MAX_FILENAME, _T("ScreenShot\\%.2d%.2d%.2d_%.2d%.2d%.2d"), st.wYear%100, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );

	_tcscat_s( szFileName, 512, pcScreenshotSuffix );

	std::string cc = nBase::w2m( szFileName );

	NiRenderer::GetRenderer()->LockRenderer();

	//#ifdef _GIVE
	bool bResult = g_pEngine->m_spRenderer->SaveScreenShot( cc.c_str(), NiRenderer::FORMAT_JPEG );
	//#else
	// 	bool bResult = g_pEngine->m_spRenderer->SaveScreenShot( cc.c_str(), NiRenderer::FORMAT_JPEG ); // 이놈은 실패가 떨어져서 주석 처리함
	// #endif

	NiRenderer::GetRenderer()->UnlockRenderer();

	TCHAR szFullPath [ 512 ];
	GetCurrentDirectory( 512, szFullPath );
	_tcscat_s( szFullPath, 512, _T("\\") );
	_tcscat_s( szFullPath, 512, szFileName );
	cPrintMsg::PrintMsg( cPrintMsg::SCREENSHOT, szFullPath );

	return true;
}

void cGlobalInput::_Mouse_Input( eMOUSE_INPUT eMouse )
{
	//2017-04-12-nova	포탈이동시 마우스입력안되도록
	if(g_pResist->IsMovePortal())
	{
		return;
	}

	bool bEnablePick = false;
	bool bEnableCamera = false;

	switch( eMouse )
	{
	case MOUSEINPUT_DISABLE:
		break;

	case MOUSEINPUT_CAMERA:
		bEnableCamera = true;
		break;
	
	case MOUSEINPUT_PICK:
		bEnablePick = true;
		break;

	case MOUSEINPUT_ENABLE:
		bEnablePick = true;
		bEnableCamera = true;
		break;

	default:
		assert_cs( false );
	}
#ifdef KEYBOARD_MOVE
	bool bIsKeyBoardMove = false;

	if( g_pResist->m_Global.s_bMove_KeyBoard && 
		( g_pServerMoveOwner->GetKeyCheck() & CUserServerMove::KEY_LEFT 
		|| g_pServerMoveOwner->GetKeyCheck() & CUserServerMove::KEY_RIGHT 
		|| g_pServerMoveOwner->GetKeyCheck() & CUserServerMove::KEY_UP 
		|| g_pServerMoveOwner->GetKeyCheck() & CUserServerMove::KEY_DOWN ) )
	{
		bIsKeyBoardMove = true;
	}
#endif
	// 마우스 클릭 이동
	if( bEnablePick && CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_DOWN)
	{	
		if(g_pServerMoveOwner)
			g_pServerMoveOwner->KeyReset();

		// 픽킹이 되었다면
		iModifier = DMKEYBOARD::KMOD_NONE;	//2016-01-25-nova 피킹상태에서 조합키 초기화

		cType type;
		CsC_AvObject* pPickObject;
		CTamerUser* pTamer = g_pCharMng->GetTamerUser();
		if( ( pPickObject = g_pMngCollector->PickObject( type, CURSOR_ST.GetPos(), false ) ) != NULL )
		{
			switch( type.m_nClass )
			{
				// 아이템 쪽은 원클릭 즉시 실행
			case nClass::Item:
				{
					if( pTamer->IsRide() )
					{
						cPrintMsg::PrintMsg( 30014 );
					}
					else
					{
						pTamer->SetArriveTarget( pPickObject );
						pTamer->SetTargetPos( pPickObject->GetPos(), 0.0f, false, false );
					}
				}
				break;

				// 라이딩 상태라면 - 테이머 선택으로
			case nClass::Digimon:
				{
					assert_cs( pPickObject->IsKindOf( RTTI_DIGIMON ) );
					CDigimon* pDigimon = (CDigimon*)pPickObject;
					if( pDigimon->IsRide() )
					{
						int nLink = pDigimon->GetTamerLink();
						if( g_pCharMng->IsTamer( nLink ) )
						{
							pPickObject = g_pCharMng->GetTamer( nLink );
							type.SetIDX(pPickObject->GetIDX(), nClass::Tamer);
							//type.m_nIDX = pPickObject->GetIDX();
							//type.m_nClass = nClass::Tamer;
						}
					}

					pTamer->ReleaseArriveTarget();
					g_pCharResMng->SetTargetMark( pPickObject );
				}				
				break;

			default:
				pTamer->ReleaseArriveTarget();				
				g_pCharResMng->SetTargetMark( pPickObject );
				break;
			}

			// 예전 클릭 된거와 같은거 클릭이라면
			bool bSameClickCoolTime = m_SameClickCoolTime.IsCoolTime();
			if( ( _IsSameClick( type ) == true )&&( bSameClickCoolTime == true ) )
			{				
				g_pServerMoveOwner->DBClick_Object( type, pPickObject );				
			}
			m_PickData.s_Type = type;
		}
		// 픽킹된것이 없다면
		else
		{
#ifdef KEYBOARD_MOVE
			if( !bIsKeyBoardMove )
			{
				bool bDoorPick = false;
				if( ( bDoorPick == false && g_pResist->m_Global.s_bMove_Mouse
					&& g_pServerMoveOwner->GetCurState() == g_pServerMoveOwner->GetBackupState() )
					|| nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
				{
					g_pServerMoveOwner->SetKeyBoardMoveDir();
					g_pServerMoveOwner->KeyAutoMapping();

					pTamer->ReleaseArriveTarget();
					CsPoint ptCursor = CURSOR_ST.GetPos();
					g_pServerMoveOwner->SetPos_FromMouse( ptCursor, 0.0f, true, false );
	#ifdef ZONEMAP_CLICK_MOVE	// 존맵 클릭 이동 중에 터레인 픽킹 이동 시 존맵의 도착 지점 갱신
					if(g_pGameIF->IsActiveWindow(cBaseWindow::WT_ZONEMAP))	
					{
						g_pGameIF->GetZoneMap()->RefreshArrivePoint(ptCursor);
					}
	#endif
				}
			}
#else	// KEYBOARD_MOVE
			pTamer->ReleaseArriveTarget();
			CsPoint ptCursor = CURSOR_ST.GetPos();
			g_pServerMoveOwner->SetPos_FromMouse( ptCursor, 0.0f, true, false );
#ifdef ZONEMAP_CLICK_MOVE	// 존맵 클릭 이동 중에 터레인 픽킹 이동 시 존맵의 도착 지점 갱신
			if(g_pGameIF->IsActiveWindow(cBaseWindow::WT_ZONEMAP))	
			{
				g_pGameIF->GetZoneMap()->RefreshArrivePoint(ptCursor);
			}
#endif	// ZONEMAP_CLICK_MOVE
#endif	// KEYBOARD_MOVE
		}
	}

	if( bEnableCamera )
	{
		// 마우스 카메라 회전
		if( CURSOR_ST.GetButtonState() == CURSOR::RBUTTON_DOWN )
		{
			m_bCameraRotation = true;
			m_ptBackupCursorPos = CURSOR_ST.GetPos();
			m_ClickPos = m_ptBackupCursorPos;

			ClientToScreen( GAMEAPP_ST.GetHWnd(), &m_ptBackupCursorPos );
			CURSOR_ST.HideCursorRes();
		}

		if(m_bCameraRotation && CURSOR_ST.GetButtonState() == CURSOR::RBUTTON_PRESS)
		{
			int deltaX = CURSOR_ST.GetPos().x - m_ClickPos.x;
			int deltaY = CURSOR_ST.GetPos().y - m_ClickPos.y;  

			if( deltaX || deltaY )
			{
				CAMERA_ST.OnRButtonPressed( deltaX, deltaY );
				SetCursorPos( m_ptBackupCursorPos.x, m_ptBackupCursorPos.y );
			}        
		}

		if( CURSOR_ST.GetButtonState() == CURSOR::RBUTTON_UP)
		{
			m_bCameraRotation = false;
			CURSOR_ST.ShowCursorRes();
		}

		if( CURSOR_ST.GetButtonState() == CURSOR::MBUTTON_DOWN )
		{
			g_pServerMoveOwner->ViewReset();
		}

		int nWheel = CURSOR_ST.GetWheel();

		if( nWheel != 0 )
		{
			CAMERA_ST.SetDeltaDist( (float) - nWheel );
			CURSOR_ST.ResetWheel();
		}
	} 
}

//=========================================================================================================
//		픽킹
//=========================================================================================================

void cGlobalInput::ReleasePickData( CsC_AvObject* pObject )
{
	// 같다면 지워주자
	if( m_PickData.s_Type.m_nUID == pObject->GetUniqID() )
	{
		m_PickData.Reset();
	}
}

bool cGlobalInput::_IsSameClick( cType type )
{
	m_SameClickCoolTime.Start();
	if( m_PickData.s_Type.m_nUID != type.m_nUID )
	{		
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
void cGlobalInput::KeyboardUpdate()
{
#ifdef ENABLE_KEYBOARD_CAMERA
	KeyBoardCameraUpdate();
#endif
}

void cGlobalInput::Modified(const MSG& p_kMsg)
{
	USHORT uScanCode = LOBYTE( HIWORD( p_kMsg.lParam ) );

	switch( uScanCode )
	{
	case 241: 
	case 242: 
		{ 

		}
		break;

	default:
		{
			BYTE byKeyCode = static_cast< BYTE >( ::MapVirtualKeyW( uScanCode, 3 ) );

			bool bIsExtend = ( ( HIBYTE( HIWORD( p_kMsg.lParam ) ) & 0x01 ) == 0x01 )?( true ):( false );			

			switch( byKeyCode )
			{
			case VK_LMENU:	
				{
					if(p_kMsg.message == WM_KEYDOWN ||  p_kMsg.message == WM_SYSKEYDOWN)
					{
						if(!bIsExtend)
						{
							iModifier = DMKEYBOARD::KMOD_LMENU;
						}
						else
						{
							iModifier = DMKEYBOARD::KMOD_RMENU;
						}
					}

					if(p_kMsg.message == WM_KEYUP ||  p_kMsg.message == WM_SYSKEYUP)
					{
						iModifier = DMKEYBOARD::KMOD_NONE;
					}
				}
				break;

			case VK_LCONTROL:	
				{					
					if(p_kMsg.message == WM_KEYDOWN)
					{
						if(!bIsExtend)
						{
							iModifier = DMKEYBOARD::KMOD_LCONTROL;
						}
						else
						{
							iModifier = DMKEYBOARD::KMOD_RCONTROL;
						}
					}

					else if(p_kMsg.message == WM_KEYUP)
					{
						iModifier = DMKEYBOARD::KMOD_NONE;
					}
				}
				break;

			case VK_LSHIFT:
				{
					if(p_kMsg.message == WM_KEYDOWN)
					{
						iModifier = DMKEYBOARD::KMOD_LSHIFT;	
					}

					else if(p_kMsg.message == WM_KEYUP)
					{
						iModifier = DMKEYBOARD::KMOD_NONE;						
					}
				}
				break;

			case VK_RSHIFT:	
				{
					if(p_kMsg.message == WM_KEYDOWN)
					{
						iModifier = DMKEYBOARD::KMOD_RSHIFT;
					}

					else if(p_kMsg.message == WM_KEYUP)
					{
						iModifier = DMKEYBOARD::KMOD_NONE;
					}
				}
				break;
			}
		}
		break;
	}
}

bool cGlobalInput::KBEventScene(const MSG& p_kMsg)
{
	switch(p_kMsg.message)
	{
	case WM_KEYUP:
		{
			switch(p_kMsg.wParam)
			{
			case VK_ESCAPE:
				{
					g_pEventScene->EventEnd();
					return true;
				}
				break;

			case VK_RETURN:
				{
					g_pEventScene->NextStep();
					return true;
				}
				break;
			}
		}
		break;
	}

	return false;
}

bool cGlobalInput::KBCashShop(const MSG& p_kMsg)
{
	//2016-03-09-nova 한영키
	UINT wParam;

	if (p_kMsg.wParam==VK_PROCESSKEY) 
	{
		int scan = (p_kMsg.lParam >> 16) & 0xff;
		wParam = ::MapVirtualKey(scan, 3);

		if(wParam == 0)
			wParam = -1;	// 2016-03-14-nova KEY_NOKEY이 0이기 때문에 -1로 변경하여 동작하지 않도록 한다
	} 
	else 
	{
		wParam=(UINT)p_kMsg.wParam;
	}

	//
	switch(p_kMsg.message)
	{
	case WM_KEYDOWN:
		{
			switch(p_kMsg.wParam)
			{ 
			case VK_ESCAPE:
				{
					g_pGameIF->OnEscapeKey();	
					return true;
				}
				break;

			case VK_RETURN:
				{
					g_pGameIF->OnEnterKey();
					return true;
				}
				break;
			}
		}
	case WM_SYSKEYDOWN:
		{
			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_CASHSHOP].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_CASHSHOP].s_nModifier)

				mHotKey->m_MKey[DMKEY::MENU_CASHSHOP].s_nKeyDown = true;
		}		
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_CASHSHOP].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_CASHSHOP].s_nModifier &&
				mHotKey->m_MKey[DMKEY::MENU_CASHSHOP].s_nKeyDown )
			{
				GAME_EVENT_ST.OnEvent( EVENT_CODE::CASHSHOP_TOGGLE, NULL );	//close	

				mHotKey->m_MKey[DMKEY::MENU_CASHSHOP].s_nKeyDown = false;

				return true;
			}
		}		
		break;
	}
	return false;
}

bool cGlobalInput::KBChattingEsc(const MSG& p_kMsg)
{
	const int isBitSet = (DWORD)p_kMsg.lParam & 0x40000000;

	//2016-03-09-nova 한영키
	UINT wParam;

	if (p_kMsg.wParam==VK_PROCESSKEY) 
	{
		int scan = (p_kMsg.lParam >> 16) & 0xff;
		wParam = ::MapVirtualKey(scan, 3);

		if(wParam == 0)
			wParam = -1;	// 2016-03-14-nova KEY_NOKEY이 0이기 때문에 -1로 변경하여 동작하지 않도록 한다
	} 
	else 
	{
		wParam=p_kMsg.wParam;
	}
	
	//
	switch(p_kMsg.message)
	{
	case WM_KEYDOWN:
		{
			switch(p_kMsg.wParam)
			{
			case VK_RETURN:
				{
					if(isBitSet == 0)
					{
						g_pGameIF->OnEnterKey();
						return true;
					}					
				}
				break;

			case VK_ESCAPE:
				{
					if(isBitSet != 0)
					{
						return true;
					}

					if( g_pGameIF->IsRenderIF() == false )
					{
						g_pGameIF->SetRenderIF( true );
						return true;
					}

					if( g_pGameIF->OnEscapeKey() == false )
					{
						// 여까이 왔으면 옵션창 열자
						g_pGameIF->GetDynamicIF( cBaseWindow::WT_MAINOPTION );
					}
					return true;
				}
				break;			
			}
		}
		break;
	}
	return false;
}

bool cGlobalInput::KeyBoardMenu(const MSG& p_kMsg)
{
	//2016-03-09-nova 한영키
	UINT wParam;

	if (p_kMsg.wParam==VK_PROCESSKEY) 
	{
		int scan = (p_kMsg.lParam >> 16) & 0xff;
		wParam = ::MapVirtualKey(scan, 3);

		if(wParam == 0)
			wParam = -1;	// 2016-03-14-nova KEY_NOKEY이 0이기 때문에 -1로 변경하여 동작하지 않도록 한다
	} 
	else 
	{
		wParam=p_kMsg.wParam;
	}
	//

	switch(p_kMsg.message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			//2016-03-03-nova	채팅창 입력과 독립적 체크를 위해서
			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_TAMER].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_TAMER].s_nModifier)
			{
				mHotKey->m_MKey[DMKEY::MENU_TAMER].s_nKeyDown = true;
			}

			if( wParam == mHotKey->m_MKey[DMKEY::MENU_DIGIVICE].s_nKey &&
				iModifier == mHotKey->m_MKey[DMKEY::MENU_DIGIVICE].s_nModifier )
			{
				mHotKey->m_MKey[DMKEY::MENU_DIGIVICE].s_nKeyDown = true;
			}

			if( wParam == mHotKey->m_MKey[DMKEY::MENU_TAMER_SKILL].s_nKey &&
				iModifier == mHotKey->m_MKey[DMKEY::MENU_TAMER_SKILL].s_nModifier )
			{
				mHotKey->m_MKey[DMKEY::MENU_TAMER_SKILL].s_nKeyDown = true;
			}

			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_INVENTORY_ALL].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_INVENTORY_ALL].s_nModifier)
			{	
				mHotKey->m_MKey[DMKEY::MENU_INVENTORY_ALL].s_nKeyDown = true;
			}

			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_QUEST_LIST].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_QUEST_LIST].s_nModifier)
			{
				mHotKey->m_MKey[DMKEY::MENU_QUEST_LIST].s_nKeyDown = true;
			}

			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_ZONEMAP].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_ZONEMAP].s_nModifier)
			{
				mHotKey->m_MKey[DMKEY::MENU_ZONEMAP].s_nKeyDown = true;
			}

			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_FRIEND_LIST].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_FRIEND_LIST].s_nModifier)
			{
				mHotKey->m_MKey[DMKEY::MENU_FRIEND_LIST].s_nKeyDown = true;
			}

			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_SMALLMAP].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_SMALLMAP].s_nModifier)
			{
				mHotKey->m_MKey[DMKEY::MENU_SMALLMAP].s_nKeyDown = true;
			}

			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_GUILD].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_GUILD].s_nModifier)
			{
				mHotKey->m_MKey[DMKEY::MENU_GUILD].s_nKeyDown = true;
			}

			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_ACHIEVE].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_ACHIEVE].s_nModifier)
			{
				mHotKey->m_MKey[DMKEY::MENU_ACHIEVE].s_nKeyDown = true;
			}

			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_CARDINVENTORY].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_CARDINVENTORY].s_nModifier)
			{
				mHotKey->m_MKey[DMKEY::MENU_CARDINVENTORY].s_nKeyDown = true;
			}

#ifdef DAMAGE_METER
			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_DAMAGEMETER].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_DAMAGEMETER].s_nModifier)
			{
				mHotKey->m_MKey[DMKEY::MENU_DAMAGEMETER].s_nKeyDown = true;
			}
#endif

			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_CASHSHOP].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_CASHSHOP].s_nModifier)
			{
				mHotKey->m_MKey[DMKEY::MENU_CASHSHOP].s_nKeyDown = true;
			}

			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_DIGIMON_STAT].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_DIGIMON_STAT].s_nModifier)
			{
				mHotKey->m_MKey[DMKEY::MENU_DIGIMON_STAT].s_nKeyDown = true;
			}

#ifdef REWARD_SYSTEM_UI
			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_REWARDSYSTEM].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_REWARDSYSTEM].s_nModifier)
			{
				mHotKey->m_MKey[DMKEY::MENU_REWARDSYSTEM].s_nKeyDown = true;
			}
#endif

#ifdef LJW_TUTORIAL_RENEWAL_20191128
			if (wParam == mHotKey->m_MKey[DMKEY::MENU_KEYHELP].s_nKey &&
				iModifier == mHotKey->m_MKey[DMKEY::MENU_KEYHELP].s_nModifier)
			{
				mHotKey->m_MKey[DMKEY::MENU_KEYHELP].s_nKeyDown = true;
			}
#endif
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_TAMER].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_TAMER].s_nModifier &&
				mHotKey->m_MKey[DMKEY::MENU_TAMER].s_nKeyDown )
			{
				bool bSetTab = true;
				int nTabNo = 0;
				ContentsStream kTmp;
				kTmp << bSetTab << nTabNo;
				GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_CLOSE_TAMERSTATUS, &kTmp );

				mHotKey->m_MKey[DMKEY::MENU_TAMER].s_nKeyDown = false;
				return true;
			}

			if( wParam == mHotKey->m_MKey[DMKEY::MENU_DIGIVICE].s_nKey &&
				iModifier == mHotKey->m_MKey[DMKEY::MENU_DIGIVICE].s_nModifier &&
				mHotKey->m_MKey[DMKEY::MENU_DIGIVICE].s_nKeyDown )
			{
				bool bSetTab = true;
				int nTabNo = 1;
				ContentsStream kTmp;
				kTmp << bSetTab << nTabNo;
				GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_CLOSE_TAMERSTATUS, &kTmp );

				mHotKey->m_MKey[DMKEY::MENU_DIGIVICE].s_nKeyDown = false;
				return true;
			}

			if( wParam == mHotKey->m_MKey[DMKEY::MENU_TAMER_SKILL].s_nKey &&
				iModifier == mHotKey->m_MKey[DMKEY::MENU_TAMER_SKILL].s_nModifier &&
				mHotKey->m_MKey[DMKEY::MENU_TAMER_SKILL].s_nKeyDown )
			{
				bool bSetTab = true;
				int nTabNo = 2;
				ContentsStream kTmp;
				kTmp << bSetTab << nTabNo;
				GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_CLOSE_TAMERSTATUS, &kTmp );

				mHotKey->m_MKey[DMKEY::MENU_TAMER_SKILL].s_nKeyDown = false;
				return true;
			}

			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_INVENTORY_ALL].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_INVENTORY_ALL].s_nModifier &&
				mHotKey->m_MKey[DMKEY::MENU_INVENTORY_ALL].s_nKeyDown )
			{	
				if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_INVENTORY, 0 ) )
				{
					GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_INVENTORY, NULL );

					if( g_pDataMng->GetPCbang()->GetPcBang() )
					{
						g_pGameIF->CloseDynamicIF( cBaseWindow::WT_PCBANGINVEN );
					}
				}
				else
				{
					bool bSound = true;
					GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INVENTORY, &bSound );
				}

				mHotKey->m_MKey[DMKEY::MENU_INVENTORY_ALL].s_nKeyDown = false;

				return true;
			}

			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_QUEST_LIST].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_QUEST_LIST].s_nModifier &&
				mHotKey->m_MKey[DMKEY::MENU_QUEST_LIST].s_nKeyDown )
			{
				if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUEST_LIST ) )
				{
					g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUEST_LIST );
				}
				else
				{
					g_pGameIF->GetDynamicIF( cBaseWindow::WT_QUEST_LIST );	
				}

				mHotKey->m_MKey[DMKEY::MENU_QUEST_LIST].s_nKeyDown = false;

				return true;
			}

			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_ZONEMAP].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_ZONEMAP].s_nModifier &&
				mHotKey->m_MKey[DMKEY::MENU_ZONEMAP].s_nKeyDown )
			{
				if( nsCsGBTerrain::g_nSvrLibType != nLIB::SVR_BATTLE )
				{
					if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_SMALLMAP ) )
					{
						g_pGameIF->CloseDynamicIF( cBaseWindow::WT_SMALLMAP );			
					}			

					if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ZONEMAP ) )
					{
						g_pGameIF->CloseDynamicIF( cBaseWindow::WT_ZONEMAP );			
					}
					else
					{
						g_pGameIF->GetDynamicIF( cBaseWindow::WT_ZONEMAP );
					}
				}

				mHotKey->m_MKey[DMKEY::MENU_ZONEMAP].s_nKeyDown = false;

				return true;
			}

			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_FRIEND_LIST].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_FRIEND_LIST].s_nModifier &&
				mHotKey->m_MKey[DMKEY::MENU_FRIEND_LIST].s_nKeyDown )
			{
				int opentype = 0;
				GAME_EVENT_STPTR->OnEvent(EVENT_CODE::TOGGLE_FRIEND_SHUTOUT_UI, &opentype);
				mHotKey->m_MKey[DMKEY::MENU_FRIEND_LIST].s_nKeyDown = false;

				return true;
			}

			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_SMALLMAP].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_SMALLMAP].s_nModifier &&
				mHotKey->m_MKey[DMKEY::MENU_SMALLMAP].s_nKeyDown )
			{
				if( nsCsGBTerrain::g_nSvrLibType != nLIB::SVR_BATTLE )
				{
					if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ZONEMAP ) )
					{
						g_pGameIF->CloseDynamicIF( cBaseWindow::WT_ZONEMAP );			
					}		

					if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_SMALLMAP ) )
					{
						g_pGameIF->CloseDynamicIF( cBaseWindow::WT_SMALLMAP );			
					}
					else
					{
						g_pGameIF->GetDynamicIF( cBaseWindow::WT_SMALLMAP );
					}
				}

				mHotKey->m_MKey[DMKEY::MENU_SMALLMAP].s_nKeyDown = false;

				return true;
			}

			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_GUILD].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_GUILD].s_nModifier &&
				mHotKey->m_MKey[DMKEY::MENU_GUILD].s_nKeyDown )
			{
				if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) == false )
				{
					g_pGameIF->GetDynamicIF( cBaseWindow::WT_COMMUNITY );
					g_pGameIF->GetCommunity()->SetTab( cCommunity::TAB_GUILD );
				}
				else
				{
					g_pGameIF->CloseDynamicIF( cBaseWindow::WT_COMMUNITY );
					if( g_pGameIF->GetCommunity()->GetTab() != cCommunity::TAB_GUILD )
					{
						g_pGameIF->GetDynamicIF( cBaseWindow::WT_COMMUNITY );
						g_pGameIF->GetCommunity()->SetTab( cCommunity::TAB_GUILD );
					}
				}

				mHotKey->m_MKey[DMKEY::MENU_GUILD].s_nKeyDown = false;

				return true;
			}

			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_ACHIEVE].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_ACHIEVE].s_nModifier &&
				mHotKey->m_MKey[DMKEY::MENU_ACHIEVE].s_nKeyDown )
			{
				if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ACHIEVE ) )
				{
					g_pGameIF->CloseDynamicIF( cBaseWindow::WT_ACHIEVE );
				}
				else
				{
					g_pGameIF->GetDynamicIF( cBaseWindow::WT_ACHIEVE );
				}

				mHotKey->m_MKey[DMKEY::MENU_ACHIEVE].s_nKeyDown = false;

				return true;
			}

			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_CARDINVENTORY].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_CARDINVENTORY].s_nModifier &&
				mHotKey->m_MKey[DMKEY::MENU_CARDINVENTORY].s_nKeyDown )
			{
				//Open/Close
				int mode = 3;
				GAME_EVENT_ST.OnEvent( EVENT_CODE::SEALMASTER_UI_OPEN_CLOSE, &mode );

				mHotKey->m_MKey[DMKEY::MENU_CARDINVENTORY].s_nKeyDown = false;

				return true;
			}

#ifdef DAMAGE_METER
			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_DAMAGEMETER].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_DAMAGEMETER].s_nModifier &&
				mHotKey->m_MKey[DMKEY::MENU_DAMAGEMETER].s_nKeyDown )
			{
				GAME_EVENT_ST.OnEvent(EVENT_CODE::TOGGLE_DAMAGEMETER_WINDOW, NULL);
				mHotKey->m_MKey[DMKEY::MENU_DAMAGEMETER].s_nKeyDown = false;
				return true;
			}
#endif

			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_CASHSHOP].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_CASHSHOP].s_nModifier &&
				mHotKey->m_MKey[DMKEY::MENU_CASHSHOP].s_nKeyDown )
			{
				GAME_EVENT_ST.OnEvent( EVENT_CODE::CASHSHOP_TOGGLE, NULL );	//close	

				mHotKey->m_MKey[DMKEY::MENU_CASHSHOP].s_nKeyDown = false;

				return true;
			}

			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_DIGIMON_STAT].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_DIGIMON_STAT].s_nModifier &&
				mHotKey->m_MKey[DMKEY::MENU_DIGIMON_STAT].s_nKeyDown )
			{
				GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_CLOSE_DIGIMONSTATUS, NULL );

				mHotKey->m_MKey[DMKEY::MENU_DIGIMON_STAT].s_nKeyDown = false;

				return true;
			}
#ifdef REWARD_SYSTEM_UI
			if( wParam		== mHotKey->m_MKey[DMKEY::MENU_REWARDSYSTEM].s_nKey &&
				iModifier	== mHotKey->m_MKey[DMKEY::MENU_REWARDSYSTEM].s_nModifier &&
				mHotKey->m_MKey[DMKEY::MENU_REWARDSYSTEM].s_nKeyDown )
			{
				if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_REWARD_EVENT_UI ) )
					g_pGameIF->CloseDynamicIF( cBaseWindow::WT_REWARD_EVENT_UI );
				else
					g_pGameIF->GetDynamicIF( cBaseWindow::WT_REWARD_EVENT_UI );

				return true;
			}
#endif

#ifdef LJW_TUTORIAL_RENEWAL_20191128
			if (wParam == mHotKey->m_MKey[DMKEY::MENU_KEYHELP].s_nKey &&
				iModifier == mHotKey->m_MKey[DMKEY::MENU_KEYHELP].s_nModifier &&
				mHotKey->m_MKey[DMKEY::MENU_KEYHELP].s_nKeyDown)
			{
				if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_TUTORIAL_HELP_UI))
					g_pGameIF->CloseDynamicIF(cBaseWindow::WT_TUTORIAL_HELP_UI);
				else
					g_pGameIF->GetDynamicIF(cBaseWindow::WT_TUTORIAL_HELP_UI);

				return true;
			}
#endif
		}
	}

	return false;
}

bool cGlobalInput::KeyBoardETC(const MSG& p_kMsg)
{
	switch(p_kMsg.message)
	{
	case WM_KEYUP:
	case WM_SYSKEYUP:
		{
			if(p_kMsg.wParam == VK_OEM_2)	// '/'
			{
				if( g_pGameIF->IsRenderIF() && g_pGameIF->GetChatMain()->_IsShowInput() == false )
				{
					//2016-02-24-nova
					ChatContents* pChatCon = (ChatContents*)CONTENTSSYSTEM_PTR->GetContents(E_CT_CHATTING_STANDARDIZATION);
					pChatCon->_SetShowInput( true, _T("/") );	//2016-03-07-nova '/' 입력시 '/' 기본추력
				}
				return true;
			}

			if(p_kMsg.wParam == VK_SNAPSHOT)	//스샷
			{
				ScreenShot();
			}

			if( p_kMsg.wParam == mHotKey->m_MKey[DMKEY::NEXT_SELECT_ENEMY].s_nKey &&
				iModifier	  == mHotKey->m_MKey[DMKEY::NEXT_SELECT_ENEMY].s_nModifier)
			{						
				g_pCharMng->NextSelectMonster();
				return true;
			}

			if( p_kMsg.wParam == mHotKey->m_MKey[DMKEY::IF_VIEW_TOGGLE].s_nKey &&
				iModifier	  == mHotKey->m_MKey[DMKEY::IF_VIEW_TOGGLE].s_nModifier)
			{						
				g_pGameIF->SetRenderIF( !g_pGameIF->IsRenderIF() );
				return true;
			}			
		}
		break;
	}

	return false;
}

void cGlobalInput::KeyBoardMove(const MSG& p_kMsg)
{
	SAFE_POINTER_RET(g_pServerMoveOwner);
	SAFE_POINTER_RET(g_pMngCollector);

	if( g_pResist->m_Global.s_bMove_KeyBoard || nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
	{
		if( g_pResist->m_Global.s_bMove_KeyBoard && 
			g_pServerMoveOwner->GetCurState() == g_pServerMoveOwner->GetBackupState() && 
			g_pCharMng->GetTamerUser()->GetProp_Animation()->GetAnimationID() != ANI::DIGIVICE_CHAGE &&		// 진화 애니 중이 아닐 때
			(g_pMngCollector->IsSceneState( CMngCollector::eSCENE_NONE ) ||
			g_pMngCollector->IsSceneState( CMngCollector::eSCENE_TUTORIAL_EVENT )) )							// 씬 이벤트 중이 아닐 때 , 다트센터 내부가 아닐 때
		{
			DWORD nKeyCheck = g_pServerMoveOwner->GetKeyCheck();			

			UINT wParam;

			if (p_kMsg.wParam==VK_PROCESSKEY) 
			{
				int scan = (p_kMsg.lParam >> 16) & 0xff;
				wParam = ::MapVirtualKey(scan, 3);

				if(wParam == 0)
					wParam = -1;	// 2016-03-14-nova KEY_NOKEY이 0이기 때문에 -1로 변경하여 동작하지 않도록 한다
			} 
			else 
			{
				wParam=p_kMsg.wParam;
			}			

			switch(p_kMsg.message)
			{
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
				{
					if( wParam    == mHotKey->m_MKey[DMKEY::KM_UP].s_nKey &&
						iModifier  == mHotKey->m_MKey[DMKEY::KM_UP].s_nModifier)
					{						
						nKeyCheck |= CUserServerMove::KEY_UP;	
					}	

					if( wParam	 == mHotKey->m_MKey[DMKEY::KM_RIGHT].s_nKey &&
						iModifier == mHotKey->m_MKey[DMKEY::KM_RIGHT].s_nModifier)
					{
						nKeyCheck |= CUserServerMove::KEY_RIGHT;	
					}

					if( wParam	 == mHotKey->m_MKey[DMKEY::KM_LEFT].s_nKey &&
						iModifier == mHotKey->m_MKey[DMKEY::KM_LEFT].s_nModifier)
					{
						nKeyCheck |= CUserServerMove::KEY_LEFT;	
					}

					if( wParam	 == mHotKey->m_MKey[DMKEY::KM_DOWN].s_nKey &&
						iModifier == mHotKey->m_MKey[DMKEY::KM_DOWN].s_nModifier)
					{
						nKeyCheck |= CUserServerMove::KEY_DOWN;	
					}					
				}
				break;

			case WM_KEYUP:
			case WM_SYSKEYUP:
				{
					if( p_kMsg.wParam == mHotKey->m_MKey[DMKEY::KM_UP].s_nKey )
					{
						nKeyCheck &= ~CUserServerMove::KEY_UP;
					}					

					if( p_kMsg.wParam == mHotKey->m_MKey[DMKEY::KM_RIGHT].s_nKey )
					{
						nKeyCheck &= ~CUserServerMove::KEY_RIGHT;
					}

					if( p_kMsg.wParam == mHotKey->m_MKey[DMKEY::KM_LEFT].s_nKey )
					{
						nKeyCheck &= ~CUserServerMove::KEY_LEFT;
					}

					if( p_kMsg.wParam == mHotKey->m_MKey[DMKEY::KM_DOWN].s_nKey )
					{
						nKeyCheck &= ~CUserServerMove::KEY_DOWN;	
					}	
				}
				break;
			}

			g_pServerMoveOwner->SetKeyCheck( nKeyCheck );
		}
	}
}

void cGlobalInput::KeyBoardCamera(const MSG& p_kMsg)
{
#ifdef ENABLE_KEYBOARD_CAMERA
	const int isBitSet = (DWORD)p_kMsg.lParam & 0x40000000;

	UINT wParam;

	if (p_kMsg.wParam==VK_PROCESSKEY) 
	{
		int scan = (p_kMsg.lParam >> 16) & 0xff;
		wParam = ::MapVirtualKey(scan, 3);

		if(wParam == 0)
			wParam = -1;	// 2016-03-14-nova KEY_NOKEY이 0이기 때문에 -1로 변경하여 동작하지 않도록 한다
	} 
	else 
	{
		wParam=p_kMsg.wParam;
	}		

	switch(p_kMsg.message)
	{
	case WM_KEYDOWN:
		{
			if(isBitSet !=0 )
			{
				break;
			}

			if( wParam	 == mHotKey->m_MKey[DMKEY::KC_ZOOM].s_nKey &&
				iModifier== mHotKey->m_MKey[DMKEY::KC_ZOOM].s_nModifier)
			{						
				int nDistLevel = CAMERA_ST.GetSetDistLevel();		

				nDistLevel = nDistLevel - 1;


				if(nDistLevel == -1)
				{
					nDistLevel = 4;
				}

				CAMERA_ST.SetDistLevel( nDistLevel, true );	
			}

			if( wParam	 == mHotKey->m_MKey[DMKEY::KC_RESET].s_nKey &&
				iModifier == mHotKey->m_MKey[DMKEY::KC_RESET].s_nModifier)
			{						
				g_pServerMoveOwner->ViewReset();
			}

			if( wParam	 == mHotKey->m_MKey[DMKEY::KC_LEFT].s_nKey &&
				iModifier == mHotKey->m_MKey[DMKEY::KC_LEFT].s_nModifier)
			{						
				bKBCamera[0] = true;
			}

			if( wParam	 == mHotKey->m_MKey[DMKEY::KC_RIGHT].s_nKey &&
				iModifier == mHotKey->m_MKey[DMKEY::KC_RIGHT].s_nModifier)
			{						
				bKBCamera[1] = true;
			}

			if( wParam	 == mHotKey->m_MKey[DMKEY::KC_UP].s_nKey &&
				iModifier == mHotKey->m_MKey[DMKEY::KC_UP].s_nModifier)
			{						
				bKBCamera[2] = true;
			}

			if( wParam	 == mHotKey->m_MKey[DMKEY::KC_DOWN].s_nKey &&
				iModifier == mHotKey->m_MKey[DMKEY::KC_DOWN].s_nModifier)
			{						
				bKBCamera[3] = true;
			}
		}
		break;

	case WM_KEYUP:
		{
			if( p_kMsg.wParam == mHotKey->m_MKey[DMKEY::KC_LEFT].s_nKey )
			{
				bKBCamera[0] = false;
			}	

			if( p_kMsg.wParam == mHotKey->m_MKey[DMKEY::KC_RIGHT].s_nKey )
			{
				bKBCamera[1] = false;
			}	

			if( p_kMsg.wParam == mHotKey->m_MKey[DMKEY::KC_UP].s_nKey )
			{
				bKBCamera[2] = false;
			}	

			if( p_kMsg.wParam == mHotKey->m_MKey[DMKEY::KC_DOWN].s_nKey )
			{
				bKBCamera[3] = false;
			}			
		}
		break;
	}	
#endif
}

void cGlobalInput::KeyBoardCameraUpdate()
{
	if(bKBCamera[0])	//LEFT
	{
		float fRotSpeed = g_pResist->m_Character.s_fStaticCameraRotSpeed*g_fDeltaTime;
		CAMERA_ST.SetDeltaRotation( -fRotSpeed, 0.0f );

		if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
			g_pServerMoveOwner->KeyAutoMapping();
	}

	if(bKBCamera[1])	//RIGHT
	{
		float fRotSpeed = g_pResist->m_Character.s_fStaticCameraRotSpeed*g_fDeltaTime;
		CAMERA_ST.SetDeltaRotation( fRotSpeed, 0.0f );

		if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
			g_pServerMoveOwner->KeyAutoMapping();
	}

	if(bKBCamera[2])	//UP
	{
		float fRotSpeed = g_pResist->m_Character.s_fStaticCameraRotSpeed*g_fDeltaTime;
		CAMERA_ST.SetDeltaRotation( 0, -fRotSpeed );
	}

	if(bKBCamera[3])	//DOWN
	{
		float fRotSpeed = g_pResist->m_Character.s_fStaticCameraRotSpeed*g_fDeltaTime;
		CAMERA_ST.SetDeltaRotation( 0, fRotSpeed );
	}
}

bool cGlobalInput::GetEqualSnKey(WPARAM wparam, uint dmKey)
{
	if(wparam == mHotKey->m_MKey[dmKey].s_nKey)
	{
		return true;
	}

	return false;
}

bool cGlobalInput::GetEqualModifier(uint dmKey)
{
	if(iModifier == mHotKey->m_MKey[dmKey].s_nModifier)
	{
		return true;
	}

	return false;
}

bool cGlobalInput::GetSpecialKeyCheck(const MSG& p_kMsg)
{
	USHORT uScanCode = LOBYTE( HIWORD( p_kMsg.lParam ) );
	switch( uScanCode )
	{
	case 241: 
	case 242: 
		{ 

		}
		break;
	default:
		{
			BYTE byKeyCode = static_cast< BYTE >( ::MapVirtualKeyW( uScanCode, 3 ) );
			bool bIsExtend = ( ( HIBYTE( HIWORD( p_kMsg.lParam ) ) & 0x01 ) == 0x01 )?( true ):( false );	
			switch( byKeyCode )
			{
			case VK_LMENU:
			case VK_LCONTROL:
			case VK_LSHIFT:
			case VK_RSHIFT:	
				{
					return true;
				}
				break;
			}			
		}
		break;
	}

	return false;
}

void cGlobalInput::SetHotKey(HotKey* hotkey)
{
	mHotKey = hotkey;
}

void cGlobalInput::ResetModifier()
{
	iModifier = DMKEYBOARD::KMOD_NONE;
}

HotKey* cGlobalInput::GetHotKeyPtr()		
{ 
	return mHotKey; 
}

uint cGlobalInput::GetModifier()	
{
	return iModifier; 
}

bool cGlobalInput::IsEnter()									//Enter키 체크
{
	if(m_Msg.message == WM_KEYUP)
	{
		switch(m_Msg.wParam)
		{
		case VK_RETURN:
			return true;
		}
	}
	return false;
}
bool cGlobalInput::IsEscape()
{
	if(m_Msg.message == WM_KEYUP)
	{
		switch(m_Msg.wParam)
		{
		case VK_ESCAPE:
			{
				return true;
			}
		}
	}
	return false;
}

bool cGlobalInput::IsShift()
{
	if(iModifier == DMKEYBOARD::KMOD_LSHIFT || iModifier == DMKEYBOARD::KMOD_RSHIFT)
	{
		return true;
	}

	return false;
}

bool cGlobalInput::IsAlt()
{
	if(iModifier == DMKEYBOARD::KMOD_LMENU || iModifier == DMKEYBOARD::KMOD_RMENU)
	{
		return true;
	}

	return false;
}

void cGlobalInput::KeyDNReset()
{
	if( mHotKey )
		mHotKey->KeyDownReset();
}

MSG	 cGlobalInput::GetInputMsg() const
{
	return m_Msg;
}

void cGlobalInput::InitMsg()
{
	MSG	InitMsg;
	ZeroMemory(&InitMsg, sizeof(InitMsg));
	m_Msg = InitMsg;
}