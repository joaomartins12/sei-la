
#pragma once 

#define LOCALIZING_STR_LEN		64
#define BUTTON_BASE_ID			1000
#define CHECK_BASE_ID			2000
#define WINDOW_BASE_ID			3000

#define Str64_BASE_ID			10000
#define Str256_BASE_ID			11000
#define Str512_BASE_ID			12000
 
class CsLocalizing
{
public:
	typedef	std::map< DWORD, CsLocalizing* >			MAP;
	typedef	std::map< DWORD, CsLocalizing* >::iterator	MAP_IT;
	typedef	std::list< CsLocalizing* >					LIST;
	typedef	std::list< CsLocalizing* >::iterator		LIST_IT;

public:
	struct sINFO
	{
		std::wstring	s_szKor;
		std::wstring	s_szEng;
		std::wstring	s_szThai;
	};
protected:
	sINFO			m_Info;

public:
	void			Delete(){}
	void			Init( TCHAR* szKor, TCHAR* szEng, wchar_t* szThai ){ m_Info.s_szKor = szKor; m_Info.s_szEng = szEng; m_Info.s_szThai = szThai; }

	sINFO*			GetInfo(){ return &m_Info; }
};

class CsStr64
{
public:
	typedef	std::map< DWORD, CsStr64* >			MAP;
	typedef	std::map< DWORD, CsStr64* >::iterator	MAP_IT;

public:
	struct sINFO
	{
		DWORD		s_nID;
		TCHAR		s_szString[ 64 ];
	};
protected:
	sINFO			m_Info;

public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};

class CsStr256
{
public:
	typedef	std::map< DWORD, CsStr256* >			MAP;
	typedef	std::map< DWORD, CsStr256* >::iterator	MAP_IT;

public:
	struct sINFO
	{
		DWORD		s_nID;
		TCHAR		s_szString[ 256 ];
	};
protected:
	sINFO			m_Info;

public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};

class CsStr512
{
public:
	typedef	std::map< DWORD, CsStr512* >			MAP;
	typedef	std::map< DWORD, CsStr512* >::iterator	MAP_IT;

public:
	struct sINFO
	{
		DWORD		s_nID;
		TCHAR		s_szString[ 512 ];
	};
protected:
	sINFO			m_Info;

public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};

