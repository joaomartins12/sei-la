#pragma once

#include "ContentsSystem/ContentsSystem.h"
#include "_Interface/Adapt/AdaptWareHouse.h"

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
#ifdef NEW_SHARESTASH
		CI_SHARESTASH,
#endif
#ifdef CROSSWARS_SYSTEM
		CI_CROSS,
#endif
		CI_TEQUIP,
		CI_CHIPSET,
		CI_TSKILL,
		CI_DSKILL,
		CI_QUICKSLOT,					// 퀵슬롯에서의 아이콘 갯수는 의미를 두지 않겠다
		CI_CENTERBAR,
		CI_TACTICSHOUSE,
		CI_CARDINVEN,
		CI_INCUBATOR,		
	};
}


namespace CLICK
{
	enum
	{
		NONE			=	0,
		INVEN,
		QUICK,		
		SKILL,
		EQUIP,
		MEMORY,
		CAPSULE,		
		CHIPSET,
	};
}

namespace CUR
{
	enum eTYPE
	{
		NORMAL,
		HAND,
		DIGITAMA,
		STORE,
		TAKL,
		ATTACK,
		TEST_CLICK,
		MAX_COUNT,
		NONE,
	};
}

namespace CURSOR_RES
{
	enum eTYPE
	{
		NORMAL,
		HAND,
		DIGITAMA,
		STORE,
		TALK,
		ATTACK,
		MAX_COUNT,
		NONE,
	};
}

#define INVALIDE_WHEEL		0

class AdaptWareHouse; 

class CCursor : public CSingleton< CCursor >
{
public:
	CCursor();
	virtual ~CCursor();

	void	Init(HINSTANCE hIns);
	void	Input(const MSG& p_kMsg);	//2016-01-12-nova
	void	Render();

	//==================================================================================================================
	//		마우스
	//==================================================================================================================
protected:
	CURSOR::eTYPE		m_nButtonState;
	CsPoint				m_ptCursorPos;	
	int					m_nWheel;
public:
	CsPoint				GetPos(){ return m_ptCursorPos; }
	int					GetWheel(){ return m_nWheel; }
	CURSOR::eTYPE		GetButtonState(){ return m_nButtonState; }
	bool				CheckClickBox( int x, int y, int sx, int sy, int type );
	CURSOR::eTYPE		CheckClickBox( int x, int y, int sx, int sy );
	CURSOR::eTYPE		CheckClickBox( CsRect rc );

	void				SetButtonState( CURSOR::eTYPE type );	

	void				ResetWheel();	//2016-02-03-nova 마우스 휠값 초기화

	bool				IsInCursor( int l, int t, int r, int b );

private:
	void				_LoadCursor( int const& nType, std::wstring const& wsFileName, LPCWSTR defultCursor, HINSTANCE hIns );

	//==================================================================================================================
	//		아이콘
	//==================================================================================================================
protected:
	CURSOR_ICON::eTYPE	m_eIconType;
	int					m_nIconSlot;
	int					m_nIconCount;

	void*				m_pIFIcon;
	bool				m_bIconDropMessageBox;
	int					m_nIconSubSlot1;
	int					m_nIconSubSlot2;
	int					m_nIconSubSlot3;

public:
	void				SetIcon( CURSOR_ICON::eTYPE eType, int nSlot, int nCount, void* pIFIcon );
	void				SetIconSubInfo( int nSubSlot1, int nSubSlot2, int nSubSlot3 );
	void				ReleaseIcon();
	void				InitCursor();
	bool				IsIcon(){ return m_eIconType != CURSOR_ICON::CI_NONE; }

	const int			GetIconSubInfo1() const { return m_nIconSubSlot1; }
	const int			GetIconSubInfo2() const { return m_nIconSubSlot2; }
	const int			GetIconSubInfo3() const { return m_nIconSubSlot3; }
	const int			GetIconSlot() const {return m_nIconSlot;}
	const int			GetIconCount() const {return m_nIconCount;}
	CURSOR_ICON::eTYPE	GetIconType() const {return m_eIconType;}

protected:	
	bool				_IconCancel();	
	bool				_IconLBtnUp();
	bool				_IconLBtnDown();

public:
	void				IconDropMessageBegin();
	bool				IconDropMessageTry();
	void				IconDropOk();
	void				IconDropCancel();	

	bool				IsNotUseItem( CURSOR_ICON::eTYPE eType, int nItemIndex );
	//========================================================================================================
	//
	//		커서
	//
	//========================================================================================================

protected:
	CURSOR_RES::eTYPE	m_eCurCursor;	
	HCURSOR				m_hCursor[ CURSOR_RES::MAX_COUNT ];
	bool				m_bShowCursor;

public:	
	void			SetCursorRes( CURSOR_RES::eTYPE type );
	void			SetCursorRes( CsC_AvObject* pObject );
	void			HideCursorRes();
	void			ShowCursorRes();	

	void			LoopReset();	//2016-02-23-nova Update에서 맨마지막에 마우스 상태 리셋

private: 
	TOwnershipGuard<AdaptWareHouse> m_pWarehouseAdapt; 
};


//---------------------------------------------------------------------------
// Singleton
#define CURSOR_ST		CCursor::GetSingleton()
#define CURSOR_STPTR	CCursor::GetSingletonPtr()
//---------------------------------------------------------------------------


