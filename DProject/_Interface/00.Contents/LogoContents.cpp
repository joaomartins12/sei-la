#include "StdAfx.h"
#include "LogoContents.h"

#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#include "../../ContentsSystem/ContentsSystemDef.h"

int const LogoContents::IsContentsIdentity(void)
{
	return E_CT_LOGO;
}

LogoContents::LogoContents(void)
{
}

LogoContents::~LogoContents(void)
{
}

int const LogoContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool LogoContents::Initialize(void)
{
	
	return true;
}

void LogoContents::UnInitialize(void)
{
}

bool LogoContents::IntraConnection(ContentsSystem& System)
{ 
	return true; 
}

void LogoContents::Update(float const& fElapsedTime)
{
}

void LogoContents::NotifyEvent(int const& iNotifiedEvt)
{
}

void LogoContents::MakeMainActorData(void)
{
}

void LogoContents::ClearMainActorData(void)
{
	ClearAllRegistered();
}

void LogoContents::EndLogo(void) const
{
	FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_LOGIN);
}