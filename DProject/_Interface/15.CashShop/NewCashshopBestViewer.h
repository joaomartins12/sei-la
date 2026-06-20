
#pragma once 

#include "../00.Contents/NewCashshopContents.h"

class NewCashshopBestViewer : public cScriptUI, public NewCashshopContents::ObserverType
{
	struct BestItemInfo : public CUSTOMDATA
	{
		DWORD dwProductID;
		BestItemInfo(DWORD productID)
			:dwProductID(productID)
		{
		}
	};
public:
	NewCashshopBestViewer();
	~NewCashshopBestViewer();

	virtual void			Create(cWindow* pkRoot, int nValue = 0 );
	virtual void			Destroy();
	virtual void			DeleteResource();

	virtual void			Update(float const& fDeltaTime);
	virtual BOOL			UpdateMouse();

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);		
	virtual	bool			Construct(void);

private:
	void					_MakeBestItemList();
	void					_OnItemSelected_BestItem(void* pSender, void* pData);

private:
	cGridListBox*			m_pkItems;
};