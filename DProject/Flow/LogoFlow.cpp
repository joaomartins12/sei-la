//---------------------------------------------------------------------------
//
// 파일명 : IntroFlow.cpp
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "LogoFlow.h"
#include "../_Interface/01.Logo/Logo.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace Flow
{
	CLogoFlow::CLogoFlow(int p_iID) : CFlow(p_iID),m_pLogo(NULL)
	{
	}
	//---------------------------------------------------------------------------
	CLogoFlow::~CLogoFlow()
	{
	}
	//---------------------------------------------------------------------------
	bool CLogoFlow::LostDevice(void* p_pvData)
	{
		return true;
	}
	//---------------------------------------------------------------------------
	bool CLogoFlow::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{
		if( m_pLogo )
			m_pLogo->ResetDevice();
		return true;
	}
	//---------------------------------------------------------------------------
	BOOL CLogoFlow::Initialize()
	{
		m_pLogo = NiNew CLogo();

		if( m_pLogo && m_pLogo->Construct() )
			m_pLogo->Init();

		g_pEngine->SetClearColor(NiColorA::WHITE);

		return TRUE;
	}

	bool CLogoFlow::BeginRenderTarget()
	{
		// BeginTarget
		return g_pEngine->BeginDefaultRenderTarget( NiRenderer::CLEAR_ALL );
	}

	//---------------------------------------------------------------------------
	void CLogoFlow::Terminate()
	{
		g_pEngine->SetClearColor(NiColorA::BLACK );
		SAFE_NIDELETE( m_pLogo );
		RESOURCEMGR_ST.CleanUpResource();
	}
	//---------------------------------------------------------------------------
	void CLogoFlow::UpdateFrame()
	{
		if( m_pLogo )
			m_pLogo->Update();
	}
	//---------------------------------------------------------------------------
	void CLogoFlow::RenderUIFrame()
	{
		if( g_pEngine )
			g_pEngine->ScreenSpace();

		if( m_pLogo )
			m_pLogo->Render();
	}
}
//---------------------------------------------------------------------------