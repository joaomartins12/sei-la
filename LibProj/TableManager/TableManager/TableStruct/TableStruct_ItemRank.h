
#pragma once 

BEGIN_NAME_TABLE

struct sItemRankData
{
	sItemRankData():s_nDropType(0),s_nDorpCount(0) {}
	USHORT		s_nDropType;
	USHORT		s_nDorpCount;
};


struct sItemRankEffect
{
	sItemRankEffect():dwItemCode(0),dwIconNo(0),nRank(0) {}

	DWORD		dwItemCode;
	DWORD		dwIconNo;
	int			nRank;
};

END_NAME_TABLE