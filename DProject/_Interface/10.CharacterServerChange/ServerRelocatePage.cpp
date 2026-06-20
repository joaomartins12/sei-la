
#include "stdafx.h"
#include "ServerRelocatePage.h"
#include "TermsAndConditionUI.h"
#include "CautiousUI.h"
#include "ServerRelocateRequestPageUI.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

CServerRelocatePage::CServerRelocatePage():m_pTermsNConditionsUI(NULL),m_pBeCautiousUI(NULL),m_pRequestPageUI(NULL)
{
}

CServerRelocatePage::~CServerRelocatePage()
{ 
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	Destroy(); 
}

void CServerRelocatePage::Destroy()
{
	GetSystem()->Clear3DMapData();

	DeleteScript();
}

bool CServerRelocatePage::Create()
{
	GetSystem()->Load3DMapData();

	InitScript( NULL, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), false );

	_CreateUI();
	return true;
}

void CServerRelocatePage::_CreateUI()
{
	// 서비스 이용 약관 UI
	m_pTermsNConditionsUI = NiNew CTermsAndConditionUI;
	if(m_pTermsNConditionsUI && m_pTermsNConditionsUI->Construct() )
	{
		m_pTermsNConditionsUI->Create();
		m_pTermsNConditionsUI->SetVisible(true);

		AddChildScriptUI( m_pTermsNConditionsUI );
	}
	// 주의 사항 UI
	m_pBeCautiousUI = NiNew CBeCautiousUI;
	if(m_pBeCautiousUI && m_pBeCautiousUI->Construct() )
	{
		m_pBeCautiousUI->Create();
		m_pBeCautiousUI->SetVisible(false);

		AddChildScriptUI( m_pBeCautiousUI );
	}

	m_pRequestPageUI = NiNew CRequestPageUI;
	if(m_pRequestPageUI && m_pRequestPageUI->Construct() )
	{
		m_pRequestPageUI->Create();
		m_pRequestPageUI->SetVisible(false);

		AddChildScriptUI( m_pRequestPageUI );
	}

}

void CServerRelocatePage::Update(float const& fDeltaTime)
{
}

void CServerRelocatePage::Update3DAccumtime( float const& fAccumTime )
{
	if( GetSystem() )
		GetSystem()->Update3DMapObject( fAccumTime );
}

BOOL CServerRelocatePage::UpdateMouse()
{
	if( m_pTermsNConditionsUI && m_pTermsNConditionsUI->UpdateMouse() )
		return TRUE;

	if( m_pBeCautiousUI && m_pBeCautiousUI->UpdateMouse() )
		return TRUE;

	if( m_pRequestPageUI && m_pRequestPageUI->UpdateMouse() )
		return TRUE;

	return FALSE;
}

void CServerRelocatePage::Render3DModel()
{
	if( GetSystem() )
		GetSystem()->Render3DMapObject();
}

void CServerRelocatePage::Render()
{
	RenderScript();
}

void CServerRelocatePage::ResetDevice()
{
	ResetDeviceScript();
}

void CServerRelocatePage::_SetPage(int const& nPageType)
{
	switch( nPageType )
	{
	case CServerRelocateContents::eTermNCondigitionPage:
		{
			if( m_pTermsNConditionsUI )
				m_pTermsNConditionsUI->SetVisible(true);
			if( m_pBeCautiousUI )
				m_pBeCautiousUI->SetVisible(false);
			if( m_pRequestPageUI )
				m_pRequestPageUI->SetVisible(false);
		}
		break;
	case CServerRelocateContents::eCautiousPage:
		{
			if( m_pTermsNConditionsUI )
				m_pTermsNConditionsUI->SetVisible(false);
			if( m_pBeCautiousUI )
				m_pBeCautiousUI->SetVisible(true);
			if( m_pRequestPageUI )
				m_pRequestPageUI->SetVisible(false);
		}
		break;
	case CServerRelocateContents::eRegistPage:
		{
			if( m_pTermsNConditionsUI )
				m_pTermsNConditionsUI->SetVisible(false);
			if( m_pBeCautiousUI )
				m_pBeCautiousUI->SetVisible(false);
			if( m_pRequestPageUI )
				m_pRequestPageUI->SetVisible(true);
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

bool CServerRelocatePage::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eChangePageStep, this );
	return true;
}

void CServerRelocatePage::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eChangePageStep:
		{
			int nSetPageType;
			kStream >> nSetPageType;
			_SetPage( nSetPageType );
		}
		break;
	}
}
