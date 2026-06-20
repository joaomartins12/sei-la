
#pragma once 

class CsTactics
{
public:
	typedef	std::map< DWORD, CsTactics* >				MAP;
	typedef	std::map< DWORD, CsTactics* >::iterator		MAP_IT;
public:
	enum DATA_GRADE_FLAG
	{
		DATA_LV1,	// 하급

#ifdef TACTICS_USE_DATA_LV2
		DATA_LV2,	// 중급
#endif
#ifdef TACTICS_USE_DATA_LV3
		DATA_LV3,	// 고급
#endif
#ifdef TACTICS_USE_DATA_LV4
		DATA_LV4,	// 최고급
#endif
#ifdef TACTICS_USE_DATA_LV5
		DATA_LV5,	// 최고급2
#endif
		DATA_MAX,
	};
	struct sINFO
	{
		int			s_nDigimonID;
		int			s_nReqItemS_Type[DATA_MAX];		// 필요 DATA 아이템 ID
		USHORT		s_nReqItemCount[DATA_MAX];		// 필요 개수
		BYTE		s_nLimitLevel[DATA_MAX];		// 제한 단계
		BYTE		s_nViewWarning[DATA_MAX];		// 경고 단계
	};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};

