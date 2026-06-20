
#pragma once 

class CsGBInput
{
protected:
	static NiInputSystemPtr			m_pInputSystem;
public:
	CsGBInput();
	~CsGBInput();

	// ==== Delete
public:
	void							Delete();

	// ==== Create
public:	
	void							Create( HINSTANCE hInst, HWND hWnd, NiDX9RendererPtr pRenderer, bool bExclusiveMouse = false );
protected:
	NiInputSystem::CreateParams*	_GetInputSystemCreateParams( HINSTANCE hInst, HWND hWnd, NiRendererPtr pRenderer, bool bExclusiveMouse = false );	


	// ==== Get
public:
	inline static NiInputSystem*	GetInputSystem(){ return m_pInputSystem; }
};



