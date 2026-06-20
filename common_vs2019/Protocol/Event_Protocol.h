#pragma once

#include "Event_Define.h"
#include <minwindef.h>
#include "../pEvent.h"

//////////////////////////////////////////////////////////////////////////
// Exp Event Data
//////////////////////////////////////////////////////////////////////////
class GS2C_RECV_EXP_EVENT_DATA
{
public:
	GS2C_RECV_EXP_EVENT_DATA()
	:m_nResult(0),m_nExpRate(0),m_nNextDayExpRate(0),m_nExpTarget(0),m_nSpecialExp(0)
	{}

public:	
	int		m_nResult;
	u4		m_nExpRate;			// 경험치 비율
	u4		m_nNextDayExpRate;	// 다음날 접속시 받게되는 경험치 비율
	u4		m_nExpTarget;		// 경험치 종류 1: 일반 2: 부스터
	u4		m_nSpecialExp;		// 해당 유저가 스폐셜 경험치 부여 대상이라면 0이 아닌 값이 들어 있습니다. 최대 500 이상은 되지 않습니다.
};


//////////////////////////////////////////////////////////////////////////
// Daily Event Data
//////////////////////////////////////////////////////////////////////////

class GS2C_RECV_DAILY_EVENT_DATA
{
public:
	GS2C_RECV_DAILY_EVENT_DATA()
	:m_nResult(0),m_nEventNo(0),m_nRemainedTimeTS(0),m_nTotalTime(0),m_nWeek(0xff)
	{}

public:
	int		m_nResult;
	n4		m_nEventNo;				// DM_Event-매일 참조 - 이벤트 번호
	u4		m_nRemainedTimeTS;		// 해당 이벤트 아이템을 받기위해 남은 시간
	u4		m_nTotalTime;			// 해당 이벤트 아이템을 받기위한 총 시간
	byte	m_nWeek;				// 2017-02-08-nova 요일
};

//////////////////////////////////////////////////////////////////////////
// Monthly Event Data
// 2017-02-16-nova
class GS2C_RECV_MONTHLY_EVENT_DATA
{
public:
	GS2C_RECV_MONTHLY_EVENT_DATA()
	:m_nEventNo(0xff),m_nAttendCnt(0xff),m_bNotify(false)
	{}

public:
	byte	m_nEventNo;				
	byte	m_nAttendCnt;		
	bool	m_bNotify;
};


#pragma pack(push,1)
struct stHotTime_Time
{
public:
	stHotTime_Time() : nHour(0),nMin(0),nSec(0)
	{
	}
	n1 nHour;
	n1 nMin;
	n1 nSec;
};
#pragma pack(pop)


class GS2C_NTF_HOTTIME_EVENT_INFO
{
public:
	GS2C_NTF_HOTTIME_EVENT_INFO()
		:nProtocol(pEvent::HotTimeEvent) , nEventState(0) , nCurrentTableIdx(0) , nNextTableIdx(0)
		,bIsGetItem(0) ,nStartTimeLeftTs(0), nEndTimeLeftTs(0)
	{	
	}
	~GS2C_NTF_HOTTIME_EVENT_INFO()
	{
	};

	WORD GetProtocol(){return nProtocol;}
public:
	WORD				nProtocol;


	n1 nEventState;			//nsHotTimeEventState
	n1 nCurrentTableIdx;	//현재 이벤트 테이블IDX
	n1 nNextTableIdx;		//만약 다음이벤트가 없다면 0으로 넣어보냄
	n1 bIsGetItem;			//0 아이템 지급받지않음 1 지급 받음
	n4 nStartTimeLeftTs;	//지급 시작까지 남은시간
	n4 nEndTimeLeftTs;		//지급 종료까지 남은시간
};


class C2GS_SEND_HOTTIME_EVENT_GET
{
public:
	C2GS_SEND_HOTTIME_EVENT_GET()
		:nProtocol(pEvent::HotTimeItemRequest) , nTableIdx(0)
	{	
	}
	~C2GS_SEND_HOTTIME_EVENT_GET()
	{
	};

	WORD GetProtocol(){return nProtocol;}
public:
	WORD				nProtocol;
	n1 nTableIdx ;
};



class GS2C_RECV_HOTTIME_GET_RESULT
{
public:
	GS2C_RECV_HOTTIME_GET_RESULT()
		:nProtocol(pEvent::HotTimeItemRequest) , nResult(0)
	{	
	}
	~GS2C_RECV_HOTTIME_GET_RESULT()
	{
	};

	WORD GetProtocol(){return nProtocol;}
public:
	WORD				nProtocol;

	n4 nResult;
};
