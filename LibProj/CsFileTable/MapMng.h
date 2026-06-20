
#pragma once 

class CsMapMng
{
public:
	void			Delete();
	bool			Init( char* cPath );

	void			Save( bool bOther, bool bList, bool bPortal, bool bCharLight );

	void			SaveBin( char* cPath );
};