#pragma once 
#include "../00.Contents/NewCashshopContents.h"
#include "NewCashshopPurchaseSubViewer.h"
class NewCashshopPurchaseViewer : public cBaseWindow, public NewCashshopContents::ObserverType
{
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);		


public:
	virtual	bool			Construct(void);

public:
	NewCashshopPurchaseViewer();
	~NewCashshopPurchaseViewer();

public:
	virtual eWINDOW_TYPE	GetWindowType(void);

	virtual void			Destroy(void);
	virtual void			DeleteResource(void);
	virtual void			Create( int nValue = 0 );	
	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse(void);
	virtual void			Render(void);
	virtual void			ResetDevice(void);	
	virtual bool			OnEscapeKey();
	virtual bool			OnEnterKey();
	virtual void			UpdateViewer(NewCashshopContents::CASHSHOP_FUNTION eBType);

private:
	void					OnClickClosePurchaseViewer(void* pSender, void* pData);
	void					OnClickPurchaseBT(void* pSender, void* pData);
	void					OnClickCancelBT(void* pSender, void* pData);
	void					UpdateCashBalance();

private:
	int						m_nCurrentPurchaseType;
	cText*					m_pTitle;
	cButton*				m_pkCloseBT;
	cButton*				m_pkPurchaseBT;
	cButton*				m_pkCancelBT;

	cText*					m_pkTotalCash;
	cText*					m_pkPriceCash;
	cText*					m_pkRemainCash;
	
	NewCashshopPurchaseSubViewer*		m_pkCurrSubViewer;
	NewCashshopPurchaseSubViewer*		m_pkPurchaseSingleViewer;
	NewCashshopPurchaseSubViewer*		m_pkPresentViewer;
	NewCashshopPurchaseSubViewer*		m_pkPurchaseMultiViewer;
};