//---------------------------------------------------------------------------
//
// 파일명 : Flow.cpp
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "Flow.h"

//---------------------------------------------------------------------------
namespace Flow
{
	CFlow::CFlow(int p_iID)
	:m_iID(p_iID)
	,m_bInitialized(FALSE)
	,m_bCreated(FALSE)
	,m_bPaused(FALSE)
	,m_bLoaded(FALSE)
	,m_bInputLock(FALSE)
	{
		SetCreated(TRUE);
	}
//---------------------------------------------------------------------------
	CFlow::~CFlow()
	{
		OnExit(-1);
		SetCreated(FALSE);
	}
//---------------------------------------------------------------------------
	void CFlow::OnEnter()
	{
		// 초기화
		if(!IsInitialized())
		{
			if(Initialize())
			{
				// Resource 
				if(LoadResource())
				{
					// Load Flag
					SetLoaded(TRUE);
				}

				// Init Flag
				SetInitialized(TRUE);
			}
			else
			{
				assert(!"Flow Initialize Failed!");
			}
		}
	//	ECFLOG("================Flow OnEnter(%d)===============\n", m_iID);
	}
//---------------------------------------------------------------------------
	void CFlow::OnIdle()
	{
		MainLoop();
	}
//---------------------------------------------------------------------------
	void CFlow::OnExit(int p_iNextFlowID)
	{
//		ECFLOG("================Flow OnExit(%d)===============\n", m_iID);

		// 종료
		if(IsInitialized())
		{
			// Resource 
			ReleaseResource();

			// Load Flag
			SetLoaded(FALSE);

			Terminate();

			// Init Flag
			SetInitialized(FALSE);

		}
	}
//---------------------------------------------------------------------------
	void CFlow::OnOverride(int p_iNextFlowID)
	{
		// 겹칩
	}
//---------------------------------------------------------------------------
	void CFlow::OnResume(int p_iNextFlowID)
	{
		// 재시작
	}
//---------------------------------------------------------------------------
	BOOL CFlow::OnMsgHandler(const MSG& p_kMsg)
	{
		return FALSE;
	}
//---------------------------------------------------------------------------
	bool CFlow::LostDevice(void* p_pvData)
	{
#ifdef DEBUG_FLOW
		DBG("Lost Device\n");
#endif // DEBUG_FLOW

		// 디바이스 손실되었을 때
		return true;
	}
//---------------------------------------------------------------------------
	bool CFlow::ResetDevice(bool p_bBeforeReset, void* p_pvData)
	{
#ifdef DEBUG_FLOW
		DBG("reset Device\n");
#endif // DEBUG_FLOW
		// 디바이스 리셋되었을 때
		// p_bBeforeReset - ture : 리셋 전 flase : 리셋 후
		return true;
	}
//---------------------------------------------------------------------------
	BOOL CFlow::Initialize() 
	{
		// 초기화
		NiDX9Renderer::GetRenderer()->SetStencilClear(0);
		return TRUE;
	}
//---------------------------------------------------------------------------
	void CFlow::Terminate()  
	{
		// 종료
	}
//---------------------------------------------------------------------------
	void CFlow::MainLoop()
	{
		if(!MeasureTime())	// MeasureTime
			return;

		GlobalUpdate();		// Update(전역) 

		if( !m_bInputLock )
			InputFrame();		// Input

		CullFrame();		// Cull

//		UpdateFrame();		// Update(지역)

		DrawFrame();        // Draw
	}
//---------------------------------------------------------------------------
	BOOL CFlow::LoadResource()
	{
		// 리소스로딩
		return TRUE;
	}
//---------------------------------------------------------------------------
	void CFlow::ReleaseResource()
	{
		// 리소스해제
	}
//---------------------------------------------------------------------------
	void CFlow::GlobalUpdate()
	{
		// Update Input
		g_pEngine->Update();

		CAMERA_ST._UpdateCamera();	

		if( g_pSoundMgr )
			g_pSoundMgr->Update( (float)g_fAccumTime );

		cEditBox::UpdateFocusEdit();
	}
//---------------------------------------------------------------------------
	BOOL CFlow::MeasureTime()
	{
		return g_pEngine->m_Frame.UpdateFrame();
	}
// ____________________________________________________________________________________
	void CFlow::InputFrame()
	{
		// 입력처리
	}
// ____________________________________________________________________________________
	void CFlow::UpdateFrame()
	{
		// 업데이트
	}
// ____________________________________________________________________________________
	void CFlow::CullFrame()
	{
		// 컬링
	}
// ____________________________________________________________________________________
	void CFlow::DrawFrame()
	{
		UpdateFrame();		         // Update(지역)
	
		// Draw
		if( BeginFrame() )				 // BeginScene
		{
			if( BeginRenderTarget() )	//
			{
				RenderBackScreenFrame();	// 2D 배경 UI

				RenderSceneFrame();		 // 3D Draw
				
				RenderScreenFrame();	 // 2D Draw
				
				RenderUIFrame();		 // UI Draw

				EndRenderTarget();

				EndFrame();						 // EndScene
				DisplayFrame();					 // Present
			}
		}
	}
// ____________________________________________________________________________________
	void CFlow::RenderSceneFrame()
	{
		// 3D 렌더, 월드, 캐릭터, 이팩트 등등 ...
	}
// ____________________________________________________________________________________
	void CFlow::RenderScreenFrame()
	{
		// 2D 렌더
	}
// ____________________________________________________________________________________
	void CFlow::RenderBackScreenFrame()
	{
		// 2D Screen
	}
// ____________________________________________________________________________________
	void CFlow::RenderUIFrame()
	{
		// UI 렌더
	}
// ____________________________________________________________________________________
	bool CFlow::BeginFrame()
	{
		// BeginScene
		return g_pEngine->BeginFrame();
		//return g_pEngine->StartRender( CEngine::PE_NONE );
	}
// ____________________________________________________________________________________
	bool CFlow::BeginRenderTarget()
	{
		// BeginTarget
		//return g_pEngine->BeginDefaultRenderTarget( NiRenderer::CLEAR_ALL );
		return g_pEngine->BeginUsingCurrentRenderTargetGroup( NiRenderer::CLEAR_ALL );
	}
// ____________________________________________________________________________________
	void CFlow::EndRenderTarget()
	{
		// EndTarget
		g_pEngine->EndRenderTarget();
	}
// ____________________________________________________________________________________
	void CFlow::EndFrame()
	{
		// EndScene
		g_pEngine->EndFrame();
	}
// ____________________________________________________________________________________
	void CFlow::DisplayFrame()
	{
		// Present
		g_pEngine->DisplayFrame();
	}

	void CFlow::ReservedChangeFlow(int p_iNextFlowID)
	{
	}

	void CFlow::ReservedPushFlow(int p_iNextFlowID)
	{
	}

	void CFlow::ReservedPopFlow()
	{
	}

	void  CFlow::SetInputLock()
	{
		m_bInputLock = TRUE;
	}

	void  CFlow::SetInputUnLock()
	{
		m_bInputLock = FALSE;
	}
}
// ____________________________________________________________________________________