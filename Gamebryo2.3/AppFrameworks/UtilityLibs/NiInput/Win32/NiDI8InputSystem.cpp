// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2007 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net
//---------------------------------------------------------------------------
// Precompiled Header
#include "NiInputPCH.h"

#include "NiDI8InputSystem.h"
#include "NiInputDI8Keyboard.h"
#include "NiInputDI8Mouse.h"
#include "NiInputDI8GamePad.h"
#include "NiVersion.h"

#include "NiInput_D3DHeaders.h"

#if defined(NIINPUT_EXPORT)
    #pragma comment(lib, "NiDX9Renderer" NI_DLL_SUFFIX ".lib")
    #pragma comment(lib, "NiD3D10Renderer" NI_DLL_SUFFIX ".lib")
#endif  //#if defined(_USRDLL)

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")


//---------------------------------------------------------------------------
// THIS FUNCTION MUST BE PRESENT FOR EACH PLATFORM-SPECIFIC IMPLEMENTATION.
//---------------------------------------------------------------------------
NiInputSystem* NiInputSystem::Create(CreateParams* pkParams)
{
    NiInputSystem* pkInputSys = 0;

    if (NiIsKindOf(NiDI8InputSystem::DI8CreateParams, pkParams))
    {
        NiDI8InputSystem::DI8CreateParams* pkDI8CreateParams = 
            (NiDI8InputSystem::DI8CreateParams*)pkParams;
        NiDI8InputSystem* pkDI8IS = NiNew NiDI8InputSystem();
        if (pkDI8IS)
        {
            // Copy in the control mapping names for the DirectInput
            // configuration screen.
            for (unsigned int ui = 0; 
                ui < NiDI8InputSystem::DEFACTION_COUNT; ui++)
            {
                NiDI8InputSystem::DefaultActions eDefAction = 
                    (NiDI8InputSystem::DefaultActions)ui;
                const char* pcName = pkDI8CreateParams->GetControlMapName(
                    eDefAction);
                pkDI8IS->SetControlMapName(eDefAction, pcName);
            }

            // DirectInput requires both the instance of the application,
            // and the handle to the window...
            pkDI8IS->SetOwnerInstance(pkDI8CreateParams->GetOwnerInstance());
            pkDI8IS->SetOwnerWindow(pkDI8CreateParams->GetOwnerWindow());
            // Initialize it
            if (pkDI8IS->Initialize(pkParams) != NIIERR_OK)
            {
                NiDelete pkDI8IS;
            }
            else
            {
                pkInputSys = (NiInputSystem*)pkDI8IS;
                // Kick off an enumeration of all of the devices
                pkDI8IS->EnumerateDevices();
            }
        }
    }
    else
    {
        NIASSERT(!"Initializing NiDI8InputSystem w/ incorrect CreateParams!");
    }

    return pkInputSys;
}

//---------------------------------------------------------------------------
NiImplementRTTI(NiDI8InputSystem, NiInputSystem);
//---------------------------------------------------------------------------
NiImplementRTTI(NiDI8InputSystem::DI8CreateParams, 
    NiInputSystem::CreateParams);
//---------------------------------------------------------------------------
HINSTANCE NiDI8InputSystem::ms_hOwnerInst = 0;
HWND NiDI8InputSystem::ms_hOwnerWnd = 0;
HINSTANCE NiDI8InputSystem::ms_hLibInst = 0;
// {0CCC7545-2FF1-406d-B14B-E8E29817B967}
static const GUID g_guidNiInputDefault = { 0xccc7545, 0x2ff1, 0x406d, 
    { 0xb1, 0x4b, 0xe8, 0xe2, 0x98, 0x17, 0xb9, 0x67 } };

unsigned int NiDI8InputSystem::ms_uiDefaultActionBufferSize = 17;

//---------------------------------------------------------------------------
bool NiDI8InputSystem::DI8CreateParams::SetControlMapName(
    NiDI8InputSystem::DefaultActions eAction, const char* pcName)
{
    if (eAction >= DEFACTION_COUNT)
        return false;

    if (m_apacMappingNames[eAction])
    {
        NiFree(m_apacMappingNames[eAction]);
        m_apacMappingNames[eAction] = 0;
    }

    if (pcName)
    {
        // For some reason, our DirectInput doesn't like the strings
        // provided by our small memory manager unless an extra few bytes
        // are slapped onto the end.
        unsigned int uiLen = strlen(pcName) + 1;
        m_apacMappingNames[eAction] = NiAlloc(char, uiLen + 2);
        NIASSERT(m_apacMappingNames[eAction]);
        NiStrcpy(m_apacMappingNames[eAction], uiLen, pcName);
        memset(m_apacMappingNames[eAction] + uiLen, 0, 2);
    }

    return true;
}
//---------------------------------------------------------------------------
const char* NiDI8InputSystem::DI8CreateParams::GetControlMapName(
    NiDI8InputSystem::DefaultActions eAction)
{
    if (eAction >= DEFACTION_COUNT)
        return 0;

    return m_apacMappingNames[eAction];
}
//---------------------------------------------------------------------------
NiDI8InputSystem::NiDI8InputSystem() :
    NiInputSystem(), 
    m_pkDirectInput8(0), 
    m_uiActions(DEFACTION_COUNT),
    m_pkActions(0), 
    m_pkD3DSurface_ConfigDevices(0)
{
    for (unsigned int ui = 0; ui < DEFACTION_COUNT; ui++)
    {
        m_apacMappingNames[ui] = 0;
    }
}
//---------------------------------------------------------------------------
NiDI8InputSystem::~NiDI8InputSystem()
{
    for (unsigned int ui = 0; ui < DEFACTION_COUNT; ui++)
    {
        if (m_apacMappingNames[ui])
        {
            NiFree(m_apacMappingNames[ui]);
        }
    }

    NiExternalDelete [] m_pkActions;
    Shutdown();
}
//---------------------------------------------------------------------------
NiInputGamePad* NiDI8InputSystem::OpenGamePad(unsigned int uiPort, 
    unsigned int uiSlot)
{
    if (!m_pkDirectInput8)
    {
        m_eLastError = NIIERR_INVALIDPARAMS;
        return 0;
    }

    unsigned int ui;

    if (uiPort < MAX_GAMEPADS)
    {
        // Since we are using action mapping, the gamepad MUST already exist.
        for (ui = 0; ui < MAX_GAMEPADS; ui++)
        {
            if (m_aspGamePads[ui])
            {
                if (m_aspGamePads[ui]->GetPort() == uiPort)
                    return m_aspGamePads[uiPort];
            }
        }
    }
    else
    {
        // Return the first gamepad
        for (ui = 0; ui < MAX_GAMEPADS; ui++)
        {
            if (m_aspGamePads[ui])
                return m_aspGamePads[ui];
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
NiInputMouse* NiDI8InputSystem::OpenMouse()
{
    if (!m_pkDirectInput8)
    {
        m_eLastError = NIIERR_INVALIDPARAMS;
        return 0;
    }

    // Check if there is already an instance of the mouse.
    if (m_spMouse)
        return m_spMouse;

    NiInputDevice::Description* pkNiDesc = 0;
    // cycle thru the descriptions and find the keyboard
    pkNiDesc = GetFirstDeviceDesc();
    while (pkNiDesc)
    {
        if (pkNiDesc->GetType() == NiInputDevice::NIID_MOUSE)
            break;
        pkNiDesc = GetNextDeviceDesc();
    }

    if (!pkNiDesc)
    {
        m_eLastError = NIIERR_DEVICENOTFOUND;
        return 0;
    }

    LPDIRECTINPUTDEVICE8 pkDIDevice = 0;

    DI8Description* pkDI8Desc = (DI8Description*)pkNiDesc;
    HRESULT hr = m_pkDirectInput8->CreateDevice(
        *(pkDI8Desc->GetGUIDInstance()), 
        &pkDIDevice, NULL);
    if (FAILED(hr))
    {
        m_eLastError = NIIERR_DEVICECREATEFAIL;
        return 0;
    }

    // Create the NiInputDI8Mouse instance.
    NiInputDI8Mouse* pkDI8Mouse = NiNew NiInputDI8Mouse(pkNiDesc, pkDIDevice,
        m_uiMouse);
    if (!pkDI8Mouse || m_eLastError != NIIERR_OK)
    {
        m_eLastError = NIIERR_DEVICECREATEFAIL;
        NiDelete pkDI8Mouse;
        return 0;
    }

    pkDI8Mouse->SetStatus(NiInputDevice::READY);
    m_spMouse = (NiInputMouse*)pkDI8Mouse;

    MapNiActionsToMouse();
    return m_spMouse;
}
//---------------------------------------------------------------------------
NiInputKeyboard* NiDI8InputSystem::OpenKeyboard()
{
    if (!m_pkDirectInput8)
    {
        m_eLastError = NIIERR_INVALIDPARAMS;
        return 0;
    }

    // Check if there is already an instance of the keyboard.
    if (m_spKeyboard)
        return m_spKeyboard;

    // cycle thru the descriptions and find the keyboard
    NiInputDevice::Description* pkNiDesc = GetFirstDeviceDesc();
    while (pkNiDesc)
    {
        if (pkNiDesc->GetType() == NiInputDevice::NIID_KEYBOARD)
            break;
        pkNiDesc = GetNextDeviceDesc();
    }

    if (!pkNiDesc)
    {
        m_eLastError = NIIERR_DEVICENOTFOUND;
        return 0;
    }

    LPDIRECTINPUTDEVICE8 pkDIDevice = 0;

    DI8Description* pkDI8Desc = (DI8Description*)pkNiDesc;
    HRESULT hr = m_pkDirectInput8->CreateDevice(
        *(pkDI8Desc->GetGUIDInstance()), 
        &pkDIDevice, NULL);
    if (FAILED(hr))
    {
        m_eLastError = NIIERR_DEVICECREATEFAIL;
        return 0;
    }

    // Create the NiInputDI8Keyboard instance.
    NiInputDI8Keyboard* pkDI8Keyboard = NiNew NiInputDI8Keyboard(pkNiDesc,
        pkDIDevice, m_uiKeyboard);
    if (!pkDI8Keyboard || m_eLastError != NIIERR_OK)
    {
        m_eLastError = NIIERR_DEVICECREATEFAIL;
        NiDelete pkDI8Keyboard;
        return 0;
    }

    pkDI8Keyboard->SetStatus(NiInputDevice::READY);
    m_spKeyboard = (NiInputKeyboard*)pkDI8Keyboard;

    MapNiActionsToKeyboard();
    return m_spKeyboard;
}
//---------------------------------------------------------------------------
NiInputErr NiDI8InputSystem::UpdateAllDevices()
{
    return NiInputSystem::UpdateAllDevices();
}
//---------------------------------------------------------------------------
NiInputErr NiDI8InputSystem::UpdateActionMap()
{
    return NiInputSystem::UpdateActionMap();
}
//---------------------------------------------------------------------------
NiInputErr NiDI8InputSystem::HandleDeviceChanges()
{
    m_eLastError = NIIERR_OK;

    // 
    NiInputErr eErr_Keyboard;
    NiInputErr eErr_Mouse;
    NiInputErr aeErr_Gamepad[MAX_GAMEPADS];
    bool bLostDevice = false;

    // Keyboard
    if (m_spKeyboard)
    {
        eErr_Keyboard = m_spKeyboard->HandleInsertion();
        if (eErr_Keyboard == NIIERR_DEVICELOST)
            bLostDevice = true;
        m_spKeyboard->SetLastError(eErr_Keyboard);
    }

    // Mouse
    if (m_spMouse)
    {
        eErr_Mouse = m_spMouse->HandleInsertion();
        if (eErr_Mouse == NIIERR_DEVICELOST)
            bLostDevice = true;
        m_spMouse->SetLastError(eErr_Mouse);
    }

    // Gamepads
    for (unsigned int ui = 0; ui < MAX_GAMEPADS; ui++)
    {
        if (m_aspGamePads[ui])
        {
            aeErr_Gamepad[ui] = m_aspGamePads[ui]->HandleInsertion();
            if (aeErr_Gamepad[ui] == NIIERR_DEVICELOST)
                bLostDevice = true;
            m_aspGamePads[ui]->SetLastError(aeErr_Gamepad[ui]);
        }
    }

    // See if any devices were lost
    if (bLostDevice)
        m_eLastError = NIIERR_DEVICELOST;

    return m_eLastError;
}
//---------------------------------------------------------------------------
NiInputErr NiDI8InputSystem::HandleRemovals()
{
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiDI8InputSystem::HandleInsertions()
{
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
bool NiDI8InputSystem::CreateDI8Interface()
{
    if ((ms_hOwnerInst == 0) || (ms_hOwnerWnd == 0))
    {
        m_eLastError = NIIERR_INVALIDPARAMS;
        return false;
    }

    HRESULT hr = DirectInput8Create(ms_hOwnerInst, DIRECTINPUT_VERSION, 
        IID_IDirectInput8A, (void**)(&m_pkDirectInput8), NULL);
    if (FAILED(hr))
    {
        m_eLastError = NIIERR_INITFAILED;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
const GUID* NiDI8InputSystem::GetNiInputDefaultGUID()
{
    return &g_guidNiInputDefault;
}
//---------------------------------------------------------------------------
NiInputErr NiDI8InputSystem::Initialize(CreateParams* pkParams)
{
    if (NiInputSystem::Initialize(pkParams) != NIIERR_OK)
        return m_eLastError;

    if (!CreateDI8Interface())
        return m_eLastError;

    return m_eLastError;
}
//---------------------------------------------------------------------------
NiInputErr NiDI8InputSystem::Shutdown()
{
    // We need to release the devices before we can release the interface!
    NiInputSystem::Shutdown();

    if (m_pkDirectInput8)
        m_pkDirectInput8->Release();
    m_pkDirectInput8 = 0;

    m_pfnCreate = 0;
    if (ms_hLibInst)
    {
        FreeLibrary(ms_hLibInst);
        ms_hLibInst = 0;
    }

    return NIIERR_OK;
}
//---------------------------------------------------------------------------
NiInputErr NiDI8InputSystem::SetActionMap()
{
    // Set the action map for each device
    // Keyboard and mouse will NOT be mapped in Immediate Mode
    if (m_spKeyboard && m_spKeyboard->GetActionMapped())
        SetActionMap(m_spKeyboard);
    if (m_spMouse && m_spMouse->GetActionMapped())
        SetActionMap(m_spMouse);
    // GamePads will ALWAYS be mapped, eith in the default 'Ni' mapping,
    // or an action map supplied by the app.
    for (unsigned int ui = 0; ui < MAX_GAMEPADS; ui++)
    {
        if (m_aspGamePads[ui])
            SetActionMap(m_aspGamePads[ui]);
    }
    return m_eLastError;
}
//---------------------------------------------------------------------------
NiInputErr NiDI8InputSystem::SetActionMap(NiInputDevice* pkDevice)
{
    LPDIRECTINPUTDEVICE8 pkDIDevice = 0;
    if (pkDevice)
    {
        switch (pkDevice->GetType())
        {
        case NiInputDevice::NIID_GAMEPAD:
            {
                NiInputDI8GamePad* pkDIGamePad = (NiInputDI8GamePad*)pkDevice;
                pkDIDevice = pkDIGamePad->GetDIDevice();
            }
            break;
        case NiInputDevice::NIID_KEYBOARD:
            {
                NiInputDI8Keyboard* pkDIKeyboard = 
                    (NiInputDI8Keyboard*)pkDevice;
                pkDIDevice = pkDIKeyboard->GetDIDevice();
            }
            break;
        case NiInputDevice::NIID_MOUSE:
            {
                NiInputDI8Mouse* pkDIMouse = (NiInputDI8Mouse*)pkDevice;
                pkDIDevice = pkDIMouse->GetDIDevice();
            }
            break;
        default:
            m_eLastError = NIIERR_INVALIDDEVICE;
            break;
        }
    }
    else
    {
        m_eLastError = NIIERR_INVALIDPARAMS;
    }

    if (pkDIDevice)
        SetActionMapOnDIDevice(pkDIDevice);

    return m_eLastError;
}
//---------------------------------------------------------------------------
NiInputErr NiDI8InputSystem::SetActionMapOnDIDevice(
    LPDIRECTINPUTDEVICE8 pkDIDevice)
{
    if (pkDIDevice)
    {
        // We have to unacuire the device.
        pkDIDevice->Unacquire();

        // Build the action map
        HRESULT hr = pkDIDevice->BuildActionMap(&m_kDIActionFormat, NULL, 0);
        if (FAILED(hr))
        {
            NILOG(NIMESSAGE_GENERAL_1, 
                "BuildActionMap> Failed - 0x%08x!\n", hr);
            m_eLastError = NIIERR_APIFAILURE;
        }
        else
        {
            // Configure and apply it to the object
            hr = pkDIDevice->SetActionMap(&m_kDIActionFormat, 
                NULL, 0);
            if (FAILED(hr))
            {
                NILOG(NIMESSAGE_GENERAL_1, 
                    "SetActionMap> Failed - 0x%08x!\n", hr);
                m_eLastError = NIIERR_APIFAILURE;
            }
        }

        // We have to re-acuire the device.
        if (SUCCEEDED(hr))
            pkDIDevice->Acquire();
    }
    else
    {
        m_eLastError = NIIERR_INVALIDPARAMS;
    }

    return m_eLastError;
}
//---------------------------------------------------------------------------
NiInputErr NiDI8InputSystem::CheckEnumerationStatus()
{
    if (!m_bEnumerating)
        return NIIERR_ENUM_NOTRUNNING;

    // Already done on PC...
    m_bEnumerating = false;
    return NIIERR_ENUM_COMPLETE;
}
//---------------------------------------------------------------------------
BOOL CALLBACK NiDI8InputSystem::DIEnumDevicesCallback(
    LPCDIDEVICEINSTANCE pkDDI, LPVOID pvRef)
{
    NiDI8InputSystem* pkInputSys = (NiDI8InputSystem*)pvRef;
    if (!pkInputSys)
    {
        NIASSERT(!"EnumDevicesCallback --> No valid input system!");
        return DIENUM_STOP;
    }

    LPDIRECTINPUT8 pkDI8 = pkInputSys->GetDirectInput8();
    if (!pkDI8)
    {
        NIASSERT(!"EnumDevicesCallback --> No valid DI8 interface!");
        return DIENUM_STOP;
    }

    // Obtain an interface to the enumerated device.
    LPDIRECTINPUTDEVICE8 pkDI8Device = NULL;     

    HRESULT hr = pkDI8->CreateDevice(pkDDI->guidInstance, &pkDI8Device, NULL);

    // If it failed, then we can't use this device. (Maybe the user unplugged
    // it while we were in the middle of enumerating it.)
    if (FAILED(hr)) 
        return DIENUM_CONTINUE;

    bool bValidDevice = false;
    
    // 
    NiDI8InputSystem::DI8Description* pkNiDesc = 
        NiNew NiDI8InputSystem::DI8Description();
    NIASSERT(pkNiDesc);

    if (memcmp((const void*)&GUID_SysKeyboard, 
        (const void*)&(pkDDI->guidInstance), sizeof(GUID)) == 0)
    {
        NILOG(NIMESSAGE_GENERAL_1, "SYSTEM KEYBOARD FOUND.\n");
    }
    if (memcmp((const void*)&GUID_SysMouse, 
        (const void*)&(pkDDI->guidInstance), sizeof(GUID)) == 0)
    {
        NILOG(NIMESSAGE_GENERAL_1, "SYSTEM MOUSE FOUND.\n");
    }

    // The device will be the next available one, but the index will be the
    // current available count... We will use this as the 'port' of the
    // device for indentification in the DirectInput version of the library.
    switch (pkDDI->dwDevType & 0x000000ff)
    {
    case DI8DEVTYPE_1STPERSON:
    case DI8DEVTYPE_DRIVING:
    case DI8DEVTYPE_FLIGHT:
    case DI8DEVTYPE_GAMEPAD:
    case DI8DEVTYPE_JOYSTICK:
    case DI8DEVTYPE_SUPPLEMENTAL:
        break;
    case DI8DEVTYPE_KEYBOARD:
        bValidDevice = true;
        break;
    case DI8DEVTYPE_MOUSE:
    case DI8DEVTYPE_SCREENPOINTER:
        bValidDevice = true;
        break;
    case DI8DEVTYPE_DEVICE:
    case DI8DEVTYPE_DEVICECTRL:
    case DI8DEVTYPE_REMOTE:
        break;
    default:
        break;
    }

    // Add it to the list.
    if (bValidDevice)
    {
        // Fill in the description
        pkInputSys->DIDeviceInstanceToNiInputDescription(pkDDI, pkNiDesc);

        // Fill in the control descriptions for the device
        pkInputSys->EnumerateDeviceControls(pkNiDesc, pkDI8Device);

        // Fill in the device-specific details.
        unsigned int uiCheckCount = pkInputSys->AddAvailableDevice(pkNiDesc);
        if (uiCheckCount != pkNiDesc->GetPort() + 1)
        {
            NIASSERT(!"Mismatched device count?");
        }
    }
    else
    {
        NiDelete pkNiDesc;
    }

    pkDI8Device->Release();

    return DIENUM_CONTINUE;
}
//---------------------------------------------------------------------------
BOOL CALLBACK NiDI8InputSystem::DIEnumDevicesBySemanticsDefaultCallback(
    LPCDIDEVICEINSTANCE pkDDI, LPDIRECTINPUTDEVICE8 pkDID, DWORD dwFlags,
    DWORD dwRemaining, LPVOID pvRef)
{
    // Test log
    NILOG(NIMESSAGE_GENERAL_1, 
        "EDBSCB> Devices Remaining: %2d\n", dwRemaining);

    NiDI8InputSystem* pkInputSys = (NiDI8InputSystem*)pvRef;

    return pkInputSys->AddDefaultActionMappedDevice(pkDDI, pkDID);
}
//---------------------------------------------------------------------------
NiInputErr NiDI8InputSystem::EnumerateDeviceControls(
    NiDI8InputSystem::DI8Description* pkNiDesc,
    LPDIRECTINPUTDEVICE8 pkDI8Device)
{
    if (!m_pkDirectInput8 || !pkDI8Device)
        return NIIERR_INVALIDPARAMS;

    pkDI8Device->EnumObjects(NiDI8InputSystem::DIEnumDeviceObjectsCallback, 
        (void*)pkNiDesc, DIDFT_ALL);

    return NIIERR_OK;
}
//---------------------------------------------------------------------------
BOOL CALLBACK NiDI8InputSystem::DIEnumDeviceObjectsCallback(
    LPCDIDEVICEOBJECTINSTANCE pkDIDOI, LPVOID pvRef)
{
    NiDI8InputSystem::DI8Description* pkNiDesc = 
        (NiDI8InputSystem::DI8Description*)pvRef;
    if (!pkNiDesc)
    {
        NIASSERT(!"DIEnumDeviceObjectsCallback --> No valid device desc!");
        return DIENUM_STOP;
    }

#if defined(_NIDI_LOG_DEVICE_OBJECTS_)
    NILOG(NIMESSAGE_GENERAL_1, "    DIEnumDeviceObjectsCallback> %s\n", 
        pkDIDOI->tszName);
#endif  //#if defined(_NIDI_LOG_DEVICE_OBJECTS_)

    NiInputDevice::ControlDesc* pkCtrlDesc = 0;
    
    // Fill in the control description...
    if ((pkDIDOI->guidType == GUID_XAxis) ||
        (pkDIDOI->guidType == GUID_YAxis) ||
        (pkDIDOI->guidType == GUID_ZAxis) ||
        (pkDIDOI->guidType == GUID_RxAxis) ||
        (pkDIDOI->guidType == GUID_RyAxis) ||
        (pkDIDOI->guidType == GUID_RzAxis) ||
        (pkDIDOI->guidType == GUID_Slider))
    {
        // Allocate a control desc
        pkCtrlDesc = NiNew NiInputDevice::ControlDesc(
            pkNiDesc->GetControlCount(), pkDIDOI->tszName, 
            NiInputDevice::ControlDesc::AXIS, 0);
        NIASSERT(pkCtrlDesc);
        pkCtrlDesc->SetAnalog(true);
        pkCtrlDesc->SetInput(true);
    }
    else if (pkDIDOI->guidType == GUID_Button)
    {
        // Allocate a control desc
        pkCtrlDesc = NiNew NiInputDevice::ControlDesc(
            pkNiDesc->GetControlCount(), pkDIDOI->tszName, 
            NiInputDevice::ControlDesc::BUTTON, 0);
        NIASSERT(pkCtrlDesc);
        pkCtrlDesc->SetAnalog(false);
        pkCtrlDesc->SetDigital(true);
        pkCtrlDesc->SetInput(true);
    }
    else if (pkDIDOI->guidType == GUID_Key)
    {
        // We do not add key descriptions, but we do bump the button count
        // to allow for checking the key count.
        pkNiDesc->IncrementButtonCount();
    }
    else if (pkDIDOI->guidType == GUID_POV)
    {
        // Allocate a control desc
        pkCtrlDesc = NiNew NiInputDevice::ControlDesc(
            pkNiDesc->GetControlCount(), pkDIDOI->tszName, 
            NiInputDevice::ControlDesc::POV, 0);
        NIASSERT(pkCtrlDesc);
        pkCtrlDesc->SetAnalog(false);
        pkCtrlDesc->SetDigital(true);
        pkCtrlDesc->SetInput(true);
    }

    if (pkCtrlDesc)
    {
        pkCtrlDesc->SetPlatformSpecificData(pkDIDOI->dwType);

        unsigned int uiCheck = pkNiDesc->AddControlDesc(pkCtrlDesc);
        if (uiCheck == 0xffffffff)
        {
            // The description was already present in the list!
            NiDelete pkCtrlDesc;
        }
        else if (uiCheck != pkCtrlDesc->GetIndex() + 1)
        {
            NIASSERT(!"Mismatched control count?");
        }
    }

    return DIENUM_CONTINUE;
}
//---------------------------------------------------------------------------
NiInputErr NiDI8InputSystem::EnumerateDevices()
{
    if (!m_pkDirectInput8)
    {
        m_eLastError = NIIERR_INVALIDPARAMS;
        return m_eLastError;
    }

    if (m_bEnumerating)
        return NIIERR_ENUM_NOTCOMPLETE;

    // Regardless of whether we are in immediate or mapped mode, 
    // we will use the 'Gamebryo' DIActionMapping.
    // Setup the default action map for the gamepad
    if (!SetupDefaultActionMap())
    {
        m_eLastError = NIIERR_INITFAILED;
        return m_eLastError;
    }

    HRESULT hr = m_pkDirectInput8->EnumDevicesBySemantics(NULL, 
        &m_kDIActionFormat, 
        NiDI8InputSystem::DIEnumDevicesBySemanticsDefaultCallback, 
        this, DIEDBSFL_ATTACHEDONLY);
    if (FAILED(hr))
    {
        m_eLastError = NIIERR_INITFAILED;
        return m_eLastError;
    }

    hr = m_pkDirectInput8->EnumDevices(DI8DEVCLASS_ALL, 
        NiDI8InputSystem::DIEnumDevicesCallback, this, 
        DIEDFL_ATTACHEDONLY);
    if (FAILED(hr))
    {
        m_eLastError = NIIERR_INITFAILED;
        return m_eLastError;
    }

    m_bEnumerating = true;
    return NIIERR_OK;
}
//---------------------------------------------------------------------------
bool NiDI8InputSystem::SetControlMapName(DefaultActions eAction, 
    const char* pcName)
{
    if (eAction >= DEFACTION_COUNT)
        return false;

    if (m_apacMappingNames[eAction])
    {
        NiFree(m_apacMappingNames[eAction]);
        m_apacMappingNames[eAction] = 0;
    }

    if (pcName)
    {
        // For some reason, our DirectInput doesn't like the strings
        // provided by our small memory manager unless an extra few bytes
        // are slapped onto the end.
        unsigned int uiLen = strlen(pcName) + 1;
        m_apacMappingNames[eAction] = NiAlloc(char, uiLen + 2);
        NIASSERT(m_apacMappingNames[eAction]);
        NiStrcpy(m_apacMappingNames[eAction], uiLen, pcName);
        memset(m_apacMappingNames[eAction] + uiLen, 0, 2);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiDI8InputSystem::DIDeviceInstanceToNiInputDescription(
    LPCDIDEVICEINSTANCE pkDDI, DI8Description* pkNiDesc)
{
    if (!pkDDI || !pkNiDesc)
        return false;

    // The device will be the next available one, but the index will be the
    // current available count... We will use this as the 'port' of the
    // device for indentification in the DirectInput version of the library.
    pkNiDesc->SetPort(GetAvailableDeviceCount());
    pkNiDesc->SetGUIDInstance(&pkDDI->guidInstance);
    pkNiDesc->SetName(pkDDI->tszProductName);
    pkNiDesc->SetDescriptionString(pkDDI->tszInstanceName);

    switch (pkDDI->dwDevType & 0x000000ff)
    {
    case DI8DEVTYPE_1STPERSON:
        {
            switch ((pkDDI->dwDevType & 0x0000ff00) >> 8)
            {
            case DI8DEVTYPE1STPERSON_LIMITED:
            case DI8DEVTYPE1STPERSON_SHOOTER:
            case DI8DEVTYPE1STPERSON_SIXDOF:
            case DI8DEVTYPE1STPERSON_UNKNOWN:
                break;
            default:    
                break;
            }
            // I'm not sure what this is... It could be some 
            // keyboard/trackball combo, or something equivalent.
            // For now, we will add it as a gamepad
            pkNiDesc->SetType(NiInputDevice::NIID_GAMEPAD);
        }
        break;
    case DI8DEVTYPE_DEVICE:
    case DI8DEVTYPE_DEVICECTRL:
        {
            switch ((pkDDI->dwDevType & 0x0000ff00) >> 8)
            {
            case DI8DEVTYPEDEVICECTRL_COMMSSELECTION:
            case DI8DEVTYPEDEVICECTRL_COMMSSELECTION_HARDWIRED:
            case DI8DEVTYPEDEVICECTRL_UNKNOWN:
                break;
            default:
                break;
            }
        }
        break;
    case DI8DEVTYPE_DRIVING:
        {
            switch ((pkDDI->dwDevType & 0x0000ff00) >> 8)
            {
            case DI8DEVTYPEDRIVING_COMBINEDPEDALS:
            case DI8DEVTYPEDRIVING_DUALPEDALS:
            case DI8DEVTYPEDRIVING_HANDHELD:
            case DI8DEVTYPEDRIVING_LIMITED:
            case DI8DEVTYPEDRIVING_THREEPEDALS:
                break;
            default:
                break;
            }
            // Gamepad
            pkNiDesc->SetType(NiInputDevice::NIID_GAMEPAD);
        }
        break;
    case DI8DEVTYPE_FLIGHT:
        {
            switch ((pkDDI->dwDevType & 0x0000ff00) >> 8)
            {
            case DI8DEVTYPEFLIGHT_LIMITED:
            case DI8DEVTYPEFLIGHT_RC:
            case DI8DEVTYPEFLIGHT_STICK:
            case DI8DEVTYPEFLIGHT_YOKE:
                break;
            default:
                break;
            }
            // Again, not sure - but likely a multiple axis joystick,
            // so we are just tagging it as a gamepad.
            pkNiDesc->SetType(NiInputDevice::NIID_GAMEPAD);
        }
        break;
    case DI8DEVTYPE_GAMEPAD:
        {
            switch ((pkDDI->dwDevType & 0x0000ff00) >> 8)
            {
            case DI8DEVTYPEGAMEPAD_LIMITED:
            case DI8DEVTYPEGAMEPAD_STANDARD:
            case DI8DEVTYPEGAMEPAD_TILT:
                break;
            default:
                break;
            }
            pkNiDesc->SetType(NiInputDevice::NIID_GAMEPAD);
        }
        break;
    case DI8DEVTYPE_JOYSTICK:
        {
            switch ((pkDDI->dwDevType & 0x0000ff00) >> 8)
            {
            case DI8DEVTYPEJOYSTICK_LIMITED:
            case DI8DEVTYPEJOYSTICK_STANDARD:
                break;
            default:
                break;
            }
            pkNiDesc->SetType(NiInputDevice::NIID_GAMEPAD);
        }
        break;
    case DI8DEVTYPE_KEYBOARD:
        {
            switch ((pkDDI->dwDevType & 0x0000ff00) >> 8)
            {
            case DI8DEVTYPEKEYBOARD_UNKNOWN:
            case DI8DEVTYPEKEYBOARD_PCXT:
            case DI8DEVTYPEKEYBOARD_OLIVETTI:
            case DI8DEVTYPEKEYBOARD_PCAT:
            case DI8DEVTYPEKEYBOARD_PCENH:
            case DI8DEVTYPEKEYBOARD_NOKIA1050:
            case DI8DEVTYPEKEYBOARD_NOKIA9140:
            case DI8DEVTYPEKEYBOARD_NEC98:
            case DI8DEVTYPEKEYBOARD_NEC98LAPTOP:
            case DI8DEVTYPEKEYBOARD_NEC98106:
            case DI8DEVTYPEKEYBOARD_JAPAN106:
            case DI8DEVTYPEKEYBOARD_JAPANAX:
            case DI8DEVTYPEKEYBOARD_J3100:
                break;
            default:
                break;
            }
            pkNiDesc->SetType(NiInputDevice::NIID_KEYBOARD);
        }
        break;
    case DI8DEVTYPE_MOUSE:
        {
            switch ((pkDDI->dwDevType & 0x0000ff00) >> 8)
            {
            case DI8DEVTYPEMOUSE_ABSOLUTE:
            case DI8DEVTYPEMOUSE_FINGERSTICK:
            case DI8DEVTYPEMOUSE_TOUCHPAD:
            case DI8DEVTYPEMOUSE_TRACKBALL:
            case DI8DEVTYPEMOUSE_TRADITIONAL:
            case DI8DEVTYPEMOUSE_UNKNOWN:
                break;
            default:
                break;
            }
            pkNiDesc->SetType(NiInputDevice::NIID_MOUSE);
        }
        break;
    case DI8DEVTYPE_REMOTE:
        {
            switch ((pkDDI->dwDevType & 0x0000ff00) >> 8)
            {
            case DI8DEVTYPEREMOTE_UNKNOWN:
                break;
            default:
                break;
            }
        }
        break;
    case DI8DEVTYPE_SCREENPOINTER:
        {
            switch ((pkDDI->dwDevType & 0x0000ff00) >> 8)
            {
            case DI8DEVTYPESCREENPTR_UNKNOWN:
            case DI8DEVTYPESCREENPTR_LIGHTGUN:
            case DI8DEVTYPESCREENPTR_LIGHTPEN:
            case DI8DEVTYPESCREENPTR_TOUCH:
                break;
            default:
                break;
            }
            pkNiDesc->SetType(NiInputDevice::NIID_MOUSE);
        }
        break;
    case DI8DEVTYPE_SUPPLEMENTAL:
        {
            switch ((pkDDI->dwDevType & 0x0000ff00) >> 8)
            {
            case DI8DEVTYPESUPPLEMENTAL_2NDHANDCONTROLLER:
            case DI8DEVTYPESUPPLEMENTAL_COMBINEDPEDALS:
            case DI8DEVTYPESUPPLEMENTAL_DUALPEDALS:
            case DI8DEVTYPESUPPLEMENTAL_HANDTRACKER:
            case DI8DEVTYPESUPPLEMENTAL_HEADTRACKER:
            case DI8DEVTYPESUPPLEMENTAL_RUDDERPEDALS:
            case DI8DEVTYPESUPPLEMENTAL_SHIFTER:
            case DI8DEVTYPESUPPLEMENTAL_SHIFTSTICKGATE:
            case DI8DEVTYPESUPPLEMENTAL_SPLITTHROTTLE:
            case DI8DEVTYPESUPPLEMENTAL_THREEPEDALS:
            case DI8DEVTYPESUPPLEMENTAL_THROTTLE:
            case DI8DEVTYPESUPPLEMENTAL_UNKNOWN:
                break;
            default:
                break;
            }
            // Again, the catch-all gamepad distinction
            pkNiDesc->SetType(NiInputDevice::NIID_GAMEPAD);
        }
        break;
    default:
        break;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiDI8InputSystem::SetupDefaultActionMap()
{
    // The default action map is created for handling the gamepad.
    // Due to the fact that GamePads seem to almost randomly assign
    // axes, we need to supply a default mapping for them.
    NiExternalDelete [] m_pkActions;

    m_pkActions = NiExternalNew DIACTION[m_uiActions];
    if (!m_pkActions)
        return false;

    memset((void*)m_pkActions, 0, sizeof(DIACTION) * m_uiActions);

    unsigned int uiActionCount = 0;

    m_pkActions[uiActionCount].uAppData = AXIS_0_V;
    m_pkActions[uiActionCount].dwSemantic = DIAXIS_ANY_Y_1;
    if (m_apacMappingNames[AXIS_0_V])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[AXIS_0_V];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = "Stick 1 Vert";
    }
    uiActionCount++;

    m_pkActions[uiActionCount].uAppData = AXIS_0_H;
    m_pkActions[uiActionCount].dwSemantic = DIAXIS_ANY_X_1;
    if (m_apacMappingNames[AXIS_0_H])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[AXIS_0_H];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = "Stick 1 Horz";
    }
    uiActionCount++;

    m_pkActions[uiActionCount].uAppData = AXIS_1_V;
    m_pkActions[uiActionCount].dwSemantic = DIAXIS_ANY_1;
    if (m_apacMappingNames[AXIS_1_V])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[AXIS_1_V];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = "Stick 2 Vert";
    }
    uiActionCount++;

    m_pkActions[uiActionCount].uAppData = AXIS_1_H;
    m_pkActions[uiActionCount].dwSemantic = DIAXIS_ANY_2;
    if (m_apacMappingNames[AXIS_1_H])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[AXIS_1_H];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = "Stick 2 Horz";
    }
    uiActionCount++;

    m_pkActions[uiActionCount].uAppData = AXIS_2_V;
    m_pkActions[uiActionCount].dwSemantic = DIAXIS_ANY_3;
    if (m_apacMappingNames[AXIS_2_V])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[AXIS_2_V];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = "Stick 3 Vert";
    }
    uiActionCount++;

    m_pkActions[uiActionCount].uAppData = AXIS_2_H;
    m_pkActions[uiActionCount].dwSemantic = DIAXIS_ANY_4;
    if (m_apacMappingNames[AXIS_2_H])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[AXIS_2_H];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = "Stick 3 Horz";
    }
    uiActionCount++;

    m_pkActions[uiActionCount].uAppData = AXIS_3_V;
    m_pkActions[uiActionCount].dwSemantic = DIAXIS_ANY_Y_2;
    if (m_apacMappingNames[AXIS_3_V])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[AXIS_3_V];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = "Stick 4 Vert";
    }
    uiActionCount++;

    m_pkActions[uiActionCount].uAppData = AXIS_3_H;
    m_pkActions[uiActionCount].dwSemantic = DIAXIS_ANY_X_2;
    if (m_apacMappingNames[AXIS_3_H])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[AXIS_3_H];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = "Stick 4 Horz";
    }
    uiActionCount++;

    m_pkActions[uiActionCount].uAppData = POV_DPAD;
    m_pkActions[uiActionCount].dwSemantic = DIHATSWITCH_FPS_GLANCE;
    if (m_apacMappingNames[POV_DPAD])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[POV_DPAD];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = 
            "D-Pad (LUP/DOWN/LEFT/RIGHT)";
    }
    uiActionCount++;

    m_pkActions[uiActionCount].uAppData = BUTTON_RDOWN;
    m_pkActions[uiActionCount].dwSemantic = DIBUTTON_ANY(0);
    if (m_apacMappingNames[BUTTON_RDOWN])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[BUTTON_RDOWN];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = "R DOWN";
    }
    uiActionCount++;

    m_pkActions[uiActionCount].uAppData = BUTTON_RRIGHT;
    m_pkActions[uiActionCount].dwSemantic = DIBUTTON_ANY(1);
    if (m_apacMappingNames[BUTTON_RRIGHT])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[BUTTON_RRIGHT];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = "R RIGHT";
    }
    uiActionCount++;

    m_pkActions[uiActionCount].uAppData = BUTTON_RLEFT;
    m_pkActions[uiActionCount].dwSemantic = DIBUTTON_ANY(2);
    if (m_apacMappingNames[BUTTON_RLEFT])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[BUTTON_RLEFT];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = "R LEFT";
    }
    uiActionCount++;

    m_pkActions[uiActionCount].uAppData = BUTTON_RUP;
    m_pkActions[uiActionCount].dwSemantic = DIBUTTON_ANY(3);
    if (m_apacMappingNames[BUTTON_RUP])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[BUTTON_RUP];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = "R UP";
    }
    uiActionCount++;

    m_pkActions[uiActionCount].uAppData = BUTTON_L1;
    m_pkActions[uiActionCount].dwSemantic = DIBUTTON_ANY(4);
    if (m_apacMappingNames[BUTTON_L1])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[BUTTON_L1];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = "L1";
    }
    uiActionCount++;

    m_pkActions[uiActionCount].uAppData = BUTTON_R1;
    m_pkActions[uiActionCount].dwSemantic = DIBUTTON_ANY(5);
    if (m_apacMappingNames[BUTTON_R1])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[BUTTON_R1];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = "R1";
    }
    uiActionCount++;

    m_pkActions[uiActionCount].uAppData = BUTTON_SELECT;
    m_pkActions[uiActionCount].dwSemantic = DIBUTTON_ANY(6);
    if (m_apacMappingNames[BUTTON_SELECT])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[BUTTON_SELECT];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = "SELECT";
    }
    uiActionCount++;

    m_pkActions[uiActionCount].uAppData = BUTTON_START;
    m_pkActions[uiActionCount].dwSemantic = DIBUTTON_ANY(7);
    if (m_apacMappingNames[BUTTON_START])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[BUTTON_START];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = "START";
    }
    uiActionCount++;

    m_pkActions[uiActionCount].uAppData = BUTTON_A;
    m_pkActions[uiActionCount].dwSemantic = DIBUTTON_ANY(8);
    if (m_apacMappingNames[BUTTON_A])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[BUTTON_A];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = "A";
    }
    uiActionCount++;

    m_pkActions[uiActionCount].uAppData = BUTTON_B;
    m_pkActions[uiActionCount].dwSemantic = DIBUTTON_ANY(9);
    if (m_apacMappingNames[BUTTON_B])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[BUTTON_B];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = "B";
    }
    uiActionCount++;

    m_pkActions[uiActionCount].uAppData = BUTTON_L2;
    m_pkActions[uiActionCount].dwSemantic = DIBUTTON_ANY(10);
    if (m_apacMappingNames[BUTTON_L2])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[BUTTON_L2];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = "L2";
    }
    uiActionCount++;

    m_pkActions[uiActionCount].uAppData = BUTTON_R2;
    m_pkActions[uiActionCount].dwSemantic = DIBUTTON_ANY(11);
    if (m_apacMappingNames[BUTTON_R2])
    {
        m_pkActions[uiActionCount].lptszActionName = 
            m_apacMappingNames[BUTTON_R2];
    }
    else
    {
        m_pkActions[uiActionCount].lptszActionName = "R2";
    }
    uiActionCount++;

    NIASSERT((uiActionCount == m_uiActions) && "Missing actions!");

    memset((void*)&m_kDIActionFormat, 0, sizeof(DIACTIONFORMAT));
    m_kDIActionFormat.dwSize = sizeof(DIACTIONFORMAT);
    m_kDIActionFormat.dwActionSize = sizeof(DIACTION);
    m_kDIActionFormat.dwDataSize = m_uiActions * sizeof(DWORD);
    m_kDIActionFormat.dwNumActions = m_uiActions;
    m_kDIActionFormat.guidActionMap = 
        *(NiDI8InputSystem::GetNiInputDefaultGUID());
    m_kDIActionFormat.dwGenre = DIVIRTUAL_FIGHTING_FPS;
    m_kDIActionFormat.rgoAction = m_pkActions;
    // Make buffer big enough for one of each actions
    m_kDIActionFormat.dwBufferSize = m_uiActions;
    m_kDIActionFormat.lAxisMin = m_iAxisRangeLow;
    m_kDIActionFormat.lAxisMax = m_iAxisRangeHigh;
    NiStrcpy(m_kDIActionFormat.tszActionMap, MAX_PATH, "NiInput Default");

    return true;
}
//---------------------------------------------------------------------------
bool NiDI8InputSystem::AddDefaultActionMappedDevice(
    const DIDEVICEINSTANCE* pkDDI, const LPDIRECTINPUTDEVICE8 pkDevice)
{
    NILOG(NIMESSAGE_GENERAL_1, "AddDefaultActionMappedDevice> %s\n", 
        pkDDI->tszInstanceName);

    // Check the device
    DWORD dwType = pkDDI->dwDevType & 0x000000ff;

    // Reject keyboard and mice for the default action mapping.
    if ((dwType == DI8DEVTYPE_KEYBOARD) || 
        (dwType == DI8DEVTYPE_MOUSE) || 
        (dwType == DI8DEVTYPE_SCREENPOINTER))
    {
        NILOG(NIMESSAGE_GENERAL_1, "AddDefaultActionMappedDevice> "
            "Does not use Keyboard or Mouse - REJECTING DEVICE!\n");
        return DIENUM_CONTINUE;
    }

    // Also reject anything the is not a 'gamepad'
    if ((dwType != DI8DEVTYPE_1STPERSON) && 
        (dwType != DI8DEVTYPE_DRIVING) && 
        (dwType != DI8DEVTYPE_FLIGHT) && 
        (dwType != DI8DEVTYPE_GAMEPAD) && 
        (dwType != DI8DEVTYPE_JOYSTICK) && 
        (dwType != DI8DEVTYPE_SUPPLEMENTAL))
    {
        NILOG(NIMESSAGE_GENERAL_1, "AddDefaultActionMappedDevice> "
            "Not a gamepad - REJECTING DEVICE!\n");
        return DIENUM_CONTINUE;
    }

    // Make sure there is an empty gamepad slot
    unsigned int uiPadIndex = 0xffffffff;
    for (unsigned int ui = 0; ui < MAX_GAMEPADS; ui++)
    {
        if (m_aspGamePads[ui] == 0)
        {
            uiPadIndex = ui;
            break;
        }
    }

    if (uiPadIndex == 0xffffffff)
    {
        m_eLastError = NIIERR_NOOPENPORTS;
        return DIENUM_CONTINUE;
    }

    // Set the action map on it
    SetActionMapOnDIDevice(pkDevice);
    if (m_eLastError != NIIERR_OK)
        return DIENUM_CONTINUE;

    // Add it to the available devices
    // First, create a device desc
    NiDI8InputSystem::DI8Description* pkNiDesc = 
        NiNew NiDI8InputSystem::DI8Description();
    NIASSERT(pkNiDesc);
    DIDeviceInstanceToNiInputDescription(pkDDI, pkNiDesc);

    // Fill in the control descriptions for the device
    EnumerateDeviceControls(pkNiDesc, pkDevice);

    unsigned int uiCheckCount = AddAvailableDevice(pkNiDesc);
    if (uiCheckCount != pkNiDesc->GetPort() + 1)
    {
        NIASSERT(!"Mismatched device count?");
    }
    else
    {
        // Create a GamePad instance
        NiInputDI8GamePad* pkDI8GamePad = NiNew NiInputDI8GamePad(pkNiDesc, 
            pkDevice, m_iAxisRangeLow, m_iAxisRangeHigh);
        if (pkDI8GamePad)
        {
            pkDevice->AddRef();
            pkDI8GamePad->SetPadIndex(uiPadIndex);
            pkDI8GamePad->SetActionMapped(true);
            pkDI8GamePad->SetStatus(NiInputDevice::READY);
            m_aspGamePads[uiPadIndex] = pkDI8GamePad;

            MapNiActionsToGamePad(pkDI8GamePad);
        }
    }

    return DIENUM_CONTINUE;
}
//---------------------------------------------------------------------------
bool NiDI8InputSystem::SetupNiActionMap()
{
    // Make sure there is a valid action map!
    if (!m_spActionMap || (m_spActionMap->GetActionCount() == 0))
        return false;

    // Allocate the actions
    m_uiActions = m_spActionMap->GetActionCount();
    m_pkActions = NiExternalNew DIACTION[m_uiActions];
    NIASSERT(m_pkActions);

    // Fill them in
    for (unsigned int ui = 0; ui < m_uiActions; ui++)
    {
        if (!MapNiActionToDIAction(m_spActionMap->GetAction(ui), 
            &m_pkActions[ui]))
        {
            m_eLastError = NIIERR_INVALIDACTION;
            break;
        }
    }

    if (m_eLastError != NIIERR_OK)
        return false;

    // Setup the action format structure
    memset((void*)&m_kDIActionFormat, 0, sizeof(DIACTIONFORMAT));
    m_kDIActionFormat.dwSize = sizeof(DIACTIONFORMAT);
    m_kDIActionFormat.dwActionSize = sizeof(DIACTION);
    m_kDIActionFormat.dwDataSize = m_uiActions * sizeof(DWORD);
    m_kDIActionFormat.dwNumActions = m_uiActions;
    unsigned int uiSize = sizeof(GUID);
    NiMemcpy((void*)&m_kDIActionFormat.guidActionMap,
        (const void*)m_spActionMap->GetPlatformSpecificData(), uiSize);
    m_kDIActionFormat.dwGenre = DIVIRTUAL_FIGHTING_FPS;
    m_kDIActionFormat.rgoAction = m_pkActions;
    // Make buffer big enough for one of each actions
    m_kDIActionFormat.dwBufferSize = m_uiActions;
    m_kDIActionFormat.lAxisMin = m_iAxisRangeLow;
    m_kDIActionFormat.lAxisMax = m_iAxisRangeHigh;
    NiStrcpy(m_kDIActionFormat.tszActionMap, MAX_PATH,
        m_spActionMap->GetName());

    return true;
}
//---------------------------------------------------------------------------
bool NiDI8InputSystem::MapNiActionToDIAction(NiAction* pkNiAction, 
    DIACTION* pkDIAction)
{
    return true;
}
//---------------------------------------------------------------------------
NiInputErr NiDI8InputSystem::ConfigureDevices()
{
    // 
    DICOLORSET kDICS;
    ZeroMemory((void*)&kDICS, sizeof(DICOLORSET));
    kDICS.dwSize = sizeof(DICOLORSET);

    DICONFIGUREDEVICESPARAMS kDICDP;
    ZeroMemory((void*)&kDICDP, sizeof(DICONFIGUREDEVICESPARAMS));
    kDICDP.dwSize = sizeof(kDICDP);
    kDICDP.dwcUsers = 1;
    kDICDP.lptszUserNames = NULL;

    kDICDP.dwcFormats = 1;
    kDICDP.lprgFormats = &m_kDIActionFormat;
    kDICDP.hwnd = NiDI8InputSystem::GetOwnerWindow();

    // The renderer is really needed here, to support this capability in
    // full-screen.
    NiRenderer* pkRenderer = m_spRenderer;
    NIASSERT(pkRenderer);

    HRESULT hr;

    if (NiIsKindOf(NiDX9Renderer, pkRenderer))
    {
        NiDX9Renderer* pkD3DRenderer = (NiDX9Renderer*)pkRenderer;
        assert(pkD3DRenderer);
        D3DPRESENT_PARAMETERS* pkD3DPP = pkD3DRenderer->GetPresentParams();

        bool bWasFullScreen = false;
        unsigned int uiW, uiH;
        if (pkD3DPP->Windowed == false)
        {
            // Flip it
            bWasFullScreen = true;
            const NiRenderTargetGroup* pkRTGroup = 
                pkD3DRenderer->GetDefaultRenderTargetGroup();
            uiW = pkRTGroup->GetWidth(0);
            uiH = pkRTGroup->GetHeight(0);

            pkD3DRenderer->Recreate(640, 480, (NiD3DRenderer::FlagType)0, 
                GetOwnerWindow());
        }

        kDICDP.lpUnkDDSTarget = 0;
        hr = m_pkDirectInput8->ConfigureDevices(0, &kDICDP, DICD_EDIT, this);

        if (bWasFullScreen)
        {
            NiDX9Renderer::FlagType eFlag;
            eFlag = NiDX9Renderer::USE_FULLSCREEN;
            pkD3DRenderer->Recreate(uiW, uiH, eFlag, GetOwnerWindow());
        }
    }
    else if (NiIsKindOf(NiD3D10Renderer, pkRenderer))
    {
        kDICDP.lpUnkDDSTarget = 0;
        hr = m_pkDirectInput8->ConfigureDevices(0, &kDICDP, DICD_EDIT, this);
    }
    else
    {
        assert ("Unsupported renderer." && false);
    }

    // Check the results
    if (SUCCEEDED(hr))
    {
        unsigned int ui;

#if defined(_RESET_MODIFIERS_ON_CONFIGURE_)
        unsigned int auiGamePadModifiers[MAX_GAMEPADS];
        unsigned int uiKeyboardModifiers;
        unsigned int uiMouseModifiers;

        for (ui = 0; ui < MAX_GAMEPADS; ui++)
        {
            if (m_aspGamePads[ui])
                auiGamePadModifiers[ui] = m_aspGamePads[ui]->GetModifiers();
        }
        if (m_spKeyboard)
            uiKeyboardModifiers = m_spKeyboard->GetModifiers();
        if (m_spMouse)
            uiMouseModifiers = m_spMouse->GetModifiers();
#endif  //#if defined(_RESET_MODIFIERS_ON_CONFIGURE_)

        // We need to release mapped devices and regrab them.
        // This will immediately re-apply the newly assigned mappings.
        // We don't check the Mode here, as we are not using DI Action
        // Mapping for NiActionMapping at this point... We ARE using it
        // for mapping the gamepads to the 'Gamebryo' definitions, though.
//        if (m_eMode == IMMEDIATE)
        {
            // We only need to remap the gamepads
            for (ui = 0; ui < MAX_GAMEPADS; ui++)
            {
                if (m_aspGamePads[ui])
                {
                    NiInputGamePad* pkGamePad = m_aspGamePads[ui];
                    NiInputDI8GamePad* pkDI8GamePad = 
                        (NiInputDI8GamePad*)pkGamePad;

                    LPDIRECTINPUTDEVICE8 pkDIDevice = 
                        pkDI8GamePad->GetDIDevice();

                    NiInputErr eRemapErr = SetActionMapOnDIDevice(
                        pkDIDevice);
                    if (eRemapErr != NIIERR_OK)
                    {
                        NIASSERT(!"Failed to set action map!");
                    }
                }
            }
        }

#if defined(_RESET_MODIFIERS_ON_CONFIGURE_)
        // We need to remove all modifiers on the devices, as the call to
        // bring up the interface will cause all releases to be missed.
        for (ui = 0; ui < MAX_GAMEPADS; ui++)
        {
            if (m_aspGamePads[ui])
            {
                m_aspGamePads[ui]->ClearModifiers();
                m_aspGamePads[ui]->AddModifiers(auiGamePadModifiers[ui]);
            }
        }
        if (m_spKeyboard)
        {
            m_spKeyboard->ClearModifiers();
            m_spKeyboard->AddModifiers(uiKeyboardModifiers);
        }
        if (m_spMouse)
        {
            m_spMouse->ClearModifiers();
            m_spMouse->AddModifiers(uiMouseModifiers);
        }
#else   //#if defined(_RESET_MODIFIERS_ON_CONFIGURE_)
        // Need to re-acquite all the device
        for (ui = 0; ui < MAX_GAMEPADS; ui++)
        {
            if (m_aspGamePads[ui])
                m_aspGamePads[ui]->HandleInsertion();
        }
        if (m_spKeyboard)
            m_spKeyboard->HandleInsertion();
        if (m_spMouse)
            m_spMouse->HandleInsertion();

        // Now, update them
        UpdateAllDevices();
#endif  //#if defined(_RESET_MODIFIERS_ON_CONFIGURE_)
    }

    return NIIERR_OK;
}
//---------------------------------------------------------------------------
