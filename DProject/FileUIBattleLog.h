#pragma once

class CFileUIBattleLog : public COptionUIChat
{
public:
	CFileUIBattleLog();
	~CFileUIBattleLog(void);

	virtual	void Write(TiXmlElement* root);		// 쓰기
	virtual	bool Read(TiXmlElement* root);		// 읽기
	virtual	void InitDefault();			// 초기 설정.
};
