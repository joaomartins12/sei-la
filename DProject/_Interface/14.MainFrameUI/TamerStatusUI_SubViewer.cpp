#include "stdafx.h"
#include "TamerStatusUI_SubViewer.h"

cTamerStatusUI_SubViewer::cTamerStatusUI_SubViewer()
{

}

cTamerStatusUI_SubViewer::~cTamerStatusUI_SubViewer()
{
	Destroy();
}

bool cTamerStatusUI_SubViewer::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void cTamerStatusUI_SubViewer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{

}

void cTamerStatusUI_SubViewer::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

