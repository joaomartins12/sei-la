

#pragma once 

#include "Monster.h"

class CsMonsterMng
{

public:
	void				Delete();
	bool				Init( char* cPath );

	
protected:
	CsMonster::MAP				m_mapMonster;
	CsMonsterDrop::MAP			m_mapMonsterDrop;
	std::map< int, int >		m_mapMonsterHit; 
	CsMonsterSkill::MAP			m_mapMonsterSkill;
	CsMonsterSkillTerms::MAP	m_mapMonsterSkillTerms;

protected:
	void				_LoadExcel();
	void				_LoadExcelDrop();
	void				_LoadExcelHit();
	void				_LoadExcelSkill();
	void				_LoadExcelSkillTerms();
	bool				_LoadBin( char* cPath );
	void				_LoadFilePack( char* cPath );

public:
	CsMonster*			GetMonster( DWORD dwMonsterTableID );
	CsMonster::MAP*		GetMonsterMap(){ return &m_mapMonster; }

	CsMonsterSkill::sINFO*		GetMonsterSkill( USHORT nSkill_IDX );
	CsMonsterSkill::MAP*		GetMonsterSkillMap(){ return &m_mapMonsterSkill; }
	float						GetEffectVal_Scale( int nIdx );
	int							GetBuffStack( int nIdx, int nCurVal  );
	CsMonsterSkillTerms::sINFO*		GetMonsterSkillTerms( USHORT nIDX );
	CsMonsterSkillTerms::MAP*		GetMonsterSkillTermsMap(){ return &m_mapMonsterSkillTerms; }

	int					GetMonsterHit( int nLv ) { return ( m_mapMonsterHit.find( nLv ) != m_mapMonsterHit.end() ) ? m_mapMonsterHit[ nLv ] : 0; }
	bool				IsMonster( DWORD dwMonsterID ){ return ( m_mapMonster.find( dwMonsterID ) != m_mapMonster.end() ); }

public:
	void				SaveBin( char* cPath );
};

