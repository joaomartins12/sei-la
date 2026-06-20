
#include "stdafx.h"
#include "EventNotice_Contents.h"

#include "../../ContentsSystem/ContentsSystemDef.h"

int const EventNoticeContents::IsContentsIdentity(void)
{
	return E_CT_EVENT_NOTICE;
}

EventNoticeContents::EventNoticeContents(void)
{
}

EventNoticeContents::~EventNoticeContents(void)
{
}

// ---------------------------------------------------------------------

int const EventNoticeContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool EventNoticeContents::Initialize(void)
{
	MakeWorldData();
	return true;
}

void EventNoticeContents::UnInitialize(void)
{
}

void EventNoticeContents::Update(float const& fElapsedTime)
{
}

// ---------------------------------------------------------------------

void EventNoticeContents::Req_OpenEventNotice(void* pkData)
{
	DBG("Event Notice Button - calling _OpenWindow()");

	_OpenWindow();
}

void EventNoticeContents::Req_CloseEventNotice(void* pkData)
{
	DBG("Event Notice Button - calling _CloseWindow()");

	_CloseWindow();
}

void EventNoticeContents::Req_ToggleEventNotice(void* pkData)
{
	bool bShow = g_pGameIF->IsActiveWindow(cBaseWindow::WT_NOTICE_WINDOW);

	if (bShow)
		_CloseWindow();
	else
		_OpenWindow();
}

// ---------------------------------------------------------------------

bool EventNoticeContents::_CheckEnableEventNotice()
{
	if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_NOTICE_WINDOW))
		return false;

	return true;
}

bool EventNoticeContents::_OpenWindow()
{
	DBG("Event Notice Button - _OpenWindow()");

	if (!_CheckEnableEventNotice())
	{
		DBG("Event Notice Button - Not enabled calling _CloseWindow()");

		_CloseWindow();
		return false;
	}

	g_pGameIF->GetDynamicIF(cBaseWindow::WT_NOTICE_WINDOW);

	return true;
}

bool EventNoticeContents::_CloseWindow()
{
	g_pGameIF->CloseDynamicIF(cBaseWindow::WT_NOTICE_WINDOW);

	return true;
}

// ---------------------------------------------------------------------

void EventNoticeContents::MakeWorldData(void)
{
	DBG("EventNoticeContents::MakeWorldData called");
	GAME_EVENT_ST.AddEvent(EVENT_CODE::REQ_OPEN_EVENT_NOTICE, this, &EventNoticeContents::Req_OpenEventNotice);
	GAME_EVENT_ST.AddEvent(EVENT_CODE::REQ_CLOSE_EVENT_NOTICE, this, &EventNoticeContents::Req_CloseEventNotice);
	GAME_EVENT_ST.AddEvent(EVENT_CODE::REQ_TOGGLE_EVENT_NOTICE, this, &EventNoticeContents::Req_ToggleEventNotice);

	if (EventRouter())
		EventRouter()->Register(CONTENTS_EVENT::EStreamEvt_EventNotice, this);
}

// ---------------------------------------------------------------------
