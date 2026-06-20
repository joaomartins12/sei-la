#include "stdafx.h"
#include "ActionKey.h"

cActionKey::cActionKey() : m_pTalkText( NULL )
{

}

cActionKey::~cActionKey()
{
}

bool cActionKey::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eChange_Near_Npc, this );
	return true;
}

void cActionKey::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	cBaseWindow::Delete();
}

void cActionKey::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eChange_Near_Npc:
		{
			std::wstring wsName;
			kStream >> wsName;

			std::wstring wsTalk = UISTRING_TEXT( "NPC_ACTIONKEY_TEXT" );
			DmCS::StringFn::Replace( wsTalk, L"#Name#", wsName );

			m_pTalkText->SetText( wsTalk.c_str() );
		}
		break;
	}
}

cBaseWindow::eWINDOW_TYPE cActionKey::GetWindowType()
{
	return cBaseWindow::WT_ACTIONKEY;
}

void cActionKey::DeleteResource()
{
	DeleteScript();
}

void cActionKey::Create(int nValue /*= 0 */)
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( NULL, CsPoint( (1024 - 58)/2, (768 - 32)/2 ), CsPoint( 58, 32 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER );
	
	CsPoint imgPos = CsPoint( 0, 124 );
	cSpriteAni* pActAniImg = NiNew cSpriteAni;
	pActAniImg->Init( cSpriteAni::LOOP, m_pRoot, imgPos, CsPoint( 58, 32 ), "ActionKey\\actionkey_ani.tga", NULL, 5, false, CsPoint( 0, 32 ), cSpriteAni::SPRITE_POS, 1 );
	pActAniImg->SetAniTime( 0.1f );
	pActAniImg->PlayOn( false );
	AddChildControl( pActAniImg );

	imgPos.x = pActAniImg->GetSize().x * 0.5f;
	imgPos.y += 32;
	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_10, NiColor::WHITE );
	ti.s_eTextAlign = DT_CENTER;
	m_pTalkText = AddText( &ti, imgPos );
}

void cActionKey::Update(float const& fDeltaTime)
{
	UpdateScript(fDeltaTime);
}

cBaseWindow::eMU_TYPE cActionKey::Update_ForMouse()
{
	return MUT_OUT_WINDOW;
}

cBaseWindow::eMU_TYPE cActionKey::Update_ForMouse_Disable()
{
	return cBaseWindow::Update_ForMouse();
}

void cActionKey::Render()
{
	RenderScript();
	EndRenderScript();
}

bool cActionKey::OnMacroKey(const MSG& p_kMsg)
{
	// 키 입력 받으면 Talk 띄우고 ActionKey UI 종료
	switch( p_kMsg.message )
	{
	case WM_KEYDOWN:
		{
			if( GLOBALINPUT_ST.GetEqualSnKey( p_kMsg.wParam, DMKEY::NPC_ACTION ) )
			{
				GetSystem()->StartTalkNpc();
				return true;
			}
		}
		break;
	}

	return false;
}

void cActionKey::ResetMap()
{
	// 맵 이동 시 UI close
	Close( false );
}

void cActionKey::ResetDevice()
{
	ResetDeviceScript();
}