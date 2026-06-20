
#pragma once

#include "Map.h"

class cProc3D
{
public:
	static void	GlobalInit();
	static void GlobalShotdown();

public:
	void		ResetDevice();
	void		Delete();
	void		Init();

	void		Update( float fDeltaTime );
	void		Render();


protected:
	cMap		m_Map;
public:
	cMap*		GetMap(){ return &m_Map; }
};

extern cProc3D* g_pProc3D;