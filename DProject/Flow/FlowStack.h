//---------------------------------------------------------------------------
//
// 파일명 : FlowStack.h
// 작성일 : 
// 작성자 : 
// 설  명 :
//
//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------

namespace Flow
{
	class CFlowStack
		: public NiMemObject
	{
	public:
		CFlowStack();
		~CFlowStack();

	public:
		// ID 존재 확인
		BOOL IsExistID(int p_iID) const;

		// Stack
		BOOL Push(int p_iID);
		int  Top();					 
		void Pop();
		int  GetTopAt() const;			  // 값만 참조, 제거하지 않음			  
		int  GetPosAt(int p_iPos) const;  // 값만 참조, 제거하지 않음
		int  GetSize() const;				  
		BOOL IsEmpty() const;				  
		void Clear();				  

	private:
		typedef std::stack<int, std::vector<int> > STL_STK_FLOWSTACK;
		STL_STK_FLOWSTACK m_stk_FlowStack;
	};
}
//---------------------------------------------------------------------------