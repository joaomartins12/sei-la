
#include "stdafx.h"
#include "BattleMapInfoUI.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

CBattleMapInfoUI::CBattleMapInfoUI() 
{
}

CBattleMapInfoUI::~CBattleMapInfoUI()
{ 
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	Destroy(); 
}

bool CBattleMapInfoUI::Create()
{
	InitScript( NULL, CsPoint( g_nScreenWidth - 256 - 40, 50 ), CsPoint( 256, 256 ), false );

	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_16, NiColor::WHITE );
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText(L" ");

	m_pMapName = AddText( &ti, CsPoint(256/2, 5));
	m_pMapImage = AddSprite( CsPoint::ZERO, CsPoint( 256, 256 ), " " );
	return true;
}

void CBattleMapInfoUI::Destroy()
{
	DeleteScript();	
}

BOOL CBattleMapInfoUI::UpdateMouse()
{
	if( !IsVisible() )
		return FALSE;

	return FALSE;
}

void CBattleMapInfoUI::Render()
{
	RenderScript();
}

void CBattleMapInfoUI::ResetDevice()
{
	ResetDeviceScript();
}

void CBattleMapInfoUI::_UpdateBattleMapUI()
{
	BRegist::sMapInfos const* pMapInfo = GetSystem()->GetCurrentReigistBattleInfo();
	SAFE_POINTER_RET( pMapInfo );
	
	if( m_pMapImage )
		m_pMapImage->ChangeTexture( pMapInfo->GetMapFileName().c_str(), false );
	if( m_pMapName )
		m_pMapName->SetText( pMapInfo->GetMapName().c_str() );
}

void CBattleMapInfoUI::_ChangeStep(BRegist::eRegistState const& step)
{
	switch( step )
	{
	case BRegist::eState_Type_Select:	SetVisible(false);		break;
	case BRegist::eState_Mode_Select:	SetVisible(true);	_UpdateBattleMapUI();		break;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////

bool CBattleMapInfoUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eChange_State, this );
	GetSystem()->Register( SystemType::eChange_Mode, this );
	return true;
}

void CBattleMapInfoUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eChange_State:
		{
			kStream.SetRp(0);
			BRegist::eRegistState step;
			kStream >> step;
			_ChangeStep( step );
		}		
		break;
	case SystemType::eChange_Mode:	_UpdateBattleMapUI();	break;
	}
}