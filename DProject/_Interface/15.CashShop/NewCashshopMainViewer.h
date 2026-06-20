#pragma once 

#include "../00.Contents/NewCashshopContents.h"
#include "NewCashshopItem.h"

class NewCashshopMainViewer : public ItemViewer
{
	struct MainItemInfo : public CUSTOMDATA
	{
		DWORD dwProductID;
		MainItemInfo(DWORD productID)
			:dwProductID(productID)
		{
		}
	};

public:
	NewCashshopMainViewer();
	~NewCashshopMainViewer();

	virtual void			Create(cWindow* pkRoot, int nValue = 0 );
	virtual void			Destroy();
	virtual void			DeleteResource();

	virtual void			Update(float const& fDeltaTime);
	virtual BOOL			UpdateMouse();

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);		
	virtual	bool			Construct(void);

private:
	cGridListBox *			_CreateItemControl(int const& nSubCategory, const CsPoint& GridStartPos, const CsPoint& GridSize, const CsPoint& SlotGap);
	void					_OnItemSelected_BestItem(void* pSender, void* pData);

private:
	cGridListBox*			m_pkUpperItems;
	cGridListBox*			m_pkMiddleItems;
	cGridListBox*			m_pkBottomItems;
};