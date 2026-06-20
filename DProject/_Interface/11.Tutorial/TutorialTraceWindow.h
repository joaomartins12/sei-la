#pragma once 

#include "../00.Contents/TutorialContents.h"
#include "TutorialUITraceData.h"

class cTutorialTraceWindow : public cBaseWindow, public CTutorialContents::ObserverType
{
public:
	cTutorialTraceWindow();
	virtual ~cTutorialTraceWindow();

public:
	virtual	bool			Construct(void);
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_TUTORIAL_TRACE; }	
	virtual void			ResetDevice();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );	
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Update(float const& fDeltaTime);
	virtual void			Render();
	virtual void			PreResetMap();
	virtual void			ResetMap();


protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void					_UpdateUITraceData( sTraceData const& newData );

	void					_TraceItemUI();
	void					_TraceTalkQeustUI();
	void					_TraceTalkWindowUI();
	void					_TraceWindowControlUI();
	void					_TraceQuickSlotUI();

private:
	sTraceData				m_CurrentData;
};


