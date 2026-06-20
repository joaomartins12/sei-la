
#pragma once 

#define MAX_GROUP_ITEM	10

class CsGotcha_Machine
{
public:
	typedef	std::map< DWORD, CsGotcha_Machine* >					MAP;
	typedef	std::map< DWORD, CsGotcha_Machine* >::iterator		MAP_IT;
public:
	struct sINFO
	{
		DWORD		s_dwNpc_Id;	
		DWORD		s_dwUseItem_Code;
		USHORT		s_nUseItem_Cnt;
		bool			s_bLimit;

		UINT			s_nStart_Date;
		UINT			s_nEnd_Date;
		UINT			s_nStart_Time;
		UINT			s_nEnd_Time;

		USHORT		s_nMin_Lv;
		USHORT		s_nMax_Lv;		
		int			s_nShowCountUI;
	};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};

class CsGotcha_Item
{
public:
	typedef	std::map< DWORD, CsGotcha_Item* >					MAP;
	typedef	std::map< DWORD, CsGotcha_Item* >::iterator		MAP_IT;
public:
	struct sINFO
	{
		USHORT		s_nGroup_Id;	
		DWORD		s_dwItem_Code[MAX_GROUP_ITEM];
		USHORT		s_nItem_Cnt[MAX_GROUP_ITEM];
	};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};

class CsGotcha_RareItem
{
public:
	typedef	std::map< DWORD, CsGotcha_RareItem* >					MAP;
	typedef	std::map< DWORD, CsGotcha_RareItem* >::iterator		MAP_IT;
public:
	struct sINFO
	{
		UINT		s_nNpcID;
		TCHAR	s_szRareItemName[ ITEM_NAME_LEN ];
		UINT		s_nRareItem;	
		UINT		s_nRareItemCnt;
		UINT		s_nRareItemGive;
	};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }
	sINFO*			GetInfo(){ return &m_Info; }

};

class CsGotcha_Mystery_Item
{
public:
	typedef	std::map< DWORD, CsGotcha_Mystery_Item* >					MAP;
	typedef	std::map< DWORD, CsGotcha_Mystery_Item* >::iterator		MAP_IT;
public:
	struct sINFO
	{
		USHORT			s_nGroup;		//그룹번호
		UINT				s_nItem;			//아이템번호
		USHORT			s_nEffect;		//레어머신 이펙트 보일지 여부( 0 : 일반, 1 : 이펙트 )
	};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};

class CsGotcha_Mystery_Coin
{
public:
	typedef	std::map< DWORD, CsGotcha_Mystery_Coin* >					MAP;
	typedef	std::map< DWORD, CsGotcha_Mystery_Coin* >::iterator		MAP_IT;
public:
	struct sINFO
	{
		USHORT		s_nCoinOrder;		//코인 우선순위
		UINT			s_nCoinIdx;			//코인 아이템 번호
		USHORT		s_nCoinCnt;		//코인 소모 개수
#ifdef EGG_PREFERMENT
		UINT		s_nMoney;			// 소모되는 돈
		USHORT		s_nStartGroupNo;	// 시작 그룹 번호
		USHORT		s_nEndGroupNo;		// 종료 그룹 번호
#endif
	};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};
