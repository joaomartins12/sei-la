//2017-03-15-nova
#pragma once
#include "ItemRewardContents_Define.h"

class GS2C_RECV_GIFT_DATA
{
public:
	GS2C_RECV_GIFT_DATA():iResult(0)
	{
		ClearData();
	}

	void SetData(cItemInfo* pInfo, int size)
	{
		mItemList.clear();
		for( int i=0; i<size; ++i )
			mItemList.insert( make_pair(i, pInfo[ i ]) );
	}

	void CopyData(GIFT_ITEM_LIST& data)
	{
		data.clear();

		if( 0 < mItemList.size() )
		{
			data.insert(mItemList.begin(), mItemList.end());
		}
	}

	void ClearData()
	{
		mItemList.clear();
	}

private:	
	int					iResult;
	GIFT_ITEM_LIST		mItemList;	
};

class GS2C_RECV_REWARD_DATA
{
public:
	GS2C_RECV_REWARD_DATA():iResult(0)
	{
		ClearData();
	}

	void AddData(int idx)
	{
		mItemList.insert(make_pair(mItemList.size(), idx));
	}

	void CopyData(REWARD_ITEM_LIST& data)
	{
		data.clear();

		if( 0 < mItemList.size() )
		{
			data.insert(mItemList.begin(), mItemList.end());
		}
	}

	void ClearData()
	{
		mItemList.clear();
	}

private:	
	int					iResult;
	REWARD_ITEM_LIST	mItemList;	
};

class GS2C_RECV_REWARD_SEND_TYPE
{
public:
	GS2C_RECV_REWARD_SEND_TYPE():iType(0), iIndex(-1), iCnt(0), bSendAll(false)
	{

	}

public:
	int		iType;
	int		iIndex;
	int		iCnt;
	bool	bSendAll;
};