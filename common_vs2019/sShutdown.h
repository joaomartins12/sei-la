#pragma once

struct sShutdownWeek
{
	int bCheckShutdown;				//주간 셧다운 적용할지 여부
	long Sunday;
	long Monday;
	long Tuesday;
	long Wednesday;
	long Thursday;
	long Friday;
	long Saturday;	

	sShutdownWeek();
	bool Check(const struct tm &t);		//tm에 설정된 요일(0 ~ 6), 시간에 대해 체크
	bool Check(int week, int hour);		//요일(0 ~ 6), 시간에 대해 체크
};