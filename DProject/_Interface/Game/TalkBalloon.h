

#pragma once 

class cTalkBalloon : public NiMemObject
{
#define LIVE_UNLIMIT		-1.0f

public:
	enum eTYPE{ TYPE_1, TYPE_2, TYPE_3, TYPE_4, TYPE_5, 
#ifdef CONSIGNMENT_BALLOON_CASH
		TYPE_6,
#endif
		MAX_TYPE, };	
public:
	struct sBALLOONE : public NiMemObject
	{
		enum INIT_TYPE{ NEW, EXIST, };
		enum LIVE_TYPE{ INCREASE, DECREASE, };
		enum OBJ_TYPE { eNOROBJ = 0, eNEWOBJ, eNPCOBJ, ePERSONSTORE_OBJ };

		static void			GlobalShotDown();
		static void			DeleteInstance( sBALLOONE* pInst );
#ifdef CONSIGNMENT_BALLOON_CASH
		static sBALLOONE*	NewInstance( eTYPE type, OBJ_TYPE ObjType, NiColor nameColor, TCHAR* szFileName );
#else
		static sBALLOONE*	NewInstance( eTYPE type, OBJ_TYPE ObjType, NiColor nameColor );
#endif
		static std::queue< sBALLOONE* >		g_MemPool[ MAX_TYPE ];


		bool			Init( uint nTargetUID, TCHAR const* szMsg, INIT_TYPE init, float fLifeTime );
		bool			Update(float const& fDeltaTime);
		void			SetTargetAlpha( float fAlpha );

		eTYPE			s_eType;
		uint			s_nTargetUID;
		cText*			s_pName;
		cSprite*		s_pBalloon1;
		cSprite*		s_pBalloon2;
		cSprite*		s_pBalloon3;
		cSprite*		s_pBalloonDiscript;
		cSprite*		s_pBalloonTail;
#ifdef CONSIGNMENT_SALE
		cSprite*		s_pBalloon5;	// 상점 찾기 전용 Balloon
		cSprite*		s_pBalloon6;
		cSprite*		s_pBalloon7;
		bool			s_bStoreSrech;
#endif
		cStringList		s_StringList;
		CsPoint			s_vSize;
		CsPoint			s_vPos;
		float			s_fDepth;		
		bool			s_bFind;

		LIVE_TYPE		s_eCurLive;
		float			s_fLiveTime;
		float			s_fAlpha;

		OBJ_TYPE		s_eObjType;

#ifdef CONSIGNMENT_BALLOON_CASH
		bool			s_bCashBalloon;
#endif
	};

public:
	static	void		GlobalInit();
	static	void		GlobalShotDown();

public:
	void				Init();
	void				PreResetMap();
	void				Delete();
	void				Delete( uint nTargetUID );

	sBALLOONE*			SetBalloone( eTYPE type, uint nTargetUID, TCHAR const* szMsg, bool bExistDelete = true, sBALLOONE::OBJ_TYPE Objtype = sBALLOONE::eNOROBJ, float fLifeTime = 5.0f );

	void				Update(float const& fDeltaTime);
	void				Render();

	sBALLOONE*			GetData( int nTargetUID );

#ifdef CONSIGNMENT_SALE
	std::list< sBALLOONE* > GetBallooneList() { return m_BallooneList; }
#endif

	int					FindBallooneName( std::wstring const& strFindName );

protected:
	void				_Cal3D_to_2D();
	void				_Sort();

protected:
	std::list< sBALLOONE* >		m_BallooneList;
	CsVectorPB< sBALLOONE* >	m_vpSort;
	CsVectorPB< sBALLOONE* >	m_vpRender;
	int							m_nNumSortItem;
	int							m_nMaxSortItem;
};

extern cTalkBalloon*	g_pTalkBallone;