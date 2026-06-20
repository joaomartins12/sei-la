#pragma once

#include "nlib/base.h"


namespace eGotchaState
{
	enum
	{		
		ItemTotal = 3,		//한라인당 레어아이템 받을수 있는갯수
		NpcTotalCnt = 10,	//  한 npc 당 등록 할 수 있는 최대 갯수
		LimitLine = 100,	//엑셀에 등록할 수있는 최대 갯수 
		eEnd,
	};
};
namespace eGotchaReset
{
	enum
	{
		UnReset = 0,		//리셋번호가 0이면 리셋할수 없음
		Reset,				//리셋번호가 1이면 리셋가능
		CountReset,			//리셋번호가 2이면 머신초기화 횟수만큼 리셋
		DayCountReset,		//리셋번호가 3이면 하루에 리셋가능한 수만큼 리셋
		NormalReset,		//리셋번호가 4이면 뽑기아이템 총합에 의한 리셋
		CheatReset = 100,	//치트키를 사용하여 리셋 시킬 경우(로그확인용)
		eEnd
	};
};
namespace eGotchaErrMsg
{
	enum
	{
		ResetCompare = 0,	 //리셋한 수가 서로 다를경우 
		RareItemCntZero,	 //레어 아이템 갯수가 0인데 익스큐트 요청이 들어온경우
		InvanSlotFull ,		 //인벤에 자리가 부족 
		CloseTime,			//시간이 안맞을때
		TimePeriodError,	//너무빨리뽑음
		eEnd
	};
};
namespace eGotchaSetting
{
	enum
	{
		NpcNo = 0,				//Npc번호
		CoinIdx,				//코인번호
		UseCoin,				//사용하는코인수
		MachineState,			//사용가능인지 아닌지
		MinLev,					//머신사용할수있는 최소레벨
		MaxLev,					//머신 사용할 수 있는 최대레벨
		ItemIdx1,				//레어아이템 번호
		ItemChance1,			//레어나올 확률
		ItemGive1,				//레어지급갯수
		ItemCnt1,				//레어갯수
		ItemIdx2,				//레어아이템 번호
		ItemChance2,			//레어나올 확률
		ItemGive2,				//레어지급 갯수
		ItemCnt2,				//레어갯수
		ItemIdx3,				//레어아이템 번호
		ItemChance3,			//레어나올 확률
		ItemGive3,				//레어지급 갯수
		ItemCnt3,				//레어 갯수
		GroupNo1,				//그룹번호
		GroupNo2,				//그룹번호
		GroupNo3,				//그룹번호
		GroupCnt,				//그룹갯수
		ResetNo,				//리셋번호 (0이면 리셋안됨, 1이면 무한, 2면 카운트만큼, 3이면 하루카운트만큼, 4이면 초기화 하지 않고 계속 사용)		
		ResetCnt,				//리셋되는 횟수
		GotchaVer,				//엑셀파일이 변한지 아닌지 알기위한
		ResetType,				//리셋됐을때 다른 사람이 켜논 머신 끄기위한
		ResetRare1,				//리셋해주기 위해 처음에 받은 레어 갯수 받아놈
		ResetRare2,				//리셋해주기 위해 처음에 받은 레어 갯수 받아놈
		ResetRare3,				//리셋해주기 위해 처음에 받은 레어 갯수 받아놈
		ResetGroup,				//리셋해주기 위해 처음에 받은 그룹 갯수 받아놈
		ResetrCnt,				//리셋해주기 위해 처음에 받은 리셋 횟수 받아놈
		MaxCnt,
		

		
		eEnd
	};
};


class cGotchaMachine
{
public:
	cGotchaMachine(void);
   ~cGotchaMachine(void);

	 u4 GotchaGroupItemIdx[eGotchaState::LimitLine][11];
	 u4 GotchaGroupItemCnt[eGotchaState::LimitLine][11];
	 u4 m_GotchaMachine[eGotchaState::LimitLine][eGotchaSetting::MaxCnt];
	 u4 m_TotalCount;
	 u4 m_DaySave;
	 u4 m_TotalGroupCnt; 
private:
	n1 m_Start_date[eGotchaState::LimitLine][32];
	n1 m_End_date[eGotchaState::LimitLine][32];
	n1 m_Start_Time[eGotchaState::LimitLine][32];
	n1 m_End_Time[eGotchaState::LimitLine][32];
public:
	void SetGotchaTotalGroupCnt(u4 x)	{m_TotalGroupCnt = x;}
	u4	 GetGotchaTotalGroupCnt()		{return m_TotalGroupCnt;}

	void SetGotchaGroupIdx(u4 x, u4 y,  u4 Index)	{GotchaGroupItemIdx[x][y] = Index;}
	u4 GetGotchaGroupIdx(u4 x, u4 y) {return GotchaGroupItemIdx[x][y];}
	
	void SetGotchaGroupCnt(u4 x, u4 y, u4 Index)	{GotchaGroupItemCnt[x][y] = Index;}
	u4 GetGotchaGroupCnt(u4 x, u4 y ) {return GotchaGroupItemCnt[x][y];}


	void SetGotcha( u4 x, u4 y,  int Index )	{ m_GotchaMachine[x][y] = Index; }
	void SetTotalCnt( u4 Count)					{ m_TotalCount = Count;}
	void SetStartDate(u4 x, n1 *Start_date)			{ strcpy(m_Start_date[x], Start_date); }
	void SetEndDate(u4 x, n1 *End_date)				{ strcpy(m_End_date[x], End_date); }
	void SetStartTime(u4 x, n1 *Start_Time)			{ strcpy(m_Start_Time[x], Start_Time); }
	void SetEndTime(u4 x, n1 *End_Time)				{ strcpy(m_End_Time[x], End_Time); }
	void SetDaySave(u4 sDay)				{m_DaySave = sDay; }
	
	u4 GetDaySave()					{return m_DaySave;}
	u4 GetGotchaInfo( u1 x, u1 y )		{ return m_GotchaMachine[x][y]; }
	u4 GetTotalCnt()				{return m_TotalCount;	}

	n1 *GetStartDate(u4 x)		{ return m_Start_date[x]; }
	n1 *GetEndDate(u4 x)			{ return m_End_date[x]; }
	n1 *GetStartTime(u4 x)		{ return m_Start_Time[x]; }
	n1 *GetEndTime(u4 x)			{ return m_End_Time[x]; }
};
extern cGotchaMachine *g_pGotchaMachine;

struct stGotchaMachine
{
	u4 NpcNo ;
	u4 ItemIDX[eGotchaState::ItemTotal] ;
	u4 ItemChance[eGotchaState::ItemTotal];
	u4 ItemGive[eGotchaState::ItemTotal];
	u4 ItemCnt[eGotchaState::ItemTotal] ;
	u4 GroupIDX[eGotchaState::ItemTotal] ;
	u4 GroupCnt ; 
	u4 TotalCnt ; 
	u4 ResetNo;
	u4 ResetCnt;
	u4 GotchaVer;

	n4 GetRemainCount() 
	{
		n4 rareCount = 0;
		for (int i = 0; i < eGotchaState::ItemTotal; ++i)
		{
			rareCount += ItemCnt[i];
		}
		return rareCount + GroupCnt;
	}
};