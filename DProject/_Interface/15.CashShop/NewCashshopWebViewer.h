#pragma once 
#include "../00.Contents/NewCashshopContents.h"

class NewCashshopWebViewer : public cScriptUI, public NewCashshopContents::ObserverType
{
private:
	struct sWebUrlData : public CUSTOMDATA
	{
		std::string sWebUrl;
		sWebUrlData(std::string sUrl)
			:sWebUrl(sUrl)
		{
		}
	};

public:
	NewCashshopWebViewer();
	~NewCashshopWebViewer();

	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create(cWindow* pkRoot, int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual BOOL			UpdateMouse();

	void					SetWebImgData(LIST_CASHSHOP_WEBDATA const* pData);

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);		
	virtual	bool			Construct(void);

private:
	void					OnClicked_Banner(void* pSender, void* pData);

private:
	cListBox*				m_pBannerList;
};