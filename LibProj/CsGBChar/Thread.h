
#pragma once 



class CsC_Thread : public NiMemObject
{
public:
	CsC_Thread(){ m_pLoadProc = NULL; m_pDeleteProc = NULL; m_pRefCheckProc = NULL; }
	~CsC_Thread() {};

public:
	static void		GlobalInit();
	static void		GlobalShotDown();
	static void		SetRefThreadSuspend( bool bValue );

public:
	void			Init();
	void			Destroy();

	void			Reset();

	void			UseThread( bool bUseThread );
	void			_ThreadSuspend( bool bValue );

protected:
	CsWaitableCollection					m_Collection;
	CsThread< CsC_ThreadProc_Load >*		m_pLoadProc;
	CsThread< CsC_ThreadProc_Delete >*		m_pDeleteProc;
	CsThread< CsC_ThreadProc_RefCheck >*	m_pRefCheckProc;
	CsC_Thread_ResMng						m_ResMng;

//public:
//	CsThread< CsC_ThreadProc_Load >*		__GetLoadProc(){ return m_pLoadProc; }
//	CsThread< CsC_ThreadProc_Delete >*		__GetDelProc(){ return m_pDeleteProc; }
//	CsThread< CsC_ThreadProc_RefCheck >*	__GetRefCheckProc(){ return m_pRefCheckProc; }
//	CsWaitableCollection*					__GetCollection(){ return &m_Collection; }

	//===============================================================================================
	//		로드 쓰레드
	//===============================================================================================
protected:	
	void		(CsC_ThreadProc_Load::*fPushUnitLoad)( sTCUnit* pUnit );
	void		(CsC_ThreadProc_Delete::*fPushUnitDelete)( sTCUnit* pUnit );
	void		_ThreadSuspend(CsThreadBase*pThread, bool bValue );
public:
	void		LoadChar( sTCUnit* pUnit ){ (m_pLoadProc->GetProcInstance()->*fPushUnitLoad)( pUnit ); }	
	void		ResistCallBack_LoadFileTable( void* func ){ m_pLoadProc->GetProcInstance()->ResistCallBack_LoadFileTable( func ); }

	//===============================================================================================
	//		델리트 쓰레드
	//===============================================================================================
public:
	void		DeleteChar( sTCUnit* pUnit ){ (m_pDeleteProc->GetProcInstance()->*fPushUnitDelete)(pUnit); }
	void		ResistCallBack_DeleteUnit( void* func ){ m_pDeleteProc->GetProcInstance()->ResistCallBack_DeleteUnit( func ); }	

	//===============================================================================================
	//		레퍼런스 체크 쓰레드
	//===============================================================================================

	//===============================================================================================
	//		쓰레드 관련 리스소 매니져
	//===============================================================================================
public:
	CsC_Thread_ResMng*					GetResMng(){ return &m_ResMng; }
};

extern CsC_Thread*	g_pThread;