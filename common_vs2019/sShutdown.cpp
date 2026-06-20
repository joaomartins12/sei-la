#include "sShutdown.h"
#include <time.h>


sShutdownWeek::sShutdownWeek()
: bCheckShutdown(false)
, Sunday(0)
, Monday(0)
, Tuesday(0)
, Wednesday(0)
, Thursday(0)
, Friday(0)
, Saturday(0)
{

}

// 시간별 비트(00:00시 부터 시작 ~ )
// 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1
bool sShutdownWeek::Check(const struct tm &t)
{
	return Check(t.tm_wday, t.tm_hour);
}

bool sShutdownWeek::Check(int week, int hour)
{
	enum 
	{
		SHUTDONW_SUNDAY
		,SHUTDONW_MONDAY
		,SHUTDONW_TUESDAY
		,SHUTDONW_WEDNESDAY
		,SHUTDONW_THURSDAY
		,SHUTDONW_FRIDAY
		,SHUTDONW_SATURDAY
	};

	if (!bCheckShutdown)
	{
		return false;
	}

	if (24 <= hour || 0 > hour)
	{
		return false;
	}

	switch(week)
	{
	case SHUTDONW_SUNDAY:		/* days since Sunday - [0,6] */
		{
			if ((Sunday >> hour) & 1)
				return true;
			return false;
		}
	case SHUTDONW_MONDAY:
		{
			if ((Monday >> hour) & 1)
				return true;
			return false;
		}
	case SHUTDONW_TUESDAY:
		{
			if ((Tuesday >> hour) & 1)
				return true;
			return false;
		}
	case SHUTDONW_WEDNESDAY:
		{
			if ((Wednesday >> hour) & 1)
				return true;
			return false;
		}
	case SHUTDONW_THURSDAY:
		{
			if ((Thursday >> hour) & 1)
				return true;
			return false;
		}
	case SHUTDONW_FRIDAY:
		{
			if ((Friday >> hour) & 1)
				return true;
			return false;
		}
	case SHUTDONW_SATURDAY:
		{
			if ((Saturday >> hour) & 1)
				return true;
			return false;
		}
	default:
		break;
	}

	return false;
}