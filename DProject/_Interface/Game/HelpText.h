
#pragma once 

#define IF_HELPTEXT_WIDTH		385
#define IF_HELPTEXT_HEIGHT		480

#include "HelpText_Title.h"
#include "HelpText_Body.h"

class cHelpText : public cBaseWindow
{
public:
	cHelpText();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_HELPTEXT; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();

	virtual void			OnMoveWindow();
	virtual bool			OnEscapeKey(){ Close(); return true; }

	// 포인터만 있는 컨트롤
protected:
	cButton*		m_pCancelButton;
	//cSprite*		m_pLogo;

protected:
	cHelpText_Title	m_Title;
	cHelpText_Body	m_Body;

public:
	cHelpText_Title* GetTitle(){ return &m_Title; }
	cHelpText_Body*	 GetBody(){ return &m_Body; }

public:
	static CsPoint	GetTotalSize(){ return CsPoint( 385, 480 ); }
	static CsPoint	GetTitleStartPos(){ return CsPoint( 2, 41 ); }
	static CsPoint	GetTitleEndPos(){ return CsPoint( 92, 478 ); }
	static CsPoint	GetBodyStartPos(){ return CsPoint( 105, 40 ); }	
	static CsPoint	GetBodyEndPos(){ return CsPoint( 358, 466 ); }	
};

