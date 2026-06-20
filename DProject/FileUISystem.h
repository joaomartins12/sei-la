#pragma once

class CFileUISystem : public COptionUIChat
{
public:
	CFileUISystem(void);
	~CFileUISystem(void);

	virtual	void Write(TiXmlElement* root);		// 쓰기
	virtual	bool Read(TiXmlElement* root);		// 읽기
	virtual	void InitDefault();					// 초기 설정.
};
