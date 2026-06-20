
#pragma once 

#ifdef VERSION_USA
#define BUFF_NAME_LEN				64
#else
#define BUFF_NAME_LEN				32
#endif
#define BUFF_COMMENT_LEN			128
#define BUFF_EFFECT_LEN				64
#define BUFF_APPLY_MAX_COUNT		3

class CsBuff
{	
public:
	typedef	std::map< USHORT, CsBuff* >					MAP;
	typedef	std::map< USHORT, CsBuff* >::iterator		MAP_IT;

public:
	struct sINFO
	{
		USHORT		s_dwID;
		TCHAR		s_szName[ BUFF_NAME_LEN ];
		TCHAR		s_szComment[ BUFF_COMMENT_LEN ];		

		USHORT		s_nBuffIcon;
		USHORT		s_nBuffType;
		USHORT		s_nBuffLifeType;
		USHORT		s_nBuffTimeType;	
		
		USHORT		s_nMinLv;		
		USHORT		s_nBuffClass;
		DWORD		s_dwSkillCode;	
		DWORD		s_dwDigimonSkillCode;	
		bool		s_bDelete;
		char		s_szEffectFile[ BUFF_EFFECT_LEN ];

		USHORT		s_nConditionLv;
	};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};

