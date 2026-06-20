


#pragma once


#include "pType.h"
#include "pLength.h"

#include "nScope.h"



namespace nSession
{
	enum
	{
		PCBang = 40,

	};
};


namespace pNet
{
	enum
	{
		Begin = nScope::Net,
		Check,
		LockCluster,	// 서버 상태를 잠금 상태로 변경한다. dmosManager->SvrAccount
		OpenCluster,	// 서버 상태를 open 상태로 변경한다. dmosManager->SvrAccount

		VersionError,	// 버전이 서로 다름

		Pause,			// 서버 가동 상태를 정지
		Resume,			// 정지 해제

		StartTime,		// 시작한 시간을 요청

		End
	};

};


namespace nBlock
{
	enum
	{
		Lv0 =     9,	//   9초, 메세지 출력하고, 연결만 끊는다.

		Lv1 =   180,	//   3분
		Lv2 =   180,
		Lv3 =   180,
		Lv4 =   180,
		Lv5 =   180,
		Lv6 =   180,
/*
		Lv1 =   300,	//   5분
		Lv2 =   600,	//  10분
		Lv3 =  1800,	//  30분
		Lv4 =  3600,	//  60분
		Lv5 =  5400,	//  90분
		Lv6 =  7200,	// 120분
//*/
		Lv7 = 86400,	//  24시
	};
};


namespace nNet
{
//////////////////////////////////////////////////////////
// nNet::GetPort
	__forceinline
	int GetPort(int nPort) 
	{
		int res = nPort;

		res |= nPort << (nPort%4);
		res ^= 9999;

		return (res % 30000);
	}


#pragma pack(push)
#pragma pack(1)
//////////////////////////////////////////////////////////
// nNet::CreateData
	struct CreateData // client -> gate server
	{
		u1 m_nSlotNo;

		u4 m_nTamerType;
		n1 m_szTamerName[Language::pLength::name+1];

		u4 m_nDigimonType;
		n1 m_szDigimonName[Language::pLength::name+1];
	};

//////////////////////////////////////////////////////////
// nNet::CreateData
	struct ListData
	{
		u1 m_cSlotNo;

		u4 m_nTamerType;
		n1 m_szTamerName[Language::pLength::name+1];

		u4 m_nDigimonType;
		n1 m_szDigimonName[Language::pLength::name+1];
	};

#pragma pack(pop)
};







