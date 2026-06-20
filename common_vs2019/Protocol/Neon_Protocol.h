#include "Neon_Define.h"



/************************************************************************/
/* Àü±¤ÆÇ Ãâ·Â															*/
/************************************************************************/

class GS2C_NOTIFY_Neon_Display
{
public:
	GS2C_NOTIFY_Neon_Display(): nProtocol(pGameTwo::NeonDisplay)
	{}

	WORD GetProtocol(){return nProtocol;}

public:
	WORD nProtocol;
	
	n2 nNeonType;
	std::string strTamerName;
	n4 nType1;
	n4 nType2;
};