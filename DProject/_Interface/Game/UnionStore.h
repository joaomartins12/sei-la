#pragma once 
#include "UnionStoreContents.h"

#define IF_UNION_X				6
#define IF_UNION_Y				3
#define	IF_UNION_COUNT			18			// X * Y

#define IF_UNION_ICONSIZE		CsPoint( 32, 32 )
#define IF_UNION_MONEY_POS		CsPoint( 69, 192 )

class cUnionStore : public cBaseWindow, public UnionStoreContents::ObserverType
{
public:
	cUnionStore();
	virtual ~cUnionStore();

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual	bool			Construct(void);

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_UNIONSTORE; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();	
	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }

	virtual bool			OnEscapeKey(){ Close(); return true; }
	virtual void			OnMoveWindow();

	virtual void			UserDie(){ Close( false ); }
	virtual void			RecvDiedAlready(){ Close( false ); }

protected:
	cButton*				m_pCancelBtn;
	cButton*				m_pEmploymentBtn;
	cButton*				m_pCollectBtn;

	cMoneyIF*				m_pMoneyIF;

	sIFIcon					m_IFIcon[ IF_WAREHOUSE_COUNT ];

	CsEnableTimeSeq			m_CollectItemTimeSeq;
	
	bool					m_bActiveCollectBtn;	
	bool					m_bEnableCollectBtn;

public:
	void					SetEnableCollectBtn( bool bEnable ){ m_bEnableCollectBtn = bEnable; }	
	bool					IsEnableCollectBtn(){ return m_bEnableCollectBtn; }
	bool					IsActiveCollectBtn(){ return m_bActiveCollectBtn; }
	void					CollectItem();

protected:
	void					PressCancelBtn( void* pSender, void* pData );		//2017-04-19-nova
	void					PressEmploymentBtn( void* pSender, void* pData );	//2017-04-19-nova
	void					PressCollecttBtn( void* pSender, void* pData );		//2017-04-19-nova
	

	void					_UpdateIcon_ForMouse();	
};