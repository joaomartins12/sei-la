
#pragma once 

#include "TotalExcel.h"

class CsTotalExcelMng
{
public:
	enum eTYPE{ SKILL, TALK_TIP, TALK_DIGIMON, TALK_EVENT, TALK_MESSAGE, TACTICS_EXPLAIN, ITEM_NAME, ITEM_COMMENT, PARCINGWORD, WORLDMAP,
				REGION, NPC_PENNAME, NPC, QUEST, HELPTEXT, TAMER, MAX_COUNT, };

public:
	void			Delete();
	bool			Init( char* cPath );
	char*			GetSheetName( int nSheetID );

protected:
	std::map< DWORD, CsTotalExcel::MAP >	m_Map;
	CsTotalExcel							m_Dummy;

protected:
	void				_LoadExcel();
	void				_LoadExcel( YExcel::BasicExcel* pExcel, eTYPE eType );

	void				_LoadBin();

public:
	bool				IsExist( eTYPE type, DWORD nID );
	bool				IsExistKor( eTYPE type, TCHAR* szKor );
	bool				IsExistKor( eTYPE type, DWORD nID, TCHAR* szKor );

	void				New( eTYPE type, DWORD nID, TCHAR* szKor );

	CsTotalExcel::MAP*	GetMap( eTYPE type ){ return &m_Map[ type ]; }
	bool				IsData( eTYPE type, DWORD nKey ){ return m_Map[ type ].find( nKey ) != m_Map[ type ].end(); }
	CsTotalExcel*		GetData( eTYPE type, DWORD nKey );
};

