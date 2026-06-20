//---------------------------------------------------------------------------
//
// 파일명 : FlowStack.cpp
// 작성일 : 
// 작성자 : 
// 설  명 :
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "FlowStack.h"
#include "Flow.h"
//---------------------------------------------------------------------------

namespace Flow
{
	CFlowStack::CFlowStack()
	:m_stk_FlowStack()
	{
	}
//---------------------------------------------------------------------------
	CFlowStack::~CFlowStack()
	{
	}
//---------------------------------------------------------------------------
	BOOL CFlowStack::IsExistID(int p_iID) const
	{
		int iSize = GetSize();

		for(int i = 0; i < iSize; ++i)
		{
			int iID = GetPosAt(i);

			if(iID == p_iID)
				return TRUE;
		}

		return FALSE;
	}
//---------------------------------------------------------------------------
	BOOL CFlowStack::Push(int p_iID)
	{
		if(IsExistID(p_iID))
			return FALSE;

		m_stk_FlowStack.push(p_iID);

		return TRUE;
	}
//---------------------------------------------------------------------------
	int CFlowStack::Top()
	{
		return m_stk_FlowStack.top();
	}
//---------------------------------------------------------------------------
	void CFlowStack::Pop()
	{
		m_stk_FlowStack.pop();
	}
//---------------------------------------------------------------------------
	int CFlowStack::GetSize() const
	{
		return static_cast<int>(m_stk_FlowStack.size());
	}
//---------------------------------------------------------------------------
	BOOL CFlowStack::IsEmpty() const
	{
		return m_stk_FlowStack.empty();
	}
//---------------------------------------------------------------------------
	void CFlowStack::Clear()
	{
		while(!IsEmpty())
		{
			Pop();
		}
	}
//---------------------------------------------------------------------------
	int CFlowStack::GetTopAt() const
	{
		int iSize = GetSize();

		if(iSize == 0)
			return CFlow::FLW_NONE;

//#if (_MSC_VER == 1500) //VC9.0
		return m_stk_FlowStack._Get_container()[iSize - 1];
//#else
//		return m_stk_FlowStack.c[iSize - 1];
//#endif
	}
//---------------------------------------------------------------------------
	int CFlowStack::GetPosAt(int p_iPos) const
	{
		assert(GetSize() > p_iPos && "GetPosAt Failed!");

//#if (_MSC_VER == 1500) //VC9.0
		return m_stk_FlowStack._Get_container()[p_iPos];		
//#else
//		return m_stk_FlowStack.c[p_iPos];
//#endif
	}
}
//---------------------------------------------------------------------------