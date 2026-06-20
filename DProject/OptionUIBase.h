#pragma once

enum eOPTION_UI_VALUE_TYPE
{
	eOPTION_UI_POS_X,
	eOPTION_UI_POS_Y,
	eOPTION_UI_EXPAND_LEVEL,
	eOPTION_UI_SHOW_ETC,
};

struct ST_CHAT_OPTION 
{
	string		Name;			// UI 창의 이름
	INT			ExpandLevel;	// UI 창 확장 레벨
	CsPoint		Pos;			// UI 창의 위치
	bool		AddShow;		// UI 추가로 표시할 게 있는지	
	ST_CHAT_OPTION() : ExpandLevel(1), Pos(CsPoint::ZERO), AddShow(false) {}

	ST_CHAT_OPTION(const ST_CHAT_OPTION& rhs)
	{
		this->Name			= rhs.Name;
		this->ExpandLevel	= rhs.ExpandLevel;
		this->Pos			= rhs.Pos;
		this->AddShow		= rhs.AddShow;
	}	
	ST_CHAT_OPTION(const ST_CHAT_OPTION* rhs)
	{
		this->Name			= rhs->Name;
		this->ExpandLevel	= rhs->ExpandLevel;
		this->Pos			= rhs->Pos;
		this->AddShow		= rhs->AddShow;
	}	
	const ST_CHAT_OPTION& operator=(const ST_CHAT_OPTION& rhs)
	{
		this->Name			= rhs.Name;
		this->ExpandLevel	= rhs.ExpandLevel;
		this->Pos			= rhs.Pos;
		this->AddShow		= rhs.AddShow;
		return *this;
	}
};

struct ST_CHAT_BASE_OPTION
{
	ST_CHAT_BASE_OPTION()	{	
		for(int i=0; i<NS_CHAT::MAXCOUNT; ++i)
			On[i] = false;
	}
	bool	On[ NS_CHAT::MAXCOUNT ];	//	ChatContents의 변수값 bool	m_bChatEnable[ NS_CHAT::MAXCOUNT ]; save, load
};

class COptionUIBase
{
public:
	virtual	void	Write(TiXmlElement* root)=0;	// 저장
	virtual	bool	Read(TiXmlElement* root)=0;		// 로드 : 값이 없는 경우 0 적용하니 주의.
	virtual void	InitDefault()=0;			// 초기 설정.

protected:
	void			IntAttribute(TiXmlElement* pElement, char* name, int* iVal);	// 값 없을 시 0 을 받는다.
	void			BoolAttribute(TiXmlElement* pElement, char* name, bool* bVal, bool	DefaultValue=false);	// 값 없을 시 false 를 받는다.
public:
	COptionUIBase(void);
	~COptionUIBase(void);
};
