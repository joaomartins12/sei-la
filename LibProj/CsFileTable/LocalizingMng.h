
#pragma once 

#include "Localizing.h"

class CsLocalizingMng
{

public:
	void			Delete();
	void			Init();


protected:
	CsLocalizing::LIST 	m_List;

protected:
	void				_LoadExcel();
	void				_LoadExcel( char* cSheet );

public:
	CsLocalizing::LIST*		GetList(){ return &m_List; }
};

