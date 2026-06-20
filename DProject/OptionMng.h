#pragma once

enum
{
	eOPT_UI=0,
	eOPT_RESULT,
	eOTP_MAX,
};

#define OPTIONFILE_NAME	L"OPTION.info"

class COptionMng : public CSingleton< COptionMng >
{	
public:
	COptionMng(void);
	virtual ~COptionMng(void);
	bool	GetCharacterPath(TCHAR*	name);

public:		// 전체 저장 함수.
	void	LoadMachineOption();	// 전역 데이터 로드
	void	LoadCharOption(wstring	wStr);		// 케릭터 데이터 로드

	void	SaveMachineOption();	// 전역 데이터 저장
	void	SaveCharOption(wstring	wStr);		// 케릭터 데이터 저장

private:
	CPlayerUIOptionDocument			m_PlayerUI;
public:
	/* 필수 사용! 현재 참조할 UI위치를 얻는다.*/
	COptionUIChat*		LoadPlayerUIChatClass(int type, bool bLoad=true);
	ST_CHAT_OPTION*		LoadPlayerUIOption(int type, bool bLoad=true);
	void				SavePlayerUIOption(int type, ST_CHAT_OPTION* pThis);

public:
	INT					GetPlayerUIOptionValue(int type, eOPTION_UI_VALUE_TYPE const OptID, bool bLoad=true);
	ST_CHAT_OPTION*		GetPlayerUIOption(int type, bool bLoad=true);
};

#define	OPTIONMNG_PTR	COptionMng::GetSingletonPtr()