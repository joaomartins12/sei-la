
#pragma once 

#ifdef VERSION_TH
#define CUIDNAME_LEN		36
#else
#define CUIDNAME_LEN		32
#endif
class CsCuid
{
public:
	typedef	std::list< CsCuid* >				LIST;
	typedef	std::list< CsCuid* >::iterator		LIST_IT;

public:	
	enum eCUID_TYPE
	{
		CT_IN		= 1,		// 단어 자체가 포함되면 안됨
		MT_ONLY		= 2,		// 해당 아이디만 금지어
	};

	
	struct sINFO
	{
		TCHAR			s_Name[CUIDNAME_LEN];
		eCUID_TYPE		s_MsgType;		
	};

protected:
	sINFO			m_Info;
public:
	void			Delete();
	void			Init( sINFO* pSrcInfo );

	sINFO*			GetInfo(){ return &m_Info; }
};

