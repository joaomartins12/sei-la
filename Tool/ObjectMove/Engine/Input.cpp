#include "stdafx.h"
#include <NiInputKeyboard.h>
#include <NiInputMouse.h>
#include <NiDI8InputSystem.h>


//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
NiInputSystem::CreateParams* CInput::GetInputSystemCreateParams( HINSTANCE hInst, HWND hWnd )
{
    NiDI8InputSystem::DI8CreateParams* pkParams = 
        NiNew NiDI8InputSystem::DI8CreateParams();
    NIASSERT(pkParams);

    pkParams->SetRenderer(g_pRenderer);
    pkParams->SetKeyboardUsage( NiInputSystem::BACKGROUND | NiInputSystem::NONEXCLUSIVE );
    pkParams->SetMouseUsage( NiInputSystem::FOREGROUND | NiInputSystem::NONEXCLUSIVE );
    pkParams->SetGamePadCount(0);
    pkParams->SetAxisRange(-100, +100);
    pkParams->SetOwnerInstance( hInst );
    pkParams->SetOwnerWindow( hWnd );

    return pkParams;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
bool CInput::CreateInputSystem( HINSTANCE hInst, HWND hWnd )
{
    // Create and initialize parameters for the input system
    NiInputSystem::CreateParams* pkParams = GetInputSystemCreateParams( hInst, hWnd );

    // Create the input system
    m_spInputSystem = NiInputSystem::Create(pkParams);
	SAFE_NIDELETE(pkParams);
    if (!m_spInputSystem)
    {
        NiMessageBox("CreateInputSystem: Creation failed.", "NiApplication Error" );
        return false;
    }

    // The creation of the input system automatically starts an enumeration
    // of the devices. 
    NiInputErr eErr = m_spInputSystem->CheckEnumerationStatus();
    switch (eErr)
    {
    case NIIERR_ENUM_NOTRUNNING:
        CsMessageBox( MB_OK, L"EnumerateDevices failed?" );
        return false;
    case NIIERR_ENUM_FAILED:
		CsMessageBox( MB_OK, L"CheckEnumerationStatus> FAILED!" );
        return false;
    case NIIERR_ENUM_COMPLETE:
    case NIIERR_ENUM_NOTCOMPLETE:
    default:
        break;
    }

    // On Win32, assume there is a mouse and keyboard
    if (!m_spInputSystem->OpenMouse() )
	{
		CsMessageBox( MB_OK, L"마우스를 찾을 수 없습니다" );
		return false;
	}		
	if( !m_spInputSystem->OpenKeyboard() )
    {
		CsMessageBox( MB_OK, L"키보드를 찾을 수 없습니다" );
        return false;
    }

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
void CInput::UpdateInput()
{
	assert_cs( m_spInputSystem );

	assert_cs( m_spInputSystem->GetMode() != NiInputSystem::MAPPED );
	if (m_spInputSystem->UpdateAllDevices() == NIIERR_DEVICELOST)
		m_spInputSystem->HandleDeviceChanges();

    // If there are action maps defined the update with action maps
    // if not then just update all of the devices
   /* if (m_spInputSystem->GetMode() == NiInputSystem::MAPPED)
    {
        if (m_spInputSystem->UpdateActionMap() == NIIERR_DEVICELOST)
            m_spInputSystem->HandleDeviceChanges();
    }
    else
    {
        if (m_spInputSystem->UpdateAllDevices() == NIIERR_DEVICELOST)
            m_spInputSystem->HandleDeviceChanges();
    }*/
}