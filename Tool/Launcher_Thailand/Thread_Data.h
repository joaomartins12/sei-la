
#pragma once

struct sTHREAD_INPUT
{
	enum eTYPE{ UNZIP, URL_DOWN, DO_PACK, };
	enum eVERSION_URL{ VU_VERSION = 9000, VU_DOWN, };

	sTHREAD_INPUT():s_nVersion(0),s_dwPatchNum(0)
	{};

	eTYPE		s_eType;
	int			s_nVersion;
	CString		s_szURLPath;
	CString		s_szDownPath;
	DWORD		s_dwPatchNum;
};

class CThread_Data
{
public:
	CThread_Data();
	~CThread_Data();

	void			DeleteQueue();	

	DWORD			ThreadProc( THREAD_PROC_ARG* pArg );
	void			Add( sTHREAD_INPUT* pTU );
	void			SetHInternet( HINTERNET* pHInternet ){ m_hSession = pHInternet; }

protected:
	bool			_Unzip( sTHREAD_INPUT* pTU );

protected:
	void			_URLDown( sTHREAD_INPUT* pTU );

protected:
	struct sPACKFILE
	{
		char		s_cPath[ MAX_PATH ];
	};

protected:
	void			_DoPack( sTHREAD_INPUT* pTU );
	void			__DoPackOnlyOne( sTHREAD_INPUT* pTU );
	void			_CheckError( sTHREAD_INPUT* pTU );

public:
	struct sINFO
	{
		sINFO(){ s_nVersion = U_WAIT; Reset(); }
		void	Reset(){ s_nTotalSize = 0; s_nProcSize = 0; }

		int		s_nVersion;
		ULONG	s_nTotalSize;
		ULONG	s_nProcSize;
	};
private:
	sINFO				__m_CurInfo;

protected:	
	HINTERNET*							m_hSession;
	CsCriticalSection					m_CS;
	CThreadSafeQueue< sTHREAD_INPUT* >	m_Queue;


protected:
	void			SetTotalSize( ULONG nSize );
	void			SetProcSize( ULONG nSize );

public:
	void			ResetInfo();

	void			SetVersion( int nVersion );
	void			GetInfo( sINFO* pDest );
	int				GetVersion();

private:
	void			ResetError();
	void			ThrowError( int nErrorState );
	int				m_nErrorState;
};