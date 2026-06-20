
#pragma once 

// 검색해서 없애 줄거 없애 주자
#define CSLAST_ERROR

class CsLastError
{
public:
	CsLastError(){ Release(); m_nIntValue1 = m_nIntValue2 = m_nIntValue3 = m_nIntValue4 = m_nIntValue5 = m_nIntValue6 = m_nUValue = 0; }

public:
	enum eTYPE{ NONE, __ThreadLoad1, __ThreadLoad2, __CalAniConstant, __attack
	};

	void*			m_pValue1;
	void*			m_pValue2;
	void*			m_pValue3;
	void*			m_pValue4;

	int				m_nIntValue1;
	int				m_nIntValue2;
	int				m_nIntValue3;

	int				m_nIntValue4;
	int				m_nIntValue5;
	int				m_nIntValue6;
	
	int				m_nUValue;
	char			m_szChar[ MAX_PATH ];

	eTYPE			m_eType;

public:
	void			Release(){ m_eType = NONE; }
};

extern CsLastError		g_CsLastError;

class CsLog
{
public:
	CsLog(){ m_bInit = false; }
protected:
	bool			m_bInit;
	TCHAR			m_szLogName[ MAX_PATH ];

public:	
	void			Init( TCHAR const* szLogName, bool bSystemTime, bool bNewFile = true );
	void			Log( TCHAR const* szMsg, ... );

	void			DeleteFile();

	void			Reset(){ m_bInit = false; }

public:
	TCHAR*			GetLogName(){ return m_szLogName; }
};