
#pragma once 

class CsMapMonster
{
public:
	enum eMONSTER_MOVE
	{
		MT_NONE		=	0,
		MT_NORMAL	=	1,
		MT_MOVE		=	2,
		MT_HOLD		=	3,
	};
public:
	typedef	std::list< CsMapMonster* >				LIST;
	typedef	std::list< CsMapMonster* >::iterator	LIST_IT;

	// ===== Info
public:
	struct sINFO
	{
		void		Init();
		DWORD		GetModelID();
		DWORD		s_dwMapID;
		DWORD		s_dwMonsterTableID;
		int			s_nCenterX;
		int			s_nCenterY;
		int			s_nRadius;
		int			s_nCount;
		int			s_nResponTime;

		int			s_nKillgen_MonFTID;
		int			s_nKillgen_Count;
		int			s_nKillgen_ViewCnt;
		int			s_nMoveType;
		bool		s_bInsRespon;// 인던에서 리젠 가능 한지 여부 체크
	};
protected:
	sINFO			m_Info;
public:
	sINFO*			GetInfo(){ return &m_Info; }

	// ===== Base Func
public:
	void			Delete();
	void			Init( sINFO* pSrcInfo );

	// sms temp
public:
	DWORD			GetModelID(){ return m_Info.GetModelID(); }
};

