
#include "stdafx.h"
#include "cData_MonsterIcon.h"

#ifdef MONSTER_ICON

void cData_MonsterIcon::Delete()
{
	m_pMonIcon = 0;	
}

void cData_MonsterIcon::Init()
{
	m_pMonIcon = NiSourceTexture::Create( "Data/interface/Icon/Achieve_Icon.tga" );
}


#endif

