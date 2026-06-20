
#pragma once 


class CsItemMng
{
public:
	CsItemMng();
	~CsItemMng();

public:
	void							Delete();
	bool							Init( char* cPath );

	void							ReloadName();

public:
	void							_LoadExcel();
	void							_LoadExcel( char* cExcel );
	void							_ReloadName( char* cExcel );
	bool							_LoadBin( char* cPath );
	void							_LoadFilePack( char* cPath );	

	void							_LoadCatagory( char* cExcel );
	void							_LoadCoolTime( char* cExcel );
	void							_LoadData( char* cExcel );
	void							_LoadRank( char* cExcel	);	// Rank테이블 load관련 추가	lks007

	void							_LoadExchange( char* cExcel );	// 물물교환상점 시트 로드	khc0729

	void							_LoadAttributeData( char* cExcel );

	void							_LoadAccessory( char* cExcel );	//악세서리 로드 chu8820

	//////////////////////////////////////////////////////////////////////////
	// 아이템 제작 테이블 로드
	//////////////////////////////////////////////////////////////////////////
public:
	MAP_Main_Categoty const*		GetItemProductionCategotyTable( DWORD const& dwNpcIdx ) const;
	STL_MAP_ITEMPRODUCTION const*	GetItemProductionTable() const;

private:
	void							_LoadItemProductionTable( char* cExcel );
	void							_LoadBin_ItemProductionTable( FILE* fp );
	void							_LoadFilePack_ItemProductionTable( int nHandle );
	void							_SaveBin_ItemProductionTable( FILE* fp );
	STL_MAP_ITEMPRODUCTION			m_mapProductionItem;	// 아이템 제작 테이블

	//////////////////////////////////////////////////////////////////////////
	// 아이템 제작시 보조할 아이템 테이블 데이터
	//////////////////////////////////////////////////////////////////////////
public:
	sAssistanceItemGroup const* GetAssistItemGroup(int const& nGroupID) const;
	LIST_AssistItemList const*	GetAssistGroupItemList(int const& nGroupID) const;

private:
	void						_LoadItemProductionAssistTable( char* cExcel );
	void						_LoadBin_AssistItemGroupTable( FILE* fp );
	void						_LoadFilePack_AssistItemGroupTable( int nHandle );
	void						_SaveBin_AssistItemGroupTable( FILE* fp );

	MAP_AssistItemGroup			m_mapAssistItemGroup;
	//////////////////////////////////////////////////////////////////////////

public:
	DWORD					TypeT_to_Disp( DWORD nTypeT );
	DWORD					TypeLS_to_Disp( DWORD nTypeL, DWORD nTypeS );

	// CAPSULE_EVOLUTION : Data 시트 소분류 타입 설명에서 스트링 얻어옴 chu8820
	TCHAR*					GetTypeName( DWORD nTypeS );

	CsItem*					GetItem( DWORD dwItemID );
	CsItem::MAP*			GetItemMap(){ return &m_Map; }
	TCHAR*					GetCategory( DWORD dwCategory );
	CsItemCoolTime*			GetCoolTime( DWORD nGroupID );

	bool					IsItem( DWORD const & dwItemID ) const;
	bool					IsCoolTime( DWORD const & dwItemID ) const;
	std::wstring			GetItemName( DWORD const & nGroupID ) const;

	void					SaveBin( char* cPath );		

	int						GetDropCount(int nItemID);	// Rank테이블 load관련 추가	lks007
	const CsItemRank::MAP*	GetItemRank() { return &m_mapRank; }	// Rank테이블 load관련 추가	lks007

	CsItemExchange::MAP		GetExchangeItemMap() { return m_mapExchange; }
	CsItemExchange*			GetExchangeItem( DWORD dwNpcID, DWORD dwItemID );

	CsElementItem*			GetElementItem1( DWORD dwTypeID ){ return m_mapElementItem1[dwTypeID ];} 
	CsElementItem::MAP*		GetElementItem1Map() { return &m_mapElementItem1;}

	CsElementItem*			GetElementItem2( DWORD dwTypeID ){ return m_mapElementItem2[dwTypeID ];} 
	CsElementItem::MAP*		GetElementItem2Map() { return &m_mapElementItem2;}

	CsAccessory_Option::MAP GetAccessoryOptionMap() { return m_mapAccessoryOption; }
	CsAccessory_Option*		GetAccessoryOption( DWORD dwSkillID ){ return m_mapAccessoryOption[ dwSkillID ];} 
	CsAccessory_Enchant*	GetAccessoryEnchant( DWORD dwSkillID ){ return m_mapAccessoryEnchant[ dwSkillID ];} 

	//////////////////////////////////////////////////////////////////////////
	// Xai System Table
	//////////////////////////////////////////////////////////////////////////
public:
	bool							IsXaiItem(DWORD const& dwItemCode) const;
	TB::sXAIStruct const*			GetXaiItem_Info( DWORD const& dwItemCode ) const;

private:
	void							_LoadItemXAI_SystemTable(char* cExcel);
	void							_LoadFilePack_ItemXAISystemTable( int nHandle );
	void							_SaveBin_ItemXAISystemTable( FILE* fp );
	void							_LoadBin_ItemXAISystemTable( FILE* fp );

private:
	TB::MAP_XAIITEM_DATA			m_XaiItem_Data;
	//////////////////////////////////////////////////////////////////////////

private:
	CsItem::MAP 					m_Map;
	CsItemTap::MAP					m_ItemCategory;
	CsItemCoolTime::MAP				m_mapCoolTime;

	std::map< DWORD, DWORD >		m_MapDisp;	

	struct sTYPENAME
	{ 
		sTYPENAME()
		{
			memset( &s_szName, 0, sizeof( s_szName ) );
		}

		TCHAR s_szName[ ITEM_NAME_LEN ]; 
	};

	std::map< DWORD, sTYPENAME* >	m_MapTypeName;	// CAPSULE_EVOLUTION : Data 시트 소분류 타입 설명에서 스트링 얻어옴 chu8820

	CsItemRank::MAP					m_mapRank;	// Rank테이블 load관련 추가 

	CsItemExchange::MAP				m_mapExchange;	//물물교환상점

	CsElementItem::MAP				m_mapElementItem1; // Attribute Type
	CsElementItem::MAP				m_mapElementItem2; // Nature Type

	CsAccessory_Option::MAP			m_mapAccessoryOption;	//악세서리 옵션 시트 정보
	CsAccessory_Enchant::MAP		m_mapAccessoryEnchant;	//악세서리 옵션 시트 정보

	//////////////////////////////////////////////////////////////////////////
	// item rank effect
	MAP_RANK_EFFECT		m_mapRankEffect;

	void				_LoadRankEffect( char* cExcel );
	void				_SaveBin_RankEffect( FILE* fp );
	void				_LoadBin_RankEffect( FILE* fp );
	void				_LoadFilePack_RankEffect( int nHandle );

public:
	sRankInfo const*	GetRankInfo( DWORD dwItemCode, DWORD dwRankItemCode ) const;
	//////////////////////////////////////////////////////////////////////////

public:
	void				_LoadDigimonPartsItem(char* cExcel);
	void				_SaveBin_DpartsItem(FILE* fp);
	void				_LoadBin_DpartsItem(FILE* fp);
	void				_LoadFilePack_DpartsItem(int nHandle);

	bool				_IsDPartsItem(DWORD const& dwItemCode, DWORD const& dwDTbIdx) const;
	bool				GetDPartsInfo(DWORD const& dwItemCode, DWORD const& dwDTbIdx, int& nPartsIdx, std::string & file);

	bool				IsEnableDigimonPartsItem( DWORD const& dwItemCode, DWORD const& dwDTbIdx, int const& nPartsIdx ) const;

	bool				GetPartsIndex( DWORD const& dwItemCode, int & nPartsIdx ) const;

private:
	std::map<DWORD, sDEquipStruct> 	m_DPartItemTables;
};

