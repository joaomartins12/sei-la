

#pragma once

#include "nlib/base.h"


#define korea		1
#define usa			2
#define thailand	3
#define hongkong	4
#define taiwan		5

namespace DM
{
	extern n4 nCountryCode;	// 1:korea, 2:usa

/////////////////////////////////////////////////////////////
// function
	void Load(void);
	void Init(void);
	void Close(void);

	inline n4 GetCountryCode(void) { return nCountryCode; }	// 국가 코드값

};


__forceinline
bool is_usa(void) { return DM::nCountryCode==usa; }

__forceinline
bool is_korea(void) { return DM::nCountryCode==korea; }

__forceinline
bool is_thailand(void) { return DM::nCountryCode==thailand; }

__forceinline
bool is_hongkong(void) { return DM::nCountryCode==hongkong; }

__forceinline
bool is_taiwan(void) { return DM::nCountryCode==taiwan; }

__forceinline
char *get_country_name(void)
{
	static char szCountryName[222] =  {0, };
	if(szCountryName[0])
	{
		return szCountryName;
	}

	switch(DM::GetCountryCode())
	{
	case korea		: strcpy_s(szCountryName, "korea");		break;
	case usa		: strcpy_s(szCountryName, "usa");		break;
	case thailand   : strcpy_s(szCountryName, "thailand");	break;
	case hongkong   : strcpy_s(szCountryName, "hongkong");	break;
	case taiwan		: strcpy_s(szCountryName, "taiwan");	break;
	default    : LOG("국가 설정값(%d) 오류입니다.", DM::GetCountryCode()); break;
	}

	return szCountryName;
}







