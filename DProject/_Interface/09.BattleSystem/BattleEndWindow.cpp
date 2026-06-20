
#include "stdafx.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"
#include "BattleEndWindow.h"

cBattleEndWindow::cBattleEndWindow():m_fTime(0.0f),m_nShowType(eNone)
{
}

cBattleEndWindow::~cBattleEndWindow()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

cBaseWindow::eWINDOW_TYPE	cBattleEndWindow::GetWindowType()
{ 
	return cBaseWindow::WT_BATTLESTART_END; 
}

void cBattleEndWindow::Destroy()
{	
	cBaseWindow::Delete();		
}

void cBattleEndWindow::DeleteResource()
{	
	DeleteScript();	
}

void cBattleEndWindow::Create(int nValue /* = 0  */)
{
	if(!cBaseWindow::Init())
		return;

	InitScript( NULL, CsPoint(0, g_nScreenHeight * 0.2), CsPoint( g_nScreenWidth, 200 ), false );

	cSprite* pBack = NiNew cSprite;
	if( pBack )
	{
		pBack->Init( GetRoot(), CsPoint::ZERO, CsPoint( g_nScreenWidth, 200 ), NiColorA( 0, 0, 0, 0.6f ), false );
		AddChildControl( pBack );
	}

	AddSprite( CsPoint( (g_nScreenWidth/2) - (502/2), (200 - 163)/2 ), CsPoint( 502, 163 ), "Battle\\Result\\battle_finished.tga" );
	SetVisible( false );
}

void cBattleEndWindow::Update(float const& fDeltaTime)
{
	if( m_nShowType == eNone )
		return;

	m_fTime -= fDeltaTime;
	if( m_fTime <= 0.0f )
		_ChangeNextStep(m_nShowType);
}

void cBattleEndWindow::_ChangeNextStep( int const& nType )
{
	switch( nType )
	{
	case eHide_Wait:// Hide_Wait -> Result_Show
		{
			m_nShowType = eResult_Show;
			m_fTime = 3.0f;		// 결과창 3초가 보여줌
			SetVisible( true );		
		}
		break;
	case eResult_Show:// Result_Show -> Wait_End
		{
			m_nShowType = eNone;
			m_fTime = 0.0f;	// 종료 패킷을 보내기 전 3초간 대기
			Close();
			GetSystem()->OpenResultPage();
		}
		break;
	}
}

void cBattleEndWindow::Render()
{
	RenderScript();
}

void cBattleEndWindow::ResetDevice()
{
	ResetDeviceScript();
}

bool cBattleEndWindow::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register(SystemType::eBattleRoundEnd, this);

	return true;
}

void cBattleEndWindow::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eBattleRoundEnd:
		m_fTime = 1.0f;
		m_nShowType = eHide_Wait;
		break;
	}
}

