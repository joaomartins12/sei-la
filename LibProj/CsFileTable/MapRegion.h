
#pragma once 

#define FT_MAP_REGION_DISC_LEN		128

class CsMapRegion
{
public:
	typedef	std::list< CsMapRegion* >				LIST;
	typedef	std::list< CsMapRegion* >::iterator		LIST_IT;

	// ===== Info
public:
	struct sINFO
	{
		void		Init( DWORD dwMapID ){
			s_dwMapID = dwMapID;
			s_nCenterX = s_nCenterY = 0;
			s_nRadius = 500;
			s_szDiscript[ 0 ] = NULL;
			s_szDiscript_Eng[ 0 ] = NULL;
			s_cBGSound[ 0 ] = NULL;
			s_nFatigue_Type = s_nFatigue_DeBuff = s_nFatigue_StartTime = s_nFatigue_AddTime = s_nFatigue_AddPoint = 0;
		}

		DWORD		s_dwMapID;
		int			s_nCenterX;
		int			s_nCenterY;
		int			s_nRadius;
		TCHAR		s_szDiscript[ FT_MAP_REGION_DISC_LEN ];
		TCHAR		s_szDiscript_Eng[ FT_MAP_REGION_DISC_LEN ];
		char		s_cBGSound[ MAX_FILENAME ];

		USHORT		s_nFatigue_Type;
		USHORT		s_nFatigue_DeBuff;
		USHORT		s_nFatigue_StartTime;
		USHORT		s_nFatigue_AddTime;
		SHORT		s_nFatigue_AddPoint;
	};
protected:
	sINFO			m_Info;
public:
	sINFO*			GetInfo(){ return &m_Info; }

	// ===== Base Func
public:
	void			Delete();
	void			Init( sINFO* pSrcInfo );
};

class CsMap_LimitEvo
{
public:
	typedef	std::list< CsMap_LimitEvo* >			LIST;
	typedef	std::list< CsMap_LimitEvo* >::iterator	LIST_IT;

	// ===== Info
public:
	struct sINFO
	{
		void		Init( DWORD dwMapID ){
			s_dwMapID = dwMapID;
			s_nCenterX = s_nCenterY = 0;
			s_nRadius = 500;
		}

		DWORD		s_dwMapID;
		int			s_nCenterX;
		int			s_nCenterY;
		int			s_nRadius;
	};
protected:
	sINFO			m_Info;
public:
	sINFO*			GetInfo(){ return &m_Info; }

	// ===== Base Func
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }
};

class CsMap_BuffRegion
{
public:
	typedef	std::list< CsMap_BuffRegion* >			LIST;
	typedef	std::list< CsMap_BuffRegion* >::iterator	LIST_IT;
public:
	struct sINFO
	{
		int regionIdx;
		DWORD	mapidx;
		int centerx;
		int centery;
		int radius;    // centerx&& centery&& radius == -1 이면 전역버프
		int act;
		int buffidx;
		int time;
		int layerno;			// 0 ~ 4
		int applytype;
		int requirement_1;
		int requirement_2;

		int objuniqid;
		//std::wstring modelpath;

		void Init(DWORD dwMapid)
		{
			mapidx = dwMapid;
			centerx = -1;
			centery = -1;
			radius = 100;
			act = 1;
			buffidx = 0;
			time = 0;
			layerno = 0;
			applytype = 0;
			requirement_1 = 0;
			requirement_2 = 0;

			objuniqid= -1;
			regionIdx = 0;
			//modelpath.clear();
		}
	};
protected:
	sINFO			m_Info;
public:
	sINFO*			GetInfo(){ return &m_Info; }
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }
};
