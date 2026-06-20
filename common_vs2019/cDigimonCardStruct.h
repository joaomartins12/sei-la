#pragma once


struct stBattleCardData
{
public:
	stBattleCardData():nItemType(0) , nCardType(0)
	{};
	~stBattleCardData(){};

public:
	n4 nItemType;
	n4 nCardType;
};

struct cCardSource
{
public:
	cCardSource()
	{};
	~cCardSource(){};
public:

	n4 nCardIdx;
	n1 nCardType;
	n4 nCost;
};