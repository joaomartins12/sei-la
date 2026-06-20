//////////////////////////////////////////////////////////////////////////
// 2016-02-02-nova 단축키 관련(윈도우 MSG 대응)
//////////////////////////////////////////////////////////////////////////

#pragma once

namespace DMKEY
{
	enum eTYPE
	{		
		// Quick Slot
		QU_0_0 = 0,
		QU_0_1, QU_0_2, QU_0_3, QU_0_4, QU_0_5, QU_0_6, QU_0_7,
		QU_1_0, QU_1_1, QU_1_2, QU_1_3, QU_1_4, QU_1_5, QU_1_6, QU_1_7,

		// Quick Evo Slot
		EVOL_0, EVOL_1, EVOL_2, EVOL_3, EVOL_4, EVOL_5,

		// Menu
		MENU_TAMER,
		MENU_DIGIVICE,
		MENU_TAMER_SKILL,
		MENU_INVENTORY_ALL,

		MENU_CARDINVENTORY,

		MENU_QUEST_LIST,
		MENU_ZONEMAP,
		MENU_DIGIMON_STAT,
		MENU_EMPTY_1,
		MENU_EMPTY_2,

		// 기타
		NEXT_SELECT_ENEMY,
		IF_VIEW_TOGGLE,

		// 친구목록
		MENU_FRIEND_LIST,
		MENU_SMALLMAP,
		MENU_GUILD,

		// 라이드
		RIDE_TOGGLE,

		// 업적
		MENU_ACHIEVE,

		// 데미지 미터기
#ifdef DAMAGE_METER
		MENU_DAMAGEMETER,
#endif
		// 캐시샵 UI 오픈 단축키
		MENU_CASHSHOP,
		// 키보드 이동
#ifdef KEYBOARD_MOVE
		KM_UP,
		KM_DOWN,
		KM_RIGHT,
		KM_LEFT,
#endif	

		// 키보드 카메라 회전
#ifdef ENABLE_KEYBOARD_CAMERA
		KC_ZOOM,
		KC_LEFT,
		KC_RIGHT,
		KC_UP,
		KC_DOWN,
		KC_RESET,
#endif
		QU_2_0, QU_2_1, QU_2_2, QU_2_3, QU_2_4, QU_2_5, QU_2_6, QU_2_7,

		MENU_REWARDSYSTEM,

		NPC_ACTION,
#ifdef LJW_TUTORIAL_RENEWAL_20191128
		MENU_KEYHELP,
#endif
		MENU_MAIL,
		MENU_EXTRA_INVENTORY,
		MENU_EVOLUTION_HELPS,

		MAX_COUNT
	};
}

namespace DMKEYBOARD
{
	enum KeyCode
    {
        KEY_NOKEY           = 0x00,

		KEY_F1				= VK_F1,
		KEY_F2				= VK_F2,
		KEY_F3				= VK_F3,
		KEY_F4				= VK_F4,
		KEY_F5				= VK_F5,
		KEY_F6				= VK_F6,
		KEY_F7				= VK_F7,
		KEY_F8				= VK_F8,
		KEY_F9				= VK_F9,
		KEY_F10				= VK_F10,
		KEY_F11				= VK_F11,
		KEY_F12				= VK_F12,
		KEY_F13				= VK_F13,           
		KEY_F14				= VK_F14,            
		KEY_F15				= VK_F15, 

		KEY_0				= 0x30,
		KEY_1				= 0x31,
		KEY_2				= 0x32,
		KEY_3				= 0x33,
		KEY_4				= 0x34,
		KEY_5				= 0x35,
		KEY_6				= 0x36,
		KEY_7				= 0x37,
		KEY_8				= 0x38,
		KEY_9				= 0x39, 

		KEY_A				= 0x41,
		KEY_B				= 0x42,
		KEY_C				= 0x43,
		KEY_D				= 0x44,
		KEY_E				= 0x45,
		KEY_F				= 0x46,
		KEY_G				= 0x47,
		KEY_H				= 0x48,
		KEY_I				= 0x49,
		KEY_J				= 0x4A,
		KEY_K				= 0x4B,
		KEY_L				= 0x4C,
		KEY_M				= 0x4D,
		KEY_N				= 0x4E,
		KEY_O				= 0x4F,
		KEY_P				= 0x50,
		KEY_Q				= 0x51,
		KEY_R				= 0x52,
		KEY_S				= 0x53,
		KEY_T				= 0x54,
		KEY_U				= 0x55,
		KEY_V				= 0x56,
		KEY_W				= 0x57,
		KEY_X				= 0x58,
		KEY_Y				= 0x59,
		KEY_Z				= 0x5A,

		KEY_BACKSLASH		= VK_OEM_5,
		KEY_COMMA			= VK_OEM_COMMA,
		KEY_PERIOD			= VK_OEM_PERIOD,
		KEY_SLASH			= VK_OEM_2,     	
		KEY_MINUS			= VK_OEM_MINUS, 
		KEY_EQUALS			= VK_OEM_PLUS,

        KEY_ESCAPE			= VK_ESCAPE,
		KEY_TAB				= VK_TAB,
		KEY_BACK			= VK_BACK,
		KEY_LSHIFT			= VK_LSHIFT,
		KEY_RSHIFT			= VK_RSHIFT,
		KEY_LMENU			= VK_LMENU,     
		KEY_RMENU			= VK_RMENU,     
		KEY_LCONTROL		= VK_LCONTROL,
		KEY_RCONTROL		= VK_RCONTROL,
		KEY_RETURN			= VK_RETURN,    
		KEY_SPACE			= VK_SPACE,
		KEY_PAUSE			= VK_PAUSE,    
		KEY_SCROLL			= VK_SCROLL,   		
		 
		KEY_NUMLOCK			= VK_NUMLOCK,
		KEY_NUMPAD0			= VK_NUMPAD0,
		KEY_NUMPAD1			= VK_NUMPAD1,
		KEY_NUMPAD2			= VK_NUMPAD2,
		KEY_NUMPAD3			= VK_NUMPAD3,
		KEY_NUMPAD4			= VK_NUMPAD4,
		KEY_NUMPAD5			= VK_NUMPAD5,
		KEY_NUMPAD6			= VK_NUMPAD6,
		KEY_NUMPAD7			= VK_NUMPAD7,
		KEY_NUMPAD8			= VK_NUMPAD8,
		KEY_NUMPAD9			= VK_NUMPAD9,
		KEY_MULTIPLY        = VK_MULTIPLY,	 
		KEY_ADD				= VK_ADD,        
		KEY_SUBTRACT		= VK_SUBTRACT,   
		KEY_DECIMAL			= VK_DECIMAL,   
		KEY_DIVIDE			= VK_DIVIDE,   
		KEY_UP				= VK_UP,        
		KEY_DOWN            = VK_DOWN,		
		KEY_LEFT			= VK_LEFT,          
		KEY_RIGHT			= VK_RIGHT,         

		KEY_INSERT			= VK_INSERT,         
		KEY_DELETE			= VK_DELETE,        
		KEY_PRIOR			= VK_PRIOR,         
		KEY_NEXT			= VK_NEXT,          

		KEY_HOME			= VK_HOME,          
		KEY_END             = VK_END,		

		KEY_LWIN			= VK_LWIN,        
		KEY_RWIN			= VK_RWIN,         
		KEY_APPS			= VK_APPS,          

		KEY_SLEEP			= VK_SLEEP,        
		
		KEY_WEBBACK			= VK_BROWSER_BACK,			
		KEY_WEBFORWARD		= VK_BROWSER_FORWARD,		
		KEY_WEBREFRESH		= VK_BROWSER_REFRESH,		
		KEY_WEBSTOP			= VK_BROWSER_STOP,			
		KEY_WEBSEARCH		= VK_BROWSER_SEARCH,		
		KEY_WEBFAVORITES	= VK_BROWSER_FAVORITES,		
		KEY_WEBHOME			= VK_BROWSER_HOME,			

		KEY_MUTE			= VK_VOLUME_MUTE,			
		KEY_VOLUMEDOWN		= VK_VOLUME_DOWN,			
		KEY_VOLUMEUP		= VK_VOLUME_UP,				
		KEY_NEXTTRACK		= VK_MEDIA_NEXT_TRACK,		
		KEY_PREVTRACK		= VK_MEDIA_PREV_TRACK,		
		KEY_MEDIASTOP		= VK_MEDIA_STOP,			
		KEY_PLAYPAUSE		= VK_MEDIA_PLAY_PAUSE,		
		KEY_MAIL			= VK_LAUNCH_MAIL,           
		KEY_MEDIASELECT		= VK_LAUNCH_MEDIA_SELECT,  

		KEY_OEM_102			= VK_OEM_102,   
		KEY_AX				= VK_OEM_AX, 
		KEY_SYSRQ			= VK_SNAPSHOT,
		KEY_NUMPADEQUALS	= VK_OEM_NEC_EQUAL,   /* = on numeric keypad (NEC PC98) */			    
		 
		KEY_SEMICOLON		= VK_OEM_1 ,
		KEY_GRAVE			= VK_OEM_3,			/* accent grave */  
        KEY_LBRACKET		= VK_OEM_4,
        KEY_RBRACKET		= VK_OEM_6,      
        KEY_APOSTROPHE		= VK_OEM_7,
               
        KEY_CAPITAL			= VK_CAPITAL,                   

        KEY_TOTAL_COUNT
    };

	// Modifiers
	enum Modifiers
	{ 
		KMOD_NONE      = 0x00000000,
		KMOD_LCONTROL  = 0x00000001,
		KMOD_RCONTROL  = 0x00000002,
		KMOD_LMENU     = 0x00000004,    // Left ALT
		KMOD_RMENU     = 0x00000008,    // Right ALT
		KMOD_LSHIFT    = 0x00000010,
		KMOD_RSHIFT    = 0x00000020
	};
}

class HotKey
{
public:
	struct sINPUT
	{
		DMKEYBOARD::KeyCode	s_nKey;
		uint				s_nModifier;
		bool				s_nKeyDown;
	};

public:
	sINPUT		m_MKey[ DMKEY::MAX_COUNT ];

public:
	void		Reset();
	void		KeyDownReset();
	bool		GetText( DMKEY::eTYPE type, TCHAR* szOut, int nOutLen );
};
