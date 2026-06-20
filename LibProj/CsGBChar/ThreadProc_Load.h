
#pragma once

typedef void (*CALLBACK_GlobalLoad)( void );


class CsC_ThreadProc_Load
{
public:
	CsC_ThreadProc_Load(){ m_TimeSeqExit.SetDeltaTime( 3000 ); m_TimeSeqExit.SetEnableTime( false ); }
public:
	DWORD							ThreadProc( THREAD_PROC_ARG* pArg );

public:
	void							PushUnit_NoThread( sTCUnit* pUnit );
	void							PushUnit_Thread( sTCUnit* pUnit );	
	
protected:
	bool							_IsEnableEnd(){ return m_queueLoad.Empty()&&m_queuePostLoad.Empty()&&m_queueWaitLoad.empty(); }
	void							_LoadChar();	
	void							_LoadUnit( sTCUnit* pUnit );
	void							_WaitQueue2LoadQueue();
	void							_PostWaitQueue();
public:
	bool							IsEmpty(){ return m_queueLoad.Empty()&&m_queuePostLoad.Empty(); }	

protected:
	std::queue< sTCUnit* >			m_queueWaitLoad;
	CThreadSafeQueue< sTCUnit* >	m_queueLoad;
	
	std::queue< sTCUnit* >			m_PostWaitqueue;
	CThreadSafeQueue< sTCUnit* >	m_queuePostLoad;	
	
	CsCriticalSection				m_CS_ProcLoad;	
	CsTimeSeq						m_TimeSeqExit;

public:
	CsCriticalSection*				GetCS_ProcLoad(){ return &m_CS_ProcLoad; }

	// ฤน้
protected:
	CALLBACK_GlobalLoad				m_CallBack_LoadFileTable;
public:
	void							ResistCallBack_LoadFileTable( void* func ){ m_CallBack_LoadFileTable = (CALLBACK_GlobalLoad)func; }
};