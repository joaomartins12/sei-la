#pragma once

#ifndef __NPROTECT_H_
#define __NPROTECT_H_


BOOL nProtect_Init();
BOOL nProtect_End();
BOOL nProtect_SetHwnd( HWND p_Hwnd);
BOOL nProtect_SendID( LPCTSTR p_strID );
BOOL nProtect_Check();
BOOL CSAuth3(UINT unUniqueIdx, PBYTE pbPacket, DWORD dwPacketSize, DWORD dwServerNumber);
BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg);

#endif //__NPROTECT_H_