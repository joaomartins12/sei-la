


#pragma once


#include "nScope.h"

namespace pShutdown
{
	enum
	{
		Begin = nScope::Shutdown,

		WeekCheck,			// 주간 셧다운 체크
		WeekCheckError,

		End
	};

};