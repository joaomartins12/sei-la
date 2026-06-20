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

// Precompiled Header
#include "NiApplicationPCH.h"

#include "NiDX9Select.h"

#include <NiDX9Renderer.h>
#include <NiDX9Headers.h>
#include <NiDX9SystemDesc.h>
#include <NiRTLib.h>
#include <NiTList.h>

#ifdef NIDX9RENDERER_IMPORT
    #pragma comment(lib, "NiDX9Renderer" NI_DLL_SUFFIX ".lib")
#else   // #ifdef NIDX9RENDERER_IMPORT
    #pragma comment(lib, "NiDX9Renderer.lib")
#endif  // #ifdef NIDX9RENDERER_IMPORT

const NiDX9DeviceDesc* NiDX9Select::ms_pkDevice = NULL;
const void* NiDX9Select::ms_pkMode = NULL;
const NiDX9AdapterDesc* NiDX9Select::ms_pkAdapter = NULL;
const NiDX9SystemDesc* NiDX9Select::ms_pkInfo = NULL;
bool NiDX9Select::ms_bFullscreen = false;
bool NiDX9Select::ms_b32BitZBuffer = false;
unsigned int NiDX9Select::ms_uiMultisample = NiDX9Renderer::FBMODE_DEFAULT;
bool NiDX9Select::ms_bDialogOpen = false;
unsigned int NiDX9Select::ms_uiDefaultWidth = 640;
unsigned int NiDX9Select::ms_uiDefaultHeight = 480;


//---------------------------------------------------------------------------
void NiDX9Select::EnumerateAdapters(NiWindowRef pDialog)
{
    SendDlgItemMessage(pDialog, IDC_ADAPTER_COMBO, CB_RESETCONTENT, 0, 0);

    unsigned int uiAdapterCount = ms_pkInfo->GetAdapterCount();

    bool bAdaptersExist = false;
    for (unsigned int i = 0; i < uiAdapterCount; i++)
    {
        const NiDX9AdapterDesc* pkAdapter = ms_pkInfo->GetAdapter(i);
        NIASSERT(pkAdapter);

        const NiDX9DeviceDesc* pkDevice = pkAdapter->GetDevice(D3DDEVTYPE_HAL);
        if (!pkDevice)
            continue;

        // we skip fullscreen - only devices unless we are looking
        // at fullscreen mode.  Also, we skip SW devices for now.
        // Later, we will add just the primary device as a SW device
        if (!ms_bFullscreen)
        {
            // test if device can render to window in the current display mode
            if (!pkAdapter->CanDeviceRenderWindowed(D3DDEVTYPE_HAL))
                continue;
        }

        // Add the Adapter name to the combo box 
        int iIndex = SendDlgItemMessage(pDialog, IDC_ADAPTER_COMBO, 
            CB_ADDSTRING, 0, (LPARAM)pkAdapter->GetStringDescription());
        SendDlgItemMessage(pDialog, IDC_ADAPTER_COMBO, 
            CB_SETITEMDATA, (WPARAM)iIndex, (LPARAM)pkAdapter);

        bAdaptersExist = true;
    }

    SendDlgItemMessage(pDialog, IDC_ADAPTER_COMBO, CB_SETCURSEL, (WPARAM)0, 
        0L);

    if (bAdaptersExist)
    {
        ms_pkAdapter = (const NiDX9AdapterDesc*)SendDlgItemMessage(pDialog, 
            IDC_ADAPTER_COMBO, CB_GETITEMDATA, (WPARAM)0, (LPARAM)0);
        ms_pkDevice = ms_pkAdapter->GetDevice(D3DDEVTYPE_HAL);
    }
    else
    {
        ms_pkAdapter = NULL;
        ms_pkDevice = NULL;
    }
}
//---------------------------------------------------------------------------
void NiDX9Select::EnumerateResolutions(NiWindowRef pDialog)
{
    SendDlgItemMessage(pDialog, IDC_RESOLUTION_COMBO, CB_RESETCONTENT, 0, 0);

    // walk list adding items
    if (!ms_bFullscreen)
        return;

    if (!ms_pkAdapter)
        return;

    unsigned int uiModeCount = ms_pkAdapter->GetModeCount();

    int i16Default = -1;
    int i32Default = -1;
    bool bResolutionsExist = false;
    
    for (unsigned int i = 0; i < uiModeCount; i++)
    {
        const NiDX9AdapterDesc::ModeDesc* pkMode = ms_pkAdapter->GetMode(i);

        char acStr[256];
        NiSprintf(acStr, 256, "%u x %u x %u bpp", pkMode->m_uiWidth, 
            pkMode->m_uiHeight, pkMode->m_uiBPP);
        
        // Add the mode string to the combo box 
        // Note - (could also use pStrDesc)
        int iIndex = SendDlgItemMessage(pDialog, IDC_RESOLUTION_COMBO, 
            CB_ADDSTRING, 0, (LPARAM)acStr);
        SendDlgItemMessage(pDialog, IDC_RESOLUTION_COMBO, 
            CB_SETITEMDATA, (WPARAM)iIndex, (LPARAM)pkMode);

        if ((pkMode->m_uiWidth == ms_uiDefaultWidth) && 
            (pkMode->m_uiHeight == ms_uiDefaultHeight))
        {
            if (pkMode->m_uiBPP == 32)
                i32Default = iIndex;
            else if (pkMode->m_uiBPP == 16)
                i16Default = iIndex;
        }

        bResolutionsExist = true;
    }

    int iDefault = (i32Default > -1 ? i32Default : i16Default);
    SendDlgItemMessage(pDialog, IDC_RESOLUTION_COMBO, CB_SETCURSEL, 
        (WPARAM)iDefault, 0L);

    if (bResolutionsExist)
    {
        ms_pkMode = (const void*)SendDlgItemMessage(pDialog,
            IDC_RESOLUTION_COMBO, CB_GETITEMDATA, 0, 0);
    }
    else
    {
        ms_pkMode = NULL;
    }
}
//---------------------------------------------------------------------------
void NiDX9Select::EnumerateMultisamples(NiWindowRef pDialog)
{
    SendDlgItemMessage(pDialog, IDC_MULTISAMPLE_COMBO, CB_RESETCONTENT, 0, 0);

    // walk list adding items
    if (!ms_pkDevice)
        return;

    D3DFORMAT eFormat;
    if (ms_pkMode)
    {
        NiDX9AdapterDesc::ModeDesc* pkModeDesc = 
            (NiDX9AdapterDesc::ModeDesc*)ms_pkMode;
        eFormat = pkModeDesc->m_eD3DFormat;
    }
    else
    {
        NIASSERT(!ms_bFullscreen);
        unsigned int uiBitDepth = (ms_b32BitZBuffer ? 32 : 16);
        NiDX9Renderer::FrameBufferFormat eNiFormat = 
            ms_pkDevice->GetNearestFrameBufferFormat(!ms_bFullscreen, 
            uiBitDepth);
        eFormat = NiDX9Renderer::GetD3DFormat(eNiFormat);
    }

    const NiDX9DeviceDesc::DisplayFormatInfo* pkDFI = 
        ms_pkDevice->GetFormatInfo(eFormat);

    NiDX9Renderer::DepthStencilFormat eNiDSFormat = 
        pkDFI->FindClosestDepthStencil(ms_b32BitZBuffer ? 32 : 16, 0);
    D3DFORMAT eDSFormat = NiDX9Renderer::GetD3DFormat(eNiDSFormat);
    
    // Fill in dialog box
    const unsigned int uiStrLen = 256;
    char acStr[uiStrLen];
    NiSprintf(acStr, uiStrLen, "No Multisampling");
    
    // Add the mode string to the combo box 
    // Note - (could also use pStrDesc)
    int iBaseIndex = SendDlgItemMessage(pDialog, IDC_MULTISAMPLE_COMBO, 
        CB_ADDSTRING, 0, (LPARAM)acStr);
    SendDlgItemMessage(pDialog, IDC_MULTISAMPLE_COMBO, 
        CB_SETITEMDATA, (WPARAM)iBaseIndex, (LPARAM)0);

    unsigned int uiQuality = pkDFI->GetMultiSampleQualityLevels(
        D3DMULTISAMPLE_NONMASKABLE, !ms_bFullscreen, eFormat, eDSFormat);

    unsigned int i;
    for (i = 0; i < uiQuality; i++)
    {
        NiSprintf(acStr, uiStrLen, "Nonmaskable Level %d", i);
    
        int iIndex = SendDlgItemMessage(pDialog, IDC_MULTISAMPLE_COMBO, 
            CB_ADDSTRING, 0, (LPARAM)acStr);
        SendDlgItemMessage(pDialog, IDC_MULTISAMPLE_COMBO, 
            CB_SETITEMDATA, (WPARAM)iIndex, 
            (LPARAM)(NiDX9Renderer::FBMODE_MULTISAMPLES_NONMASKABLE | i));
    }

    for (unsigned int i = 1; i < 16; i++)
    {
        D3DMULTISAMPLE_TYPE eType = (D3DMULTISAMPLE_TYPE)(i + 1);
    
        if (pkDFI->IsMultiSampleValid(!ms_bFullscreen, eType, eFormat, 
            eDSFormat))
        {
            uiQuality = pkDFI->GetMultiSampleQualityLevels(eType,
                !ms_bFullscreen, eFormat, eDSFormat);

            for (unsigned int j = 0; j < uiQuality; j++)
            {
                if (uiQuality > 1)
                {
                    NiSprintf(acStr, uiStrLen, "%d Multisamples, Level %d", 
                        i + 1, j);
                }
                else
                {
                    NiSprintf(acStr, uiStrLen, "%d Multisamples", i + 1);
                }

                int iIndex = SendDlgItemMessage(pDialog, IDC_MULTISAMPLE_COMBO,
                    CB_ADDSTRING, 0, (LPARAM)acStr);
                SendDlgItemMessage(pDialog, IDC_MULTISAMPLE_COMBO, 
                    CB_SETITEMDATA, (WPARAM)iIndex, (LPARAM)(i << 16 | j));
            }
        }
    }

    SendDlgItemMessage(pDialog, IDC_MULTISAMPLE_COMBO, CB_SETCURSEL, 
        (WPARAM)iBaseIndex, 0L);

    ms_uiMultisample = NiDX9Renderer::FBMODE_DEFAULT;
}
//---------------------------------------------------------------------------
bool NiDX9Select::UpdateAdapter(NiWindowRef pDialog)
{
    bool bReturn = true;

    int iIndex = SendDlgItemMessage(pDialog, IDC_ADAPTER_COMBO, 
        CB_GETCURSEL, 0, 0);
    if (iIndex >= 0)
    {
        const NiDX9AdapterDesc* pkAdapter = 
            (const NiDX9AdapterDesc*)SendDlgItemMessage(pDialog, 
            IDC_ADAPTER_COMBO, CB_GETITEMDATA, iIndex, 0);
        bReturn = (pkAdapter != ms_pkAdapter);

        ms_pkAdapter = pkAdapter;
        ms_pkDevice = ms_pkAdapter->GetDevice(D3DDEVTYPE_HAL);
    }
    else
    {
        ms_pkDevice = NULL;
    }

    return bReturn;
}
//---------------------------------------------------------------------------
bool NiDX9Select::UpdateResolution(NiWindowRef pDialog)
{
    bool bReturn = true;

    int iIndex = SendDlgItemMessage(pDialog, IDC_RESOLUTION_COMBO, 
        CB_GETCURSEL, 0, 0);
    if (iIndex >= 0)
    {
        void* pvMode = (void*)SendDlgItemMessage(pDialog, 
            IDC_RESOLUTION_COMBO, CB_GETITEMDATA, iIndex, 0);
        bReturn = (pvMode != ms_pkMode);

        ms_pkMode = pvMode;
    }
    else
    {
        ms_pkMode = NULL;
    }

    return bReturn;
}
//---------------------------------------------------------------------------
bool NiDX9Select::UpdateMultisample(NiWindowRef pDialog)
{
    bool bReturn = true;

    int iIndex = SendDlgItemMessage(pDialog, IDC_MULTISAMPLE_COMBO, 
        CB_GETCURSEL, 0, 0);
    if (iIndex >= 0)
    {
        unsigned int uiMultisample = (unsigned int)SendDlgItemMessage(
            pDialog, IDC_MULTISAMPLE_COMBO, CB_GETITEMDATA, iIndex, 0);
        bReturn = (uiMultisample != ms_uiMultisample);

        ms_uiMultisample = uiMultisample;
    }
    else
    {
        ms_uiMultisample = NiDX9Renderer::FBMODE_DEFAULT;
    }

    return bReturn;
}
//---------------------------------------------------------------------------
BOOL CALLBACK NiDX9Select::Callback(NiWindowRef pDialog, unsigned int uiMsg, 
    WPARAM wParam, LPARAM lParam)
{
    switch (uiMsg)
    {
        case WM_INITDIALOG:
        {
            SetWindowText(pDialog, "Select DX9 Display Device");

            SendDlgItemMessage(pDialog, IDC_MODE_COMBO, CB_RESETCONTENT, 0, 
                0);

            // Add "windowed" and "fullscreen" to the mode combo box
            // and set the item data to be true for fullscreen and
            // false for windowed mode
            int iIndex = SendDlgItemMessage(pDialog, IDC_MODE_COMBO, 
                CB_ADDSTRING, 0, (LPARAM)"Windowed");
            SendDlgItemMessage(pDialog, IDC_MODE_COMBO, CB_SETITEMDATA, 
                (WPARAM)iIndex, (LPARAM)false);

            SendDlgItemMessage(pDialog, IDC_MODE_COMBO, CB_SETCURSEL, 
                (WPARAM)0, 0L);
            ms_bFullscreen = false;

            iIndex = SendDlgItemMessage(pDialog, IDC_MODE_COMBO, 
                CB_ADDSTRING, 0, (LPARAM)"Fullscreen");
            SendDlgItemMessage(pDialog, IDC_MODE_COMBO, CB_SETITEMDATA, 
                (WPARAM)iIndex, (LPARAM)true);

            ms_bDialogOpen = true;

            // Find all of the Adapters and add them to the
            // combo box interface
            EnumerateAdapters(pDialog);
            EnumerateResolutions(pDialog);
            EnumerateMultisamples(pDialog);

            // Check 24/32-bit Zbuffer as default
            CheckDlgButton(pDialog, IDC_ZBUFFER_CHECK, BST_CHECKED);

            return TRUE;
        }
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case IDC_MODE_COMBO:
                {
                    if (!ms_bDialogOpen)
                        return TRUE;

                    // if the user has changed the screen mode, then we
                    // must regenerate the Adapter list box, as we will
                    // enumerate different sets of devices based on the
                    // settings (e.g. in windowed mode, we only enumerate
                    // windowed display cards)
                    int iIndex = SendDlgItemMessage(pDialog, IDC_MODE_COMBO, 
                        CB_GETCURSEL, 0, 0);

                    bool bOldMode = ms_bFullscreen;
                    if (iIndex >= 0)
                    {
                        ms_bFullscreen = SendDlgItemMessage(pDialog, 
                            IDC_MODE_COMBO, CB_GETITEMDATA, iIndex, 0) ? 
                            true : false;
                    }

                    if (bOldMode != ms_bFullscreen)
                    {
                        EnumerateAdapters(pDialog);
                        EnumerateResolutions(pDialog);
                        EnumerateMultisamples(pDialog);
                    }

                    return TRUE;
                }
                case IDC_ADAPTER_COMBO:
                {
                    if (!ms_bDialogOpen)
                        return TRUE;

                    // only regenerate resolutions and multisamples if
                    // the Adapter has changed
                    bool bNewAdapter = UpdateAdapter(pDialog);

                    if (bNewAdapter)
                    {
                        EnumerateResolutions(pDialog);
                        EnumerateMultisamples(pDialog);
                    }

                    return TRUE;
                }
                case IDC_RESOLUTION_COMBO:
                {
                    if (!ms_bDialogOpen)
                        return TRUE;

                    bool bNewResolution = UpdateResolution(pDialog);

                    if (bNewResolution)
                    {
                        EnumerateMultisamples(pDialog);
                    }

                    return TRUE;
                }
                case IDOK:
                {
                    // on acceptance, find the selected Adapter
                    UpdateAdapter(pDialog);
                    UpdateResolution(pDialog);
                    UpdateMultisample(pDialog);

                    ms_bDialogOpen = false;

                    if (BST_CHECKED & SendDlgItemMessage(pDialog, 
                        IDC_ZBUFFER_CHECK, BM_GETSTATE, 0, 0))
                    {
                        ms_b32BitZBuffer = true;
                    }
                    else
                    {
                        ms_b32BitZBuffer = false;
                    }

                    SendDlgItemMessage(pDialog, IDC_MODE_COMBO, 
                        CB_RESETCONTENT, 0, 0);
                    SendDlgItemMessage(pDialog, IDC_ADAPTER_COMBO, 
                        CB_RESETCONTENT, 0, 0);
                    SendDlgItemMessage(pDialog, IDC_RESOLUTION_COMBO, 
                        CB_RESETCONTENT, 0, 0);
                    SendDlgItemMessage(pDialog, IDC_MULTISAMPLE_COMBO, 
                        CB_RESETCONTENT, 0, 0);
                    EndDialog(pDialog, 1);
                    return TRUE;
                }
                case IDCANCEL:
                {
                    // on cancel, mark the box as cancelled and return
                    ms_bDialogOpen = false;

                    SendDlgItemMessage(pDialog, IDC_MODE_COMBO, 
                        CB_RESETCONTENT, 0, 0);
                    SendDlgItemMessage(pDialog, IDC_ADAPTER_COMBO, 
                        CB_RESETCONTENT, 0, 0);
                    SendDlgItemMessage(pDialog, IDC_RESOLUTION_COMBO, 
                        CB_RESETCONTENT, 0, 0);
                    SendDlgItemMessage(pDialog, IDC_MULTISAMPLE_COMBO, 
                        CB_RESETCONTENT, 0, 0);
                    EndDialog(pDialog, 0);
                    return TRUE;
                }
            }
            break;
        }
    }

    return FALSE;
}
//---------------------------------------------------------------------------
void NiDX9Select::CopyToWideChar(unsigned short** ppusStrOut, 
    const char* pcStrIn)
{
    unsigned int uiLen  = lstrlen(pcStrIn);
    unsigned short* pStrOut = *ppusStrOut;

    if (uiLen)
        uiLen = MultiByteToWideChar(CP_ACP, 0, pcStrIn, uiLen, 
        (LPWSTR)pStrOut, uiLen);
    pStrOut[uiLen++] = L'\0'; // Add the null terminator
    *ppusStrOut += uiLen;
}
//---------------------------------------------------------------------------
void NiDX9Select::AddDialogControl(unsigned short** ppusTemplate, 
    unsigned int uiStyle, short sSX, short sSY, short sCX, short sCY, 
    unsigned short usID, const char* pcStrClassName, const char* pcStrTitle)
{
    // DWORD align the current ptr
    DLGITEMTEMPLATE* pkTemplate = (DLGITEMTEMPLATE*)
        (((((ULONG)(*ppusTemplate))+3)>>2)<<2);

    pkTemplate->style = uiStyle | WS_CHILD | WS_VISIBLE;
    pkTemplate->dwExtendedStyle = 0L;
    pkTemplate->x = sSX;
    pkTemplate->y = sSY;
    pkTemplate->cx = sCX;
    pkTemplate->cy = sCY;
    pkTemplate->id = usID;

    *ppusTemplate = (unsigned short*)(++pkTemplate); // Advance ptr

    CopyToWideChar((unsigned short**)ppusTemplate, pcStrClassName);//Set Name
    CopyToWideChar((unsigned short**)ppusTemplate, pcStrTitle);    //Set Title

    (*ppusTemplate)++;          // Skip Extra Stuff
}
//---------------------------------------------------------------------------
DLGTEMPLATE* NiDX9Select::BuildAdapterSelectTemplate()
{
    // Allocate ample memory for building the template
    DLGTEMPLATE* pkDlgTemplate = NiExternalNew DLGTEMPLATE[50];
    if (NULL == pkDlgTemplate)
        return NULL;
    ZeroMemory(pkDlgTemplate, 50 * sizeof(DLGTEMPLATE));
    
    // Fill in the DLGTEMPLATE info
    DLGTEMPLATE* pkDT = pkDlgTemplate;
    pkDT->style = DS_MODALFRAME | DS_NOIDLEMSG | DS_SETFOREGROUND | DS_3DLOOK 
        | DS_CENTER | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU 
        | DS_SETFONT;
    pkDT->dwExtendedStyle = 0L;
    pkDT->cdit = 11;
    pkDT->x = 0;
    pkDT->y = 0;
    pkDT->cx = 200;
    pkDT->cy = 140;

    // Add menu array, class array, dlg title, font size and font name
    unsigned short* pusW = (unsigned short*)(++pkDT);
    *pusW++ = L'\0';         // Set Menu array to nothing
    *pusW++ = L'\0';         // Set Class array to nothing
    CopyToWideChar((unsigned short**)&pusW, 
        TEXT("Select Desired DirectX9 Device"));
    *pusW++ = 8;                                            // Font Size
    CopyToWideChar(&pusW, TEXT("Arial")); // Font Name
 
    // Add the mode select combobox
    AddDialogControl(&pusW, CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 
        55, 5, 133, 45, IDC_MODE_COMBO, TEXT("COMBOBOX"), TEXT(""));

    // Add the "Mode:" text
    AddDialogControl(&pusW, SS_LEFT, 5, 5, 41, 13, IDC_STATIC, TEXT("STATIC"),
        TEXT("Mode:"));

    // Add the Adapter select combobox
    AddDialogControl(&pusW, CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 55, 27,
        133, 45, IDC_ADAPTER_COMBO, TEXT("COMBOBOX"), TEXT(""));

    // Add the "Adapter:" text
    AddDialogControl(&pusW, SS_LEFT, 5, 27, 41, 13, IDC_STATIC, TEXT("STATIC"),
        TEXT("Adapter:"));

    // Add the resolution select combobox
    AddDialogControl(&pusW, CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 55, 49,
        133, 45, IDC_RESOLUTION_COMBO, TEXT("COMBOBOX"), TEXT(""));

    // Add the "Resolution:" text
    AddDialogControl(&pusW, SS_LEFT, 5, 49, 41, 13, IDC_STATIC, TEXT("STATIC"),
        TEXT("Resolution:"));

    AddDialogControl(&pusW, BS_AUTOCHECKBOX | WS_TABSTOP, 40, 66, 120, 14, 
        IDC_ZBUFFER_CHECK, TEXT("BUTTON"), 
        TEXT("Use 24/32-bit ZBuffer if available"));

    // Add the multisample select combobox
    AddDialogControl(&pusW, CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 55, 88,
        133, 45, IDC_MULTISAMPLE_COMBO, TEXT("COMBOBOX"), TEXT(""));

    // Add the "MultiSample:" text
    AddDialogControl(&pusW, SS_LEFT, 5, 88, 41, 13, IDC_STATIC, TEXT("STATIC"),
        TEXT("MultiSample:"));

    // Add the cancel button
    AddDialogControl(&pusW, BS_PUSHBUTTON | WS_TABSTOP, 123, 110, 51, 14, 
        IDCANCEL, TEXT("BUTTON"), TEXT("CANCEL"));

    // Add the okay button
    AddDialogControl(&pusW, BS_PUSHBUTTON | WS_TABSTOP, 40, 110, 51, 14, 
        IDOK, TEXT("BUTTON"), TEXT("OK"));

    return pkDlgTemplate;
}
//---------------------------------------------------------------------------
NiDX9Select::ReturnVal NiDX9Select::Select(NiWindowRef pkWnd, 
    unsigned int uiDefaultWidth, unsigned int uiDefaultHeight,
    bool& bFullscreen, bool& b32BitZBuffer, const void*& pvAdapter, 
    const void*& pvDevice, const void*& pkMode, unsigned int& uiFBMode)
{
    bFullscreen = false;

    ms_uiDefaultWidth = uiDefaultWidth;
    ms_uiDefaultHeight = uiDefaultHeight;

    // Pop up a dialog box for the user's choice of Adapter/device/mode
    NiInstanceRef pInstance = (NiInstanceRef)
        GetWindowLong(pkWnd, GWL_HINSTANCE);

    // Create dynamic dialog template
    DLGTEMPLATE* pkDlgSelect = BuildAdapterSelectTemplate();
    unsigned int uiReturn = 0;
    if (pkDlgSelect)
    {
        // Create dialog box from template
        uiReturn = DialogBoxIndirectParam(pInstance, pkDlgSelect, pkWnd, 
            (DLGPROC)Callback, 0L);
        NiExternalDelete[] pkDlgSelect;
    }

    if (uiReturn)
    {
        pvAdapter = ms_pkAdapter;
        pvDevice = ms_pkDevice;

        bFullscreen = ms_bFullscreen;

        if (ms_bFullscreen)
            pkMode = ms_pkMode;
        else
            pkMode = NULL;

        b32BitZBuffer = ms_b32BitZBuffer;
        
        uiFBMode = ms_uiMultisample;

        return RETURN_OK;
    }
    else
    {
        return RETURN_CANCEL;
    }
}
//---------------------------------------------------------------------------
NiRenderer* NiDX9Select::DialogCreateRenderer(NiWindowRef pkTopLevelWnd, 
    NiWindowRef pkRenderWnd, unsigned int uiDefaultWidth, 
    unsigned int uiDefaultHeight, bool bStencil, bool bMultiThread, 
    bool bRefRast, bool bSWVertex, bool& bNVPerfHUD, bool& bFullscreen)
{
    NiRenderer* pkRenderer = NULL;

    // save the position and size of the window in case we have to
    // put the window back there later
    RECT kRect;
    GetWindowRect(pkTopLevelWnd, &kRect);

    while (!pkRenderer)
    {
        unsigned int eFlags = 0;
        
        if (bStencil)
            eFlags |= NiDX9Renderer::USE_STENCIL;

        if (bMultiThread)
            eFlags |= NiDX9Renderer::USE_MULTITHREADED;

        const NiDX9AdapterDesc::ModeDesc* pkMode = NULL;
        const NiDX9DeviceDesc* pkDevice = NULL;
        const NiDX9AdapterDesc* pkAdapter = NULL;
        bool bUse32BitZBuffer = NULL;
        unsigned int uiFBMode = 0;

        // select and set up the user's desired DirectDraw object
        // and return the desired renderer mode and display mode
        NiDX9Select::ReturnVal eRet = NiDX9Select::Select(pkTopLevelWnd,
            uiDefaultWidth, uiDefaultHeight,  bFullscreen, bUse32BitZBuffer, 
            (const void *&)pkAdapter, (const void *&)pkDevice, 
            (const void *&)pkMode, uiFBMode);

        if (eRet == NiDX9Select::RETURN_CANCEL)
            return false;

        if (!bUse32BitZBuffer)
            eFlags |= NiDX9Renderer::USE_16BITBUFFERS;

        // select fullscreen or windowed, based on user selection
        if (bFullscreen)
        {
            eFlags |= NiDX9Renderer::USE_FULLSCREEN;
 
            if (pkAdapter && pkDevice && pkMode)
            {
                NiDX9Renderer::DeviceDesc eDesc = NiDX9Renderer::DEVDESC_PURE;

                if (bRefRast || pkDevice->GetDeviceType() == D3DDEVTYPE_REF) 
                    eDesc = NiDX9Renderer::DEVDESC_REF;
                else if (bSWVertex)
                    eDesc = NiDX9Renderer::DEVDESC_HAL_SWVERTEX;

                // Handle nVidia's NVPerfHUD adapter in a special manner
                const char* pcAdapterName = pkAdapter->GetStringDescription();
                if (strstr(pcAdapterName, "NVPerfHUD"))
                {
                    bNVPerfHUD = true;
                    if (bRefRast || bSWVertex)
                        eDesc = NiDX9Renderer::DEVDESC_REF;
                    else
                        eDesc = NiDX9Renderer::DEVDESC_REF_HWVERTEX;
                }

                // create and return the renderer
                pkRenderer = NiDX9Renderer::Create(
                    pkMode->m_uiWidth, pkMode->m_uiHeight, 
                    (NiDX9Renderer::FlagType)eFlags, 
                    pkTopLevelWnd, pkTopLevelWnd, pkAdapter->GetAdapterIndex(),
                    eDesc, NiDX9Renderer::GetNiFBFormat(
                    pkMode->m_eD3DFormat),NiDX9Renderer::DSFMT_UNKNOWN,
                    NiDX9Renderer::PRESENT_INTERVAL_IMMEDIATE,
                    NiDX9Renderer::SWAPEFFECT_DEFAULT, uiFBMode);
                // Change PRESENT_INTERVAL_IMMEDIATE to PRESENT_INTERVAL_ONE to
                // lock to v-sync.
            }
            else
            {
                // create and return the renderer
                pkRenderer = NiDX9Renderer::Create(640, 480, 
                    (NiDX9Renderer::FlagType)eFlags, 
                    pkTopLevelWnd, pkTopLevelWnd);
            }
        }
        else
        {
            NiDX9Renderer::DeviceDesc eDesc = NiDX9Renderer::DEVDESC_PURE;
            if (bRefRast || pkDevice->GetDeviceType() == D3DDEVTYPE_REF) 
                eDesc = NiDX9Renderer::DEVDESC_REF;
            else if (bSWVertex)
                eDesc = NiDX9Renderer::DEVDESC_HAL_SWVERTEX;

            // Handle nVidia's NVPerfHUD adapter in a special manner
            const char* pcAdapterName = pkAdapter->GetStringDescription();
            if (strstr(pcAdapterName, "NVPerfHUD"))
            {
                if (bRefRast)
                    eDesc = NiDX9Renderer::DEVDESC_REF;
                else if (bSWVertex)
                    eDesc = NiDX9Renderer::DEVDESC_REF_MIXEDVERTEX;
                else
                    eDesc = NiDX9Renderer::DEVDESC_REF_HWVERTEX;
            }

            // create and return the renderer
            pkRenderer = NiDX9Renderer::Create(0, 0, 
                (NiDX9Renderer::FlagType)eFlags, pkRenderWnd, pkTopLevelWnd, 
                pkAdapter->GetAdapterIndex(), eDesc, 
                NiDX9Renderer::FBFMT_UNKNOWN, NiDX9Renderer::DSFMT_UNKNOWN,
                NiDX9Renderer::PRESENT_INTERVAL_IMMEDIATE,
                NiDX9Renderer::SWAPEFFECT_DEFAULT, uiFBMode);
            // Change PRESENT_INTERVAL_IMMEDIATE to PRESENT_INTERVAL_ONE to
            // lock to v-sync.
        }

        if (!pkRenderer)
        {
            // Put the window back where it was if the renderer
            // creation failed.  If this is not done, then the
            // window could be left at the size of the last attempted
            // fullscreen renderer
            MoveWindow(pkTopLevelWnd, kRect.left, kRect.top, 
                kRect.right - kRect.left, kRect.bottom-kRect.top, TRUE);
        }
    }

    return pkRenderer;
}
//---------------------------------------------------------------------------
NiRendererPtr NiDX9Select::CreateRenderer(NiWindowRef pkTopLevelWnd, 
    NiWindowRef pkRenderWnd, bool bDialog, unsigned int uiBitDepth, 
    unsigned int uiWidth, unsigned int uiHeight, bool bStencil,
    bool bMultiThread, bool bRefRast, bool bSWVertex, bool& bNVPerfHUD,
    bool& bFullscreen)
{
    ms_pkInfo = NiDX9Renderer::GetSystemDesc();

    if (ms_pkInfo == NULL)
        return NULL;

    NiRendererPtr spRenderer = NULL;

    if (!bDialog || uiBitDepth != 0)
    {
        unsigned int eFlags = 0;
    
        if (bStencil)
            eFlags |= NiDX9Renderer::USE_STENCIL;

        if (bMultiThread)
            eFlags |= NiDX9Renderer::USE_MULTITHREADED;

        unsigned int uiAdapter = D3DADAPTER_DEFAULT;
        NiDX9Renderer::DeviceDesc eDevType = NiDX9Renderer::DEVDESC_PURE;
        if (bNVPerfHUD)
        {
            // Look for 'NVIDIA NVPerfHUD' adapter
            // If it is present, override default settings
            NIASSERT(ms_pkInfo);
            unsigned int uiAdapterCount = ms_pkInfo->GetAdapterCount();

            unsigned int i;
            for (i = 0; i < uiAdapterCount; i++)
            {
                const NiDX9AdapterDesc* pkAdapter = ms_pkInfo->GetAdapter(i);
                const char* pcAdapterName = pkAdapter->GetStringDescription();

                if (strstr(pcAdapterName,"NVPerfHUD"))
                {
                    bNVPerfHUD = true;
                    uiAdapter = i;
                    if (bRefRast)
                        eDevType = NiDX9Renderer::DEVDESC_REF;
                    else if (bSWVertex)
                        eDevType = NiDX9Renderer::DEVDESC_REF_MIXEDVERTEX;
                    else
                        eDevType = NiDX9Renderer::DEVDESC_REF_HWVERTEX;
                    break;
                }
            }
        }

        if (uiBitDepth == 16 || uiBitDepth == 32)
        {
            eFlags |= NiDX9Renderer::USE_FULLSCREEN;

            if (uiBitDepth == 16)
                eFlags |= NiDX9Renderer::USE_16BITBUFFERS;

            spRenderer = NiDX9Renderer::Create(uiWidth, uiHeight, 
                (NiDX9Renderer::FlagType)eFlags, 
                pkTopLevelWnd, pkTopLevelWnd, uiAdapter, eDevType, 
                NiDX9Renderer::FBFMT_UNKNOWN, 
                NiDX9Renderer::DSFMT_UNKNOWN,
                NiDX9Renderer::PRESENT_INTERVAL_IMMEDIATE);
            // Change PRESENT_INTERVAL_IMMEDIATE to PRESENT_INTERVAL_ONE to
            // lock to v-sync.
        }
        else
        {
            if (bFullscreen)
            {
                eFlags |= NiDX9Renderer::USE_FULLSCREEN;
    
                spRenderer = NiDX9Renderer::Create(uiWidth, uiHeight, 
                    (NiDX9Renderer::FlagType)eFlags, 
                    pkTopLevelWnd, pkTopLevelWnd, uiAdapter, eDevType, 
                    NiDX9Renderer::FBFMT_UNKNOWN, 
                    NiDX9Renderer::DSFMT_UNKNOWN,
                    NiDX9Renderer::PRESENT_INTERVAL_IMMEDIATE);
                // Change PRESENT_INTERVAL_IMMEDIATE to PRESENT_INTERVAL_ONE to
                // lock to v-sync.
            }
            else
            {
                spRenderer = NiDX9Renderer::Create(0, 0, 
                    (NiDX9Renderer::FlagType)eFlags, 
                    pkRenderWnd, pkRenderWnd, uiAdapter, eDevType, 
                    NiDX9Renderer::FBFMT_UNKNOWN, 
                    NiDX9Renderer::DSFMT_UNKNOWN,
                    NiDX9Renderer::PRESENT_INTERVAL_IMMEDIATE);
                // Change PRESENT_INTERVAL_IMMEDIATE to PRESENT_INTERVAL_ONE to
                // lock to v-sync.
            }
        }
    }    
    else
    {
        spRenderer = DialogCreateRenderer(pkTopLevelWnd, pkRenderWnd, 
            uiWidth, uiHeight, bStencil, bMultiThread, bRefRast, bSWVertex, 
            bNVPerfHUD, bFullscreen);
    }

    
    return spRenderer;
}
//---------------------------------------------------------------------------
