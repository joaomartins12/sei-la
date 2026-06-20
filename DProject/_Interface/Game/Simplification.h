

#pragma once 

#ifdef SIMPLEFICATION
class cSimplification : public cBaseWindow
{
public:
	cSimplification();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_SIMPLE_BTN; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();	
	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }

protected:
	CsPoint					m_Point;

	// 포인터만 있는 컨트롤
protected:
	cText*					m_pText;
	cButton*				m_pSimpleChangeBtn;
	cButton*				m_pSimpleCancleBtn;
	//간략화버튼 우측 메시지 띄우기 위해 선언
	cButton*				m_pSimpleMessage;

protected:
	bool					m_bLBtnDown;

#ifdef VERSION_USA
	cSprite*				m_pBG;				// 간략화 배경
#endif

#ifdef CRASHDUMP_0305
	void					_UpdatePos();		//Update 중 ResetDevice 대체
#endif
};

extern cSimplification* g_pSimple;
#endif