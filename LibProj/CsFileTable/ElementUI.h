
#pragma once

#define ELEMENT_UI_PATH				128

class CsAttributeTypeUI
{
public:
	typedef	std::map< DWORD, CsAttributeTypeUI* >				MAP;
	typedef	std::map< DWORD, CsAttributeTypeUI* >::iterator		MAP_IT;

public:
	CsAttributeTypeUI(){}

	// ==== Info
public:
	struct sINFO
	{
		DWORD		s_nAttID;		
		char		s_cPath[ ELEMENT_UI_PATH ];
		int			s_nX;
		int			s_nY;
		int			s_nBarX;
		int			s_nBarY;
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


class CsNatureTypeUI
{
public:
	typedef	std::map< DWORD, CsNatureTypeUI* >				MAP;
	typedef	std::map< DWORD, CsNatureTypeUI* >::iterator		MAP_IT;

public:
	CsNatureTypeUI(){}

	// ==== Info
public:
	struct sINFO
	{
		DWORD		s_nNatID;		
		char		s_cPath[ ELEMENT_UI_PATH ];
		int			s_nX;
		int			s_nY;
		int			s_nGaugeX;
		int			s_nGaugeY;
		float		s_nAngle;
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