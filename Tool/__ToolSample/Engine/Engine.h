
#pragma once 

class cEngine
{
public:
	static void		GlobalInit( HWND hWnd );
	static void		GlobalShutDown();

protected:
	ShaderHelper	m_ShaderHelper;

public:
	void			Delete();
	void			Init( HWND hWnd );

	void			Update();

public:
	void			Render();

protected:
	bool			__BeginRender();
	bool			__EndRender();
};


extern	cEngine* g_pEngine;