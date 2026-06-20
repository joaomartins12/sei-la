#include "stdafx.h"
#include "EventUIViewer.h"

IEventViewer::IEventViewer()
{

}

IEventViewer::~IEventViewer()
{

}

void IEventViewer::Create(cWindow* pkRoot, int nValue )
{
	InitScript( pkRoot, NULL, CsPoint(224,56), CsPoint(550,500), false );
	AddSprite( CsPoint(42,40), CsPoint(465,123), "Event\\event_deco.tga");
}

void IEventViewer::Destroy()
{

}

void IEventViewer::DeleteResource()
{

}

//////////////////////////////////////////////////////////////////////////
void IEventViewer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{

}

bool IEventViewer::Construct(void)
{
	return true;
}

void IEventViewer::SetSubViewer(int const& nSubType)
{

}
