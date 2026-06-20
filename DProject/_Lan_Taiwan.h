#pragma once

#ifndef _LAN
	#define _LAN( str )		__LanGetString_Taiwan( L##str )
#endif //_LAN

TCHAR*	__LanGetString_Taiwan( TCHAR* str );