#pragma once

#ifndef _LAN
	#define _LAN( str )		__LanGetString_Thai( L##str )
#endif //_LAN

TCHAR*	__LanGetString_Thai( TCHAR* str );