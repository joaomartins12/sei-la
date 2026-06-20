

#pragma once 

class cTalkBalloon : public CEventCall_Base
{
public:
	enum eTYPE{ TYPE_1, TYPE_2, TYPE_3, MAX_TYPE, };
public:
	struct sBALLOONE : public NiMemObject
	{
		enum INIT_TYPE{ NEW, EXIST, };
		enum LIVE_TYPE{ INCREASE, DECREASE, };
		enum OBJ_TYPE { eNOROBJ = 0, eNEWOBJ, eNPCOBJ, };

		static void			GlobalShotDown();
		static void			DeleteInstance( sBALLOONE* pInst );
		static sBALLOONE*	NewInstance( eTYPE type, OBJ_TYPE ObjType );
		static std::queue< sBALLOONE* >		g_MemPool[ MAX_TYPE ];

		void			Init( uint nTargetUID, TCHAR* szMsg, INIT_TYPE init );
		bool			Update();
		void			SetTargetAlpha( float fAlpha );

		eTYPE			s_eType;
		uint			s_nTargetUID;
		cText*			s_pName;
		cSprite*		s_pBalloon1;
		cSprite*		s_pBalloon2;
		cSprite*		s_pBalloon3;
		cSprite*		s_pBalloonDiscript;
		cSprite*		s_pBalloonTail;
		cStringList		s_StringList;
		CsPoint			s_vSize;
		CsPoint			s_vPos;
		float			s_fDepth;		
		bool			s_bFind;

		LIVE_TYPE		s_eCurLive;
		float			s_fLiveTime;
		float			s_fAlpha;

		OBJ_TYPE		s_eObjType;
	};

public:
	virtual void	EventTalk( UINT nTargetUID, TCHAR* szMsg, bool bExistDelete, int ObjType );

public:
	static	void		GlobalInit();
	static	void		GlobalShotDown();	

public:
	void				Init();
	void				PreResetMap();
	void				Delete();

	void				SetBalloone( eTYPE type, uint nTargetUID, TCHAR* szMsg, bool bExistDelete = true, sBALLOONE::OBJ_TYPE Objtype = sBALLOONE::eNOROBJ );

	void				Update();
	void				Render();

protected:
	void				_Cal3D_to_2D();
	void				_Sort();

protected:
	std::list< sBALLOONE* >		m_BallooneList;
	CsVectorPB< sBALLOONE* >	m_vpSort;
	CsVectorPB< sBALLOONE* >	m_vpRender;
	int							m_nNumSortItem;
	int							m_nMaxSortItem;

	CsC_AvObject*  m_pTarget;
};

extern cTalkBalloon*	g_pTalkBallone;