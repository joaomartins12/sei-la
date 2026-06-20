#pragma once 
#include "../00.Contents/ChannelContents.h"
class cChannel : public cBaseWindow, public ChannelContents::ObserverType
{
public:

	cChannel();
	virtual ~cChannel();

	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			ResetDevice();
	virtual void			Create( int nValue = 0 );

	virtual	bool			Construct(void);

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();	
	virtual void			Render();

	
	virtual void			ResetMap(){ Close( false ); }

	virtual void			PlayOpenSound(){}


	void					RecvChannelList();
	void					ShowChannelList(void* pSender, void* pData);
	void					SetChannelTxt();

	// 메모리잡힌 컨트롤
protected:
	virtual	void	Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

	// 포인터만 있는 컨트롤
protected:
	cText*					m_pText;
	cButton*				m_pChannelChangeBtn;

};



