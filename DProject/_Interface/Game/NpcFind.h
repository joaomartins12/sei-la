
#pragma once 

class cNpcFind : public cBaseWindow
{
public:
	cNpcFind(){}

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_NPCFIND; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			PreResetMap(){ Close( false ); }
	virtual bool			OnEscapeKey(){ Close(); return true; }
	virtual void			OnMoveWindow();

	// 메모리잡힌 컨트롤
protected:
	cSprite*		m_pMouseOnMask;
	cSprite*		m_pSelectMask;

	// 포인터만 있는 컨트롤
protected:
	cButton*		m_pCancelButton;
	cButton*		m_pFindButton;
	cScrollBar*		m_pScrollBar;

protected:
	cStringList		m_StringList;

	int				m_nMouseOnMaskIndex;
	int				m_nSelectMaskIndex;
	bool			m_bRenderMouseOnMask;
	bool			m_bRenderSelectMask;

	bool			bClickDn;	//2016-02-17-nova 영역안에서 클릭여부 체크
};