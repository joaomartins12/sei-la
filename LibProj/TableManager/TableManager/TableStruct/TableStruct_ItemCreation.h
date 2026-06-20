
#pragma once 

BEGIN_NAME_TABLE

struct sAssistanceItem
{
	sAssistanceItem():dwItemCode(0),nSuccessPercentage(0)	{};
	~sAssistanceItem() 	{};

	DWORD	dwItemCode;			// 아이템 테이블 번호
	int		nSuccessPercentage;// 추가 확률
};

typedef std::list<sAssistanceItem>					LIST_AssistItemList;
typedef std::list<sAssistanceItem>::iterator		LIST_AssistItemList_IT;
typedef std::list<sAssistanceItem>::const_iterator	LIST_AssistItemList_CIT;

struct sAssistanceItemGroup
{
	enum eAssistType
	{
		ePercentageAssist = 0,	// 0 : 확률 보조 아이템
		eProtectAssist,			// 1 : 보호 아이템 
	};

	sAssistanceItemGroup():nAssistType(0){};
	~sAssistanceItemGroup() 	{};

	int							nAssistType;		// 보조형 타입, 0 : 확률 보조 아이템, 1 : 보호 아이템 
	LIST_AssistItemList			assisteItemList;
};


typedef std::map<int, sAssistanceItemGroup>					MAP_AssistItemGroup;
typedef std::map<int, sAssistanceItemGroup>::iterator		MAP_AssistItemGroup_IT;
typedef std::map<int, sAssistanceItemGroup>::const_iterator	MAP_AssistItemGroup_CIT;

//////////////////////////////////////////////////////////////////////////

struct sMaterialItemList
{
	sMaterialItemList():m_dwItemIdx(0),m_nItemNum(0)	{};
	~sMaterialItemList()	{};

	DWORD						m_dwItemIdx;	// 제작할 아이템 인덱스 번호
	int							m_nItemNum;		// 제작하는 아이템의 갯수
};

typedef std::list<sMaterialItemList>					LIST_MaterialList;
typedef std::list<sMaterialItemList>::iterator			LIST_MaterialList_IT;
typedef std::list<sMaterialItemList>::const_iterator	LIST_MaterialList_CIT;


struct sProductionItemList
{
	sProductionItemList():m_nUniqueIdx(0),m_dwItemIdx(0),m_nItemNum(0),m_nProbabilityofSuccess(0)
		,m_dwItemCost(0),m_nPercentageAssistItemGroup(0),m_nProtectAssistItemGroup(0)
	{};
	~sProductionItemList()
	{};

	int							m_nUniqueIdx;				// 제작 고유 번호
	DWORD						m_dwItemIdx;				// 제작할 아이템 인덱스 번호
	int							m_nItemNum;					// 제작하는 아이템의 갯수
	int							m_nProbabilityofSuccess;	// 성공확률(만분률)
	DWORD						m_dwItemCost;				// 제작 비용
	LIST_MaterialList			m_MaterialList;				// 재료 아이템 목록
	int							m_nPercentageAssistItemGroup;
	int							m_nProtectAssistItemGroup;
};

typedef std::list<sProductionItemList>					LIST_ProductionList;
typedef std::list<sProductionItemList>::iterator		LIST_ProductionList_IT;
typedef std::list<sProductionItemList>::const_iterator	LIST_ProductionList_CIT;

struct sSubCategoty
{
	sSubCategoty(int nType):m_nCategotyType(nType)
	{};

	~sSubCategoty()
	{};

	int							m_nCategotyType;		// 카테고리 타입 번호
	std::wstring				m_wsCategotyName;		// 카테고리 이름
	LIST_ProductionList			m_listProductionItems;	// 제작 아이템 리스트
};

typedef std::map<int,sSubCategoty>					MAP_Sub_Categoty;
typedef std::map<int,sSubCategoty>::iterator		MAP_Sub_Categoty_IT;
typedef std::map<int,sSubCategoty>::const_iterator	MAP_Sub_Categoty_CIT;

struct sMainCategoty
{
	sMainCategoty(int nType):m_nCategotyType(nType)
	{};

	~sMainCategoty()
	{};

	int							m_nCategotyType;		// 카테고리 타입 번호
	std::wstring				m_wsCategotyName;		// 카테고리 이름
	MAP_Sub_Categoty			m_mapSubCategoty;		// 소분류 카테고리 목록
};

typedef std::map<int,sMainCategoty>					MAP_Main_Categoty;
typedef std::map<int,sMainCategoty>::iterator		MAP_Main_Categoty_IT;
typedef std::map<int,sMainCategoty>::const_iterator	MAP_Main_Categoty_CIT;

struct sItemProductionNpcTable
{
	sItemProductionNpcTable(DWORD dwNpcIdx):m_dwNpcIdx(dwNpcIdx)
	{
		m_mapMainCategoty.clear();
	};

	~sItemProductionNpcTable()
	{
		m_mapMainCategoty.clear();
	};

	DWORD				m_dwNpcIdx;						// NPC 인덱스 번호 (Key 값)
	MAP_Main_Categoty	m_mapMainCategoty;				// 대분류 카테고리 목록
};

typedef std::map<DWORD, sItemProductionNpcTable>					STL_MAP_ITEMPRODUCTION;
typedef std::map<DWORD, sItemProductionNpcTable>::iterator			STL_MAP_ITEMPRODUCTION_IT;
typedef std::map<DWORD, sItemProductionNpcTable>::const_iterator	STL_MAP_ITEMPRODUCTION_CIT;

END_NAME_TABLE