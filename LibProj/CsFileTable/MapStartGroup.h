
#pragma once 

class CsMapStartGroup
{
public:
	typedef	std::map< DWORD, CsMapStartGroup* >				MAP;
	typedef	std::map< DWORD, CsMapStartGroup* >::iterator	MAP_IT;

protected:
	CsMapStart::LIST	m_list_MapStart;

public:
	void				Delete();
	void				Init();

	void				AddStart( CsMapStart::sINFO* pSrcInfo );
	void				DeleteStart( CsMapStart* pMapStart );
	CsMapStart::LIST*	GetStartList(){ return &m_list_MapStart; }	
	void				Clone( CsMapStartGroup* pSrcGroup );
};

