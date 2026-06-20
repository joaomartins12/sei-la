
#pragma once 
#ifdef VERSION_TH
#define CUID_CHAT_LEN			36
#else
#define CUID_CHAT_LEN			32
#endif
class CsCuid_Chat
{
public:
	typedef	std::list< CsCuid_Chat* >						LIST;
	typedef	std::list< CsCuid_Chat* >::iterator				LIST_IT;
	typedef	std::list< CsCuid_Chat* >::const_iterator		LIST_CIT;
public:	
	struct sINFO
	{
		TCHAR			s_Name[ CUID_CHAT_LEN ];
	};

protected:
	sINFO			m_Info;
public:
	void			Delete();
	void			Init( sINFO* pSrcInfo );

	sINFO*			GetInfo(){ return &m_Info; }
	const TCHAR*	GetName(){ return m_Info.s_Name; } // [3/2/2016 hyun] Ãß°¡
};

