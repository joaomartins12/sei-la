#pragma once 
#include "EventContents.h"
#include "EventUIViewer.h"

class cRewardHotTimeUI : public IEventViewer
{
public:
	cRewardHotTimeUI();
	virtual ~cRewardHotTimeUI();	

public:
	virtual void		Create(cWindow* pkRoot, int nValue = 0 );
	virtual BOOL		UpdateMouse();
	virtual void		RenderScript();
	
protected:
	virtual	void		Notify(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual	bool		Construct(void);

private:
	void				UpdateTimeMsg( std::wstring const& wsStartDate, std::wstring const& wsEndDate );
	void				ReSizeGridList( int const& nItemCount );
	std::wstring		GetWeekString( int const& nWeek );			
	void				UpdateHotTimeItem();
	void				AddGridItem( int const& nItemNo, bool bItemGet, CsEventTable::sHotTimeEvent const* sEventInfo );

	std::wstring		SplitEventDate( wstring wsEventDate );
	std::wstring		SplitRewardTime( wstring wsStartTime, wstring wsEndTime );

	void				PressAcceptButton( void* pSender, void* pData );

private:
	cGridListBox*		m_pRewardGride;
	cButton*			m_pAcceptBtn;
	cText*				m_pEventTimeText;
};