#pragma once

#include "../00.Contents/Mail_Contents.h"

#define  MAIL_ITEM_COUNT 6
class cMail : public cBaseWindow, public MailContents::ObserverType
{
public:
	cMail();
	virtual ~cMail();

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();	
	virtual void			ResetMap();
	virtual bool			Construct();

	virtual void			ResetDevice();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);	
	virtual eMU_TYPE		Update_ForMouse();	

	virtual void			Render();		
	virtual	void			OnMoveWindow();

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void					_ReleaseItemInfo();
	bool					_UpdateIcon_ForMouse();
	void					_ItemAnalysis( TCHAR const* szStr, cText::sTEXTINFO* ti );
	void					_InsertItemInfo( int nItemIdx, int nItemCnt );
	void					SendMessageConfirm(void* pSender, void* pData);

private:	
	cButton*				m_pOkButton;
	cScrollBar*				m_pScrollBar;		//스크롤

	cStringList				m_strMessage;
	std::wstring			m_wsCashProductName;
	sIFIcon					m_IFIcon[ MAIL_ITEM_COUNT ];
		
	// 아이템 정보
	CsVectorPB< int >		m_vpItemIdx;
	CsVectorPB< int >		m_vpItemCnt;
};