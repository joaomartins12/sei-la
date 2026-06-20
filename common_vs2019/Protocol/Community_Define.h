#pragma once
#include <string>

namespace GuildInfo
{
	struct sGuild_PersonInfo
	{
		sGuild_PersonInfo():nMapNO(0), nClass(0), nTamerType(0), nTamerLevel(0), nMemberExp(0), nChannelNO(0), nRank(1), nLastTime(0)
		{}

		u1 nClass;
		u2 nMapNO;			// 맵번호
		u4 nTamerType;
		u1 nTamerLevel;			// 테이머 레벨, 레벨이 0인 경우 offline 처리
		u4 nMemberExp;			// 길드 멤버의 길드에 대한 기여도
		u1 nChannelNO;		// 채널 번호
		
		std::wstring wsName;	// 이름
		
		//길드 리뉴얼 추가 변수
		u1 nRank;
		u4 nLastTime;
		std::wstring wsMemo;	// 길드원 메모
	};
	struct sGuild_History 
	{
		sGuild_History():nMasterClass(0), nTimeTS(0), nMemberClass(0), nTime(0), nType(0)
		{}
		n1 nType;
		uint nTime;
		u4 nTimeTS;
		n1 nMasterClass;
		n1 nMemberClass;
		std::wstring szMasterName;
		std::wstring szMemberName;
	};
};