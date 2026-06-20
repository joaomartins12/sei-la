#pragma once 

#include "EventContents.h"

class IEventViewer : public cScriptUI, public EventContents::ObserverType
{
protected:
	struct sRewardItemData : public CUSTOMDATA
	{
		sRewardItemData(int const& nItemCode):s_nItemCode(nItemCode)
		{}
		~sRewardItemData() {};

		int s_nItemCode;
	};

public:
	IEventViewer();
	virtual ~IEventViewer();

	virtual void			Create(cWindow* pkRoot, int nValue = 0 );
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual BOOL			UpdateMouse() { return FALSE; };

	virtual void			SetSubViewer( int const& nSubType );

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);		
	virtual	bool			Construct(void);
};
