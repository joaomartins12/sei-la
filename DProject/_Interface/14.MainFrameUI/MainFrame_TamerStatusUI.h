#pragma once

#include "../00.Contents/MainFrame_Contents.h"
#include "TamerStatusUI_SubViewer.h"

class cTamerStatusUI : public cBaseWindow, public CMainFrameContents::ObserverType
{
public:
	cTamerStatusUI();
	~cTamerStatusUI();

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

	virtual void			OnLButtonUp( CsPoint pos );
	virtual void			OnRButtonUp( CsPoint pos );
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

	virtual bool			OnEscapeKey();

private:
	enum eTabType { eTabTamer, eTabDigivice, eTabSkill, eTabCross };
	struct sTabInfo : public CUSTOMDATA
	{
		sTabInfo( eTabType tabType ) : eType( tabType ) {}
		~sTabInfo() {}

		eTabType	eType;
	};
	cRadioButton*			m_pStatusTab;
	cButton*				m_pCancelBtn;

	cSprite*				m_pBgImg;

	cTamerStatusUI_SubViewer*		m_pCurViewer;
	cTamerStatusUI_SubViewer*		m_pTamerViewer;
	cTamerStatusUI_SubViewer*		m_pDigiviceViewer;
	cTamerStatusUI_SubViewer*		m_pSkillViewer;
#ifdef CROSSWARS_SYSTEM
	cTamerStatusUI_SubViewer*		m_pCrossViewer;
#endif

	void					_MakeStatusTab( int nTabCount );
	void					_AddStatusTab( CsPoint& ptPos, CsPoint ptSize, CsPoint ptTex, std::wstring wsTabName, eTabType eType );
	void					_SetStatusTab( int nTabNo );

	// Event Function
	void					_OnClickTabButton( void* pSender, void* pData );
	void					_OnClickCloseButton( void* pSender, void* pData );

	//--------------------------------------Barcode--------------------------------------
#ifdef USE_BARCODE_REDER
private:
	cButton*		m_pBarcodeBtn;
	uint			m_BarcodeState;

private:
	void			_DeleteBarcode();
	void			_InitBarcode();
	void			_InitBarcodeState();
	bool			_CalBarcodeState();

	void			_OnClickRegisterButton( void* pSender, void* pData );
#endif
};

