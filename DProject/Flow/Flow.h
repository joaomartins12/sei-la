//---------------------------------------------------------------------------
//
// 파일명 : Flow.h
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
#include "FlowState.h"
//---------------------------------------------------------------------------
namespace Flow
{
	class CFlow	: public IFlowState
	{
	public:
		// FlowID
		typedef enum _FLOW_IDS
		{
			FLW_NONE = -1,
			FLW_INTRO,			// 인트로 화면
			FLW_LOGO,			// 로고 화면
			FLW_LOGIN,			// 로그인 화면
			FLW_SERVERSEL,		// 서버 선택 화면
			FLW_SECURITY,		// 2차비밀번호 화면
			FLW_CHARSEL,		// 캐릭터 선택 화면
			FLW_CHARCREATE,		// 캐릭터 생성 화면
			FLW_DIGIMON_CREATE,	// 디지몬 생성 화면
			FLW_LOADING,		// 로딩 화면
			FLW_MAINGAME,		// 게임 화면
			FLW_DATSCENTER,		// Dats Center
			FLW_DATATRADECENTER,// 데이터 교환 화면. Dats Center와 같은 데이터 사용.
			FLW_BATTLE_REGISTER,// 배틀 등록 페이지
			FLW_BATTLE,			// 배틀 
			FLW_BATTLE_RESULT,	// 배틀 결과 페이지
			FLW_SERVER_RELOCATE,	// 서버 변경 페이지

			FLW_ID_COUNT

		} FLOW_IDS;

	public:
		CFlow(int p_iID);
		virtual ~CFlow();

	public:
		// ************************
		// IFlowState
		// ************************
		virtual void OnEnter();
		virtual void OnExit(int p_iNextFlowID);
		virtual void OnIdle();
		virtual void OnOverride(int p_iNextFlowID);
		virtual void OnResume(int p_iNextFlowID);
		// ************************

		// ************************
		// OnMsgHandler
		// ************************
		virtual BOOL OnMsgHandler(const MSG& p_kMsg);
		// ************************

		// ************************
		// Lost, Reset Device
		// ************************
		virtual bool LostDevice(void* p_pvData);
		virtual bool ResetDevice(bool p_bBeforeReset, void* p_pvData);
		// ************************

		// 변경될 플로우가 예약 瑛?때 호출함
		virtual void ReservedChangeFlow(int p_iNextFlowID);
		virtual void ReservedPushFlow(int p_iNextFlowID);
		virtual void ReservedPopFlow();

	protected:
		// ************************
		// Init, Term, Loop
		// ************************
		virtual BOOL Initialize();
		virtual void Terminate();
		virtual void MainLoop();
		// ************************

		// ************************
		// Resource
		// ************************
		virtual BOOL LoadResource();
		virtual void ReleaseResource();
		// ************************

		// ************************
		// Update, Cull, Render
		// ************************
		virtual void GlobalUpdate();          // Update(전역)
		virtual BOOL MeasureTime();           // Time
		virtual void InputFrame();            // Input     
		virtual void UpdateFrame();	          // Update(지역)
		virtual void CullFrame();             // Cull
        virtual void DrawFrame();             // Draw
		virtual bool BeginFrame();			  // BeginScene
		virtual bool BeginRenderTarget();     // BeginTarget
		virtual void RenderSceneFrame();	  // 3D Draw
		virtual void RenderScreenFrame();     // 2D Draw
		virtual void RenderBackScreenFrame(); // 2D Screen Draw
		virtual void RenderUIFrame();         // UI Draw
		virtual void EndRenderTarget();       // EndTarget
		virtual void EndFrame();			  // EndScene
		virtual void DisplayFrame();		  // Present
		// ************************

	public:
		// ************************
		// Flag Get
		// ************************
		BOOL  IsInitialized() const;
		BOOL  IsCreated()     const;
		//BOOL  IsLoaded()      const;
		BOOL  IsPaused()      const;
		int   GetID()         const;
		// ************************
	
		// ************************
		// Flag Set
		// ************************
		void  SetInitialized(BOOL p_bOn);
		void  SetCreated(BOOL p_bOn);
		void  SetLoaded(BOOL p_bOn);
		void  SetPaused(BOOL p_bOn);
		void  SetID(int p_iID);
		// ************************

		void  SetInputLock();
		void  SetInputUnLock();

	protected:
		int					m_iID;
		BOOL				m_bInitialized;
		BOOL				m_bCreated;
		BOOL				m_bPaused;
		BOOL				m_bLoaded;

	private:
		BOOL				m_bInputLock;
	};
}

#include "Flow.inl"
//---------------------------------------------------------------------------