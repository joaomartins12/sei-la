#pragma once

namespace nsPersonStore
{
	struct sSellItemInfo
	{
		sSellItemInfo() : nItemPrice( 0 )
		{
			ItemData.Clear();
		};

		cItemData	ItemData;
		n8			nItemPrice;
	};
}