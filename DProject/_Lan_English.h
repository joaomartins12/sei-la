#pragma once

#ifndef _LAN
	#define _LAN( str )		__LanGetString_Eng( L##str )
#endif //_LAN

TCHAR*	__LanGetString_Eng( TCHAR* str );
