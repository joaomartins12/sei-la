#pragma once

#include "DownEngineSDK.h"

struct sTHREAD_INPUT
{
	enum eTYPE{ INSTALL, UPDATE, DO_PACK,DOWNLOADED_INSTALL };	

	eTYPE		s_eType;	
	DWORD		s_dwSize;
	TCHAR		s_szURLPath[ 512 ];
	TCHAR		s_szDownFile[ 512 ];
	DWORD		s_dwPatchNum;
};

class CThreadProc
{
public:
	CThreadProc(void);
	~CThreadProc(void);

public:
	struct sINFO
	{
		sINFO(){ Reset(); }
		void	Reset(){ s_nTotalSize = 0; s_nProcSize = 0; }
		
		ULONGLONG	s_nTotalSize;
		ULONGLONG	s_nProcSize;
	};
private:
	sINFO				m_CurInfo;
	sINFO				m_TotalInfo;
	int					m_nTry;
	bool				m_bStopThread;	
	
public:
	void			DeleteQueue();
	DWORD			ThreadProc( THREAD_PROC_ARG* pArg );

	void			SetInternet( HINTERNET* pSession ) { m_hSession = pSession; }
	void			Add( sTHREAD_INPUT* pData ) { m_Queue.Put( pData ); }	

public:
	void			SetCurTotalSize( ULONGLONG nSize );
	void			SetAddCurProcSize( ULONGLONG nSize );
	void			SetCurProcSize( ULONGLONG nSize );
	void			GetCurInfo( sINFO* pDest );
	void			ResetCurInfo();

	void			SetTotalTotalSize( ULONGLONG nSize );
	void			SetAddTotalProcSize( ULONGLONG nSize );
	void			SetTotalProcSize( ULONGLONG nSize );
	void			GetTotalInfo( sINFO* pDest );
	void			ResetTotalInfo();	

	void			StopThread();
	bool			IsStopThread();

protected:	
	void			_DownLoad( sTHREAD_INPUT* pData );
	bool			_Unzip( sTHREAD_INPUT* pData );

	void			_DownLoadCheckFile( sTHREAD_INPUT* pData );
	bool			_ErrorCheck();

	BOOL			DeleteAllFolder( CString strPath );
	void			ResetError();
	void			ThrowError( int nErrorState );

protected:
	struct sPACKFILE
	{
		char		s_cPath[ MAX_PATH ];
	};

protected:
	//void			_CalFilePackList( std::list< sPACKFILE* >& listPackFile, TCHAR* szDir );
	bool			_DoPack( sTHREAD_INPUT* pTU );
	bool			__DoPack( sTHREAD_INPUT* pTU );
	// 파일 하나씩 팩킹 하는 함수
	bool			__DoPackOnlyOne( sTHREAD_INPUT* pTU );

protected:	
	HINTERNET*							m_hSession;
	CsCriticalSection					m_CS;
	CThreadSafeQueue< sTHREAD_INPUT* >	m_Queue;


private:
	int									m_nErrorState;
};
