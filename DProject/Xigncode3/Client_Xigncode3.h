#pragma once

#ifndef __XIGNCODE_H_
#define __XIGNCODE_H_

#ifdef SDM_DEF_XIGNCODE3_20181107
	#include "zwave_sdk_client.h"

	static xstatus XCALL Client_UserInfoCallback(  xuint iid, xwstr buffer, xsize size, xpvoid context );
	static void XCALL OnProbeComplete(xhandle CodeBox, CPACKETDATA RecevidePacket,PACKETDATA Buffer, SIZE_T BufferSize, xpvoid Context);

	bool XignCode_Initialize();
	bool XignCode_UnInitialize();
	bool XignCode_ProbeEx( xpcch ReceivedPacket,xsize ReplySize, xpvoid context );
	xbool XCALL Client_OnHackDetected(ULONG code, ULONG wp, PVOID lp, PVOID context);
#endif

#endif //__XIGNCODE_H_