#pragma once

#define UI_WINDOW_X				"X"
#define UI_WINDOW_Y				"Y"
#define	UI_WINDOW_EXPAND_LEVEL	"ExLevel"
#define UI_SHOW_ETC				"ShowEtc"

class COptionUIChat : public COptionUIBase
{
public:
	COptionUIChat(void);
	virtual ~COptionUIChat(void);

public:
	virtual	void	Write(TiXmlElement* root)=0;	// 저장
	virtual	bool	Read(TiXmlElement* root)=0;		// 로드 : 값이 없는 경우 0 적용하니 주의.
	virtual void	InitDefault()=0;			// 초기 설정.

protected:	// 필수 함수 하위 함수는 무조건 사용한다.
	TiXmlElement*	SetCommonAttribute(TiXmlElement* root);		
	void			GetCommonAttribute(TiXmlElement* root);
	void			AddPath(string path);

public:
	typedef	vector<string>::iterator	VEC_STRING_ITER;
	typedef	vector<string>	VEC_STRING;

protected:
	ST_CHAT_OPTION	m_UiData;
	VEC_STRING		m_ClassPath;


public:
	ST_CHAT_OPTION*	GetChatOption();
	vector<string>	GetChatPath() const;

public:	
	const	char*	GetName()	const;
	void	SetChatOption(ST_CHAT_OPTION * pThis);
};
