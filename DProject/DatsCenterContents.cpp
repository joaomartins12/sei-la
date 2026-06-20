#include "StdAfx.h"
#include "DatsCenterContents.h"

//#include "../../Flow/FlowMgr.h"	\_Interface\Game
//#include "../../Flow/Flow.h"		\_Interface\Game

#include "Flow/FlowMgr.h"
#include "Flow/Flow.h"

//////////////////////////////////////////////////////////////////////////
////////////////////	CDatsCenterContents	//////////////////////
//////////////////////////////////////////////////////////////////////////
int const CDatsCenterContents::IsContentsIdentity(void)
{
	return E_CT_DATS_CENTER;
}

CDatsCenterContents::CDatsCenterContents(void)
{
}

CDatsCenterContents::~CDatsCenterContents(void)
{
}

int const CDatsCenterContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CDatsCenterContents::Initialize(void)
{

	return true;
}

void CDatsCenterContents::UnInitialize(void)
{
}

bool CDatsCenterContents::IntraConnection(ContentsSystem& System)
{ 
	return true; 
}

void CDatsCenterContents::Update(float const& fElapsedTime)
{
}

void CDatsCenterContents::NotifyEvent(int const& iNotifiedEvt)
{
}

void CDatsCenterContents::MakeMainActorData(void)
{
}

void CDatsCenterContents::ClearMainActorData(void)
{
}