#pragma once

class CsAddExpMng
{
public:
	CsAddExpMng(void);
	~CsAddExpMng(void);

	struct sAddExp
	{
		DWORD	s_nID;
		DWORD	s_nSkillID;
		int		s_nIconID;	

		TCHAR	s_szTitle[ ITEM_NAME_LEN ];
		TCHAR	s_szComment[ ITEM_COMMENT_LEN ];
	};

public:
	std::map< DWORD, sAddExp* >	m_mapExp;

public:
	void			Delete();
	bool			Init( char* cPath );

	bool			IsData( int nID ){ return ( m_mapExp.find( nID ) != m_mapExp.end() ); }
	sAddExp*		GetData( int nID ) { assert_cs( IsData( nID ) ); return m_mapExp[ nID ]; }	
	sAddExp*		FindData_FromSkill( DWORD dwSkill );
	void			SaveBin( char* cPath );

	std::map< DWORD, sAddExp* >*	GetMap(){ return &m_mapExp; }

protected:
	void			_LoadExcel();	
	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

};