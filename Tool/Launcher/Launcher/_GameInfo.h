#pragma once

enum eGameType 
{ 
	GAME_DIGIMONMASTERS = 0, 
	GAME_DIGIMONRPG, 
	GAME_NOIR, 
	GAME_END 
};

enum eServerType 
{ 
	ST_REAL, 
	ST_BETA, 
	ST_ALPH, 
	ST_END 
};

class cGameInfo
{
public:
	cGameInfo(); 
	~cGameInfo() {};
public:
	struct sGame
	{	
		sGame() { s_bPack = false; }

		TCHAR	s_szPatchInfo[ MAX_PATH ];			// 패치 인포 경로및 파일명	
		TCHAR	s_szGameDirectory[ MAX_PATH ];			// 레지스트리에 등록된 게임 Path
		TCHAR	s_szGameFileName[ MAX_PATH ];			// 실행 파일명
		TCHAR	s_szRegistryPath[ MAX_PATH ];			// 레지스트리에 기록될 주소
		TCHAR	s_szInstallerName[ MAX_PATH ];		// 게임 설치 파일명	
		bool	s_bPack;								// 패킹 사용게임인가?	

		// Config.ini 파일이 있으면 셋팅
		// 없을 경우 디폴트로 셋팅 하자.
		TCHAR	s_szServerName[ MAX_PATH ];			// http 접속 주소
		TCHAR	s_szInstallServer[ MAX_PATH ];		// 인스톨러 접속 주소	
		TCHAR	s_szInstallURL[ MAX_PATH ];			// 수동설치 클라이언트 다운로드 URL
		TCHAR	s_szHomePageUrl[ MAX_PATH ];		// 게임 홈페이지 주소
		TCHAR	s_szPatchPageUrl[ MAX_PATH ];		// 게임 업데이트페이지 주소
		TCHAR	s_szMainUrl[ MAX_PATH ];			// 우상단 큰 웹페이지 주소
		TCHAR	s_szSubUrl[ MAX_PATH ];				// 좌하단 작은 광고 웹페이지 주소	

	};

public:
	bool		Init( TCHAR * szParm, sConfiguration const& sConfigInfo );
	sGame*		GetGameInfo() { return &m_GameInfo[ m_eGameType ]; }
	eGameType	GetGameType() { return m_eGameType; }
	eServerType GetServerType() { return m_eServerType; } 	
	bool		IsUserType( TCHAR* pUserType ) const;
	
	TCHAR*		GetID() { return m_szID; } 
	TCHAR*		GetKey() { return m_szKey; } 
	TCHAR*		GetUT() { return m_szUsrType; } 
	TCHAR*		GetST() { return m_szSrvType; }
 
	void		SetGameType( eGameType eType ) { m_eGameType = eType; }
	
private:
	eGameType		m_eGameType;
	sGame			m_GameInfo[ GAME_DIGIMONRPG ];	
	eServerType		m_eServerType;

private:	
	TCHAR		m_szID[ 64 ];
	TCHAR		m_szKey[ 128 ];
	TCHAR		m_szUsrType[ 8 ];
	TCHAR		m_szSrvType[ 8 ];	
};

extern cGameInfo*	g_GameInfo;


