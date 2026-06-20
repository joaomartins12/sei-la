
#pragma once 

#include "../00.Contents/MainFrame_Contents.h"

class cMainBar : public cBaseWindow, public CMainFrameContents::ObserverType
{
	struct sMainBtnData : public CUSTOMDATA
	{
		sMainBtnData(int nBtnID, int nHotKeyID, std::wstring wsTooltipName)
			: m_nBtnID(nBtnID),wsName(wsTooltipName),m_nHotkeyID(nHotKeyID)
		{}
		~sMainBtnData() {};
		int m_nBtnID;
		int m_nHotkeyID;
		std::wstring wsName;
	};
public:
	cMainBar();
	~cMainBar();

	enum eButtonID				// MainBar Buttons
	{
		MAIN_BT_MENU,			// Menu Button
		MAIN_BT_WARE,			// Warehouse Button
		MAIN_BT_COMMUNITY,		// Community Button
		MAIN_BT_INVEN,			// Inventory Button
		MAIN_BT_DATA,			// Data Button
		MAIN_BT_CHAR,			// Character Button
		MAIN_BT_EVENT,			// Event Button

		MAIN_BT_MAX,			// Total Buttons
	};

private:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

public:
	virtual eWINDOW_TYPE		GetWindowType(){ return WT_MAIN_BAR; }
	virtual void				Destroy();
	virtual void				DeleteResource();
	virtual void				Create( int nValue = 0 );

	virtual void				Update(float const& fDeltaTime);
	virtual eMU_TYPE			Update_ForMouse();

	virtual void				Render();	
	virtual void				ResetDevice();

	void						SetButtonEnable(eButtonID ebtID, bool bEnable);

private:
	void						_CreateMainBarBtn();
	void						_SetToolTip( int const& hotkeytype, std::wstring const& pText, CsPoint pos, CsPoint patch );
	void						_MainBarBtnClick( void* pSender, void* pData );
#ifdef UI_INVENTORY_RENEWAL
	void						_SetNotifyNew( eButtonID ebtID, bool bEnable );
#endif

private:
	cSprite*							m_pMainBarBg;
	cSprite*							m_pMainBarSubBg;
	cGridListBox*						m_pMainBarGrid;
	std::map<int,cGridListBoxItem*>		m_pmapControls;
};