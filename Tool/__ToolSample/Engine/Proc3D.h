
#pragma once

#include "Helper.h"

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
	cHelper		m_Helper;
};

extern cProc3D* g_pProc3D;