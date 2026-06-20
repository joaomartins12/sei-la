//---------------------------------------------------------------------------
//
// 파일명 : FlowCmdQueue.h
// 작성일 : 
// 작성자 : 
// 설  명 :
//
//---------------------------------------------------------------------------
#pragma once

namespace Flow
{
	class CFlowCmdQueue
		: public NiMemObject
	{
	public:
		// CMDType
		typedef enum _CMDTYPE
		{
			CMD_PUSH,
			CMD_POP,
			CMD_CHANGE,
			CMD_CHANGE_POP_ALL,
			CMD_POP_ALL

		} CMDTYPE;

		// FlowCMD
		typedef struct _FLOWCMD
		{
			CMDTYPE eCmdType;
			float   fDelay;
			int     iID;
			
		} FLOWCMD;

	public:
		CFlowCmdQueue();
		~CFlowCmdQueue();

	public:
		// Flow Cmd
		void    PushFlow(int p_iID);
		void    ChangeFlow(int p_iID);
		void    PopFlow(int p_iID);
		void    PopAllFlow();
		void	ChangePopAllFlow(int p_iID);

		bool	IsFrontPushFlow(int p_iID) const;
		bool	IsFrontChange(int p_iID) const;
		bool	IsFrontChangePopAll(int p_iID) const;
		bool	IsFrontPop(int p_iID) const;
		bool	IsFrontPopAll() const;
		
		// Queue
		void    Pop();          
		FLOWCMD Front() const;		

		FLOWCMD GetAt();	// 값 참조와 함께 제거        
		int     GetSize() const;      
		BOOL    IsEmpty() const;      
		void    Clear();        

	private:
	private:
		typedef std::queue<FLOWCMD> STL_QUE_FLOWCMD;
		STL_QUE_FLOWCMD	m_que_FlowCMD;
	};
}
//---------------------------------------------------------------------------