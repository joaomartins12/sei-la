

#pragma once 



class CsNpcMng
{
	//=================================================================================================================================
	//
	//		Npc 
	//
	//=================================================================================================================================
public:
	void			Delete();
	bool			Init( char* cPath );

protected:
	CsNpc::MAP		m_mapNpc;

public:
	void			DeleteNpc( DWORD dwNpcTableID );
	void			AddNpc( CsNpc* pSrcNpc );

	CsNpc*			GetNpc( DWORD dwNpcTableID );
	CsNpc*			FindNpc( DWORD nModelID );
	void			FindNpc_FromModelID(DWORD const& dwModelID, std::list<DWORD>& listNpcID );
	CsNpc::MAP*		GetNpcMap(){ return &m_mapNpc; }
	bool			IsNpc( DWORD dwNpcID );


	// 그룹
protected:
	CsNpcGroup::MAP	m_mapNpcGroup;

public:
	bool			IsGroup( DWORD dwMapID );
	CsNpcGroup*		GetGroup( DWORD dwMapID );

	//=================================================================================================================================
	//
	//		Model
	//
	//=================================================================================================================================
public:
	struct sMODEL_INFO
	{
		DWORD		s_nModelID;
		SHORT		s_nOffset[ 3 ];
		TCHAR		s_szComment[ 64 ];
	};
protected:
	std::map< DWORD, sMODEL_INFO* >		m_mapModelInfo;

public:
	sMODEL_INFO*						GetModelInfo( DWORD nModelID ){ return m_mapModelInfo[ nModelID ]; }
	std::map< DWORD, sMODEL_INFO* >*	GetModelInfoMap(){ return &m_mapModelInfo; }


	//=================================================================================================================================
	//
	//		Npc_Event
	//
	//=================================================================================================================================
#define MAX_CARD_ITEM	5

public:
	struct sITEM_INFO
	{
		DWORD	s_nItemID;
		SHORT	s_nCount;
	};

	struct sNPC_EVENT_INFO
	{
		sNPC_EVENT_INFO():s_nNpcID(0),s_nTry(0),s_nExhaustMoney(0)
			,s_dwExhaustItem(0),s_unItemCount(0)
		{};

		DWORD	s_nNpcID;			// npc id
		SHORT	s_nTry;				// 도전 횟수
		DWORD	s_nExhaustMoney;	// 소모 bit
		DWORD	s_dwExhaustItem;	// 소모 아이템 코드 번호
		unsigned int s_unItemCount;	// 소모 아이템 갯수

		sITEM_INFO	s_aItemInfo[ MAX_CARD_ITEM ];	// 지급 item 정보
	};

protected:
	std::map< DWORD, sNPC_EVENT_INFO* >		m_mapCardGameInfo;

public:
	sNPC_EVENT_INFO*						GetCardGameInfo( DWORD nNpcID );
	std::map< DWORD, sNPC_EVENT_INFO* >*	GetCardGameInfoMap() { return &m_mapCardGameInfo; }



	//=================================================================================================================================
	//
	//		Save Load
	//
	//=================================================================================================================================	
public:
	void			Save_Npc();
	void			Save_ModelInfo();

	void			Reload_ZoomOffset();
protected:
	void			_PreSave();
	void			_LoadExcel();

	void			_LoadExcel_Npc();
	void			_LoadExcel_ModelInfo();

	void			_LoadExcel_Gotcha_Machine();
	void			_LoadExcel_Gotcha_Item();
public:
	void			SaveBin( char* cPath );
protected:
	void			_SaveBin_Npc( FILE *fp );
	void			_SaveBin_ModelInfo( FILE *fp );

	bool			_LoadBin( char* cPath );
	void			_LoadBin_Npc( FILE* fp );
	void			_LoadBin_ModelInfo( FILE* fp );

	void			_LoadFilePack( char* cPath );
	void			_LoadFilePack_Npc( int nHandle );
	void			_LoadFilePack_ModelInfo( int nHandle );

	// 피에몬 카드 게임
	void			_LoadExcel_Npc_Event();
	void			_SaveBin_NpcEvent( FILE* fp );
	void			_LoadBin_NpcEvent( FILE* fp );
	void			_LoadFilePack_NpcEvent( int nHandle );
};

