
#pragma once 

class CsFileTableMng
{
public:
	CsFileTableMng();

public:
	char*			GetLanguagePath( nsCsFileTable::eLANGUAGE eLanguage, char* cBuffer );

public:
	void			Delete();
	bool			Init( nsCsFileTable::eFILE_TYPE eFileType, nsCsFileTable::eLANGUAGE eLanguage );

	void			SaveBin( nsCsFileTable::eLANGUAGE eLanguage );

	void			StringChange( nsCsFileTable::eLANGUAGE eLanguage );
};