
#include "stdafx.h"
#include "BattleRegisterPage.h"

#include "BattleDigimonListUI.h"
#include "BattleMapInfoUI.h"
#include "BattleModeInfoUI.h"
#include "BattleRegistBackUI.h"
#include "BattleTypeSelectUI.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

CBattleRegister::CBattleRegister()
:m_BattleTypeSelectUI(NULL),m_pRegistBackUI(NULL),
 m_pModeInfoUI(NULL),m_pMapInfoUI(NULL),m_pDigimonListUI(NULL)
{
}

CBattleRegister::~CBattleRegister()
{ 
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	Destroy(); 
}

void CBattleRegister::Destroy()
{
	DeleteScript();
	GetSystem()->EndRegistPage();
}

bool CBattleRegister::Create()
{
	GetSystem()->StartRegistPage();

	InitScript( NULL, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), false );
	AddSprite( CsPoint(0, 0), CsPoint( g_nScreenWidth, 128 ), "Battle\\Common\\battle_bg_top.tga" );
	AddSprite( CsPoint(g_nScreenWidth / 2 - (1920 / 2), g_nScreenHeight - 128), CsPoint( 1920, 128 ), "Battle\\Common\\battle_bg_bot.tga" );

	_CreateUI();
	return true;
}

void CBattleRegister::_CreateUI()
{
	m_BattleTypeSelectUI = NiNew CBattleTypeSelectUI;
	if(m_BattleTypeSelectUI && m_BattleTypeSelectUI->Construct() )
	{
		m_BattleTypeSelectUI->Create();
		m_BattleTypeSelectUI->SetVisible(true);

		AddChildScriptUI( m_BattleTypeSelectUI );
	}

	m_pRegistBackUI = NiNew CBattleRegistBackUI;
	if(m_pRegistBackUI && m_pRegistBackUI->Construct() )
	{
		m_pRegistBackUI->Create();
		m_pRegistBackUI->SetVisible(false);

		AddChildScriptUI( m_pRegistBackUI );
	}

	m_pModeInfoUI = NiNew CBattleModeInfoUI;
	if(m_pModeInfoUI && m_pModeInfoUI->Construct() )
	{
		m_pModeInfoUI->Create();
		m_pModeInfoUI->SetVisible(false);

		AddChildScriptUI( m_pModeInfoUI );
	}

	m_pMapInfoUI = NiNew CBattleMapInfoUI;
	if(m_pMapInfoUI && m_pMapInfoUI->Construct() )
	{
		m_pMapInfoUI->Create();
		m_pMapInfoUI->SetVisible(false);

		AddChildScriptUI( m_pMapInfoUI );
	}

	m_pDigimonListUI = NiNew CBattleDigimonListUI;
	if(m_pDigimonListUI && m_pDigimonListUI->Construct() )
	{
		m_pDigimonListUI->Create();
		m_pDigimonListUI->SetVisible(false);

		AddChildScriptUI( m_pDigimonListUI );
	}
}

void CBattleRegister::Update(float const& fDeltaTime)
{
	if( GetSystem() )
		GetSystem()->UpdateRegistObject( fDeltaTime );
}

void CBattleRegister::Update3DAccumtime( float const& fAccumTime )
{
	if( GetSystem() )
		GetSystem()->UpdateRegistBackMap( fAccumTime );
}

BOOL CBattleRegister::UpdateMouse()
{
	if( m_BattleTypeSelectUI && m_BattleTypeSelectUI->UpdateMouse() )
		return TRUE;

	if( m_pRegistBackUI && m_pRegistBackUI->UpdateMouse() )
		return TRUE;

	if( m_pModeInfoUI && m_pModeInfoUI->UpdateMouse() )
		return TRUE;

	if( m_pMapInfoUI && m_pMapInfoUI->UpdateMouse() )
		return TRUE;

	if( m_pDigimonListUI && m_pDigimonListUI->UpdateMouse() )
		return TRUE;

	return FALSE;
}

BOOL CBattleRegister::UpdateKeyboard(const MSG& p_kMsg)
{
	switch(p_kMsg.message)
	{	
	case WM_KEYDOWN:
		{
			switch(p_kMsg.wParam)
			{
			case VK_ESCAPE:
				{
					if( GetSystem() )
						GetSystem()->GotoBack();
				}break;
			}
		}break;
	}
	return FALSE;
}	

void CBattleRegister::Render3DModel()
{
	if( GetSystem() )
		GetSystem()->RenderRegistBackMap();
}

void CBattleRegister::Render()
{
	RenderScript();
}

void CBattleRegister::ResetDevice()
{
	ResetDeviceScript();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////

bool CBattleRegister::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;
	
	return true;
}

void CBattleRegister::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}