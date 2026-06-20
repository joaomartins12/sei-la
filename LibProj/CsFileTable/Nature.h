
#pragma once 

#define FT_NATURE_WORST			0
#define FT_NATURE_DOWN			51
#define FT_NATURE_EQUAL			100
#define FT_NATURE_UP			101
#define FT_NATURE_BEST			151

class CsNature
{
public:
	// 이값은 여기서만 쓰자
	enum eTYPE{
		Fire, Thunder, Steel, Wind, Wood, Water, Darkness, Earth, Ice, Light, None, MaxCount,
	};
public:
	typedef	std::map< DWORD, CsNature* >				MAP;
	typedef	std::map< DWORD, CsNature* >::iterator		MAP_IT;
public:
	struct sINFO
	{
		USHORT		s_nID;
		short		s_nElement[ MaxCount ];
	};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};

