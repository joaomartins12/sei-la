
#pragma once

class CsC_ThreadProc_RefCheck
{
public:
	CsC_ThreadProc_RefCheck(){ m_pCS_ProcLoad = NULL; }
public:
	DWORD							ThreadProc( THREAD_PROC_ARG* pArg );

protected:
	CsCriticalSection*				m_pCS_ProcLoad;
public:
	void		SetCriticalSection_LoadProp( CsCriticalSection* pCS ){ m_pCS_ProcLoad = pCS; }
};