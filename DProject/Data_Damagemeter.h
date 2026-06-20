#pragma once
// 
// class cData_DamageMeter
// {
// #define MAX_DAMAGEMETER_COUNT		MAX_PARTY_COUNT+1	// 본인 포함 파티 최대 인원 수
// 
// public:
// 	cData_DamageMeter()	{ m_bIsData = false; }
// public:
// 	void			Delete();
// 	void			LogOut()	{ Delete(); }
// 	void			Init();
// 
// 	struct sDamageInfo
// 	{
// 		void			Init(){ s_nDigimonUID = 0; memset( s_szDigimonName, 0, Language::pLength::name + 1 ); 
// 								s_nDamage_Done = 0; s_nDamage_Taken = 0; s_nStartTime = 0; s_nEndTime = 0; s_bBattle = true; }
// 		sDamageInfo(){ Init(); }
// 		void			Delete(){ Init(); }
// 		u2				s_nDigimonUID;
// 		TCHAR			s_szDigimonName[ Language::pLength::name + 1 ];
// 		u4				s_nDamage_Done;
// 		u4				s_nDamage_Taken;
// 		u4				s_nStartTime;
// 		u4				s_nEndTime;
// 		bool			s_bBattle;
// 	};
// 
// 	typedef std::list< sDamageInfo* >				LIST;
// 	typedef std::list< sDamageInfo* >::iterator		LIST_IT;
// 
// protected:
// 	LIST				m_listDmgInfo;
// 	bool				m_bIsData;
// public:
// 	bool				IsData()							{ return m_bIsData; }
// 	LIST*				GetDmgInfolist()					{ return &m_listDmgInfo; }
// 	sDamageInfo*		GetDmgInfo( u2 nUID );
// 	sDamageInfo*		GetDmgInfo( TCHAR* szName );
// 
// 	void				AddDmgInfo( sDamageInfo* pInfo )	{ m_listDmgInfo.push_back( pInfo ); m_bIsData = true; }
// 	bool				DeleteDmgInfo( u2 nUID );
// 	bool				DeleteDmgInfo( TCHAR* szName );
// 
// 	void				SetStartTime( u2 nTamerUID, u4 nStartTime )		{ GetDmgInfo( nTamerUID )->s_nStartTime = nStartTime;/*m_nStartTime = nStartTime;*/ }
// 	u4					GetStartTime( u2 nTamerUID )					{ return GetDmgInfo( nTamerUID )->s_nStartTime;/*return m_nStartTime;*/ }
// 	
// 	enum eORDERTYPE{ ORDERTYPE_NONE = 0, DAMAGE_DONE = 1, DPS = 2, DAMAGE_TAKEN = 3, ORDERTYPE_MAX, };
// 	void				OrderByType( eORDERTYPE eOrderType );
// 
// 	void				_SetDamageInfo( u2 nHitterUID, u2 nTargetUID, int nDamage, u4 nCurTime );
// };