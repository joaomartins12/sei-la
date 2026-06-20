#pragma once

enum{
	XML_CHAT_MAIN=0,
	XML_CHAT_SYSTEM,
	XML_CHAT_BATTLELOG,
	XML_CHAT_CONTENTS,
};

class CPlayerUIOptionDocument
{
public:
	CPlayerUIOptionDocument(void);
	~CPlayerUIOptionDocument(void);

private:
	void		CreateOptionData();
	void		RemoveOptionData();

public:	// 지정 저장 함수
	void	SaveToPlayerUIRecord(TiXmlDocument*	pDoc, string parent);
	void	LoadToPlayerUIRecord(TiXmlDocument*	pDoc, string parent);

public:
	void			SetUIOption(int type, ST_CHAT_OPTION*	pThis);

	COptionUIChat*	GetUIChatClass(int type, bool bLoad);	// UICHAT
	ST_CHAT_OPTION*	GetUIOption(int type, bool bLoad);
	INT				GetUIOptionValue(int type, eOPTION_UI_VALUE_TYPE const OptID, bool	bLoad=true);

private:
	typedef		map<int, COptionUIChat*>					MAP_CAHT_UI;
	typedef		map<int, COptionUIChat*>::iterator			MAP_CAHT_UI_ITOR;

	MAP_CAHT_UI												m_OptionChatUI;
};
