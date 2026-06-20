//---------------------------------------------------------------------------
//
// 파일명 : IntroFlow.h
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
#include "Flow.h"
//---------------------------------------------------------------------------
namespace Flow
{
	class CIntroFlow : public CFlow
	{
	public:
		CIntroFlow(int p_iID);
		virtual ~CIntroFlow();

	public:
		// ************************
		// Lost, Reset Device
		// ************************
		virtual bool LostDevice(void* p_pvData);
		virtual bool ResetDevice(bool p_bBeforeReset, void* p_pvData);
		// ************************

	protected:
		// ************************
		// Init, Term
		// ************************
		virtual BOOL Initialize();
		virtual void Terminate();
		// ************************

		// ************************
		// Update, Cull, Render
		// ************************
		virtual void InputFrame();
		virtual void UpdateFrame();
		virtual void CullFrame();
		virtual void RenderSceneFrame();
		virtual void RenderScreenFrame();
		virtual void RenderUIFrame();
		// ************************
	};
}
//---------------------------------------------------------------------------