#pragma once

class cCapsuleEvolution : public cBaseWindow
{
public:
	cCapsuleEvolution();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_CAPSULE_EVOLUTION; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			OnMoveWindow();

	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			ResetMap(){ Close( false ); }
	virtual bool			OnEscapeKey();
	virtual bool			Close( bool bSound = false );

	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

	virtual void			UserDie(){ Close( false ); }

private:
	cButton*		m_pCancelBtn;	// ´Ý±â ¹öÆ°
	cButton*		m_pEvolBtn;		// È®ÀÎ ¹öÆ°

	cString*		m_pItemName;	// Ä¸½¶ ¾ÆÀÌÅÛ ÀÌ¸§
	int				m_nCapsuleItem;	// Ä¸½¶ ¾ÆÀÌÅÛ¹øÈ£
	int				m_nInvenIdx;	// µî·ÏÇÑ Ä¸½¶ÀÇ ÀÎº¥ ½½·Ô
	int				m_nEvolSlot;	// ¿ÀÇÂÇÏ·Á´Â µðÁö¸ó ÁøÈ­½½·Ô

public:
	bool			IsResist( int nInvenIdx );
	void			SetResist( int nInvenIdx ) { SetResist( nInvenIdx, 1 ); }
	void			SetResist( int nInvenIdx, int nCount );

	void			_SetInvenIdx( int nIdx ){ m_nInvenIdx = nIdx; }
	void			_SetCapsuleItem( int nItemID ){ m_nCapsuleItem = nItemID; }

	void			_RecvResult( bool bSuccess );

};