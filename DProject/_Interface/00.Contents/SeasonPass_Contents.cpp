
#include "stdafx.h"
#include "SeasonPass_Contents.h"

#include "../../ContentsSystem/ContentsSystemDef.h"

int const SeasonPassContents::IsContentsIdentity(void)
{
	return E_CT_SEASON_PASS;
}

SeasonPassContents::SeasonPassContents(void)
{
}

SeasonPassContents::~SeasonPassContents(void)
{
}

// ---------------------------------------------------------------------

int const SeasonPassContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool SeasonPassContents::Initialize(void)
{
	MakeWorldData();
	return true;
}

void SeasonPassContents::UnInitialize(void)
{
}

void SeasonPassContents::Update(float const& fElapsedTime)
{
}

// ---------------------------------------------------------------------

void SeasonPassContents::Req_Open(void* pkData)
{
	_OpenWindow();
}

void SeasonPassContents::Req_Close(void* pkData)
{
	_CloseWindow();
}

void SeasonPassContents::Req_Toggle(void* pkData)
{
	bool bShow = g_pGameIF->IsActiveWindow(cBaseWindow::WT_SEASONPASS_WINDOW);

	if (bShow)
		_CloseWindow();
	else
		_OpenWindow();
}

// ---------------------------------------------------------------------

bool SeasonPassContents::_CheckEnable()
{
	if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_SEASONPASS_WINDOW))
		return false;

	return true;
}

bool SeasonPassContents::_OpenWindow()
{
	if (!_CheckEnable())
	{
		_CloseWindow();
		return false;
	}

	g_pGameIF->GetDynamicIF(cBaseWindow::WT_SEASONPASS_WINDOW);

	return true;
}

bool SeasonPassContents::_CloseWindow()
{
	g_pGameIF->CloseDynamicIF(cBaseWindow::WT_SEASONPASS_WINDOW);

	return true;
}

// ---------------------------------------------------------------------

void SeasonPassContents::MakeWorldData(void)
{
	DBG("EventNoticeContents::MakeWorldData called");
	GAME_EVENT_ST.AddEvent(EVENT_CODE::REQ_OPEN_SEASON_PASS, this, &SeasonPassContents::Req_Open);
	GAME_EVENT_ST.AddEvent(EVENT_CODE::REQ_CLOSE_SEASON_PASS, this, &SeasonPassContents::Req_Close);
	GAME_EVENT_ST.AddEvent(EVENT_CODE::REQ_TOGGLE_SEASON_PASS, this, &SeasonPassContents::Req_Toggle);

	if (EventRouter())
		EventRouter()->Register(CONTENTS_EVENT::EStreamEvt_EventNotice, this);
}

// ---------------------------------------------------------------------
