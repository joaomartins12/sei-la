
#pragma once 

class CsSkillMng
{
public:
	void			Delete();
	bool			Init( char* cPath );

	void			Reload( char* cPath );

protected:
	CsSkill::MAP	m_mapSkill;

	CsTamerSkill::TSKILL_MAP	m_mapTamerSkill;
	CsAreaCheck::AREACHECK_MAP	m_mapAreaCheck;

public:
	void			_LoadExcel();
	void			_LoadExcel( char* cExcel, char* cSheet, CsSkill::eSKILL_TYPE st );
	void			_LoadExcel( char* cExcel, char* cSheet );
	void			_LoadAreaCheck( char* cExcel, char* cSheet );

	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

public:
	bool			IsSkill( DWORD dwID );
	CsSkill*		GetSkill( DWORD dwID );
	CsSkill::MAP*	GetSkillMap(){ return &m_mapSkill; }
	
	bool			IsTamer_Skill( DWORD dwSkillCode );
	CsTamerSkill*	GetTamer_Skill( int nIndex, int dwSkillCode );

	bool			IsArea_Check( DWORD dwIndex ) { return ( m_mapAreaCheck.find( dwIndex) != m_mapAreaCheck.end() ); }
	CsAreaCheck*	GetArea_Check( DWORD dwIndex );
	bool			FindArea_Check( DWORD dwIndex, USHORT nMapID );

public:
	void			SaveBin( char* cPath );
	void			StringChange( __CsStringChange* pSC );
};