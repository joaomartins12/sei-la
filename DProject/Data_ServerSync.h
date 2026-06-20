
#pragma once 


class cData_ServerSync
{
public:
	enum eSYNC{ DIGIMON_EVOLUTION_OPEN,
				DIGIMON_RIDE_OPEN,
				ELEMENT_ITEM_CHECK, TACTICSHOUSE_LIST,
				CASH_REFRESH,
				CASH_EXTRA_INVEN,
				CASH_EXTRA_WAREHOUSE,
				CASH_EXTRA_TACTICSHOUSE,
				CASH_CHANGE_TNAME,
				CASH_CHANGE_TAMER,
				CASH_CAPSULE_COIN_CLIENT,
				CASH_CAPSULE_COIN_SERVER,
				CASH_CAPSULE_COIN_WAIT,
				CASH_CAPSULE_COIN_COMPLEATE,

				CASH_ITEM_BUY,
				CASH_ITEM_PRESENT,

				POSTAL_REFRESH,
							
				GUILD_INVATE,
				GUILD_CREATE, GUILD_DESTROY,
				GUILD_LEAVE, GUILD_BAN,
				GUILD_AP_MASTER, GUILD_AP_2MASTER, 
				GUILD_AP_DAT, GUILD_AP_NORMAL,
				GUILD_AP_NEWBI,

				FRIEND_LIST, SHUTOUT_LIST,

				CHANGE_KFM, CHANGE_KFM_ING,
				CHANGE_PARTNER, CHANGE_PARTNER_ING,
#ifdef CROSSWARS_SYSTEM
				CROSSCHANGE_PARTNER,CROSSCHANGE_PARTNER_ING,				
				CROSSRECOVER_PARTNER, CROSSRECOVER_PARTNER_ING,
#endif
				CHANGE_PARTNER_WITHOUTSCENE,
				USE_DIGIMON_SKILL,

				DELETE_TACTICS,

				RIDE_ON, RIDE_OFF,

				TEMP_FRIEND_LOCAL_TO_SERVER,
#ifdef CROSSWARS_SYSTEM
				CASH_EXTRA_CROSSINVEN,
#endif
				REWARD_REFRESH,
				GET_REWARD,
				CASH_EXTRA_CARDINVEN,
				CASH_IDENTIFY_CARD,
				CASH_CARD_RESEAL,
				TACTICS_OPEN,
				MEMORY_SKILL_DELETE,

				MAX_SYNC_COUNT };

	struct sCHANGE_PARTNER
	{
		u4		nUID;
		u1		nNextArrIDX;
		n4		nNextType;
		n1		nNextLevel;
		u2		nNextScale;
		wchar	szNextDigimonName[Language::pLength::name];
	};

public:
	void		Delete(){}
	void		Init(){ Reset(); }
	void		Reset();
	void		Update();

protected:
	struct sSYNC
	{
		void	Reset(){ s_nRefCount = 0; s_Vector.ClearElement(); s_nEtcValue1 = 0; s_nEtcValue2 = 0; }
		void	Increase( int nRefConstant );
		void	Decrease( int nRefConstant );
		CsVectorPB< int >	s_Vector;
		int					s_nRefCount;

		int					s_nEtcValue1;
		int					s_nEtcValue2;
	};
	sSYNC		m_Sync[ MAX_SYNC_COUNT ];

public:
	void		SendServer( eSYNC sync, int nRefConstant, void* pValue1 = NULL, void* pValue2 = NULL, void* pValue3 = NULL, void* pValue4 = NULL );
	void		RecvServer( eSYNC sync, int nRefConstant, void* pValue1 = NULL, void* pValue2 = NULL, void* pValue3 = NULL, void* pValue4 = NULL );
	bool		IsEmptyRefCount( eSYNC sync ){ return ( m_Sync[ sync ].s_nRefCount == 0 ); }
	bool		IsEmptyRefCount( eSYNC sync, int nRefConstant ){ return ( IsEmptyRefCount( sync ) || m_Sync[ sync ].s_Vector.IsExistData( nRefConstant ) ); }
	sSYNC*		GetSyncData( eSYNC sync );


protected:
	void		_EnableNet( eSYNC sync, int nRefConstant, void* pValue1 = NULL, void* pValue2 = NULL, void* pValue3 = NULL, void* pValue4 = NULL );
	void		_DisableNet( eSYNC sync, int nRefConstant, void* pValue1 = NULL, void* pValue2 = NULL, void* pValue3 = NULL, void* pValue4 = NULL );


	// 자주 쓰는거
public:
	bool		IsChangeParter(){ return ( ( IsEmptyRefCount( cData_ServerSync::CHANGE_PARTNER ) == false ) ||
										   ( IsEmptyRefCount( cData_ServerSync::CHANGE_PARTNER_ING ) == false ) ); }
	bool		IsChangeKFM(){ return ( ( IsEmptyRefCount( cData_ServerSync::CHANGE_KFM ) == false ) ||
										( IsEmptyRefCount( cData_ServerSync::CHANGE_KFM_ING ) == false ) ); }
#ifdef CROSSWARS_SYSTEM
	bool		IsCrossChangeParter(){ return ( ( IsEmptyRefCount( cData_ServerSync::CROSSCHANGE_PARTNER ) == false ) ||
												( IsEmptyRefCount( cData_ServerSync::CROSSCHANGE_PARTNER_ING ) == false ) ); }
	bool		IsCrossRecover(){ return ( ( IsEmptyRefCount( cData_ServerSync::CROSSRECOVER_PARTNER ) == false ) ||
												( IsEmptyRefCount( cData_ServerSync::CROSSRECOVER_PARTNER_ING ) == false ) ); }
	bool		IsChageDigimon(){ return ( IsChangeParter() || IsChangeKFM() || IsCrossChangeParter() || IsCrossRecover() ); }
#else										
	bool		IsChageDigimon(){ return ( IsChangeParter() || IsChangeKFM() ); }
#endif
};