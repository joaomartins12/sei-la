
#pragma once 

#include "WareHouseContents.h"
#define IF_WAREHOUSE_X					7
#define IF_WAREHOUSE_Y					10
#define	IF_WAREHOUSE_COUNT				70			// X * Y

#ifdef NEW_SHARESTASH
	#define IF_SHARESTASH_X					7
	#define IF_SHARESTASH_Y					2
	#define IF_SHARESTASH_COUNT				14
#endif

class cWarehouse : public cBaseWindow, public WareHouseContents::ObserverType
{
protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

public:
	virtual	bool			Construct(void);

public:
	cWarehouse(){};
	~cWarehouse(){};

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_WAREHOUSE; }
	virtual void			Destroy();
	virtual bool			Close( bool bSound = true ){ return cBaseWindow::Close( bSound ); }
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();	
	virtual void			Render();

	virtual void			OnMouseOutWindow();
	virtual void			OnLButtonUp( CsPoint pos );
	virtual void			OnRButtonUp( CsPoint pos );
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }

	virtual bool			OnEscapeKey(){ Close(); return true; }
	virtual void			OnMoveWindow();

	virtual void			PlayOpenSound(){}

	virtual void			UserDie(){ Close( false ); }
	virtual void			RecvDiedAlready(){ Close( false ); }

protected:
	bool					_UpdateIcon_ForMouse();


	// 메모리잡힌 컨트롤
protected:
	cSprite*				m_pBlock;
	cSprite*				m_pBlockInItem;

	// 포인터만 있는 컨트롤
protected:
	cButton*				m_pCancelButton;
	cScrollBar*				m_pScrollBar;
#ifndef NEW_WAREHOUSESORT
	CsCoolTimeSeq			m_SortSeq;
#endif
	cButton*				m_pWareHouseSort;
#ifdef NEW_SHARESTASH
	cButton*				m_pShareStashSort;
#endif
	
protected:
	CsC_AvObject*			m_pTarget;
	NiPoint2				m_ptTargetPos;
	sIFIcon					m_IFIcon[ IF_WAREHOUSE_COUNT ];	
#ifdef NEW_SHARESTASH
	sIFIcon					m_IFIcon_ex[ IF_SHARESTASH_COUNT ];	
#endif
public:
	void			SetTarget( CsC_AvObject* pTarget );
	void			SetTargetPos( NiPoint2 pos );


	//========================================================================================================
	//
	//	등록
	//
	//========================================================================================================
public:
	//	함수 하나로 통일 해야한다...
	bool			SetResist( int nInvenIndex, int nWarehouseIndex, int nCount );
	
#ifdef NEW_SHARESTASH
	bool			MoveItemStoW( int nSrc, int nDest, int nCount );
	bool			MoveItemItoW( int nSrc, int nDest, int nCount );
	bool			MoveItemWtoW( int nSrc, int nDest, int nCount );

	bool			MoveItemWtoS( int nSrc, int nDest, int nCount );	
	bool			MoveItemItoS( int nSrc, int nDest, int nCount );	
	bool			MoveItemStoS( int nSrc, int nDest, int nCount );
#else
	bool			MoveItem( int nSrc, int nDest, int nCount );
#endif
};