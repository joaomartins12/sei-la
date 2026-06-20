
#pragma once

#define FT_RIDE_OPENINFO		2

class CsRide
{
public:
	typedef	std::map< DWORD, CsRide* >				MAP;
	typedef	std::map< DWORD, CsRide* >::iterator	MAP_IT;

public:
	CsRide(){}

	// ==== Info
public:
	struct sINFO
	{
		DWORD				s_dwDigimonID;
		
		DWORD				s_dwChangeRide;
		float				s_fMoveSpeed;
		TCHAR				s_szComment[ ITEM_COMMENT_LEN ];
		int					s_nRideType;
		float				s_fAniRate_Run;

		struct sOPEN_INFO
		{
			int		s_nItemType_S;
			int		s_nNeedCount;
		};
		sOPEN_INFO			s_OpenInfo[ FT_RIDE_OPENINFO ];
	};
protected:
	sINFO				m_Info;
public:
	sINFO*				GetInfo(){ return &m_Info; }


	// ==== Base
public:
	void				Delete(){}
	void				Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }
};

