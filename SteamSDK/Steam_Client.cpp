#include "stdafx.h"
#include "Steam_Client.h"

#ifdef VERSION_STEAM

#include <sstream>
#pragma comment(lib, "steam_api.lib")
#pragma comment(lib, "sdkencryptedappticket.lib")

namespace Steam
{
	extern "C" void __cdecl SteamAPIDebugTextHook( int nSeverity, const char *pchDebugText )
	{
		// if you're running in the debugger, only warnings (nSeverity >= 1) will be sent
		// if you add -debug_steamapi to the command-line, a lot of extra informational messages will also be sent
		::OutputDebugStringA( pchDebugText );
	}

	//-----------------------------------------------------------------------------
	// Purpose: Helper to display critical errors
	//-----------------------------------------------------------------------------
	int Alert( const char *lpCaption, const char *lpText )
	{
		return ::MessageBoxA( NULL, lpText, lpCaption, MB_OK );
	}


	CSteam_Client::CSteam_Client():__initialize_check(false)		
	{
	}

	CSteam_Client::~CSteam_Client()
	{

	}

	bool CSteam_Client::InitSteam()		// 스팀 초기화
	{
		if ( SteamAPI_RestartAppIfNecessary( k_uAppIdInvalid ) )
		{
			Alert( "Fatal Error", "SteamAPI_RestartAppIfNecessary() failed.\n" );
			return false;
		}

		if ( !SteamAPI_IsSteamRunning() )
		{
			Alert( "Fatal Error", "Steam Is Not Running (SteamAPI_IsSteamRunning() failed).\n" );
			return false;
		}

		if ( !SteamAPI_Init() )
		{
			Alert( "Fatal Error", "Steam must be running to play this game (SteamAPI_Init() failed).\n" );
			return false;
		}

		SteamClient()->SetWarningMessageHook( &SteamAPIDebugTextHook );

		if ( !SteamUser()->BLoggedOn() )
		{
			Alert( "Fatal Error", "Steam user must be logged in to play this game (SteamUser()->BLoggedOn() returned false).\n" );
			return false;	
		}

		SteamUtils()->SetOverlayNotificationPosition( ENotificationPosition::k_EPositionTopLeft );
		
		__initialize_check = true;
		return true;

	}

	void CSteam_Client::SteamCallbackUpdate()
	{
		SteamAPI_RunCallbacks();
	}

	bool CSteam_Client::ReleaseSteam()		// 스팀을 종료한다.
	{
		SteamAPI_Shutdown();
		return true;
	}

	bool CSteam_Client::GetUserData(UserData &userdata)// 스팀의 유저데이터를 얻는다.
	{
		userdata.clear();

		if (false == __initialize_check)
		{
			if ( !InitSteam() )
			{
				return false;
			}
		}

		userdata.m_cSteamID = SteamUser()->GetSteamID();

		userdata.m_szUserName = SteamFriends()->GetPersonaName();

		userdata.m_szGameLanguage = SteamApps()->GetCurrentGameLanguage();
		if (userdata.m_szGameLanguage.empty())
		{
			userdata.m_szGameLanguage = SteamApps()->GetAvailableGameLanguages();
		}

		CGameID app(SteamUtils()->GetAppID());
		userdata.m_cGameID = app;

		userdata.m_eUserstate = SteamFriends()->GetPersonaState();
		userdata.m_hAuthTicket = SteamUser()->GetAuthSessionTicket( userdata.m_rgchToken, sizeof( userdata.m_rgchToken ), &userdata.m_nTokenLen );

		return true;	
	}

	bool CSteam_Client::IsOverlayEnable()
	{
		SAFE_POINTER_RETVAL( SteamUtils(), false );
		return SteamUtils()->IsOverlayEnabled();
	}

	void CSteam_Client::SteamCashChageWebOpen()
	{
		if( !__initialize_check )
			return;

		if( SteamUtils()->IsOverlayEnabled() )
			SteamFriends()->ActivateGameOverlayToWebPage( "https://store.steampowered.com/steamaccount/addfunds" );
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	void CSteam_Client::OnInGameItemPurchase( MicroTxnAuthorizationResponse_t* pResult )
	{
		SAFE_POINTER_RET( pResult );
		
		if( pResult->m_unAppID == SteamUtils()->GetAppID()  )
		{
			SAFE_POINTER_RET( GAME_EVENT_STPTR );

			pair<uint8,uint64>	pairResult;
			pairResult.first = pResult->m_bAuthorized;
			pairResult.second = pResult->m_ulOrderID;
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::REQUEST_STEAM_CASHITEM_PURCHASE, &pairResult );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	__int64 CSteam_Client::string_t_number(std::string val)
	{
		__int64 ret = 0;	
		if (!val.empty())
		{
			std::stringstream convertstring(val);
			convertstring >> ret;
		}
		return ret;
	}

	std::string CSteam_Client::number_to_string(__int64 val)
	{
		std::stringstream convertstring;
		convertstring << val;
		return convertstring.str();
	}

	std::wstring CSteam_Client::number_to_wstring(__int64 val)
	{
		std::wstring temp;
		std::string ntos = number_to_string(val);
		return temp.assign(ntos.begin(), ntos.end());
	}

	bool CSteam_Client::token_to_string(const char* bytes, const int size, std::string& str_out)
	{
		str_out.clear();

		const char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
		char ch;

		for (int i = 0; i < size; ++i)
		{
			ch = *(bytes+i);
			str_out.append(&hex[(ch & 0xF0) >> 4], 1);
			str_out.append(&hex[ch & 0xF], 1);
		}

		return true;
	}

	bool CSteam_Client::token_to_wstring(const char* bytes, const int size, std::wstring& str_out)
	{
		str_out.clear();

		std::string temp;
		if (token_to_string(bytes, size, temp))
		{
			str_out.assign(temp.begin(), temp.end());
			return true;
		}
		return false;
	}

}

#endif