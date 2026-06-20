// #pragma once
// 
// BEGIN_NAME_TABLE
// 
// struct sDigimonTalk
// {
// 	sDigimonTalk():s_dwParam(0),s_nType(0) {};
// 
// 	enum TP
// 	{
// 		TP_QUEST	=	0x00000001,
// 		TP_NPC		=	0x00000002,		 
// 		TP_DIGIMON	=	0x00000004,		 
// 		TP_SKILL	=	0x00000008,
// 	};
// 
// 	DWORD		s_dwParam;
// 	int			s_nType;
// 	std::wstring s_szText;
// 	std::wstring s_szList;
// };
// 
// struct sMessageTalk
// {
// 	enum eMESSAGE_TYPE
// 	{
// 		MT_BOX				= 1, 
// 		MT_SYSTEM			= 2,
// 		MT_CHAT				= 3,
// 		MT_NOTICE			= 4,
// 		MT_MONSTER_SKILL	= 5,	// 몬스터 스킬 사용 시 화면 중앙 토크( Monster_Skill 테이블 참조 )
// 	};
// 
// 	enum eTYPE
// 	{
// 		MT_OK			= 1,
// 		MT_OK_CANCEL	= 2,
// 		MT_CANCEL		= 3,
// 		MT_NOBUTTON		= 4,
// 	};
// 
// 	sMessageTalk():s_MsgType(MT_BOX), s_Type(MT_OK), s_dwLinkID(0) {};
// 
// 	DWORD			s_dwLinkID;
// 
// 	eMESSAGE_TYPE	s_MsgType;
// 	eTYPE			s_Type;
// 
// 	std::wstring 	s_TitleName;
// 	std::wstring 	s_Message;
// };
// 
// 
// struct sTooltips
// {
// 	std::wstring 	s_TitleName;
// 	std::wstring 	s_Message;
// };
// 
// END_NAME_TABLE
