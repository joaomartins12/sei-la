
#pragma once 

struct sTamerCreateInfo
{
	sTamerCreateInfo():m_dwTalbeID(0),m_bShow(false),m_bEnable(false),m_nSeasonType(0),m_nIconIdx(0)
	{}
	~sTamerCreateInfo() {};

	DWORD				m_dwTalbeID;
	bool				m_bShow;
	bool				m_bEnable;
	int					m_nIconIdx;
	int					m_nSeasonType;
	std::string			m_sVoiceFile;
	std::list<DWORD>	m_listCostume;
};

typedef std::list<sTamerCreateInfo>					LIST_TAMER_CREATE_INFO;
typedef std::list<sTamerCreateInfo>::iterator		LIST_TAMER_CREATE_INFO_IT;
typedef std::list<sTamerCreateInfo>::const_iterator	LIST_TAMER_CREATE_INFO_CIT;


struct sDigimonCreateInfo
{
	sDigimonCreateInfo():m_dwTalbeID(0),m_bShow(false),m_bEnable(false)
	{}
	~sDigimonCreateInfo(){};

	DWORD		m_dwTalbeID;
	bool		m_bShow;
	bool		m_bEnable;
	std::string	m_sVoiceFile;
};

typedef std::list<sDigimonCreateInfo>					LIST_DIGIMON_CREATE_INFO;
typedef std::list<sDigimonCreateInfo>::iterator			LIST_DIGIMON_CREATE_INFO_IT;
typedef std::list<sDigimonCreateInfo>::const_iterator	LIST_DIGIMON_CREATE_INFO_CIT;


class CsCharCreateTableMng
{
public:
	CsCharCreateTableMng();
	~CsCharCreateTableMng();

	void			Delete();
	bool			Init( char* cPath );
	void			SaveBin( char* cPath );

	LIST_TAMER_CREATE_INFO const&	GetTamerCreateInfo() const;
	LIST_DIGIMON_CREATE_INFO const&	GetDigimonCreateInfo() const;

private:
	void			_LoadExcel_TamerCreate();
	void			_LoadExcel_DigimonCreate();

	void			_SaveBin_TamerCreateInfo( FILE* fp );
	void			_SaveBin_DigimonCreateInfo( FILE* fp );

	bool			_LoadBin( char* cPath );
	void			_LoadBin_TamerCreateInfo( FILE *fp );
	void			_LoadBin_DigimonCreateInfo( FILE *fp );

	bool			_LoadFilePack( char* cPath );
	void			_LoadFile_TamerCreateInfo( int nHandle );
	void			_LoadFile_DigimonCreateInfo( int nHandle );

private:
	LIST_TAMER_CREATE_INFO		m_listTamerCreateInfos;
	LIST_DIGIMON_CREATE_INFO	m_listDigimonCreateInfos;
};