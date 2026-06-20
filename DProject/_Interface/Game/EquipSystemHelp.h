#pragma once

#include "../00.Contents/EquipSystemContents.h"

class cEquipSystemHelp : public cBaseWindow, public cEquipSystemContents::ObserverType
{
public:
	cEquipSystemHelp();
	~cEquipSystemHelp();

private:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();

	virtual void			Create( int nValue = 0 );
	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetMap(void){ Close( false ); }
	virtual void			ResetDevice();

	virtual bool			OnEscapeKey();

private:
	cText*					m_pTabName;
	cButton*				m_pCloseBtn;
	cScrollBar*				m_pScrollBar;
	cStringList				m_ExplainText;

private:
	void					_MakeExplain( SystemType::eTabState eTab );
	void					_OnClickCloseButton( void* pSender, void* pData );
};