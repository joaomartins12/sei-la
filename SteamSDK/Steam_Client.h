#pragma once

#ifdef VERSION_STEAM
#include "..\DProject\Singleton.h"
#include "steam_api.h"
// #include "isteamgameserver.h"
// #include "isteamgameserverstats.h"

namespace Steam
{
	const int STEAM_DEFAULT_TOKEN_SIZE = 1024;

	struct UserData
	{	
		UserData() : m_eUserstate(k_EPersonaStateOffline)
		{
			clear();
		}
		void clear()
		{
			m_cSteamID.Clear();
			m_cGameID.Reset();
			m_eUserstate = k_EPersonaStateOffline;
			m_szUserName.clear();
			m_szGameLanguage.clear();
			memset(m_rgchToken, 0x00, sizeof(m_rgchToken));

			m_hAuthTicket = k_HAuthTicketInvalid;
			m_nTokenLen = 0;
		}

		CSteamID		m_cSteamID;
		CGameID			m_cGameID;

		EPersonaState	m_eUserstate;

		std::string		m_szUserName;
		std::string		m_szGameLanguage;

		HAuthTicket		m_hAuthTicket;
		char			m_rgchToken[STEAM_DEFAULT_TOKEN_SIZE];
		unsigned __int32 m_nTokenLen;		
	};	

	class CSteam_Client : public CSingleton< CSteam_Client >
	{
	public:
		CSteam_Client();
		~CSteam_Client();

		bool	InitSteam();		// 스팀 초기화
		bool	ReleaseSteam();		// 스팀을 종료한다.

		bool	GetUserData(UserData &userdata);// 스팀의 유저데이터를 얻는다.
		void	SteamCashChageWebOpen();		// 스팀 충전 페이지 오픈
		void	SteamCallbackUpdate();
		bool	IsOverlayEnable();

	private:
		// 유틸함수
		__int64 string_t_number(std::string val);
		std::string number_to_string(__int64 val);
		std::wstring number_to_wstring(__int64 val);
		bool token_to_string(const char* bytes, const int size, std::string& str_out);
		bool token_to_wstring(const char* bytes, const int size, std::wstring& str_out);

		STEAM_CALLBACK( CSteam_Client, OnInGameItemPurchase, MicroTxnAuthorizationResponse_t );	

	private:
		bool	__initialize_check;

		// SteamID for the local user on this client
		CSteamID m_SteamIDLocalUser;
	};

	//---------------------------------------------------------------------------
	// Singleton
	#define STEAM_ST		Steam::CSteam_Client::GetSingleton()
	#define STEAM_STPTR		Steam::CSteam_Client::GetSingletonPtr()
	//---------------------------------------------------------------------------
}
#endif