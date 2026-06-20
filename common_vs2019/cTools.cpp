

#include "cTools.h"

#include "nlib/cProfile.h"

#if 0 // unused
n4 DM::nCountryCode = korea;


void DM::Load(void)
{
	cProfile x("server.ini");

	char szCountry[333] = "서비스 국가명이 확인되지 아니하였습니다.";
	nBase::strcpy(szCountry, x.GetStr("common", "country"));

	if(_stricmp(szCountry, "korea") == 0)
	{
		DM::nCountryCode = korea;
	}
	else if(_stricmp(szCountry, "usa") == 0)
	{
		DM::nCountryCode = usa;
	}
	else if(_stricmp(szCountry, "thailand") == 0)
	{
		DM::nCountryCode = thailand;
	}
	else if(_stricmp(szCountry, "hongkong") == 0)
	{
		DM::nCountryCode = thailand;
	}
	else if(_stricmp(szCountry, "taiwan") == 0)
	{
		DM::nCountryCode = thailand;
	}
	else
	{
		DM::nCountryCode = korea;
		nBase::strcpy(szCountry, "korea (설정되어 있지 않음, 기본값:korea)");
	}

	LOG2("");
	LOG2("\t\t  COUNTRY : %s", nBase::toupper(szCountry));
	if(nBase::GetUTC() > 0)
	{
		LOG2("\t\tTIME BIAS : UTC+%d UTCS+%d", nBase::GetUTC(), nBase::GetUTCS());
	}
	else
	{
		LOG2("\t\tTIME BIAS : UTC%d UTCS%d", nBase::GetUTC(), nBase::GetUTCS());
	}
	LOG2("");
}


void DM::Init(void)
{
	Load();

}


void DM::Close(void)
{

}

#endif
