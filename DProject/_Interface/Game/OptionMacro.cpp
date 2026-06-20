

#include "stdafx.h"
#include "OptionMacro.h"

void cOptionMacro::Destroy()
{
	cOptionBase::Delete();
}

void cOptionMacro::DeleteResource()
{
	cOptionBase::DeleteResource();
}

void cOptionMacro::Create(int nValue /* = 0  */)
{
	if( cOptionBase::CreateBase( UISTRING_TEXT( "OPTION_MACRO_TITLE_SHORTCUT_SETTING" ).c_str(), nValue ) == false )
		return;

	m_pDefaultBtn = AddButton( CsPoint( 14, 556 ), CsPoint( 101, 31 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_DEFAULT" ).c_str() );	
	m_pReleaseBtn = AddButton( CsPoint( 150, 556 ), CsPoint( 101, 31 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "OPTION_MACRO_BTN_UNLOCK_MACRO" ).c_str() );

	memcpy( &mHotKey, GLOBALINPUT_ST.GetHotKeyPtr(), sizeof( HotKey ) );

	_InitMacroKeyText();

	m_nOldScrollIndex = 0;

	iSameKey = 0;

	hInput.s_nKey = DMKEYBOARD::KEY_NOKEY;
	hInput.s_nModifier = DMKEYBOARD::KMOD_NONE;
}

void cOptionMacro::_InitMacroKeyText()
{
	_DeleteControl();

#ifdef KEYBOARD_MOVE
	//==========================================================================================
	// 이동
	//==========================================================================================
	_AddControl_Title( UISTRING_TEXT( "OPTION_MACRO_TITLE_KEYBOARD_MOVE" ).c_str() );

	_MakeKey( DMKEY::KM_UP, UISTRING_TEXT( "OPTION_MACRO_MENU_MOVE_UP" ).c_str() );
	_MakeKey( DMKEY::KM_RIGHT, UISTRING_TEXT( "OPTION_MACRO_MENU_MOVE_RIGHT" ).c_str() );
	_MakeKey( DMKEY::KM_LEFT, UISTRING_TEXT( "OPTION_MACRO_MENU_MOVE_LEFT" ).c_str() );
	_MakeKey( DMKEY::KM_DOWN, UISTRING_TEXT( "OPTION_MACRO_MENU_MOVE_DOWN" ).c_str() );
#endif

	//==========================================================================================
	// 메뉴
	//==========================================================================================
	_AddControl_Title( UISTRING_TEXT( "OPTION_MACRO_TITLE_MENU_SHORTCUT" ).c_str() );

	_MakeKey( DMKEY::MENU_TAMER, UISTRING_TEXT( "OPTION_MACRO_MENU_TAMER_WINDOW" ).c_str() );
	_MakeKey( DMKEY::MENU_DIGIVICE, UISTRING_TEXT( "OPTION_MACRO_MENU_DIGIVICE_WINDOW" ).c_str() );
	_MakeKey( DMKEY::MENU_TAMER_SKILL, UISTRING_TEXT( "OPTION_MACRO_MENU_TAMER_SKILL_WINDOW" ).c_str() );
	_MakeKey( DMKEY::MENU_DIGIMON_STAT, UISTRING_TEXT( "OPTION_MACRO_MENU_DIGIMON_WINDOW" ).c_str() );
	_MakeKey( DMKEY::MENU_INVENTORY_ALL, UISTRING_TEXT( "INVENTORY_TITLE_INVENTORY" ).c_str() );
	_MakeKey( DMKEY::MENU_CARDINVENTORY, UISTRING_TEXT( "OPTION_MACRO_MENU_CARD_INVENTORY" ).c_str() );
	_MakeKey( DMKEY::MENU_QUEST_LIST, UISTRING_TEXT( "QUESTLIST_TITLE_QUEST_LIST" ).c_str() );
	_MakeKey( DMKEY::MENU_FRIEND_LIST, UISTRING_TEXT( "OPTION_MACRO_MENU_FRIEND_LIST" ).c_str() );
	_MakeKey( DMKEY::MENU_GUILD, UISTRING_TEXT( "OPTION_MACRO_MENU_GUILD_WINDOW" ).c_str() );
	_MakeKey( DMKEY::MENU_ACHIEVE, UISTRING_TEXT( "OPTION_MACRO_MENU_ACHIEVEMENT_WINDOW" ).c_str() );
	_MakeKey( DMKEY::MENU_ZONEMAP, UISTRING_TEXT( "OPTION_MACRO_MENU_LOCAL_MAP" ).c_str() );
	_MakeKey( DMKEY::MENU_SMALLMAP, UISTRING_TEXT( "OPTION_MACRO_MENU_MINIMIZE_MAP" ).c_str() );
#ifdef DAMAGE_METER
	_MakeKey( DMKEY::MENU_DAMAGEMETER, UISTRING_TEXT("DAMAGEMETER_TITLE").c_str() );
#endif

	_MakeKey( DMKEY::MENU_CASHSHOP, UISTRING_TEXT( "COMMON_TXT_CASHSHOP" ).c_str() );

#ifdef REWARD_SYSTEM_UI
	_MakeKey( DMKEY::MENU_REWARDSYSTEM, UISTRING_TEXT( "COMMON_TXT_EVENT" ).c_str() );	//2017-03-27-nova
#endif

#ifdef LJW_TUTORIAL_RENEWAL_20191128
	_MakeKey(DMKEY::MENU_KEYHELP, UISTRING_TEXT("OPTION_MACRO_MENU_KEYHELP").c_str());
#endif

	//==========================================================================================
	// 기타
	//==========================================================================================
	_AddControl_Title( UISTRING_TEXT( "OPTION_TITLE_OTHERS" ).c_str() );

	_MakeKey( DMKEY::NEXT_SELECT_ENEMY, UISTRING_TEXT( "OPTION_MACRO_SELECT_NEARBY_ENEMY" ).c_str() );
	_MakeKey( DMKEY::RIDE_TOGGLE, UISTRING_TEXT( "OPTION_MACRO_TOGGLE_DIGIMON_RIDING" ).c_str() );
	_MakeKey( DMKEY::IF_VIEW_TOGGLE, UISTRING_TEXT( "OPTION_MACRO_TOGGLE_INTERFACE_VIEW" ).c_str() );

	//==========================================================================================
	// 진화단축키
	//==========================================================================================	
	_AddControl_Title( UISTRING_TEXT( "OPTION_MACRO_TITLE_FAST_EVOLUTION_SHORTCUT" ).c_str() );

	std::wstring wsButton;
	DmCS::StringFn::Format( wsButton, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_FAST_EVOLUTION_BUTTON" ).c_str(), 1 );
	_MakeKey( DMKEY::EVOL_0, wsButton.c_str() );
	DmCS::StringFn::Format( wsButton, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_FAST_EVOLUTION_BUTTON" ).c_str(), 2 );
	_MakeKey( DMKEY::EVOL_1, wsButton.c_str() );
	DmCS::StringFn::Format( wsButton, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_FAST_EVOLUTION_BUTTON" ).c_str(), 3 );
	_MakeKey( DMKEY::EVOL_2, wsButton.c_str() );
	DmCS::StringFn::Format( wsButton, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_FAST_EVOLUTION_BUTTON" ).c_str(), 4 );
	_MakeKey( DMKEY::EVOL_3, wsButton.c_str() );
	DmCS::StringFn::Format( wsButton, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_FAST_EVOLUTION_BUTTON" ).c_str(), 5 );
	_MakeKey( DMKEY::EVOL_4, wsButton.c_str() );

	_MakeKey( DMKEY::EVOL_5, UISTRING_TEXT( "OPTION_MACRO_FAST_DEGENERATE_BUTTON" ).c_str() ); 

	//==========================================================================================
	// 퀵슬롯 - 테이머
	//==========================================================================================
	_AddControl_Title( UISTRING_TEXT( "OPTION_MACRO_TITLE_TAMER_SLOT" ).c_str() );

	std::wstring wsSlot;
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 1 );
	_MakeKey( DMKEY::QU_0_0, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 2 );
	_MakeKey( DMKEY::QU_0_1, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 3 );
	_MakeKey( DMKEY::QU_0_2, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 4 );
	_MakeKey( DMKEY::QU_0_3, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 5 );
	_MakeKey( DMKEY::QU_0_4, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 6 );
	_MakeKey( DMKEY::QU_0_5, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 7 );
	_MakeKey( DMKEY::QU_0_6, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 8 );
	_MakeKey( DMKEY::QU_0_7, wsSlot.c_str() );

	//==========================================================================================
	// 퀵슬롯 - 디지몬 1
	//==========================================================================================
	_AddControl_Title( UISTRING_TEXT( "OPTION_MACRO_TITLE_DIGIMON_SLOT" ).c_str() );

	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 1 );
	_MakeKey( DMKEY::QU_1_0, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 2 );
	_MakeKey( DMKEY::QU_1_1, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 3 );
	_MakeKey( DMKEY::QU_1_2, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 4 );
	_MakeKey( DMKEY::QU_1_3, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 5 );
	_MakeKey( DMKEY::QU_1_4, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 6 );
	_MakeKey( DMKEY::QU_1_5, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 7 );
	_MakeKey( DMKEY::QU_1_6, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 8 );
	_MakeKey( DMKEY::QU_1_7, wsSlot.c_str() );

	//==========================================================================================
	// 퀵슬롯 - 추가 퀵슬롯
	//==========================================================================================
	_AddControl_Title( UISTRING_TEXT( "OPTION_ADDITIONAL_QUICKSLOT" ).c_str() );

	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 1 );
	_MakeKey( DMKEY::QU_2_0, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 2 );
	_MakeKey( DMKEY::QU_2_1, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 3 );
	_MakeKey( DMKEY::QU_2_2, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 4 );
	_MakeKey( DMKEY::QU_2_3, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 5 );
	_MakeKey( DMKEY::QU_2_4, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 6 );
	_MakeKey( DMKEY::QU_2_5, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 7 );
	_MakeKey( DMKEY::QU_2_6, wsSlot.c_str() );
	DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), 8 );
	_MakeKey( DMKEY::QU_2_7, wsSlot.c_str() );

#ifdef ENABLE_KEYBOARD_CAMERA
	//==========================================================================================
	// 카메라
	//==========================================================================================
	_AddControl_Title( UISTRING_TEXT( "COMMON_TXT_CAMERA" ).c_str() );

	_MakeKey( DMKEY::KC_LEFT, UISTRING_TEXT( "OPTION_MACRO_MENU_ROTATE_LEFT" ).c_str() );
	_MakeKey( DMKEY::KC_RIGHT, UISTRING_TEXT( "OPTION_MACRO_MENU_ROTATE_RIGHT" ).c_str() );
	_MakeKey( DMKEY::KC_UP, UISTRING_TEXT( "OPTION_MACRO_MENU_ROTATE_UP" ).c_str() );
	_MakeKey( DMKEY::KC_DOWN, UISTRING_TEXT( "OPTION_MACRO_MENU_ROTATE_DOWN" ).c_str() );
	_MakeKey( DMKEY::KC_ZOOM, UISTRING_TEXT( "OPTION_MACRO_MENU_ZOOM_LV" ).c_str() );
	_MakeKey( DMKEY::KC_RESET, UISTRING_TEXT( "OPTION_MACRO_MENU_REVERSE" ).c_str() );
#endif

	_ResizeScrollBar();
}

void cOptionMacro::_MakeKey( DMKEY::eTYPE type, TCHAR const* szOrder )
{
	TCHAR szKey[ MAX_FILENAME ];
	mHotKey.GetText( type, szKey, MAX_FILENAME );
	_AddControl_CheckRadio( szOrder, szKey, 1, type );
}

void cOptionMacro::Update(float const& fDeltaTime)
{
	cOptionBase::Update(fDeltaTime);

	if( cMessageBox::IsMessageBox() )
		return;
}

bool cOptionMacro::OnMacroKey(const MSG& p_kMsg)
{
	if( GetCheckRadio() == NULL )
		return false;

	if( GLOBALINPUT_ST.GetSpecialKeyCheck(p_kMsg) )
		return false;

	iKeyVal = GetCheckRadio()->s_nValue;

	UINT wParam;

	if (p_kMsg.wParam==VK_PROCESSKEY) 
	{
		int scan = (p_kMsg.lParam >> 16) & 0xff;
		wParam = ::MapVirtualKey(scan, 3);
	} 
	else 
	{
		wParam = p_kMsg.wParam;
	}	

	switch(p_kMsg.message)
	{
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		{
			if(p_kMsg.wParam != VK_RETURN &&
			   p_kMsg.wParam != VK_SPACE &&
			   p_kMsg.wParam != VK_SNAPSHOT)
			{

				hInput.s_nKey = (DMKEYBOARD::KeyCode)wParam;
				hInput.s_nModifier = GLOBALINPUT_ST.GetModifier();	

				// 중복된 키가 있는지 체크한다.

				for( int m=0; m< DMKEY::MAX_COUNT; ++m )
				{
					if( m == iKeyVal )
						continue;

					if( ( mHotKey.m_MKey[ m ].s_nKey == hInput.s_nKey )&&( mHotKey.m_MKey[ m ].s_nModifier == hInput.s_nModifier ) )
					{
						cPrintMsg::PrintMsg( cPrintMsg::SAME_MACRO_KEY );
						
						mBackupOP = GetCheckRadio();
						mBackupKey = hInput.s_nKey;
						mBackupModifier = hInput.s_nModifier;						
						iSameKey = m;

						return true;
					}
				}

				mHotKey.m_MKey[ iKeyVal ].s_nKey = hInput.s_nKey;
				mHotKey.m_MKey[ iKeyVal ].s_nModifier = hInput.s_nModifier;	

 				TCHAR szKey[ MAX_FILENAME ];
 				mHotKey.GetText( (DMKEY::eTYPE)iKeyVal, szKey, MAX_FILENAME );
 				_MakeKey( (DMKEY::eTYPE)iKeyVal, szKey );
 				GetCheckRadio()->s_pOverText->SetText( szKey );
				ReleaseCheckRadio();			
			}
		}
		break;
	}
	return false;
}

cBaseWindow::eMU_TYPE
cOptionMacro::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	switch( m_pDefaultBtn->Update_ForMouse() )
	{
	case cButton::ACTION_CLICK:
		_OnDefault();
	case cButton::ACTION_DOWN:
		return muReturn;
	}

	switch( m_pReleaseBtn->Update_ForMouse() )
	{
	case cButton::ACTION_CLICK:
		_OnRelease();
	case cButton::ACTION_DOWN:
		return muReturn;
	}

	return cOptionBase::Update_ForMouse();
}

void cOptionMacro::Render()
{
	cOptionBase::Render();
}

void cOptionMacro::ResetDevice()
{
	cOptionBase::ResetDevice();
}

void cOptionMacro::OnMoveWindow()
{
	cOptionBase::OnMoveWindow();
}

void cOptionMacro::_OnOk()
{
	memcpy( GLOBALINPUT_ST.GetHotKeyPtr(), &mHotKey, sizeof( HotKey ) );

	g_pGameIF->ChangeMacroKey();

	cOptionBase::_OnOk();
}

void cOptionMacro::_OnCancel()
{
	ReleaseCheckRadio();

	cOptionBase::_OnCancel();
}

void cOptionMacro::_OnDefault()
{
	ReleaseCheckRadio();

	mHotKey.Reset();

	_InitMacroKeyText();
}

void cOptionMacro::_OnRelease()
{
	if( GetCheckRadio() == NULL )
		return;

	mHotKey.m_MKey[ GetCheckRadio()->s_nValue ].s_nKey = DMKEYBOARD::KEY_NOKEY;
	mHotKey.m_MKey[ GetCheckRadio()->s_nValue ].s_nModifier = DMKEYBOARD::KMOD_NONE;
	GetCheckRadio()->s_pOverText->SetText( _T( "" ) );

	ReleaseCheckRadio();
}

void cOptionMacro::SetBackupKey()
{
	TCHAR szKey[ MAX_FILENAME ];

	mHotKey.GetText( (DMKEY::eTYPE)iSameKey, szKey, MAX_FILENAME );

	mBackupOP->s_pOverText->SetText( szKey );
	mHotKey.m_MKey[ iKeyVal ].s_nKey = hInput.s_nKey;
	mHotKey.m_MKey[ iKeyVal ].s_nModifier = hInput.s_nModifier;		

	// 같은 단축키로 지정되어있는것을 해제
	
	std::list< sOP_BASE* >* pList = GetListControl();
	std::list< sOP_BASE* >::iterator it = pList->begin();
	std::list< sOP_BASE* >::iterator itEnd = pList->end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->GetType() != sOP_BASE::CHECK_RADIO )
			continue;

		if( (*it)->s_nValue == iSameKey )
		{
			( (sOP_CHECK_RADIO*)(*it) )->s_pOverText->SetText( _T( "" ) );
			mHotKey.m_MKey[ iSameKey ].s_nKey = DMKEYBOARD::KEY_NOKEY;
			mHotKey.m_MKey[ iSameKey ].s_nModifier = DMKEYBOARD::KMOD_NONE;
			break;
		}
	}

	ReleaseCheckRadio();
}