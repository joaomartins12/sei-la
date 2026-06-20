//---------------------------------------------------------------------------
//
// 파일명 : GameApp.h
// 작성일 : 
// 작성자 : 
// 설  명 :
//
//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
#include "Win32App.h"
//---------------------------------------------------------------------------
namespace App
{
	class CGameApp	: public CSingleton< CGameApp >, public CWin32App
	{
	public:
		CGameApp();
		virtual ~CGameApp();

	public:

	protected:
		virtual BOOL OnInitialize();		         // 초기화						
		virtual void OnIdle();				         // 루프
		virtual void OnIdleExtern();				 // 외부루프
		virtual void OnTerminate();                  // 종료
		virtual BOOL OnMsgHandler(const MSG& p_kMsg);// WndProc 메시지 

	public:
		// Lost, Reset Device
		static bool LostDevice(void* p_pvData);
		static bool ResetDevice(bool p_bBeforeReset, void* p_pvData);
	public:
		bool RecreateDevice(WORD wWidth, WORD wHeight, BYTE byBit, BYTE RefRate, bool bFullMode);

		void SetInitialStarting(bool p_bOn) { m_bInitialStarting = p_bOn; }
		bool GetInitialStarting() { return m_bInitialStarting; }

	private:
		bool m_bInitialStarting;

		float m_fCheckGameGuardTimer;	// 게임가드 업데이트 체크 timer(네트워크 메니저를 만들어서 관리해야 하지만 일단 여기에서 처리하도록)
	};
}

//---------------------------------------------------------------------------
// Singleton
#define GAMEAPP_ST		App::CGameApp::GetSingleton()
#define GAMEAPP_STPTR	App::CGameApp::GetSingletonPtr()
//---------------------------------------------------------------------------