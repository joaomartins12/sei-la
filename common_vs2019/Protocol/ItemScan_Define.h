#pragma once
#include "../cItemData.h"
namespace nsItemScan
{
	struct sScanInfo
	{
		sScanInfo() : uInvenIdx(0) {}
		~sScanInfo() {}

		u4			uInvenIdx;
		cItemData	itemData;
	};
}