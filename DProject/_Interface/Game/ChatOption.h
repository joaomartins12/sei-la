
#pragma once 

class cChatOption : public cBaseWindow
{
public:
	cChatOption(){}

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_CHATOPTION; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();

protected:

	// 메모리잡힌 컨트롤
protected:

	// 포인터만 있는 컨트롤
protected:

	cCheckBox*		m_pCheckBox[ 6 ];
};


