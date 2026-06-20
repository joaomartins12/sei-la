
#pragma once  

class cMainOption : public cBaseWindow
{
public:
	cMainOption();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_MAINOPTION; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }

	virtual bool			OnEscapeKey(){ Close(); return true; }

	// 포인터만 있는 컨트롤
protected:
	cButton*		m_pCancelButton;
	cButton*		m_pInterface;
	cButton*		m_pGraphicBtn;	
	cButton*		m_pSoundBtn;
	cButton*		m_pMacroKeyBtn;
#ifdef VERSION_KOR
	cButton*		m_pUserGradeBtn;
#endif
	cButton*		m_pLogOutBtn;
	cButton*		m_pGameExitBtn;
};

