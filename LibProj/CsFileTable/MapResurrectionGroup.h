
#pragma once 

class CsMapResurrectionGroup
{
public:
	typedef	std::map< DWORD, CsMapResurrectionGroup* >				MAP;
	typedef	std::map< DWORD, CsMapResurrectionGroup* >::iterator	MAP_IT;

protected:
	CsMapResurrection::LIST		m_list_MapResurrection;

public:
	void						Delete();
	void						Init();

	void						AddResurrection( CsMapResurrection::sINFO* pSrcInfo );
	void						DeleteResurrection( CsMapResurrection* pSrc );
	CsMapResurrection::LIST*	GetResurrectionList(){ return &m_list_MapResurrection; }
	void						Clone( CsMapResurrectionGroup* pSrcGroup );
};



