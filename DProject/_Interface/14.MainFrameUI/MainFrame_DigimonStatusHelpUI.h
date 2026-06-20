#pragma once

#include "../00.Contents/MainFrame_Contents.h"

class cDigimonStatusUI_Help : public cBaseWindow, public CMainFrameContents::ObserverType
{
public:
	cDigimonStatusUI_Help();
	~cDigimonStatusUI_Help();

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
	enum eTab{ eNormal, eNature, eAttack, eField };
	struct sTabInfo : public CUSTOMDATA
	{
		sTabInfo( eTab eType ) : eTabType( eType ) {}
		~sTabInfo() {}

		eTab				eTabType;
	};
	cRadioButton*			m_pHelpTab;
	cButton*				m_pCloseButton;

	cScriptUI*				m_pNormalInfo;
	cScriptUI*				m_pNatureInfo;
	cScriptUI*				m_pAttackInfo;
	cScriptUI*				m_pFieldInfo;

	cStringList				m_NormalExpl;
	cStringList				m_NatureExpl;
	cStringList				m_AttackExpl;
	cStringList				m_FieldExpl;

private:
	void					_MakeHelpTab();
	void					_AddHelpTab( CsPoint ptPos, std::wstring wsTabName, eTab eType );

	void					_MakeNormalInfo();
	void					_MakeNatureInfo();
	void					_MakeAttackInfo();
	void					_MakeFieldInfo();

	void					_SetCurrentTabInfo( eTab eType );

	void					_OnClickCloseButton( void* pSender, void* pData );
	void					_OnClickTabButton( void* pSender, void* pData );
};
