#include "stdafx.h"
#include "Client_nProtect.h"

#ifdef DEF_CORE_NPROTECT

#pragma comment(lib,"NpGameLib.lib")
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"winmm.lib")

#ifdef DEF_CORE_NO_GAMEGUARD
#define NO_GAMEGUARD			// 게임 가드를 적용하지 않을 때 주석 제거
#endif

#ifdef VERSION_KOR
	#include "../../nProtect/Include/NPGameLib.h"
	#ifdef DEF_CORE_NPROTECT_TEST
		CNPGameLib *npgl= new CNPGameLib(L"DigiMonMTest");
	#else
		CNPGameLib *npgl= new CNPGameLib(L"DigiMonM");
	#endif
#elif VERSION_USA
	#include "../../nProtect_USA/Include/NPGameLib.h"

	#ifdef DEF_CORE_NPROTECT_TEST
		CNPGameLib *npgl= new CNPGameLib(L"DigiMonMUSTest");
	#else
		CNPGameLib *npgl= new CNPGameLib(L"DigiMonMUS");
	#endif
#elif VERSION_TH
	#include "../../nProtect_TH/Include/NPGameLib.h"

	#ifdef DEF_CORE_NPROTECT_TEST
		CNPGameLib *npgl= new CNPGameLib(L"DigimonMTHTest");
	#else
		CNPGameLib *npgl= new CNPGameLib(L"DigimonMTH");
	#endif
#elif VERSION_HK
	#include "../../nProtect_HK/Include/NPGameLib.h"
	CNPGameLib *npgl= new CNPGameLib(L"DigimonMHK");
#endif

bool g_bGameExit=false;

wchar_t g_szHackMsg[MAX_PATH]={0,};

UINT g_unUniqueIdx = 0;

BOOL nProtect_Init()
{
	if( !npgl )
		return FALSE;

	DWORD dwResult = npgl->Init();
	if (dwResult == NPGAMEMON_SUCCESS)
		return TRUE;

	// ‘6. 주요에러코드’를 참조하여 상황에 맞는 메시지를 출력해줍니다.
	switch (dwResult)
	{
	case NPGAMEMON_ERROR_EXIST:			_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "게임가드가 실행 중 입니다. 잠시 후나 재부팅 후에 다시 실행해보시기 바랍니다." ) );	break;
	case NPGAMEMON_ERROR_GAME_EXIST:	_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "게임이 중복 실행되었거나 게임가드가 이미 실행 중 입니다. 게임 종료후 다시 실행해보시기 바랍니다.") );		break;
	case NPGAMEMON_ERROR_INIT:			_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "게임가드 초기화 에러입니다. 재부팅 후 다시 실행해보거나 충돌할 수있는 다른 프로그램들을 종료한 후 실행해 보시기 바랍니다.") );		break;
	case NPGAMEMON_ERROR_AUTH_GAMEGUARD:
	case NPGAMEMON_ERROR_NFOUND_GG:
	case NPGAMEMON_ERROR_AUTH_INI:
	case NPGAMEMON_ERROR_NFOUND_INI:	_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "게임가드 파일이 없거나 변조되었습니다. 게임가드 셋업 파일을 설치해보시기 바랍니다." ));		break;
	case NPGAMEMON_ERROR_CRYPTOAPI:		_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "윈도우의 일부 시스템 파일이 손상되었습니다. 인터넷 익스플로러(IE)를\t다시 설치해보시기 바랍니다." ));		break;
	case NPGAMEMON_ERROR_EXECUTE:		_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "게임가드 실행에 실패했습니다. 게임가드 셋업 파일을 다시 설치해보시기 바랍니다." ));		break;
	case NPGAMEMON_ERROR_ILLEGAL_PRG:	_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "불법 프로그램이 발견되었습니다. 불필요한 프로그램을 종료한 후 다시 실행해보시기 바랍니다." ) );		break;
	case NPGMUP_ERROR_ABORT:			_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "게임가드 업데이트를 취소하셨습니다. 접속이 계속 되지 않을 경우 인터넷 및 개인 방화벽 설정을 조정해 보시기 바랍니다." ) );		break;
	case NPGMUP_ERROR_CONNECT:			_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "게임가드 업데이트 서버 접속에 실패하였습니다. 잠시 후 다시 접속하거나, 네트웍 상태를 점검해봅니다." ) );		break;
	case NPGAMEMON_ERROR_GAMEGUARD:		_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "게임가드 초기화 에러 또는 구버젼의 게임가드 파일입니다. 게임가드 셋업파일을 다시 설치하고 게임을 실행해봅니다." ) );		break;
	case NPGMUP_ERROR_PARAM:			_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "ini 파일이 없거나 변조되었습니다. 게임가드 셋업 파일을 설치하면 해결할 수 있습니다." ) );		break;
	case NPGMUP_ERROR_INIT:				_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "npgmup.des 초기화 에러입니다. 게임가드폴더를 삭제후 다시 게임실행을 해봅니다." ) );		break;
	case NPGMUP_ERROR_DOWNCFG:			_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "게임가드 업데이트 서버 접속에 실패하였습니다. 잠시 후 재시도 해보거나, 개인 방화벽이 있다면 설정을 조정해 보시기 바랍니다." ) );		break;
	case NPGMUP_ERROR_AUTH:				_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "게임가드 업데이트를 완료하지 못 했습니다. 바이러스 백신을 일시 중시시킨 후 재시도 해보시거나, PC 관리 프로그램을 사용하시면 설정을 조정해 보시기 바랍니다." ) );		break;
	case NPGAMEMON_ERROR_NPSCAN:		_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "바이러스 및 해킹툴 검사 모듈 로딩에 실패 했습니다. 메모리 부족이거나 바이러스에 의한 감염일 수 있습니다." ));		break;
	case NPGG_ERROR_COLLISION:			_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "게임가드와 충돌 프로그램이 발견되었습니다." ) );		break;
	default:							_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "게임가드 실행 중 에러가 발생하였습니다. 게임 폴더 안의 GameGuard폴더에 있는 *.erl 파일들을 DigimonM@inca.co.kr로 첨부하여 메일 보내주시기 바랍니다." ) );		break;
	}

	TCHAR msg[256]={0,};
	wsprintf(msg, TEXT("GameGuard Error : %lu"), dwResult);
	CsMessageBox_Title( MB_OK, msg, g_szHackMsg );
	memset( g_szHackMsg, 0, MAX_PATH);
	return FALSE;
}

BOOL nProtect_End()
{
	if( npgl )
	{
		delete npgl;
		npgl = NULL;

		return TRUE;
	}
	else 
		return FALSE;
}

BOOL nProtect_SetHwnd( HWND p_Hwnd)
{
	if( !npgl )
		return FALSE;

	npgl->SetHwnd( p_Hwnd );
	return TRUE;
}

// 반드시 문자열 끝에 종료(NULL) 를 포함시켜서 전달해야 합니다.
BOOL nProtect_SendID( LPCTSTR p_strID )
{
	if( !npgl )
		return FALSE;

	npgl->Send( p_strID );
	return TRUE;
}

BOOL nProtect_Check()
{
	if( g_bGameExit )
	{
		if( nProtect_End() )
		{
			net::stop();
			net::end();
		}		

		return FALSE;
	}

	if( !npgl )
		return FALSE;

	if (npgl->Check() != NPGAMEMON_SUCCESS)
	{
		g_bGameExit = true;
		_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "게임가드가 실행 중이 아닙니다." ) );
		cMessageBox::InsertList( L"GameGuard Error", g_szHackMsg, cPrintMsg::USE_MANUAL_MESSAGE_DISCONNECT, CsTalk_Message::MT_OK );
		memset( g_szHackMsg, 0, MAX_PATH);
		return FALSE;
	}
	return TRUE;
}

BOOL CSAuth3( UINT dwUniqueIdx, PBYTE pbPacket, DWORD dwPacketSize, DWORD dwServerNumber)
{
	if( !npgl )
		return FALSE;

	g_unUniqueIdx = dwUniqueIdx;

	npgl->Auth3( pbPacket, dwPacketSize, dwServerNumber );

	return TRUE;
}

BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg)
{
	BOOL bResult = TRUE;
	switch (dwMsg)
	{
	case NPGAMEMON_COMM_ERROR:
	case NPGAMEMON_COMM_CLOSE:		
		{
			_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "게임가드가 실행 중 입니다. 잠시 후나 재부팅 후에 다시 실행해보시기 바랍니다.(%lu)" ) );
			wsprintf(g_szHackMsg,g_szHackMsg,dwArg);
			bResult = FALSE; // 종료 코드		
		}
		break;
	case NPGAMEMON_INIT_ERROR:
		{
			_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "게임가드 초기화 에러 : %lu" ) );
			wsprintf(g_szHackMsg,g_szHackMsg,dwArg);
			bResult = FALSE; // 종료 코드
		}
		break;
	case NPGAMEMON_SPEEDHACK:
		{
			_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "스피드핵이 감지되었습니다." ) );
			bResult = FALSE; // 종료 코드
		}
		break;
	case NPGAMEMON_GAMEHACK_KILLED:
		{
			_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "게임핵이 발견되었습니다." ) );	
			bResult = FALSE; // 종료 코드
		}
		break;
	case NPGAMEMON_GAMEHACK_DETECT:
		{
			_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "게임핵이 발견되었습니다." ) );
			bResult = FALSE; // 종료 코드
		}
		break;
	case NPGAMEMON_GAMEHACK_DOUBT:
		{
			_tcscpy_s( g_szHackMsg, MAX_PATH, _LAN( "게임이나 게임가드가 변조되었습니다." ) );
			bResult = FALSE; // 종료 코드
			//게임핵으로 의심되는 프로그램이 실행 중 입니다. 혹은 게임이나 게임가드가 변조되었습니다. 
			//게임 종료 처리를 시작하고 종료 직전에 불필요한 프로그램을 종료하고 다시 게임을 해보라는 메시지를 출력해줍니다.
		}
		break;
	case NPGAMEMON_GAMEHACK_REPORT:
		{
			//게임핵이 발견되었지만 정상 동작합니다. 게임클라이언트
			//는 전달받은 데이터를 서버에 전송하고 정상동작 합니다.
			DWORD dwHackInfoSize = 0;
			LPBYTE pHackInfo = NULL;
			pHackInfo = npgl->GetHackInfo(&dwHackInfoSize); // C++ 일 경우.
			if (pHackInfo && dwHackInfoSize > 0)
			{
				// 아래 함수는 게임가드에서 제공하는 함수가 아닙니다.
				net::nProtect_BotDetect( g_unUniqueIdx, pHackInfo , dwHackInfoSize );
			}
		}
	case NPGAMEMON_CHECK_CSAUTH3:
		{
			PCSAuth3Data pCSAuth3 = (PCSAuth3Data)dwArg;
			net::nProtect_Send( g_unUniqueIdx, pCSAuth3->bPacket, pCSAuth3->dwPacketSize ,pCSAuth3->dwServerNumber );
		}
		break;
	}

	if(!bResult && !g_bGameExit)
	{
		g_bGameExit = true;
		cMessageBox::InsertList( L"GameGuard Error", g_szHackMsg, cPrintMsg::USE_MANUAL_MESSAGE_DISCONNECT, CsTalk_Message::MT_OK );
		memset( g_szHackMsg, 0, MAX_PATH);
	}

	// 콜백 함수의 메시지에 따라 게임을 계속 진행할 경우에는 return TRUE를, 게임을 종료하는 경우에는 return FALSE를 해줍니다.
	return bResult; // 계속 진행
}
#else
	BOOL nProtect_Init(){return TRUE;}
	BOOL nProtect_End(){return TRUE;}
	BOOL nProtect_SetHwnd( HWND p_Hwnd){return TRUE;}
	BOOL nProtect_SendID( LPCTSTR p_strID ){return TRUE;}
	BOOL nProtect_Check(){return TRUE;}
	BOOL CSAuth3(UINT unUniqueIdx, PBYTE pbPacket, DWORD dwPacketSize, DWORD dwServerNumber){return TRUE;}
	BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg){return TRUE;}
#endif
