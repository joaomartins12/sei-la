#pragma once

namespace TB
{
	enum eExchangeWayType
	{
		eNeedAll = 1,	// 필요 재료 모두 충족해야만 교환 가능
		eNeedOne,		// 필요 재료중 한개씩만 충족하면 교환 가능
	};

	enum eExchangeType
	{
		eItemToDigion = 1,	// 아이템으로 디지몬 진화
		eDigionToItem,		// 디지몬을 아이템으로 교환
	};

	enum eExchangeMaterialType
	{
		eMaterialDigion = 1,	// 아이템으로 디지몬 진화
		eMaterialItem,		// 디지몬을 아이템으로 교환
	};

	struct sExchangeMaterialData
	{
		sExchangeMaterialData() : dwMaterialIDX(0), nCount(1)
		{
		}

		DWORD	dwMaterialIDX;
		int		nCount;
	};

	typedef std::list<sExchangeMaterialData>					LIST_MATERIALDATA;
	typedef std::list<sExchangeMaterialData>::iterator			LIST_MATERIALDATA_IT;
	typedef std::list<sExchangeMaterialData>::const_iterator	LIST_MATERIALDATA_CIT;

	struct sExchangeData
	{
		sExchangeData():dwObjectIDX(0),usMaterialType(0), nNeedMaterialValue(0),usWayType(0),dwNeedMoney(0)
		{
		}
		
		DWORD				dwObjectIDX;			// 디지몬 테에이블 번호 이거나 아이템 테이블 번호
		USHORT				usMaterialType;			// 재료의 타입
		int					nNeedMaterialValue;		// 디지몬일 경우 디지몬 레벨
		DWORD				dwNeedMoney;			// 필요 금액
		USHORT				usWayType;				// 1:
		LIST_MATERIALDATA	listNeedMainMaterial;	// 메인 재료
		LIST_MATERIALDATA	listNeedSubMaterial;	// 보조 재료
	};

	typedef std::list<sExchangeData>					LIST_EXCHAGEDATA;
	typedef std::list<sExchangeData>::iterator			LIST_EXCHAGEDATA_IT;
	typedef std::list<sExchangeData>::const_iterator	LIST_EXCHAGEDATA_CIT;

	typedef std::map<USHORT, LIST_EXCHAGEDATA>						MAP_EXTRAEVOLUTION_DATA;
	typedef std::map<USHORT, LIST_EXCHAGEDATA>::iterator			MAP_EXTRAEVOLUTION_DATA_IT;
	typedef std::map<USHORT, LIST_EXCHAGEDATA>::const_iterator		MAP_EXTRAEVOLUTION_DATA_CIT;

};