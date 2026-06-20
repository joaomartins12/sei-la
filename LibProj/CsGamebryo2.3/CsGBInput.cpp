
#include "stdafx.h"
#include "CsGBInput.h"

NiInputSystemPtr	CsGBInput::m_pInputSystem;

CsGBInput::CsGBInput()
{

}

CsGBInput::~CsGBInput()
{

}

void CsGBInput::Delete()
{
	m_pInputSystem = 0;
}

NiInputSystem::CreateParams*
CsGBInput::_GetInputSystemCreateParams( HINSTANCE hInst, HWND hWnd,
										NiRendererPtr pRenderer, bool bExclusiveMouse /*=false*/  )
{
	NIASSERT( pRenderer != NULL );

	NiDI8InputSystem::DI8CreateParams* pkParams = NiNew NiDI8InputSystem::DI8CreateParams();
	NIASSERT( pkParams != NULL );

	pkParams->SetRenderer( pRenderer );
	pkParams->SetKeyboardUsage( NiInputSystem::FOREGROUND | NiInputSystem::NONEXCLUSIVE );

	unsigned int uiMouseFlags = NiInputSystem::FOREGROUND;
	if( bExclusiveMouse )
		uiMouseFlags |= NiInputSystem::EXCLUSIVE;
	else
		uiMouseFlags |= NiInputSystem::NONEXCLUSIVE;

	pkParams->SetMouseUsage(uiMouseFlags);
	pkParams->SetGamePadCount(2);
	pkParams->SetAxisRange(-100, +100);
	pkParams->SetOwnerInstance( hInst );
	pkParams->SetOwnerWindow( hWnd );

	return pkParams;
}

void CsGBInput::Create( HINSTANCE hInst, HWND hWnd, NiDX9RendererPtr pRenderer, bool bExclusiveMouse /*=false*/  )
{
	// Create and initialize parameters for the input system
	NiInputSystem::CreateParams* pkParams = _GetInputSystemCreateParams( hInst, hWnd, (NiRendererPtr)pRenderer, bExclusiveMouse );

	// Create the input system
	m_pInputSystem = NiInputSystem::Create(pkParams);
	NiDelete pkParams;
	NIASSERT( m_pInputSystem != NULL );

	// The creation of the input system automatically starts an enumeration
	// of the devices. 
	NiInputErr eErr = m_pInputSystem->CheckEnumerationStatus();
	switch (eErr)
	{
	case NIIERR_ENUM_NOTRUNNING:
	case NIIERR_ENUM_FAILED:
		NIASSERT( false );
		break;
	case NIIERR_ENUM_COMPLETE:
	case NIIERR_ENUM_NOTCOMPLETE:
	default:
		break;
	}

	// On Win32, assume there is a mouse and keyboard
	NIASSERT( m_pInputSystem->OpenMouse() && m_pInputSystem->OpenKeyboard() );

	// Gamepad may not exist, but attempt to open any
	for( unsigned int iGamePad = 0; iGamePad < NiInputSystem::MAX_GAMEPADS; ++iGamePad )
	{
		m_pInputSystem->OpenGamePad(iGamePad,0);
	}	
}