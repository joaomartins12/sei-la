#pragma once
#include <sstream>
namespace neFriend
{
	const int nMaxFriendCount = 100;		// 친구 최대 등록 갯수
	const int nMaxShutOutCount = 100;	// 차단 최대 등록 갯수
};

namespace nsFriendInfo
{
	struct sFriendInfo
	{

		sFriendInfo():s_eOnlineState(0) {}
		~sFriendInfo() {}

		n1						s_eOnlineState;		//logon/logff 상태
		std::wstring			s_szName;
		std::wstring			s_szMemo;
	};
};
namespace nsShutout
{
	struct sShutoutInfo
	{
		sShutoutInfo() {}
		~sShutoutInfo() {}

		std::wstring		s_szName;
		std::wstring		s_szMemo;
	};
};
