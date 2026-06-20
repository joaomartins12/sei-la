#pragma once

#include "common_vs2019/plength.h"

namespace st_Relocate
{
#pragma pack(push, 1)
	struct RelocateInfo
	{
		RelocateInfo()
		{
			fromUserIdx = 0;
			fromTamerIdx = 0;
			fromTamerDeletedIdx = 0;

			itemInvenPos = -1;
			fromServerIdx = -1;
			toServerIdx = -1;

			digimonCount = 0;
			itemCount = 0;
			incubaterListCount = 0;
			sealCount = 0;

			newTamerIdx = 0;
			newDigimonIdx = 0;
			newItemIdx = 0;
			newIncubaterListIdx = 0;
			newSealIdx = 0;
			newEvolutionIdx = 0;

			DeleteItemIdx = 0;

			newTamerName[0] = '\0';
			szID[0] = '\0';
		}

		n2 itemInvenPos;
		n4 fromServerIdx;
		uint fromUserIdx;
		n8 fromTamerIdx;
		n4 fromTamerDeletedIdx;
		n4 toServerIdx;

		u2 digimonCount;
		u2 itemCount;
		u2 incubaterListCount;
		u2 sealCount;

		n8 newTamerIdx;
		n8 newDigimonIdx;
		n8 newItemIdx;
		n8 newIncubaterListIdx;
		n8 newSealIdx;
		n8 newEvolutionIdx;
		n8 DeleteItemIdx;
		
		char newTamerName[Language::pLength::name + 1];
		char szID[Language::pLength::id+1];
	};
#pragma pack(pop)

	struct Result
	{
		Result()
		{
			header = -1;
			result = -1;
		}

		int header;
		int result;
		RelocateInfo Info;
	};


	//////////////////////////////////////////////////////////////////////////
	// 2017-10-12
	// GyeongSu. Seo.
	// Tamer relocate Rebuild
	//////////////////////////////////////////////////////////////////////////
#pragma pack(push)
#pragma pack(1)
	struct stREQ_RELOCATE_CHECK_NAME
	{
		stREQ_RELOCATE_CHECK_NAME() : bToServerID(0),iUserIDX(0)
		{
			memset(szName,0,_countof(szName));
		}
		unsigned int	iUserIDX;
		int				bToServerID;
		char			szName[Language::pLength::name + 1];
	};

	struct stRES_RELOCATE_CHECK_NAME
	{
		stRES_RELOCATE_CHECK_NAME() : iUserIDX(0), iResultCode(-1)
		{}
		unsigned int	iUserIDX;
		int		iResultCode;
	};

	struct stREQ_RELOCATE_APPLY
	{
		stREQ_RELOCATE_APPLY() : bToServerID(0), bFromServerID(0), iUserIDX(-1), iOldTamerIDX(-1)
		{
			memset(szName,0,_countof(szName));
			memset(szOldName,0,_countof(szOldName));
		}
		int				bToServerID;
		int				bFromServerID;
		unsigned int	iUserIDX;
		__int64			iOldTamerIDX;
		char			szName[Language::pLength::name + 1];
		char			szOldName[Language::pLength::name + 1];
	};

	struct stRES_RELOCATE_APPLY
	{
		stRES_RELOCATE_APPLY() : iResultCode(-1), iUserIDX(0), iOldTamerIDX(-1)
		{}
		unsigned int	iUserIDX;
		__int64			iOldTamerIDX;
		int				iResultCode;
	};

	struct stRelocateTamerInfo				// 테이머 정보 업데이트
	{
		__int64			iSessionID;
		__int64			idxTamer;
		int				idxUser;
		int				iType;
		__int64			iMoney;
		short			sInvenSlot;
		short			sWarehouseSlot;
		__int64			iExp;
		short			sLevel;
		short			sStatPoint;
		short			sMhp;
		short			sMds;
		short			sHP;
		short			sDS;
		short			sDP;
		short			sAP;
		short			sFS;
		short			sFP;
		int				iX;
		int				iY;
		short			sMap;
		char			bQuest[524];
		int				iEggType;
		int				iEggLevel;
		char			bOpenRegion[256];
		int				iLogout;
		int				iDigimonBaseSlot;
		int				iDigimonCareSlot;
		int				iGuild;
		int				iGuildInfo;
		int				iOption;
		int				iHunt;
		int				iKeep;
		char			szLast_levelup[30];
		char			szDt_exp[30];
		short			sAchievement;
		float			fHatchLostGage;
		int				iHatchLostGageLevel;
		int				iClientOption;
		char			szCreateDate[30];
		int				iBackupdisk;
		__int64			iCumon;
		int				iDeckBuff;
		stRelocateTamerInfo() : idxTamer(-1), idxUser(-1), iSessionID(0)
		{
			memset(bQuest, 0, sizeof(char) * _countof(bQuest));
			memset(bOpenRegion, 0, sizeof(char) * _countof(bOpenRegion));
			memset(szCreateDate, 0, sizeof(char) * _countof(szCreateDate));
			memset(szLast_levelup, 0, sizeof(char) * _countof(szLast_levelup));
			memset(szDt_exp, 0, sizeof(char) * _countof(szDt_exp));
		}
	};

	struct stDIGIMONLIST					// 디지몬 리스트
	{
		__int64			iRelocateIDX;		// 중개 서버 table index
		char			szName[Language::pLength::name + 1];
		int				iType;
		__int64			iExp;
		short			sLevel;
		short			sStatPoint;
		int				iEvoStep;
		int				iBaseEvoStep;
		short			sMHP;
		short			sMDS;
		short			sHP;
		short			sDS;
		short			sDP;
		short			sAP;
		short			sFS;
		short			sScale;
		int				iEvoMask;
		char			szCreated[30];
		int				iSlotType;
		int				iSlotNo;
		int				iHatchLevel;
		char			szLast_levelup[30];
		char			szDt_exp[30];
		stDIGIMONLIST()
		{
			memset(szName, 0, sizeof(char) * Language::pLength::name + 1);
			memset(szCreated, 0, sizeof(char) * 30);
			memset(szLast_levelup, 0, sizeof(char) * 30);
			memset(szDt_exp, 0, sizeof(char) * 30);
		}
	};

	struct stITEMLIST						// 아이템 리스트
	{
		__int64			iRelocateIDX;		// 중개 서버 table index
		int				iType;
		short			sInfo;
		int				iTab;
		int				iSlot;
		int				iRate;
		short			sSock1;
		short			sSock2;
		int				iRate1;
		int				iRate2;
		int				iMaker;
		int				iLimited;
		int				iEndTime;
		int				UserIdx;
		short			sSock3;
		short			sRate3;
		int				iTradeLimitTime;
		stITEMLIST() : iRelocateIDX(-1)
		{}
	};

	struct stSEALLIST						// 씰
	{
		__int64			iRelocateIDX;		// 중개 서버 table index
		int				iType;
		short			sCount;
		short			sCardIDX;
		stSEALLIST():iRelocateIDX(-1),iType(-1),sCount(0),sCardIDX(0)
		{}
	};

	struct stINCULIST						// 인큐베이터리스트
	{
		__int64			iRelocateIDX;		// 중개 서버 table index
		__int64			iDigimonIDX;
		int				iSlotNo;
		int				iCareSlotNo;
		int				iItemType;
		int				iEndTime;
		stINCULIST():iRelocateIDX(-1),iSlotNo(-1),iDigimonIDX(-1),iCareSlotNo(-1),iItemType(-1),iEndTime(-1)
		{}
	};

	struct stEVOLIST						// 진화정보
	{
		__int64			iRelocateIDX;		// 중개 서버 table index
		__int64			iDigimonIDX;
		int				iSlot;
		int				iSlotState;
		int				iCard;
		int				iCrest;
		int				iSkillExp;
		int				iSkillExpLevel;
		short			sSkillPoint;
		int				iSkillLv1;
		int				iSkillLv2;
		int				iSkillLv3;
		int				iSkillLv4;
		int				iSkillLv5;
		stEVOLIST() : iRelocateIDX(-1), iDigimonIDX(-1), iSlot(-1), iSlotState(-1),iCard(-1),iCrest(-1),iSkillExp(-1),iSkillExpLevel(-1),
			iSkillLv1(-1),iSkillLv2(-1),iSkillLv3(-1),iSkillLv4(-1),iSkillLv5(-1)
		{}
	};

	struct stIDX_RES						// 결과값 돌려 받는 용도
	{
		__int64			iIdx;				// 중개 서버 table index
		__int64			iNewIDX;			// 신규 생성 디지몬 또는 아이템 인덱스
		stIDX_RES() : iIdx(-1), iNewIDX(-1) {}
	};
#pragma pack(pop)
}