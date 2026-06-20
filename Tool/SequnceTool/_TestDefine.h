#pragma once 


#define FIX_TOOL_RESIZE		// 14.03.20	시퀀스 툴 윈도우 영역 저장/불러오기			kimhoshok



#ifdef VERSION_USA

#include "EngDefine.h"

#elif VERSION_TH

#include "ThaiDefine.h"

#elif VERSION_HK

#include "HongkongDefine.h"

#elif VERSION_TW

#include "TaiwanDefine.h"

#else

#include "KorDefine.h"

#endif