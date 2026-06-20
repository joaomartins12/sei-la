#pragma once

#include "TableStruct_ExtraEvolution.h"

class CsExtraEvolutionMng
{
public:
	CsExtraEvolutionMng();
	~CsExtraEvolutionMng();

	bool					Init( char* cPath );
	void					Delete();

public:
	bool					LoadFilePack( char* cPath );
	void					SaveBin( char* cPath );

	TB::MAP_EXTRAEVOLUTION_DATA const*	GetExtraEvolData( DWORD const& dwNpcID ) const;

private:
	bool					_LoadExcel();
	bool					_LoadBin(char* cPath);
	bool					_LoadFilePack(char* cPath);

private:
	std::map<DWORD/*NPC IDX*/,TB::MAP_EXTRAEVOLUTION_DATA>		m_mapExtraEvoData;
};

