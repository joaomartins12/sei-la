#include "StdAfx.h"
#include "HotKey.h"

void HotKey::Reset()
{
	KeyDownReset();
	
	//==================================================================================================
	// Keyboard Movement
	//==================================================================================================
#ifdef KEYBOARD_MOVE
	m_MKey[ DMKEY::KM_UP ].s_nKey				= DMKEYBOARD::KEY_W;
	m_MKey[ DMKEY::KM_UP ].s_nModifier			= DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::KM_DOWN ].s_nKey				= DMKEYBOARD::KEY_S;
	m_MKey[ DMKEY::KM_DOWN ].s_nModifier		= DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::KM_RIGHT ].s_nKey			= DMKEYBOARD::KEY_D;
	m_MKey[ DMKEY::KM_RIGHT ].s_nModifier		= DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::KM_LEFT ].s_nKey				= DMKEYBOARD::KEY_A;
	m_MKey[ DMKEY::KM_LEFT ].s_nModifier		= DMKEYBOARD::KMOD_NONE;
#endif

	//==================================================================================================
	// keyboard camera rotation
	//==================================================================================================
#ifdef ENABLE_KEYBOARD_CAMERA
	m_MKey[ DMKEY::KC_ZOOM ].s_nKey				= DMKEYBOARD::KEY_NUMPAD7;
	m_MKey[ DMKEY::KC_ZOOM ].s_nModifier		= DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::KC_LEFT ].s_nKey				= DMKEYBOARD::KEY_NUMPAD4;
	m_MKey[ DMKEY::KC_LEFT ].s_nModifier		= DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::KC_RIGHT ].s_nKey			= DMKEYBOARD::KEY_NUMPAD6;
	m_MKey[ DMKEY::KC_RIGHT ].s_nModifier		= DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::KC_UP ].s_nKey				= DMKEYBOARD::KEY_NUMPAD8;
	m_MKey[ DMKEY::KC_UP ].s_nModifier			= DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::KC_DOWN ].s_nKey				= DMKEYBOARD::KEY_NUMPAD5;
	m_MKey[ DMKEY::KC_DOWN ].s_nModifier		= DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::KC_RESET ].s_nKey			= DMKEYBOARD::KEY_NUMPAD9;
	m_MKey[ DMKEY::KC_RESET ].s_nModifier		= DMKEYBOARD::KMOD_NONE;
#endif

	//==================================================================================================
	// keyboard Menu
	//==================================================================================================
	
	m_MKey[ DMKEY::MENU_TAMER ].s_nKey				= DMKEYBOARD::KEY_C;		// In-game Tamer shortcut keys
	m_MKey[ DMKEY::MENU_TAMER ].s_nModifier			= DMKEYBOARD::KMOD_NONE;

	m_MKey[DMKEY::MENU_DIGIVICE].s_nKey				= DMKEYBOARD::KEY_V;		// In-game Digivice shortcut keys
	m_MKey[DMKEY::MENU_DIGIVICE].s_nModifier		= DMKEYBOARD::KMOD_NONE;

	m_MKey[DMKEY::MENU_TAMER_SKILL].s_nKey			= DMKEYBOARD::KEY_K;		// In-game TamerSkill shortcut keys
	m_MKey[DMKEY::MENU_TAMER_SKILL].s_nModifier		= DMKEYBOARD::KMOD_NONE;
	
	m_MKey[ DMKEY::MENU_INVENTORY_ALL ].s_nKey		= DMKEYBOARD::KEY_I;		// In-game Inventory shortcut keys
	m_MKey[ DMKEY::MENU_INVENTORY_ALL ].s_nModifier	= DMKEYBOARD::KMOD_NONE;
	
	m_MKey[ DMKEY::MENU_QUEST_LIST ].s_nKey			= DMKEYBOARD::KEY_L;		// In-game Quest shortcut keys
	m_MKey[ DMKEY::MENU_QUEST_LIST ].s_nModifier	= DMKEYBOARD::KMOD_NONE;
	
	m_MKey[ DMKEY::MENU_ZONEMAP ].s_nKey			= DMKEYBOARD::KEY_M;		// 존맵
	m_MKey[ DMKEY::MENU_ZONEMAP ].s_nModifier		= DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::MENU_FRIEND_LIST ].s_nKey		= DMKEYBOARD::KEY_F;		// In-game FriendList shortcut keys
	m_MKey[ DMKEY::MENU_FRIEND_LIST ].s_nModifier	= DMKEYBOARD::KMOD_NONE;
	
	m_MKey[ DMKEY::MENU_SMALLMAP ].s_nKey			= DMKEYBOARD::KEY_N;		// 작은맵
	m_MKey[ DMKEY::MENU_SMALLMAP ].s_nModifier		= DMKEYBOARD::KMOD_NONE;
	
	m_MKey[ DMKEY::MENU_GUILD ].s_nKey				= DMKEYBOARD::KEY_G;		// 길드
	m_MKey[ DMKEY::MENU_GUILD ].s_nModifier			= DMKEYBOARD::KMOD_NONE;
	
	m_MKey[ DMKEY::MENU_ACHIEVE ].s_nKey			= DMKEYBOARD::KEY_Y;		// 업적창
	m_MKey[ DMKEY::MENU_ACHIEVE ].s_nModifier		= DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::MENU_CARDINVENTORY ].s_nKey		= DMKEYBOARD::KEY_T;		// 카드 인벤토리
	m_MKey[ DMKEY::MENU_CARDINVENTORY ].s_nModifier	= DMKEYBOARD::KMOD_NONE;

#ifdef DAMAGE_METER
	m_MKey[ DMKEY::MENU_DAMAGEMETER ].s_nKey		= DMKEYBOARD::KEY_B;		// 데미지 미터기(표시창)
	m_MKey[ DMKEY::MENU_DAMAGEMETER ].s_nModifier	= DMKEYBOARD::KMOD_NONE;
#endif
	
	m_MKey[ DMKEY::MENU_CASHSHOP ].s_nKey			= DMKEYBOARD::KEY_O;		// In-game cash shop shortcut keys
	m_MKey[ DMKEY::MENU_CASHSHOP ].s_nModifier		= DMKEYBOARD::KMOD_NONE;

#ifndef KEYBOARD_MOVE
	m_MKey[ DMKEY::MENU_DIGIMON_STAT ].s_nKey		= DMKEYBOARD::KEY_D;		// 디지몬 스탯
	m_MKey[ DMKEY::MENU_DIGIMON_STAT ].s_nModifier	= DMKEYBOARD::KMOD_NONE;
#else
	m_MKey[ DMKEY::MENU_DIGIMON_STAT ].s_nKey		= DMKEYBOARD::KEY_X;
	m_MKey[ DMKEY::MENU_DIGIMON_STAT ].s_nModifier	= DMKEYBOARD::KMOD_NONE;
#endif

#ifdef REWARD_SYSTEM_UI
	m_MKey[ DMKEY::MENU_REWARDSYSTEM ].s_nKey		= DMKEYBOARD::KEY_E;		
	m_MKey[ DMKEY::MENU_REWARDSYSTEM ].s_nModifier	= DMKEYBOARD::KMOD_NONE;
#endif

#ifdef LJW_TUTORIAL_RENEWAL_20191128
	m_MKey[DMKEY::MENU_KEYHELP].s_nKey				= DMKEYBOARD::KEY_EQUALS;
	m_MKey[DMKEY::MENU_KEYHELP].s_nModifier			= DMKEYBOARD::KMOD_NONE;
#endif

	m_MKey[DMKEY::MENU_MAIL].s_nKey					= DMKEYBOARD::KEY_U;		// In-game Mail shortcut keys
	m_MKey[DMKEY::MENU_MAIL].s_nModifier			= DMKEYBOARD::KMOD_NONE;

	m_MKey[DMKEY::MENU_EXTRA_INVENTORY].s_nKey		= DMKEYBOARD::KEY_LCONTROL;	// In-game ExtraInventory shortcut keys
	m_MKey[DMKEY::MENU_EXTRA_INVENTORY].s_nModifier = DMKEYBOARD::KMOD_NONE;

	m_MKey[DMKEY::MENU_EVOLUTION_HELPS].s_nKey		= DMKEYBOARD::KEY_H;		// In-game EvolutionHelps shortcut keys
	m_MKey[DMKEY::MENU_EVOLUTION_HELPS].s_nModifier = DMKEYBOARD::KMOD_NONE;

	//==================================================================================================
	// 기타
	//==================================================================================================

	// 근처 적 선택
	m_MKey[ DMKEY::NEXT_SELECT_ENEMY ].s_nKey =		 DMKEYBOARD::KEY_TAB;
	m_MKey[ DMKEY::NEXT_SELECT_ENEMY ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	// 인터페이스 보이기 토글
	m_MKey[ DMKEY::IF_VIEW_TOGGLE ].s_nKey =		 DMKEYBOARD::KEY_Z;
	m_MKey[ DMKEY::IF_VIEW_TOGGLE ].s_nModifier =	 DMKEYBOARD::KMOD_LCONTROL;

	// 라이드 토글 버튼
	m_MKey[ DMKEY::RIDE_TOGGLE ].s_nKey =			 DMKEYBOARD::KEY_R;
	m_MKey[ DMKEY::RIDE_TOGGLE ].s_nModifier =		 DMKEYBOARD::KMOD_NONE;

	// NPC 액션키 임의로 Space
	m_MKey[ DMKEY::NPC_ACTION ].s_nKey =			 DMKEYBOARD::KEY_SPACE;
	m_MKey[ DMKEY::NPC_ACTION ].s_nModifier =		 DMKEYBOARD::KMOD_NONE;

	//==================================================================================================
	// 퀵슬롯 0
	//==================================================================================================
	m_MKey[ DMKEY::QU_0_0 ].s_nKey		= DMKEYBOARD::KEY_1;
	m_MKey[ DMKEY::QU_0_0 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::QU_0_1 ].s_nKey		= DMKEYBOARD::KEY_2;
	m_MKey[ DMKEY::QU_0_1 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::QU_0_2 ].s_nKey		= DMKEYBOARD::KEY_3;
	m_MKey[ DMKEY::QU_0_2 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::QU_0_3 ].s_nKey		= DMKEYBOARD::KEY_4;
	m_MKey[ DMKEY::QU_0_3 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::QU_0_4 ].s_nKey		= DMKEYBOARD::KEY_5;
	m_MKey[ DMKEY::QU_0_4 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::QU_0_5 ].s_nKey		= DMKEYBOARD::KEY_6;
	m_MKey[ DMKEY::QU_0_5 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::QU_0_6 ].s_nKey		= DMKEYBOARD::KEY_7;
	m_MKey[ DMKEY::QU_0_6 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::QU_0_7 ].s_nKey		= DMKEYBOARD::KEY_8;
	m_MKey[ DMKEY::QU_0_7 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	//==================================================================================================
	// 퀵슬롯 1
	//==================================================================================================
	m_MKey[ DMKEY::QU_1_0 ].s_nKey		= DMKEYBOARD::KEY_F1;
	m_MKey[ DMKEY::QU_1_0 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::QU_1_1 ].s_nKey		= DMKEYBOARD::KEY_F2;
	m_MKey[ DMKEY::QU_1_1 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::QU_1_2 ].s_nKey		= DMKEYBOARD::KEY_F3;
	m_MKey[ DMKEY::QU_1_2 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::QU_1_3 ].s_nKey		= DMKEYBOARD::KEY_F4;
	m_MKey[ DMKEY::QU_1_3 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::QU_1_4 ].s_nKey		= DMKEYBOARD::KEY_F5;
	m_MKey[ DMKEY::QU_1_4 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::QU_1_5 ].s_nKey		= DMKEYBOARD::KEY_F6;
	m_MKey[ DMKEY::QU_1_5 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::QU_1_6 ].s_nKey		= DMKEYBOARD::KEY_F7;
	m_MKey[ DMKEY::QU_1_6 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::QU_1_7 ].s_nKey		= DMKEYBOARD::KEY_F8;
	m_MKey[ DMKEY::QU_1_7 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	//==================================================================================================
	// 퀵슬롯 2
	//==================================================================================================
	m_MKey[ DMKEY::QU_2_0 ].s_nKey		= DMKEYBOARD::KEY_1;
	m_MKey[ DMKEY::QU_2_0 ].s_nModifier = DMKEYBOARD::KMOD_LMENU;

	m_MKey[ DMKEY::QU_2_1 ].s_nKey		= DMKEYBOARD::KEY_2;
	m_MKey[ DMKEY::QU_2_1 ].s_nModifier = DMKEYBOARD::KMOD_LMENU;

	m_MKey[ DMKEY::QU_2_2 ].s_nKey		= DMKEYBOARD::KEY_3;
	m_MKey[ DMKEY::QU_2_2 ].s_nModifier = DMKEYBOARD::KMOD_LMENU;

	m_MKey[ DMKEY::QU_2_3 ].s_nKey		= DMKEYBOARD::KEY_4;
	m_MKey[ DMKEY::QU_2_3 ].s_nModifier = DMKEYBOARD::KMOD_LMENU;

	m_MKey[ DMKEY::QU_2_4 ].s_nKey		= DMKEYBOARD::KEY_5;
	m_MKey[ DMKEY::QU_2_4 ].s_nModifier = DMKEYBOARD::KMOD_LMENU;

	m_MKey[ DMKEY::QU_2_5 ].s_nKey		= DMKEYBOARD::KEY_6;
	m_MKey[ DMKEY::QU_2_5 ].s_nModifier = DMKEYBOARD::KMOD_LMENU;

	m_MKey[ DMKEY::QU_2_6 ].s_nKey		= DMKEYBOARD::KEY_7;
	m_MKey[ DMKEY::QU_2_6 ].s_nModifier = DMKEYBOARD::KMOD_LMENU;

	m_MKey[ DMKEY::QU_2_7 ].s_nKey		= DMKEYBOARD::KEY_8;
	m_MKey[ DMKEY::QU_2_7 ].s_nModifier = DMKEYBOARD::KMOD_LMENU;

	//==================================================================================================
	// 퀵 에볼
	//==================================================================================================
	m_MKey[ DMKEY::EVOL_0 ].s_nKey		= DMKEYBOARD::KEY_INSERT;
	m_MKey[ DMKEY::EVOL_0 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::EVOL_1 ].s_nKey		= DMKEYBOARD::KEY_HOME;
	m_MKey[ DMKEY::EVOL_1 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::EVOL_2 ].s_nKey		= DMKEYBOARD::KEY_PRIOR;
	m_MKey[ DMKEY::EVOL_2 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::EVOL_3 ].s_nKey		= DMKEYBOARD::KEY_DELETE;
	m_MKey[ DMKEY::EVOL_3 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::EVOL_4 ].s_nKey		= DMKEYBOARD::KEY_END;
	m_MKey[ DMKEY::EVOL_4 ].s_nModifier = DMKEYBOARD::KMOD_NONE;

	m_MKey[ DMKEY::EVOL_5 ].s_nKey		= DMKEYBOARD::KEY_NEXT;
	m_MKey[ DMKEY::EVOL_5 ].s_nModifier = DMKEYBOARD::KMOD_NONE;
}

bool HotKey::GetText( DMKEY::eTYPE type, TCHAR* szOut, int nOutLen )
{
	memset( szOut, 0, sizeof( TCHAR )*nOutLen );

	uint nKey = m_MKey[ type ].s_nKey;
	uint nModifier = m_MKey[ type ].s_nModifier;

	if( ( nModifier & DMKEYBOARD::KMOD_LSHIFT ) != 0 )
		_tcscat_s( szOut, nOutLen, _T( "Sft " ) );

	if( ( nModifier & DMKEYBOARD::KMOD_RSHIFT ) != 0 )
		_tcscat_s( szOut, nOutLen, _T( "RSft " ) );

	if( ( nModifier & DMKEYBOARD::KMOD_LCONTROL ) != 0 )
		_tcscat_s( szOut, nOutLen, _T( "Ctl " ) );

	if( ( nModifier & DMKEYBOARD::KMOD_RCONTROL ) != 0 )
		_tcscat_s( szOut, nOutLen, _T( "RCtl " ) );

	if( ( nModifier & DMKEYBOARD::KMOD_LMENU ) != 0 )
		_tcscat_s( szOut, nOutLen, _T( "Alt " ) );

	if( ( nModifier & DMKEYBOARD::KMOD_RMENU ) != 0 )
		_tcscat_s( szOut, nOutLen, _T( "RAlt " ) );


	switch( nKey )
	{
	case DMKEYBOARD::KEY_NOKEY:		szOut[ 0 ] = NULL;								return true;
	case DMKEYBOARD::KEY_ESCAPE:	assert_cs( false )								return false;				// ESC 키는 단축키로쓰이면 안된다.
	case DMKEYBOARD::KEY_1:			_tcscat_s( szOut, nOutLen, _T( "1" ) );			break;
	case DMKEYBOARD::KEY_2:			_tcscat_s( szOut, nOutLen, _T( "2" ) );			break;
	case DMKEYBOARD::KEY_3:			_tcscat_s( szOut, nOutLen, _T( "3" ) );			break;
	case DMKEYBOARD::KEY_4:			_tcscat_s( szOut, nOutLen, _T( "4" ) );			break;
	case DMKEYBOARD::KEY_5:			_tcscat_s( szOut, nOutLen, _T( "5" ) );			break;
	case DMKEYBOARD::KEY_6:			_tcscat_s( szOut, nOutLen, _T( "6" ) );			break;
	case DMKEYBOARD::KEY_7:			_tcscat_s( szOut, nOutLen, _T( "7" ) );			break;
	case DMKEYBOARD::KEY_8:			_tcscat_s( szOut, nOutLen, _T( "8" ) );			break;
	case DMKEYBOARD::KEY_9:			_tcscat_s( szOut, nOutLen, _T( "9" ) );			break;
	case DMKEYBOARD::KEY_0:			_tcscat_s( szOut, nOutLen, _T( "0" ) );			break;
	case DMKEYBOARD::KEY_MINUS:		_tcscat_s( szOut, nOutLen, _T( "-" ) );			break;      /* - on main keyboard */
	case DMKEYBOARD::KEY_EQUALS:	_tcscat_s( szOut, nOutLen, _T( "=" ) );			break;
	case DMKEYBOARD::KEY_BACK:		_tcscat_s( szOut, nOutLen, _T( "BS" ) );		break;       /* backspace */
	case DMKEYBOARD::KEY_TAB:		_tcscat_s( szOut, nOutLen, _T( "Tab" ) );		break;
	case DMKEYBOARD::KEY_Q:			_tcscat_s( szOut, nOutLen, _T( "Q" ) );			break;
	case DMKEYBOARD::KEY_W:			_tcscat_s( szOut, nOutLen, _T( "W" ) );			break;
	case DMKEYBOARD::KEY_E:			_tcscat_s( szOut, nOutLen, _T( "E" ) );			break;
	case DMKEYBOARD::KEY_R:			_tcscat_s( szOut, nOutLen, _T( "R" ) );			break;
	case DMKEYBOARD::KEY_T:			_tcscat_s( szOut, nOutLen, _T( "T" ) );			break;
	case DMKEYBOARD::KEY_Y:			_tcscat_s( szOut, nOutLen, _T( "Y" ) );			break;
	case DMKEYBOARD::KEY_U:			_tcscat_s( szOut, nOutLen, _T( "U" ) );			break;
	case DMKEYBOARD::KEY_I:			_tcscat_s( szOut, nOutLen, _T( "I" ) );			break;
	case DMKEYBOARD::KEY_O:			_tcscat_s( szOut, nOutLen, _T( "O" ) );			break;
	case DMKEYBOARD::KEY_P:			_tcscat_s( szOut, nOutLen, _T( "P" ) );			break;
	case DMKEYBOARD::KEY_LBRACKET:	_tcscat_s( szOut, nOutLen, _T( "[" ) );			break;
	case DMKEYBOARD::KEY_RBRACKET:	_tcscat_s( szOut, nOutLen, _T( "]" ) );			break;
	case DMKEYBOARD::KEY_RETURN:	assert_cs( false )								return false; /* Enter on main keyboard */	
	case DMKEYBOARD::KEY_LCONTROL:													return false;
	case DMKEYBOARD::KEY_A:			_tcscat_s( szOut, nOutLen, _T( "A" ) );			break;
	case DMKEYBOARD::KEY_S:			_tcscat_s( szOut, nOutLen, _T( "S" ) );			break;
	case DMKEYBOARD::KEY_D:			_tcscat_s( szOut, nOutLen, _T( "D" ) );			break;
	case DMKEYBOARD::KEY_F:			_tcscat_s( szOut, nOutLen, _T( "F" ) );			break;
	case DMKEYBOARD::KEY_G:			_tcscat_s( szOut, nOutLen, _T( "G" ) );			break;
	case DMKEYBOARD::KEY_H:			_tcscat_s( szOut, nOutLen, _T( "H" ) );			break;
	case DMKEYBOARD::KEY_J:			_tcscat_s( szOut, nOutLen, _T( "J" ) );			break;
	case DMKEYBOARD::KEY_K:			_tcscat_s( szOut, nOutLen, _T( "K" ) );			break;
	case DMKEYBOARD::KEY_L:			_tcscat_s( szOut, nOutLen, _T( "L" ) );			break;
	case DMKEYBOARD::KEY_SEMICOLON:	_tcscat_s( szOut, nOutLen, _T( ";" ) );			break;
	case DMKEYBOARD::KEY_APOSTROPHE:_tcscat_s( szOut, nOutLen, _T( "'" ) );			break;
	case DMKEYBOARD::KEY_GRAVE:		_tcscat_s( szOut, nOutLen, _T( "~" ) );			break;
	case DMKEYBOARD::KEY_LSHIFT:													return false;	
	case DMKEYBOARD::KEY_BACKSLASH:	_tcscat_s( szOut, nOutLen, _T( "\\" ) );		break;
	case DMKEYBOARD::KEY_Z:			_tcscat_s( szOut, nOutLen, _T( "Z" ) );			break;
	case DMKEYBOARD::KEY_X:			_tcscat_s( szOut, nOutLen, _T( "X" ) );			break;
	case DMKEYBOARD::KEY_C:			_tcscat_s( szOut, nOutLen, _T( "C" ) );			break;
	case DMKEYBOARD::KEY_V:			_tcscat_s( szOut, nOutLen, _T( "V" ) );			break;
	case DMKEYBOARD::KEY_B:			_tcscat_s( szOut, nOutLen, _T( "B" ) );			break;
	case DMKEYBOARD::KEY_N:			_tcscat_s( szOut, nOutLen, _T( "N" ) );			break;
	case DMKEYBOARD::KEY_M:			_tcscat_s( szOut, nOutLen, _T( "M" ) );			break;
	case DMKEYBOARD::KEY_COMMA:		_tcscat_s( szOut, nOutLen, _T( "," ) );			break;
	case DMKEYBOARD::KEY_PERIOD:	_tcscat_s( szOut, nOutLen, _T( "." ) );			break;			   /* . on main keyboard */
	case DMKEYBOARD::KEY_SLASH:		_tcscat_s( szOut, nOutLen, _T( "/" ) );			return false;	   /* / on main keyboard */
	case DMKEYBOARD::KEY_RSHIFT:													return false;
	case DMKEYBOARD::KEY_MULTIPLY:  _tcscat_s( szOut, nOutLen, _T( "*" ) );			break;  /* * on numeric keypad */
	case DMKEYBOARD::KEY_LMENU:														return false;  /* left Alt */
	case DMKEYBOARD::KEY_SPACE:		_tcscat_s( szOut, nOutLen, _T( "space" ) );		break;
	case DMKEYBOARD::KEY_CAPITAL:	_tcscat_s( szOut, nOutLen, _T( "Cap" ) );		break;
	case DMKEYBOARD::KEY_F1:		_tcscat_s( szOut, nOutLen, _T( "F1" ) );		break;
	case DMKEYBOARD::KEY_F2:		_tcscat_s( szOut, nOutLen, _T( "F2" ) );		break;
	case DMKEYBOARD::KEY_F3:		_tcscat_s( szOut, nOutLen, _T( "F3" ) );		break;
	case DMKEYBOARD::KEY_F4:		_tcscat_s( szOut, nOutLen, _T( "F4" ) );		break;
	case DMKEYBOARD::KEY_F5:		_tcscat_s( szOut, nOutLen, _T( "F5" ) );		break;
	case DMKEYBOARD::KEY_F6:		_tcscat_s( szOut, nOutLen, _T( "F6" ) );		break;
	case DMKEYBOARD::KEY_F7:		_tcscat_s( szOut, nOutLen, _T( "F7" ) );		break;
	case DMKEYBOARD::KEY_F8:		_tcscat_s( szOut, nOutLen, _T( "F8" ) );		break;
	case DMKEYBOARD::KEY_F9:		_tcscat_s( szOut, nOutLen, _T( "F9" ) );		break;
	case DMKEYBOARD::KEY_F10:		_tcscat_s( szOut, nOutLen, _T( "F10" ) );		break;
	case DMKEYBOARD::KEY_NUMLOCK:	_tcscat_s( szOut, nOutLen, _T( "NumLk" ) );		break;
	case DMKEYBOARD::KEY_SCROLL:	_tcscat_s( szOut, nOutLen, _T( "ScoLk" ) );		break; /* Scroll Lock */
	case DMKEYBOARD::KEY_NUMPAD7:	_tcscat_s( szOut, nOutLen, _T( "Num7" ) );		break;
	case DMKEYBOARD::KEY_NUMPAD8:	_tcscat_s( szOut, nOutLen, _T( "Num8" ) );		break;
	case DMKEYBOARD::KEY_NUMPAD9:	_tcscat_s( szOut, nOutLen, _T( "Num9" ) );		break;
	case DMKEYBOARD::KEY_SUBTRACT:	_tcscat_s( szOut, nOutLen, _T( "Num-" ) );		break;   /* - on numeric keypad */
	case DMKEYBOARD::KEY_NUMPAD4:	_tcscat_s( szOut, nOutLen, _T( "Num4" ) );		break;
	case DMKEYBOARD::KEY_NUMPAD5:	_tcscat_s( szOut, nOutLen, _T( "Num5" ) );		break;
	case DMKEYBOARD::KEY_NUMPAD6:	_tcscat_s( szOut, nOutLen, _T( "Num6" ) );		break;
	case DMKEYBOARD::KEY_ADD:		_tcscat_s( szOut, nOutLen, _T( "Num+" ) );		break;/* + on numeric keypad */
	case DMKEYBOARD::KEY_NUMPAD1:	_tcscat_s( szOut, nOutLen, _T( "Num1" ) );		break;
	case DMKEYBOARD::KEY_NUMPAD2:	_tcscat_s( szOut, nOutLen, _T( "Num2" ) );		break;
	case DMKEYBOARD::KEY_NUMPAD3:	_tcscat_s( szOut, nOutLen, _T( "Num3" ) );		break;
	case DMKEYBOARD::KEY_NUMPAD0:	_tcscat_s( szOut, nOutLen, _T( "Num0" ) );		break;
	case DMKEYBOARD::KEY_DECIMAL:    _tcscat_s( szOut, nOutLen, _T( "Num." ) );		break;  /* . on numeric keypad */
	case DMKEYBOARD::KEY_OEM_102:													return false;  /* <> or \| on RT 102-key keyboard (Non-U.S.) */
	case DMKEYBOARD::KEY_F11:		_tcscat_s( szOut, nOutLen, _T( "F11" ) );		break;
	case DMKEYBOARD::KEY_F12:		_tcscat_s( szOut, nOutLen, _T( "F12" ) );		break;
	case DMKEYBOARD::KEY_F13:														return false;  /*                     (NEC PC98) */
	case DMKEYBOARD::KEY_F14:														return false;  /*                     (NEC PC98) */
	case DMKEYBOARD::KEY_F15:														return false;  /*                     (NEC PC98) */
	case DMKEYBOARD::KEY_DIVIDE:    _tcscat_s( szOut, nOutLen, _T( "Num/" ) );		break;			/* / on numeric keypad */
	case DMKEYBOARD::KEY_SYSRQ:		assert_cs( false );								return false;
	case DMKEYBOARD::KEY_RMENU:          											return false;  /* right Alt */
	case DMKEYBOARD::KEY_PAUSE:     _tcscat_s( szOut, nOutLen, _T( "Pause" ) );		break;			/* Pause */
	case DMKEYBOARD::KEY_HOME:      _tcscat_s( szOut, nOutLen, _T( "Home" ) );		break;			/* Home on arrow keypad */
	case DMKEYBOARD::KEY_UP:        _tcscat_s( szOut, nOutLen, _T( "Up" ) );		break;			/* UpArrow on arrow keypad */
	case DMKEYBOARD::KEY_PRIOR:     _tcscat_s( szOut, nOutLen, _T( "PgUp" ) );		break;			/* PgUp on arrow keypad */
	case DMKEYBOARD::KEY_LEFT:      _tcscat_s( szOut, nOutLen, _T( "Left" ) );		break;			/* LeftArrow on arrow keypad */
	case DMKEYBOARD::KEY_RIGHT:     _tcscat_s( szOut, nOutLen, _T( "Right" ) );		break;			/* RightArrow on arrow keypad */
	case DMKEYBOARD::KEY_END:       _tcscat_s( szOut, nOutLen, _T( "End" ) );		break;			/* End on arrow keypad */
	case DMKEYBOARD::KEY_DOWN:      _tcscat_s( szOut, nOutLen, _T( "Down" ) );		break;			/* DownArrow on arrow keypad */
	case DMKEYBOARD::KEY_NEXT:      _tcscat_s( szOut, nOutLen, _T( "PgDn" ) );		break;		/* PgDn on arrow keypad */
	case DMKEYBOARD::KEY_INSERT:    _tcscat_s( szOut, nOutLen, _T( "Ins" ) );		break;		/* Insert on arrow keypad */
	case DMKEYBOARD::KEY_DELETE:	_tcscat_s( szOut, nOutLen, _T( "Del" ) );		break;		/* Delete on arrow keypad */

	case DMKEYBOARD::KEY_LWIN:           											return false;  /* Left Windows key */
	case DMKEYBOARD::KEY_RWIN:           											return false;  /* Right Windows key */
	case DMKEYBOARD::KEY_APPS:           											return false;  /* AppMenu key */
	case DMKEYBOARD::KEY_SLEEP:          											return false;  /* System Sleep */
	case DMKEYBOARD::KEY_WEBSEARCH:      											return false;  /* Web Search */
	case DMKEYBOARD::KEY_WEBFAVORITES:   											return false;  /* Web Favorites */
	case DMKEYBOARD::KEY_WEBREFRESH:     											return false;  /* Web Refresh */
	case DMKEYBOARD::KEY_WEBSTOP:        											return false;  /* Web Stop */
	case DMKEYBOARD::KEY_WEBFORWARD:     											return false;  /* Web Forward */
	case DMKEYBOARD::KEY_WEBBACK:        											return false;  /* Web Back */
	case DMKEYBOARD::KEY_MAIL:           											return false;  /* Mail */
	case DMKEYBOARD::KEY_MEDIASELECT:    											return false;  /* Media Select */	
	case DMKEYBOARD::KEY_NUMPADEQUALS: 												return false;  /* = on numeric keypad (NEC PC98) */
	case DMKEYBOARD::KEY_PREVTRACK:													return false;  /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
	case DMKEYBOARD::KEY_AX:             											return false;  /*                     (Japan AX) */
	case DMKEYBOARD::KEY_NEXTTRACK:      											return false;  /* Next Track */
 	case DMKEYBOARD::KEY_RCONTROL:													return false;  
 	case DMKEYBOARD::KEY_MUTE:           											return false;  /* Mute */
 	case DMKEYBOARD::KEY_PLAYPAUSE:      											return false;  /* Play / Pause */
 	case DMKEYBOARD::KEY_MEDIASTOP:      											return false;  /* Media Stop */
 	case DMKEYBOARD::KEY_VOLUMEDOWN:     											return false;  /* Volume - */
 	case DMKEYBOARD::KEY_VOLUMEUP:       											return false;  /* Volume + */
 	case DMKEYBOARD::KEY_WEBHOME:        											return false;  /* Web home */
	default:			assert_cs( true );
	}

	return true;
}

void HotKey::KeyDownReset()
{
	for(int i=0; i<DMKEY::MAX_COUNT; ++i )
	{
		m_MKey[i].s_nKeyDown = false;
	}
}