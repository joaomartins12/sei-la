#include "stdafx.h"
#include "Data_TamerIcon.h"

cData_TamerIcon::cData_TamerIcon():m_pMatchIcon(NULL)//,m_nDispTitle(0)
{

}

cData_TamerIcon::~cData_TamerIcon()
{
	Delete();
}

void cData_TamerIcon::Delete()
{
	m_pMatchIcon = 0;
	//m_pTamerIcon = 0;	
}

void cData_TamerIcon::Init()
{
	//마스터즈매칭 팀 아이콘
	m_pMatchIcon = NiSourceTexture::Create( "Data/interface/MastersMatch/MastersMatch_TeamIcon.tga" );
// 	m_pTamerIcon = NiSourceTexture::Create( "Data/interface/Icon/Tamer_Icon.tga" );
}

void cData_TamerIcon::InitNetOff()
{
	//마스터즈매칭 팀 아이콘
	m_pMatchIcon = NiSourceTexture::Create( "Data/interface/MastersMatch/MastersMatch_TeamIcon.tga" );
// 	m_pTamerIcon = NiSourceTexture::Create( "Data/interface/Icon/Tamer_Icon.tga" );

}

