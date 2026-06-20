#pragma once

#include "InventoryContents.h"

class cGiftBoxUI : public cBaseWindow, public cInventoryContents::ObserverType
{
private:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

public:
	cGiftBoxUI();
	~cGiftBoxUI();

	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );
	virtual void			Destroy();

	virtual void			Update(float const& fDeltaTime);
	virtual void			Render();

	virtual void			ResetMap();
	virtual void			ResetDevice();

	virtual void			PlayOpenSound() {}
	virtual void			PlayCloseSound() {}

private:
	cSprite*				m_pBgImg;
	cSprite*				m_pItemImg;
	cSpriteAni*				m_pEffectAni;
	cText*					m_pAcquireTxt;

	struct sOpenedItem
	{
		sOpenedItem() : nIconNo( 0 ), nItemCount( 0 ), nItemRank( 0 ) {}
		~sOpenedItem() {}

		void SetItem( uint nIconID, uint nCount, std::wstring wsName, int nRank )
		{
			nIconNo = nIconID;
			nItemCount = nCount;
			wsItemName = wsName;
			nItemRank = nRank;
		}

		uint			nIconNo;
		uint			nItemCount;
		std::wstring	wsItemName;
		int				nItemRank;
	};
	std::list< sOpenedItem >	m_lOpenList;
	bool						m_bEndEffect;

private:
	void			_AddGiftBox( sOpenedItem sItem );
	void			_SetGiftBox( sOpenedItem sItem );
	void			_NextOpenedItem( void* pSender, void* pData );
	void			_SetAniEffect( int nRank );
};
