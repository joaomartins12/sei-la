#pragma once

#include "../00.Contents/DigimonArchiveContents.h"

class cDigimonTranscendenceHelp : public cBaseWindow, public CDigimonArchiveContents::ObserverType
{
public:
	cDigimonTranscendenceHelp();
	~cDigimonTranscendenceHelp();

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
	enum eTabType{ eSystemTab, eTermsTab, eProgressTab, eChargeTab };
	struct sTabInfo : public CUSTOMDATA
	{
		sTabInfo( eTabType eTab ) : eType( eTab ) {}
		~sTabInfo(){}

		eTabType			eType;
	};
	cButton*				m_pCloseBtn;
	cRadioButton*			m_pTabBtn;
	cScrollBar*				m_pScrollBar;
	cStringList				m_ExplainText;

private:
	void					_MakeTab();
	void					_AddTab( CsPoint pos, CsPoint size, CsPoint token, std::wstring wsTabName, eTabType eType );
	void					_MakeExplain( eTabType eType );

	void					_OnClickTabButton( void* pSender, void* pData );
	void					_OnClickCloseButton( void* pSender, void* pData );
};