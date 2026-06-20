
#pragma once 

class cToolResource
{
public:
#define TOOLRESOURCE_VERSION		0x00010000
	struct sHEADER
	{
		DWORD	s_dwVersion;
		int		s_nCount;
	};
	struct sMAPPING
	{
		TCHAR	s_szPath[ MAX_PATH ];
	};
	CsMemPool< sMAPPING >		m_mempoolMapping;
	std::list< sMAPPING* >		m_listMapping;
	struct sOBJECT
	{
		TCHAR	s_szPath[ MAX_PATH ];
	};
	CsMemPool< sOBJECT >		m_mempoolObject;
	std::list< sOBJECT* >		m_listObject;


public:
	void		SaveMapping();
	void		SaveObject();
	void		LoadMapping();
	void		LoadObject();
};

extern cToolResource		g_ToolResource;