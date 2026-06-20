
#pragma once 

class CsMapCharLightGroup
{
public:
	typedef	std::map< DWORD, CsMapCharLightGroup* >				MAP;
	typedef	std::map< DWORD, CsMapCharLightGroup* >::iterator	MAP_IT;

protected:
	CsMapCharLight::LIST	m_List;

public:
	void				Delete();
	void				Init();

	void				Add( CsMapCharLight::sINFO_BASE* pSrcInfo );

	CsMapCharLight::LIST*	GetList(){ return &m_List; }
};

