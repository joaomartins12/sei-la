
#pragma once

class cData_MapObject
{
public:
	cData_MapObject() {};
	~cData_MapObject() {};

public:
	void			Init( bool bOpen = false );
	void			OpenAndCloseObject( DWORD const& dwObjectId, bool bValue );
	void			SetMapObjectAni( DWORD const& dwObjectId, DWORD const& dwAniID );
};