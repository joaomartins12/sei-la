
#pragma once 


class CsMapMonsterGroup
{
public:
	typedef	std::map< DWORD, CsMapMonsterGroup* >				MAP;
	typedef	std::map< DWORD, CsMapMonsterGroup* >::iterator		MAP_IT;

protected:
	CsMapMonster::LIST		m_listMapMonster;

public:
	void			Delete();
	void			Init();

	void				AddMonster( CsMapMonster* pSrcMonster );

	CsMapMonster*		GetMonster( int nListIndex );
	CsMapMonster*		GetMonster_ByMonsterID( DWORD dwMonsterID );

	void				DeleteMonster( int nDeleteListIndex );
	CsMapMonster::LIST*	GetMonsterList(){ return &m_listMapMonster; }

	void				Clone( CsMapMonsterGroup* pSrcGroup );
};
