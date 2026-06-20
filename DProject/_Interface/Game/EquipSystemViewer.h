#pragma once
#include "../00.Contents/EquipSystemContents.h"
#include "EquipSystemSubViewer.h"

class cEquipSystemViewer : public cBaseWindow, public cEquipSystemContents::ObserverType
{
public:
	cEquipSystemViewer();
	~cEquipSystemViewer();

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

	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

	virtual void			ResetDevice();
	virtual void			ResetMap();

	virtual bool			OnEscapeKey();
	virtual void			OnMoveWindow();

	virtual void			UserDie();
	virtual void			RecvDiedAlready();

private:
	enum eTabType { eTabEquipCharge, eTabEquipChargeQuick, eTabElemManage };
	struct sTabInfo : public CUSTOMDATA
	{
		sTabInfo( eTabType eType ) : eTab( eType ) {}
		~sTabInfo() {}

		eTabType			eTab;
	};
	cRadioButton*			m_pTabButton;
	cButton*				m_pCloseButton;

	cEquipSystemSubViewer*	m_pCurViewer;
	cEquipSystemSubViewer*	m_pElemViewer;
	cEquipSystemSubViewer*	m_pNormalChargeViewer;
	cEquipSystemSubViewer*	m_pQuickChargeViewer;

private:
	void			_OnClickTabButton( void* pSender, void* pData );
	void			_OnClickCloseButton( void* pSender, void* pData );

private:
	void			_SetViewer( cEquipSystemSubViewer* pViewer, bool bEnable );
};