#pragma once

#pragma pack(push,1)
struct stHotTimeItemInfo
{
	stHotTimeItemInfo():nItemIdx(0), nItemCount(0)
	{};

	n4		nItemIdx;
	n4		nItemCount;
};
#pragma pack(pop)

namespace nsHotTimeEventState
{
	enum
	{
		NO_EVENT = 0,
		NOT_INTIME ,
		INTIME,
	};
}

struct stHotTimeDate
{
	int m_nYear;
	int m_nMonth;
	int m_nDay;
};