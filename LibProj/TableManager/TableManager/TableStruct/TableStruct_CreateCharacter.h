#pragma once

BEGIN_NAME_TABLE

struct sTamerCreateInfo
{
	sTamerCreateInfo():m_dwTalbeID(0),m_bShow(false),m_bEnable(false),m_nSeasonType(0),m_nIconIdx(0)
	{}
	~sTamerCreateInfo() {};

	DWORD				m_dwTalbeID;
	bool				m_bShow;
	bool				m_bEnable;
	int					m_nSeasonType;
	std::string			m_sVoiceFile;
	int					m_nIconIdx;
	std::list<DWORD>	m_listCostume;
};



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

END_NAME_TABLE