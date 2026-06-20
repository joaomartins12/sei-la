#pragma once 

class CsQuestMng
{
protected:
	CsQuest::MAP		m_mapQuest;

public:
	void			Delete();
	bool			Init( char* cPath );

	void			DeleteQuest( DWORD dwQuestID );
	CsQuest*		AddQuest( DWORD dwUniqID );
	CsQuest*		AddQuest( CsQuest* pSrcQuest, DWORD dwUniqID );
	CsQuest*		GetQuest( DWORD const& dwQuestID );

	bool			IsQuest( DWORD const& dwQuestID ){ return ( m_mapQuest.find( dwQuestID ) != m_mapQuest.end() ); }
	CsQuest::MAP*	GetQuestMap(){ return &m_mapQuest; }
	void			GetQuestListFormType( CsQuest::eQUEST_TYPE const& nQuestType, std::list<DWORD>& list );

	DWORD			GetLastUniqID();	 

protected:
	bool			_LoadExcel( char* cPath );
	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

public:
	void			SaveExcel( float fExpMul );
	void			SaveBin( char* cPath );

	bool			Reload( char* cPath );
};
