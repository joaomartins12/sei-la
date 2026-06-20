
#pragma once

#include "ThreadProc.h"

class CThread
{
public:
	CThread(){}
public:
	static bool		GlobalInit();
	static void		GlobalShotDown();

public:
	bool			Init();
	void			Destroy();	

	CThreadProc*			GetProc() { return m_pThreadProc->GetProcInstance(); }

protected:	
	CsWaitableCollection		m_collection;
	CsThread< CThreadProc >*	m_pThreadProc;
};

extern CThread*	g_pThread;