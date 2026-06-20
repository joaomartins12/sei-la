
#pragma once 

#include "MoveObject.h"

class CsMoveObjectMng
{
public:
	void				Delete();
	bool				Init();

	void				Save();
	void				ExcelSave();
	bool				Load();
	void				LoadFilePack();

public:
	bool				IsObject( CsMoveObject::eKIND kind, DWORD dwID );
	bool				IsObject( DWORD dwMoveObjectID );
	CsMoveObject*		GetObject( CsMoveObject::eKIND kind, DWORD dwID );
	CsMoveObject*		GetObject( DWORD dwMoveObjectID );	

protected:
	CsMoveObject::MAP	m_mapObject;
};