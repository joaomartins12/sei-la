#pragma once 

class CsMaster_Card
{
public:
	enum {FT_CARD_NORMAL = 0, FT_CARD_BRONZE, FT_CARD_SHILVER, FT_CARD_GOLD, FT_CARD_PLATINUM, FT_CARD_MASTER, FT_CARD_MAX };
	typedef	std::map< DWORD, CsMaster_Card* >				MAP;
	typedef	std::map< DWORD, CsMaster_Card* >::iterator		MAP_IT;
public:
	struct GradeInfo 
	{
		DWORD		s_nItem;
		USHORT		s_nIcon;
		USHORT		s_nMax;
		USHORT		s_nIdentiQuantity;
		USHORT		s_nEff1;
		USHORT		s_nEff1val;
		USHORT		s_nEff2;
		USHORT		s_nEff2val;

	};
	struct sINFO
	{		
		DWORD		s_nID;
		TCHAR		s_szName[ MAX_FILENAME ];
		DWORD		s_nDigimonID;
		USHORT		s_nIcon;
		DWORD		s_nLeader;
		USHORT		s_nScale;


		GradeInfo	s_stGradeInfo[FT_CARD_MAX];
	};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};

class CsMaster_CardLeader
{
public:
	typedef	std::map< DWORD, CsMaster_CardLeader* >				MAP;
	typedef	std::map< DWORD, CsMaster_CardLeader* >::iterator		MAP_IT;
public:
	struct sINFO
	{
		DWORD		s_nID;
		DWORD		s_nDigimonID;
		USHORT		s_nPetID;
		DWORD		s_nAni1;
		DWORD		s_nAni2;
		DWORD		s_nSpecial1;
		DWORD		s_nSpecial2;

		DWORD		s_nAbil1;
		DWORD		s_nAbil2;
		DWORD		s_nAbil3;
		DWORD		s_nAbil4;
	};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};


#define		ABILITY_MAX		3

class CsMaster_CardLeaderAbility
{
public:
	typedef	std::map< DWORD, CsMaster_CardLeaderAbility* >				MAP;
	typedef	std::map< DWORD, CsMaster_CardLeaderAbility* >::iterator		MAP_IT;
public:
	struct sTerms
	{
		DWORD		s_nterm;
		DWORD		s_ntermval;
		DWORD		s_nEff;

	};

	struct sINFO
	{
		USHORT		s_nID;
		sTerms		s_stTerms[ABILITY_MAX];

	};

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }

	sINFO*			GetInfo(){ return &m_Info; }
};


class CsMaster_Filter
{
public:
	struct sINFO
	{		
		DWORD			s_nMapGroupID;
		DWORD			s_nMapAreaID;
		DWORD			s_nSealItemID;
		//std::wstring	s_sGroupName;
		//std::wstring	s_sMapName;
		//std::wstring	s_sSealName;
		void			SetInfo(sINFO& src);
	};

	typedef	std::map< DWORD, sINFO* >							Seal;
	typedef	std::map< DWORD, sINFO* >::iterator					Seal_IT;	

	struct sMap
	{
		DWORD	s_nMapID;
		Seal	s_sSeal;
	};
	typedef	std::map< DWORD, sMap* >							Map;
	typedef	std::map< DWORD, sMap* >::iterator					Map_IT;

	struct sMapGroup
	{
		DWORD	s_nMapGroupID;
		Map		s_sMap;
	};

	typedef	std::map< DWORD, sMapGroup* >							MapGroup;
	typedef	std::map< DWORD, sMapGroup* >::iterator					MapGroup_IT;

protected:
	sINFO			m_Info;
public:
	void			Delete(){}
	//void			Init( sINFO* pSrcInfo ){ memcpy( &m_Info, pSrcInfo, sizeof( sINFO ) ); }	
};

namespace neSealMaster
{
	struct sPlatePath
	{	
		sPlatePath():s_nID(0) {};
		int				s_nID;
		std::wstring	s_szName;
		std::string		s_szNifFilePath;
		std::string		s_szGradeBackImagePath;
	};

	struct sDigimonImgPath
	{
		sDigimonImgPath():s_nDigimon(0){};

		DWORD			s_nDigimon;
		std::string		s_szDigimonSealImgPath;
	};

	struct sAttribtue
	{
		int				s_nType;
		std::string		s_nFilePath;
	};

	struct sElemental
	{
		int				s_nType;
		std::string		s_nFilePath;
	};
};

typedef std::map<int, neSealMaster::sPlatePath>						MAP_SEALMASTER_PLATHPATH;
typedef std::map<int, neSealMaster::sPlatePath>::iterator			MAP_SEALMASTER_PLATHPATH_IT;
typedef std::map<int, neSealMaster::sPlatePath>::const_iterator		MAP_SEALMASTER_PLATHPATH_CIT;

typedef std::map<DWORD, neSealMaster::sDigimonImgPath>					MAP_SEALMASTER_DIGIIMGPATH;
typedef std::map<DWORD, neSealMaster::sDigimonImgPath>::iterator		MAP_SEALMASTER_DIGIIMGPATH_IT;
typedef std::map<DWORD, neSealMaster::sDigimonImgPath>::const_iterator	MAP_SEALMASTER_DIGIIMGPATH_CIT;

typedef std::map<int, neSealMaster::sAttribtue>					MAP_SEALMASTER_ATTRIBUTE;
typedef std::map<int, neSealMaster::sAttribtue>::iterator		MAP_SEALMASTER_ATTRIBUTE_IT;
typedef std::map<int, neSealMaster::sAttribtue>::const_iterator	MAP_SEALMASTER_ATTRIBUTE_CIT;

typedef std::map<int, neSealMaster::sElemental>					MAP_SEALMASTER_ELEMENTAL;
typedef std::map<int, neSealMaster::sElemental>::iterator		MAP_SEALMASTER_ELEMENTAL_IT;
typedef std::map<int, neSealMaster::sElemental>::const_iterator	MAP_SEALMASTER_ELEMENTAL_CIT;