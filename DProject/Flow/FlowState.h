//---------------------------------------------------------------------------
//
// 파일명 : FlowState.h
// 작성일 : 
// 작성자 : 
// 설  명 : 
//
//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------

namespace Flow
{
	class IFlowState
		: public NiMemObject
	{
	public:
		virtual ~IFlowState() {}
		virtual void OnEnter()    = 0;
		virtual void OnExit(int p_iNextFlowID)     = 0;
		virtual void OnIdle()     = 0;
		virtual void OnOverride(int p_iNextFlowID) = 0;
		virtual void OnResume(int p_iNextFlowID)   = 0;
	};
}
//---------------------------------------------------------------------------