
#pragma once 

#include "Input.h"
#include "..\\Control\Cursor.h"

class cEngine
{
public:
	static void		GlobalInit();
	static void		GlobalShutDown();
	static bool		CALLBACK_ThreadDelete( sTCUnit* pUnit );

protected:
	ShaderHelper	m_ShaderHelper;

public:
	void			Delete();
	void			Init( HINSTANCE hInst, HWND hWnd );

	void			Update();

public:
	void			Render();

protected:
	bool			__BeginRender();
	bool			__EndRender();

	//==============================================================================
	//
	//		ÀÎÇ²
	//
	//==============================================================================
public:
	CInput*				m_pInput;
	NiInputKeyboard*	m_pkKeyboard;
	NiInputMouse*		m_pkMouse;
};


extern	cEngine* g_pEngine;