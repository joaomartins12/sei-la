#pragma once
#include "../cItemData.h"
namespace nsItemDel
{
	struct sItemInfo
	{
		sItemInfo() : u2InvenIdx(0) {}
		~sItemInfo() {}

		u2			u2InvenIdx;
		cItemData	itemData;
	};
};