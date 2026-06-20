#pragma once

#include "DigimonArchive_Define.h"



class GS2C_RECV_ARCHIVE_CHANGE
{

public:
	GS2C_RECV_ARCHIVE_CHANGE()
		:Src(0), Dst(0), Price(0)
	{
	}
	~GS2C_RECV_ARCHIVE_CHANGE(){}

public:
	int Src;
	int Dst;
	u4 Price;
};

struct IncubatorInfo
{
	int incubatorslotidx;
	u4 archiveslotidx;				
	u4 itemtype;			
	u4 remaintime;		
};
class GS2C_RECV_INCUBATOR_INFO
{
public:
	GS2C_RECV_INCUBATOR_INFO()
	{

	}
	~GS2C_RECV_INCUBATOR_INFO(){}
	void AddIncuInfo(int iIncubatorSlotIdx, int iArchiveSlotIdx, int iItemType, int iRemainTime)
	{
		std::map<int, IncubatorInfo>::iterator kIter = mIncuRegists.find(iIncubatorSlotIdx);
		if(kIter == mIncuRegists.end())
		{
			IncubatorInfo kInfo;
			kInfo.incubatorslotidx = iIncubatorSlotIdx;
			kInfo.archiveslotidx = iArchiveSlotIdx;
			kInfo.itemtype = iItemType;
			kInfo.remaintime = iRemainTime;

			mIncuRegists.insert(std::map<int, IncubatorInfo>::value_type(iIncubatorSlotIdx, kInfo));
		}
	}


public:
	std::map<int, IncubatorInfo> mIncuRegists;
};

class GS2C_RECV_INCUBATOR_CHANGE
{
public:
	GS2C_RECV_INCUBATOR_CHANGE()
		:Incu_slotIdx(0),
		Archive_slotIdx(0)
	{

	}
	~GS2C_RECV_INCUBATOR_CHANGE(){}

	int Incu_slotIdx;
	int Archive_slotIdx;



};
struct GS2C_RECV_START_INCUBATING_INFO
{
	int Incu_slotIdx;
	int RemainTime;
};

struct GS2C_RECV_STOP_INCUBATING_INFO
{
	int Incu_slotIdx;
};
class GS2C_RECV_EXTEND_ARCHIVE
{
public:
	GS2C_RECV_EXTEND_ARCHIVE()
		:ExtendedArchiveCnt(0)
	{

	}
	~GS2C_RECV_EXTEND_ARCHIVE(){}

	const int GetExtendArchiveCnt(void){return ExtendedArchiveCnt;}
	void SetExtendArchiveCnt(int iValue){ExtendedArchiveCnt = iValue;}
	int ExtendedArchiveCnt;
};
class GS2C_RECV_OPENED_ARCHIVE_COUNT
{
public:
	GS2C_RECV_OPENED_ARCHIVE_COUNT()
		:OpenedCnt(0)
	{

	}
	~GS2C_RECV_OPENED_ARCHIVE_COUNT(){}

	const int GetOpenedArchiveCnt(void){return OpenedCnt;}
	void SetOpenedArchiveCnt(int iValue){OpenedCnt = iValue;}
	int OpenedCnt;
};
class GS2C_RECV_EXTEND_ACCOMPANY_SLOT
{
public:
	GS2C_RECV_EXTEND_ACCOMPANY_SLOT()
		:OpenedCnt(0){

	}
	~GS2C_RECV_EXTEND_ACCOMPANY_SLOT(){}
	const int GetOpenedAccompanyCnt(void){return OpenedCnt;}
	void SetOpenedAccompanyCnt(int iValue){OpenedCnt = iValue;}
	int OpenedCnt;
};





/************************************************************************/
/* 보관소 <-> 보관소 내부이동											*/
/************************************************************************/

class C2GS_SEND_Archive_Internal_Move
{
public:
	C2GS_SEND_Archive_Internal_Move(): nProtocol(pDigimon::CareDigimonMove)
	{}

	WORD GetProtocol(){return nProtocol;}

public:
	WORD nProtocol;

	n4 nSlot1;		//FROM
	n4 nSlot2;		//TO
	n4 nNpcIDX;		//NPCIDX
};

class GS2C_RECV_Archive_Internal_Move		//성공시에만보냄(실패는 연결끊음)
{
public:
	GS2C_RECV_Archive_Internal_Move(): nProtocol(pDigimon::CareDigimonMove)
	{}

	WORD GetProtocol(){return nProtocol;}

public:
	WORD nProtocol;

	n4 nSlot1;		//FROM
	n4 nSlot2;		//TO
};