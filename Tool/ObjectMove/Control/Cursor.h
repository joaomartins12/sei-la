#ifndef __CURSOR_H__
#define __CURSOR_H__



namespace CURSOR
{
    enum eTYPE
    {
		BUTTON_NONE			= 0,
		LBUTTON_DOWN		= 1,
		LBUTTON_UP			= 2,
		LBUTTON_PRESS		= 3,
		RBUTTON_DOWN		= 4,
		RBUTTON_UP			= 5,
		RBUTTON_PRESS		= 6,
		MBUTTON_DOWN		= 7,
		MBUTTON_PRESS		= 8,
		MBUTTON_UP			= 9,
		BUTTON_OUTWINDOW	= 10,
    };
}

namespace CURSOR_ICON
{
	enum eTYPE
	{
		CI_NONE			=	0,
		CI_INVEN,
		CI_WAREHOUSE,
		CI_TEQUIP,
		CI_CHIPSET,
		CI_TSKILL,
		CI_DSKILL,
		CI_QUICKSLOT,					// 퀵슬롯에서의 아이콘 갯수는 의미를 두지 않겠다
		CI_TACTICSHOUSE,

		/*QUICK,		
		SKILL,
		EQUIP,
		MEMORY,
		CAPSULE,		
		CHIPSET,*/
	};
}

#define INVALIDE_WHEEL		INT_MAX

class CCursor
{
public:
	CCursor()
	{
		m_nButtonState = CURSOR::BUTTON_NONE;		
		m_nWheel = INVALIDE_WHEEL;
	}
	virtual ~CCursor()
	{
	}

	void	Init();

	void	Update( HWND hWnd );	
	void	Render();

	//bool	IsInWindow(){ return ( ( ( m_nX < 0 )||( m_nX > g_nScreenWidth )||( m_nY < 0 )||( m_nY > g_nScreenHeight ) ) == false ); }
	void	ResetDevice();


	// 위에거 없애 버리자
	//==================================================================================================================
	//		마우스
	//==================================================================================================================
protected:
	CURSOR::eTYPE		m_nButtonState;
	CsPoint				m_ptCursorPos;	
	int					m_nWheel;
	UINT				m_nModifier;

	CsCheckInTimeSeq	m_DbClickTime;
	bool				m_bDoubleClick;

public:
	CsPoint				GetPos(){ return m_ptCursorPos; }
	UINT				GetModifier(){ return m_nModifier; }
	int					GetWheel(){ return m_nWheel; }
	CURSOR::eTYPE		GetButtonState(){ return m_nButtonState; }
	bool				CheckClickBox( int x, int y, int sx, int sy, int type ){ return ( m_nButtonState == type )&&IsInCursor( x, y, x+sx, y+sy ); }
	CURSOR::eTYPE		CheckClickBox( int x, int y, int sx, int sy ){ return IsInCursor( x, y, x+sx, y+sy ) ? GetButtonState() : CURSOR::BUTTON_OUTWINDOW; }
	CURSOR::eTYPE		CheckClickBox( CsRect rc ){ return rc.PtInRect( m_ptCursorPos ) == TRUE ? GetButtonState() : CURSOR::BUTTON_OUTWINDOW; }

	void				SetButtonState( CURSOR::eTYPE type );

	bool				IsShift(){ return ( ( m_nModifier & NiInputMouse::MMOD_LSHIFT )||( m_nModifier & NiInputMouse::MMOD_RSHIFT ) ); }	
	bool				IsAlt(){ return ( ( m_nModifier & NiInputMouse::MMOD_LMENU )||( m_nModifier & NiInputMouse::MMOD_RMENU ) ); }	
	bool				IsControl(){ return ( ( m_nModifier & NiInputMouse::MMOD_LCONTROL )||( m_nModifier & NiInputMouse::MMOD_RCONTROL ) ); }	
	bool				IsDbClick(){ return m_bDoubleClick; }

protected:
	bool				IsInCursor( int l, int t, int r, int b ){ return ( (m_ptCursorPos.x >= l) && (m_ptCursorPos.y >= t ) && (m_ptCursorPos.x <= r) && (m_ptCursorPos.y <= b) ); }
};

extern CCursor g_Cursor;

#endif