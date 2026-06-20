
#pragma once 

#define	FT_TALK_MSG_TITLE_LEN	16
#define FT_TALK_MSG_BODY_LEN	256

class CsTalk_Message
{
public:
	typedef	std::map< DWORD, CsTalk_Message* >				MAP;
	typedef	std::map< DWORD, CsTalk_Message* >::iterator	MAP_IT;
public:	
	enum eMESSAGE_TYPE
	{
		MT_BOX				= 1, 
		MT_SYSTEM			= 2,
		MT_CHAT				= 3,
		MT_NOTICE			= 4,
		MT_MONSTER_SKILL	= 5,	// 몬스터 스킬 사용 시 화면 중앙 토크( Monster_Skill 테이블 참조 )
	};

	enum eTYPE
	{
		MT_OK			= 1,
		MT_OK_CANCEL	= 2,
		MT_CANCEL		= 3,
		MT_NOBUTTON		= 4,
	};


	struct sINFO
	{
		DWORD			s_dwID;
		eMESSAGE_TYPE	s_MsgType;
		eTYPE			s_Type;

		TCHAR			s_TitleName[ FT_TALK_MSG_TITLE_LEN ];
		TCHAR			s_Message[ FT_TALK_MSG_BODY_LEN ];
		DWORD			s_dwLinkID;
	};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};

