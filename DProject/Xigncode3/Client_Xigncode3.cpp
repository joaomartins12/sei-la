#include "stdafx.h"
#include "Client_Xigncode3.h"

#ifdef SDM_DEF_XIGNCODE3_20181107
#include <strsafe.h>

#define XIGNCODE_LICENSE _T("mTFcpcybtNtq")

#ifdef _GIVE 
	#ifdef VERSION_QA
		#define XIGNCODE_PATH _T("XIGNCODE_Test")
	#else
 		#define XIGNCODE_PATH _T("XIGNCODE")
	#endif
#else
	#define XIGNCODE_PATH _T("XIGNCODE_Debug")
#endif


bool XignCode_Initialize()
{
	if( X_TRUE != ZCWAVE_SysInit() )
	{
		ULONG uError = GetLastError();
		WCHAR wsMsg[255] ={0};
		swprintf_s( wsMsg, 255, _LAN("비정상적인 게임 수행으로 인해 게임을 종료합니다.\n\t\tCODE = %08X"), uError );
		MessageBoxW( NULL, wsMsg, L"Security Alert", MB_OK );
		return false;//에러
	}

	if( X_TRUE != ZCWAVE_SysEnter( XIGNCODE_LICENSE, XIGNCODE_PATH, 0 ) )
	{
		ULONG uError = GetLastError();
		WCHAR wsMsg[255] ={0};
		swprintf_s( wsMsg, 255, _LAN("비정상적인 게임 수행으로 인해 게임을 종료합니다.\n\t\tCODE = %08X"), uError );
		MessageBoxW( NULL, wsMsg, L"Security Alert", MB_OK );
		ZCWAVE_SysCleanup();
		return false;
	}

// 	if( X_TRUE != ZCWAVE_RegisterCallback( Client_OnHackDetected, NULL ) )
// 	{
// 		ULONG uError = GetLastError();
// 		WCHAR wsMsg[255] ={0};
// 		swprintf_s( wsMsg, 255, _LAN("비정상적인 게임 수행으로 인해 게임을 종료합니다.\n\t\tCODE = %08X"), uError );
// 		MessageBoxW( NULL, wsMsg, L"Security Alert", MB_OK );
// 		XignCode_UnInitialize();
// 		return false;
// 	}

	if( X_TRUE != ZCWAVE_SendCommand( ZCMD_SET_USER_INFO_CALLBACKW, Client_UserInfoCallback, NULL ) )
	{
		ULONG uError = GetLastError();
		WCHAR wsMsg[255] ={0};
		swprintf_s( wsMsg, 255, _LAN("비정상적인 게임 수행으로 인해 게임을 종료합니다.\n\t\tCODE = %08X"), uError );
		MessageBoxW( NULL, wsMsg, L"Security Alert", MB_OK );
		XignCode_UnInitialize();
		return false;
	}

	return true;
}

bool XignCode_UnInitialize()
{
	ZCWAVE_SysExit();
	ZCWAVE_SysCleanup();

	return true;
}

xstatus XCALL Client_UserInfoCallback(  xuint iid, xwstr buffer, xsize size, xpvoid context )
{
	BHPRT( "Callback Xigncode To Client_UserInfoCallback (%d)", iid );
	if( !GLOBALDATA_ST.GetAccountID().empty() )	
	{
		if( iid == XUID_USER_ACCOUNT )
		{
			BHPRT( " Xigncode To Client_UserInfoCallback - XUID_USER_ACCOUNT" );
			StringCbCopyW( buffer, size, GLOBALDATA_ST.GetAccountIDW().c_str() );
			return ZCWAVE_OK;
		}
		else
			return ZCWAVE_E_USER_INFO_ACCESS_DENIED;
	}

	return ZCWAVE_E_USER_INFO_IS_NOT_READY;
}

// XignCode에서 핵킹을 감지하면 날라오는 콜백함수
// 핵킹이 감지되면 처리 해줘야 할 것,,
xbool XCALL Client_OnHackDetected(ULONG code, ULONG wp, PVOID lp, PVOID context)
{
	if( wp )
	{
		xwch path[X_MAX_PATH];
		ZCWAVE_SendCommand( ZCMD_SR_GET_PATH, wp, path, (xulong)sizeof(path) );
		xwch ext[X_MAX_PATH];
		ZCWAVE_SendCommand( ZCMD_SR_GET_EXTRA_INFO, wp, ext, (xulong)sizeof(ext) );
		xulong major;
		ZCWAVE_SendCommand( ZCMD_SR_GET_MAJOR_CODE, wp, &major );

		BHPRT( " Xigncode To Client_OnHackDetected" );
	}

	cMessageBox::InsertList( L"XIGNCODE", _LAN( "게임핵이 발견되었습니다." ), cPrintMsg::USE_MANUAL_MESSAGE_DISCONNECT, CsTalk_Message::MT_OK );
	return X_TRUE;
}

bool XignCode_ProbeEx( xpcch ReceivedPacket,xsize ReplySize, xpvoid context )
{
	BHPRT( "Recv - XignCode_ProbeEx(Size : %d)", ReplySize );
	if( X_TRUE != ZCWAVE_ProbeEx( ReceivedPacket, ReplySize, OnProbeComplete, context ) )
	{
		BHPRT( "Server To Xigncode - XignCode_ProbeEx : False" );
		//ECERROR( "XIGNCODE Error :: ZCWAVE_ProbeCodeBoxEx - %08X", GetLastError() );
		return false;
	}
	BHPRT( " Recv - XignCode_ProbeEx : True");
	return true;
}

void XCALL OnProbeComplete(xhandle CodeBox, CPACKETDATA RecevidePacket,PACKETDATA Buffer, SIZE_T BufferSize, xpvoid Context)
{
	net::Xigncode_SendPacket( Buffer, BufferSize );	
	BHPRT( "Xigncode =====>> Server - OnProbeComplete(BufferSize : %d)", BufferSize );
}
#endif