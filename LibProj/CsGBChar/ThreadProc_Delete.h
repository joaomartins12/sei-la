
#pragma once

typedef bool(*CALLBACK_DeleteUnit)( sTCUnit* pUnit );

class CsC_ThreadProc_Delete
{
public:
	CsC_ThreadProc_Delete(){ m_CallBackDelete = NULL; m_TimeSeqExit.SetDeltaTime( 3000 ); m_TimeSeqExit.SetEnableTime( false ); }
public:
	DWORD							ThreadProc( THREAD_PROC_ARG* pArg );

public:
	void							(CsC_ThreadProc_Delete::*fPushUnitDelete)( sTCUnit* pUnit );
public:
	void							PushUnit_NoThread( sTCUnit* pUnit ){ pUnit->s_pLoadedObject->PreDelete(); m_queueDelete.Put( pUnit ); _DeleteChar();  }
	void							PushUnit_Thread( sTCUnit* pUnit ){ pUnit->s_pLoadedObject->PreDelete(); m_queueDelete.Put( pUnit ); }

public:
	bool							IsEmpty(){ return m_queueDelete.Empty(); }
	

protected:
	bool							_IsEnableEnd(){ return ( m_queueDelete.Empty() == true ); }
	void							_CheckWaitDelete();
	void							_DeleteChar();

protected:
	CThreadSafeQueue< sTCUnit* >	m_queueDelete;
	std::queue< sTCUnit* >			m_queueWaitDelete;
	CsTimeSeq						m_TimeSeqExit;

	// ฤน้
protected:
	CALLBACK_DeleteUnit				m_CallBackDelete;

public:
	void		ResistCallBack_DeleteUnit( void* func ){ m_CallBackDelete = (CALLBACK_DeleteUnit)func; }
};