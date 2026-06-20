#pragma once

class CFileUIContents : public COptionUIChat
{
public:
	CFileUIContents(void);
	~CFileUIContents(void);

	virtual	void Write(TiXmlElement* root);		// 쓰기
	virtual	bool Read(TiXmlElement* root);		// 읽기
	virtual	void InitDefault();					// 초기 설정.

	ST_CHAT_BASE_OPTION		m_bChatEnable;
};
