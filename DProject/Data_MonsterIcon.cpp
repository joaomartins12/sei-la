
#include "stdafx.h"
#include "Data_MonsterIcon.h"

void cData_MonsterIcon::Delete()
{
	m_mapComp.clear();
	m_listProc.clear();
	m_pMonsterIcon = 0;	
	m_pMonsterIcon2 = 0;
}

void cData_MonsterIcon::Init()
{
	m_pMonsterIcon = NiSourceTexture::Create( "Data/interface/Icon/Achieve_Icon.tga" );
	m_pMonsterIcon2 = NiSourceTexture::Create( "Data/interface/Icon/Achieve_Icon_02.tga" );
}

void cData_MonsterIcon::InitNetOff()
{
}
