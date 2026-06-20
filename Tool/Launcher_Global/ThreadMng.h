
#pragma once

class CThread
{
public:
	CThread(){ m_pUnzip = NULL; }
public:
	static void		GlobalInit();
	static void		GlobalShotDown();

public:
	void			Init();
	void			Destroy();

	void			Add( sTHREAD_INPUT* pTU ){ m_pUnzip->GetProcInstance()->Add( pTU ); }
	CThread_Data*	Get(){ return m_pUnzip->GetProcInstance(); }

protected:
	CsWaitableCollection				m_Collection;
	CsThread< CThread_Data >*			m_pUnzip;
};

extern CThread*	g_pThread;