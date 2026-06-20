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

#include "NiD3D10Select.h"

#include <NiD3D10Renderer.h>
#include <NiD3D10Headers.h>
#include <NiD3D10SystemDesc.h>
#include <NiRTLib.h>
#include <NiTList.h>

#ifdef NID3D10RENDERER_IMPORT
    #pragma comment(lib, "NiD3D10Renderer" NI_DLL_SUFFIX ".lib")
#else   // #ifdef NID3D10RENDERER_IMPORT
    #pragma comment(lib, "NiD3D10Renderer.lib")
#endif  // #ifdef NID3D10RENDERER_IMPORT

bool NiD3D10Select::ms_bDialogOpen = false;
bool NiD3D10Select::ms_b32BitZBuffer = true;
bool NiD3D10Select::ms_bFullscreen = false;
bool NiD3D10Select::ms_bRefRast = false;
unsigned int NiD3D10Select::ms_uiDefaultWidth = 640;
unsigned int NiD3D10Select::ms_uiDefaultHeight = 480;
unsigned int NiD3D10Select::ms_uiMultisampleCount = 1;
unsigned int NiD3D10Select::ms_uiMultisampleQuality = 0;

const NiD3D10AdapterDesc* NiD3D10Select::ms_pkAdapterDesc = NULL;
const NiD3D10DeviceDesc* NiD3D10Select::ms_pkDeviceDesc = NULL;
const NiD3D10OutputDesc* NiD3D10Select::ms_pkOutputDesc = NULL;
const DXGI_MODE_DESC* NiD3D10Select::ms_pkDXGIModeDesc = NULL;
DXGI_FORMAT NiD3D10Select::ms_eDXGIFormat = DXGI_FORMAT_UNKNOWN;

//---------------------------------------------------------------------------
bool ConvertWCHARToCHAR(char* pcDestination, const WCHAR* pwSource,
    unsigned int uiDestChar)
{
    if (pcDestination == NULL || pwSource == NULL || uiDestChar == 0)
        return false;

    int iResult = WideCharToMultiByte(CP_ACP, 0, pwSource, -1, pcDestination,
        uiDestChar*sizeof(char), NULL, NULL);
    pcDestination[uiDestChar - 1] = 0;
    
    return (iResult != 0);
}
//---------------------------------------------------------------------------
void NiD3D10Select::EnumerateAdapters(NiWindowRef pkDialog)
{
    SendDlgItemMessage(pkDialog, IDC_ADAPTER_COMBO, CB_RESETCONTENT, 0, 0);

    NiD3D10SystemDescPtr spSysDesc;
    NiD3D10SystemDesc::GetSystemDesc(spSysDesc);
    assert(spSysDesc);

    unsigned int uiAdapterCount = spSysDesc->GetAdapterCount();

    bool bAdaptersExist = false;
    for (unsigned int i = 0; i < uiAdapterCount; i++)
    {
        const NiD3D10AdapterDesc* pkAdapterDesc
            = spSysDesc->GetAdapterDesc(i);
        assert(pkAdapterDesc);

        // Select rasterization:  hardware or reference.
        const NiD3D10DeviceDesc* pkDeviceDesc = ((ms_bRefRast) ?
            (pkAdapterDesc->GetRefDevice()) : (pkAdapterDesc->GetHWDevice()));
        if (!pkDeviceDesc)
            continue;

        const DXGI_ADAPTER_DESC* pkDXGIAdapterDesc = pkAdapterDesc->GetDesc();
        assert(pkDXGIAdapterDesc);

        // Add the adapter description to the combo box.

        char acDescr[128];
        ConvertWCHARToCHAR(acDescr, pkDXGIAdapterDesc->Description, 128);
        char acStr[140];
        NiSprintf(acStr, 140, "%s   [%1d]", acDescr, i);

        int iIndex = SendDlgItemMessage(pkDialog, IDC_ADAPTER_COMBO, 
            CB_ADDSTRING, 0, (LPARAM)acStr);
        SendDlgItemMessage(pkDialog, IDC_ADAPTER_COMBO, CB_SETITEMDATA,
            (WPARAM)iIndex, (LPARAM)pkAdapterDesc);

        bAdaptersExist = true;
    }

    SendDlgItemMessage(pkDialog, IDC_ADAPTER_COMBO, CB_SETCURSEL, (WPARAM)0,
        0L);

    if (bAdaptersExist)
    {
        ms_pkAdapterDesc = (const NiD3D10AdapterDesc*)SendDlgItemMessage(
            pkDialog, IDC_ADAPTER_COMBO, CB_GETITEMDATA, (WPARAM)0, 0L);

        // Select rasterization:  hardware or reference.
        ms_pkDeviceDesc = ((ms_bRefRast) ? (ms_pkAdapterDesc->GetRefDevice())
            : (ms_pkAdapterDesc->GetHWDevice()));
    }
    else
    {
        ms_pkAdapterDesc = NULL;
        ms_pkDeviceDesc = NULL;
    }
}
//---------------------------------------------------------------------------
void NiD3D10Select::EnumerateDisplays(NiWindowRef pkDialog)
{
    SendDlgItemMessage(pkDialog, IDC_DISPLAY_COMBO, CB_RESETCONTENT, 0, 0);

    assert(ms_pkAdapterDesc);

    unsigned int uiDisplayCount = ms_pkAdapterDesc->GetOutputCount();

    bool bDisplaysExist = false;

    for (unsigned int i = 0; i < uiDisplayCount; i++)
    {
        const NiD3D10OutputDesc* pkOutputDesc
            = ms_pkAdapterDesc->GetOutputDesc(i);
        assert(pkOutputDesc);

        const DXGI_OUTPUT_DESC* pkDXGIOutputDesc = pkOutputDesc->GetDesc();
        assert(pkDXGIOutputDesc);

        // Add the display device name to the combo box.

        char acStr[32];
        ConvertWCHARToCHAR(acStr, pkDXGIOutputDesc->DeviceName, 32);

        int iIndex = SendDlgItemMessage(pkDialog, IDC_DISPLAY_COMBO,
            CB_ADDSTRING, 0, (LPARAM)acStr);
        SendDlgItemMessage(pkDialog, IDC_DISPLAY_COMBO, CB_SETITEMDATA,
            (WPARAM)iIndex, (LPARAM)pkOutputDesc);

        bDisplaysExist = true;
    }

    SendDlgItemMessage(pkDialog, IDC_DISPLAY_COMBO, CB_SETCURSEL, (WPARAM)0, 
        0L);

    if (bDisplaysExist)
    {
        ms_pkOutputDesc = (const NiD3D10OutputDesc*)SendDlgItemMessage(
            pkDialog, IDC_DISPLAY_COMBO, CB_GETITEMDATA, (WPARAM)0, 0L);
    }
    else
    {
        ms_pkOutputDesc = NULL;
    }
}
//---------------------------------------------------------------------------
void NiD3D10Select::EnumerateResolutions(NiWindowRef pkDialog)
{
    SendDlgItemMessage(pkDialog, IDC_RESOLUTION_COMBO, CB_RESETCONTENT, 0, 0);

    if (!ms_bFullscreen)
        return;

    assert(ms_pkOutputDesc);

    unsigned int uiDisplayModeCount = ms_pkOutputDesc->GetDisplayModeCount();
    assert(uiDisplayModeCount);
    
    const DXGI_MODE_DESC* pkDXGIModeDesc
        = ms_pkOutputDesc->GetDisplayModeArray();
    assert(pkDXGIModeDesc);

    bool bResolutionsExist = false;
    unsigned int uiMaxUsedCount = 0;
    int iDefault = 0;

    NiTPrimitiveArray<const DXGI_MODE_DESC*> kModesToUse(24, 4);

    kModesToUse.SetAtGrow(uiMaxUsedCount, pkDXGIModeDesc);
    uiMaxUsedCount++;

    for (unsigned int i = 1; i < uiDisplayModeCount; i++)
    {
        unsigned int uiModeIndx = uiMaxUsedCount;
        for (unsigned int j=0; j < uiMaxUsedCount; j++)
        {
            if ((kModesToUse.GetAt(j)->Width == (pkDXGIModeDesc+i)->Width)
                && (kModesToUse.GetAt(j)->Height == 
                (pkDXGIModeDesc+i)->Height))
            {
                uiModeIndx = j;
                break;
            }
        }
        if (uiModeIndx == uiMaxUsedCount)
        {
            kModesToUse.SetAtGrow(uiModeIndx, pkDXGIModeDesc+i);
            uiMaxUsedCount++;
        }
        else
        {
            float fRational0 = (float)
                (kModesToUse.GetAt(uiModeIndx)->RefreshRate.Numerator)
                / (float)
                (kModesToUse.GetAt(uiModeIndx)->RefreshRate.Denominator);
            float fRational1 =
                (float)((pkDXGIModeDesc+i)->RefreshRate.Numerator)
                / (float)((pkDXGIModeDesc+i)->RefreshRate.Denominator);
            if (fRational1 < fRational0)
                kModesToUse.SetAtGrow(uiModeIndx, pkDXGIModeDesc+i);
        }
    }

    for (unsigned int i = 0; i < uiMaxUsedCount; i++)
    {
        unsigned int uiWidth = kModesToUse.GetAt(i)->Width;
        unsigned int uiHeight = kModesToUse.GetAt(i)->Height;

        if ((uiWidth < 640) || (uiHeight < 480))
            continue;

        char acStr[20];
        NiSprintf(acStr, 20, "%u x %u", uiWidth, uiHeight);

        // Add the resolution (width x height) to the combo box.

        int iIndex = SendDlgItemMessage(pkDialog, IDC_RESOLUTION_COMBO,
            CB_ADDSTRING, 0, (LPARAM)acStr);
        SendDlgItemMessage(pkDialog, IDC_RESOLUTION_COMBO, CB_SETITEMDATA,
            (WPARAM)iIndex, (LPARAM)(kModesToUse.GetAt(i)));

        if ((uiWidth == ms_uiDefaultWidth) &&
            (uiHeight == ms_uiDefaultHeight))
        {
            iDefault = iIndex;
        }

        bResolutionsExist = true;
    }

    SendDlgItemMessage(pkDialog, IDC_RESOLUTION_COMBO, CB_SETCURSEL,
        (WPARAM)iDefault, 0L);

    if (bResolutionsExist)
    {
        ms_pkDXGIModeDesc = (DXGI_MODE_DESC*)SendDlgItemMessage(pkDialog,
            IDC_RESOLUTION_COMBO, CB_GETITEMDATA, (WPARAM)iDefault, 0L);
    }
    else
    {
        ms_pkDXGIModeDesc = NULL;
    }
}
//---------------------------------------------------------------------------
void NiD3D10Select::EnumerateFormats(NiWindowRef pkDialog)
{
    SendDlgItemMessage(pkDialog, IDC_FORMAT_COMBO, CB_RESETCONTENT, 0, 0);

    if (!ms_bFullscreen)
        return;

    assert(ms_pkOutputDesc);

    unsigned int uiDisplayModeCount = ms_pkOutputDesc->GetDisplayModeCount();
    assert(uiDisplayModeCount);

    const DXGI_MODE_DESC* pkDXGIModeDesc
        = ms_pkOutputDesc->GetDisplayModeArray();
    assert(pkDXGIModeDesc);

    bool bFormatsExist = false;
    unsigned int uiMaxUsedCount = 0;
    int iDefault = 0;

    DXGI_FORMAT eDefaultDXGIFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    const DXGI_MODE_DESC* pkDefaultDXGIModeDesc = ms_pkDXGIModeDesc;
        
    NiTPrimitiveArray<const DXGI_MODE_DESC*> kModesToUse(4, 4);

    for (unsigned int i = 0; i < uiDisplayModeCount; i++)
    {
        if ((ms_pkDXGIModeDesc->Width == (pkDXGIModeDesc+i)->Width)
            && (ms_pkDXGIModeDesc->Height == (pkDXGIModeDesc+i)->Height))
        {
            unsigned int uiModeIndx = uiMaxUsedCount;
            for (unsigned int j=0; j < uiMaxUsedCount; j++)
            {
                if (kModesToUse.GetAt(j)->Format
                    == (pkDXGIModeDesc+i)->Format)
                {
                    uiModeIndx = j;
                    break;
                }
            }
            if (uiModeIndx == uiMaxUsedCount)
            {
                kModesToUse.SetAtGrow(uiModeIndx, pkDXGIModeDesc+i);
                uiMaxUsedCount++;
            }
        }
    }

    for (unsigned int i = 0; i < uiMaxUsedCount; i++)
    {
        DXGI_FORMAT eDXGIFormat = kModesToUse.GetAt(i)->Format;

        const char* pcFormatName
            = NiD3D10PixelFormat::GetFormatName(eDXGIFormat, false);
        char acStr[140];
        NiSprintf(acStr, 140, "%s", pcFormatName);

        // Add the resolution (width x height) to the combo box.

        int iIndex = SendDlgItemMessage(pkDialog, IDC_FORMAT_COMBO,
            CB_ADDSTRING, 0, (LPARAM)acStr);
        SendDlgItemMessage(pkDialog, IDC_FORMAT_COMBO, CB_SETITEMDATA,
            (WPARAM)iIndex, (LPARAM)eDXGIFormat);

        if (eDXGIFormat == eDefaultDXGIFormat)
        {
            iDefault = iIndex;
            pkDefaultDXGIModeDesc = kModesToUse.GetAt(i);
        }

        bFormatsExist = true;
    }

    SendDlgItemMessage(pkDialog, IDC_FORMAT_COMBO, CB_SETCURSEL,
        (WPARAM)iDefault, 0L);

    if (bFormatsExist)
    {
        ms_eDXGIFormat = (DXGI_FORMAT)SendDlgItemMessage(pkDialog,
            IDC_FORMAT_COMBO, CB_GETITEMDATA, (WPARAM)iDefault, 0L);

        // Replace the DXGI_MODE_DESC* stored with the currently selected
        // entry in the Resolutions dialog item with one that represents the
        // same resolution, but has the newly selected format.  This step is
        // done because hitting "OK" means reacquiring the data to be used
        // from the dialog items themselves.

        // Sanity check on preserving same width and height as before.
        assert(ms_pkDXGIModeDesc->Width == pkDefaultDXGIModeDesc->Width);
        assert(ms_pkDXGIModeDesc->Height == pkDefaultDXGIModeDesc->Height);

        ms_pkDXGIModeDesc = pkDefaultDXGIModeDesc;

        int iIndex = SendDlgItemMessage(pkDialog, IDC_RESOLUTION_COMBO,
            CB_GETCURSEL, 0, 0);
        if (iIndex >= 0)
        {
            SendDlgItemMessage(pkDialog, IDC_RESOLUTION_COMBO, CB_SETITEMDATA,
                (WPARAM)iIndex, (LPARAM)(pkDefaultDXGIModeDesc));
        }
    }
    else
    {
        ms_eDXGIFormat = DXGI_FORMAT_UNKNOWN;
    }
}
//---------------------------------------------------------------------------

void NiD3D10Select::EnumerateMultisampleCounts(NiWindowRef pkDialog)
{
    SendDlgItemMessage(pkDialog, IDC_MULTISAMPLECOUNT_COMBO, CB_RESETCONTENT,
        0, 0);

    if (!ms_pkDeviceDesc)
        return;

    unsigned int uiHighestMultisampleCount
        = ms_pkDeviceDesc->GetHighestMultisampleCount();

    bool bMultisampleCountsExist = false;

    char acStr[6];
    int iDefaultValue = 1;
    NiSprintf(acStr, 6, "%d", iDefaultValue);

    // Add the multisample count to the combo box.

    int iDefaultIndx = SendDlgItemMessage(pkDialog, IDC_MULTISAMPLECOUNT_COMBO,
        CB_ADDSTRING, 0, (LPARAM)acStr);
    SendDlgItemMessage(pkDialog, IDC_MULTISAMPLECOUNT_COMBO, CB_SETITEMDATA,
        (WPARAM)iDefaultIndx, (LPARAM)(iDefaultValue));

    for (unsigned int i = 2; i < uiHighestMultisampleCount; i++)
    {
        unsigned int uiQualityLevels = 0;

        if (ms_pkDeviceDesc->GetMultisampleSupport(i, uiQualityLevels))
        {
            NiSprintf(acStr, 6, "%d", i);

            // Add the multisample count to the combo box.

            int iIndex = SendDlgItemMessage(pkDialog,
                IDC_MULTISAMPLECOUNT_COMBO, CB_ADDSTRING, 0, (LPARAM)acStr);
            SendDlgItemMessage(pkDialog, IDC_MULTISAMPLECOUNT_COMBO,
                CB_SETITEMDATA, (WPARAM)iIndex, (LPARAM)(i));

            bMultisampleCountsExist = true;
        }
    }

    SendDlgItemMessage(pkDialog, IDC_MULTISAMPLECOUNT_COMBO, CB_SETCURSEL,
        (WPARAM)iDefaultIndx, 0L);

    if (bMultisampleCountsExist)
    {
        ms_uiMultisampleCount = (unsigned int)SendDlgItemMessage(pkDialog,
            IDC_MULTISAMPLECOUNT_COMBO, CB_GETITEMDATA, (WPARAM)iDefaultIndx,
            0L);
    }
    else
    {
        ms_uiMultisampleCount = iDefaultValue;
    }
}

//---------------------------------------------------------------------------
void NiD3D10Select::EnumerateMultisampleQuality(NiWindowRef pkDialog)
{
    SendDlgItemMessage(pkDialog, IDC_MULTISAMPLEQUALITY_COMBO,
        CB_RESETCONTENT, 0, 0);

    if (!ms_pkDeviceDesc)
        return;

    bool bMultisampleQualityExist = false;

    char acStr[6];
    int iDefault = 0;
    NiSprintf(acStr, 6, "%d", iDefault);

    // Add the multisample count to the combo box.

    int iIndex = SendDlgItemMessage(pkDialog, IDC_MULTISAMPLEQUALITY_COMBO,
        CB_ADDSTRING, 0, (LPARAM)acStr);
    SendDlgItemMessage(pkDialog, IDC_MULTISAMPLEQUALITY_COMBO, CB_SETITEMDATA,
        (WPARAM)iIndex, (LPARAM)(iDefault));

    unsigned int uiQualityLevels = 0;

    if (ms_pkDeviceDesc->GetMultisampleSupport(ms_uiMultisampleCount,
        uiQualityLevels))
    {
        for (unsigned int i=1; i < uiQualityLevels; i++)
        {
            NiSprintf(acStr, 6, "%d", i);

            // Add the multisample count to the combo box.

            int iIndex = SendDlgItemMessage(pkDialog,
                IDC_MULTISAMPLEQUALITY_COMBO, CB_ADDSTRING, 0, (LPARAM)acStr);
            SendDlgItemMessage(pkDialog, IDC_MULTISAMPLEQUALITY_COMBO,
                CB_SETITEMDATA, (WPARAM)iIndex, (LPARAM)(i));

            bMultisampleQualityExist = true;
        }
    }

    SendDlgItemMessage(pkDialog, IDC_MULTISAMPLEQUALITY_COMBO, CB_SETCURSEL,
        (WPARAM)iDefault, 0L);

    if (bMultisampleQualityExist)
    {
        ms_uiMultisampleQuality = (unsigned int)SendDlgItemMessage(pkDialog,
            IDC_MULTISAMPLEQUALITY_COMBO, CB_GETITEMDATA, (WPARAM)iDefault,
            0L);
    }
    else
    {
        ms_uiMultisampleQuality = iDefault;
    }
}
//---------------------------------------------------------------------------
bool NiD3D10Select::UpdateAdapter(NiWindowRef pkDialog)
{
    bool bReturn = true;

    int iIndex = SendDlgItemMessage(pkDialog, IDC_ADAPTER_COMBO, CB_GETCURSEL,
        0, 0);
    if (iIndex >= 0)
    {
        const NiD3D10AdapterDesc* pkAdapterDesc = 
            (const NiD3D10AdapterDesc*)SendDlgItemMessage(pkDialog,
            IDC_ADAPTER_COMBO, CB_GETITEMDATA, iIndex, 0);
        bReturn = (pkAdapterDesc != ms_pkAdapterDesc);

        ms_pkAdapterDesc = pkAdapterDesc;

        // Select rasterization:  hardware or reference.
        ms_pkDeviceDesc = ((ms_bRefRast) ? (ms_pkAdapterDesc->GetRefDevice())
            : (ms_pkAdapterDesc->GetHWDevice()));
    }
    else
    {
        ms_pkDeviceDesc = NULL;
    }

    return bReturn;
}
//---------------------------------------------------------------------------
bool NiD3D10Select::UpdateDisplay(NiWindowRef pkDialog)
{
    bool bReturn = true;

    int iIndex = SendDlgItemMessage(pkDialog, IDC_DISPLAY_COMBO, CB_GETCURSEL,
        0, 0);
    if (iIndex >= 0)
    {
        NiD3D10OutputDesc* pkOutputDesc = (NiD3D10OutputDesc*)
            SendDlgItemMessage(pkDialog, IDC_DISPLAY_COMBO, CB_GETITEMDATA,
            iIndex, 0);

        bReturn = (pkOutputDesc != ms_pkOutputDesc);
        ms_pkOutputDesc = pkOutputDesc;
    }
    else
    {
        ms_pkOutputDesc = NULL;
    }

    return bReturn;
}
//---------------------------------------------------------------------------
bool NiD3D10Select::UpdateResolution(NiWindowRef pkDialog)
{
    bool bReturn = true;

    int iIndex = SendDlgItemMessage(pkDialog, IDC_RESOLUTION_COMBO,
        CB_GETCURSEL, 0, 0);
    if (iIndex >= 0)
    {
        DXGI_MODE_DESC* pkDXGIModeDesc = (DXGI_MODE_DESC*)SendDlgItemMessage(
            pkDialog, IDC_RESOLUTION_COMBO, CB_GETITEMDATA, iIndex, 0);
        bReturn = (pkDXGIModeDesc != ms_pkDXGIModeDesc);

        ms_pkDXGIModeDesc = pkDXGIModeDesc;
    }
    else
    {
        ms_pkDXGIModeDesc = NULL;
    }

    return bReturn;
}
//---------------------------------------------------------------------------
bool NiD3D10Select::UpdateFormat(NiWindowRef pkDialog)
{
    bool bReturn = true;

    int iIndex = SendDlgItemMessage(pkDialog, IDC_FORMAT_COMBO, CB_GETCURSEL,
        0, 0);
    if (iIndex >= 0)
    {
        DXGI_FORMAT eDXGIFormat = (DXGI_FORMAT)SendDlgItemMessage(pkDialog,
            IDC_FORMAT_COMBO, CB_GETITEMDATA, iIndex, 0);

        bReturn = (eDXGIFormat != ms_eDXGIFormat);
        ms_eDXGIFormat = eDXGIFormat;
    }
    else
    {
        ms_eDXGIFormat = DXGI_FORMAT_UNKNOWN;
    }

    return bReturn;
}
//---------------------------------------------------------------------------
bool NiD3D10Select::UpdateMultisampleCount(NiWindowRef pkDialog)
{
    bool bReturn = true;

    int iIndex = SendDlgItemMessage(pkDialog, IDC_MULTISAMPLECOUNT_COMBO,
        CB_GETCURSEL, 0, 0);
    if (iIndex >= 0)
    {
        unsigned int uiMultisampleCount = (unsigned int)SendDlgItemMessage(
            pkDialog, IDC_MULTISAMPLECOUNT_COMBO, CB_GETITEMDATA, iIndex, 0);

        bReturn = (uiMultisampleCount != ms_uiMultisampleCount);
        ms_uiMultisampleCount = uiMultisampleCount;
    }
    else
    {
        ms_uiMultisampleCount = 1;
    }

    return bReturn;
}
//---------------------------------------------------------------------------
bool NiD3D10Select::UpdateMultisampleQuality(NiWindowRef pkDialog)
{
    bool bReturn = true;

    int iIndex = SendDlgItemMessage(pkDialog, IDC_MULTISAMPLEQUALITY_COMBO, 
        CB_GETCURSEL, 0, 0);
    if (iIndex >= 0)
    {
        unsigned int uiMultisampleQuality = (unsigned int)SendDlgItemMessage(
            pkDialog, IDC_MULTISAMPLEQUALITY_COMBO, CB_GETITEMDATA, iIndex,
            0);

        bReturn = (uiMultisampleQuality != ms_uiMultisampleQuality);
        ms_uiMultisampleQuality = uiMultisampleQuality;
    }
    else
    {
        ms_uiMultisampleQuality = 0;
    }

    return bReturn;
}
//---------------------------------------------------------------------------
BOOL CALLBACK NiD3D10Select::Callback(NiWindowRef pkDialog,
    unsigned int uiMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uiMsg)
    {
        case WM_INITDIALOG:
        {
            SetWindowText(pkDialog, "Select Direct3D 10 Display Device");

            SendDlgItemMessage(pkDialog, IDC_MODE_COMBO, CB_RESETCONTENT, 0,
                0);

            // Add "windowed" and "fullscreen" to the mode combo box
            // and set the item data to be true for fullscreen and
            // false for windowed mode
            int iIndex = SendDlgItemMessage(pkDialog, IDC_MODE_COMBO, 
                CB_ADDSTRING, 0, (LPARAM)"Windowed");
            SendDlgItemMessage(pkDialog, IDC_MODE_COMBO, CB_SETITEMDATA,
                (WPARAM)iIndex, (LPARAM)false);

            SendDlgItemMessage(pkDialog, IDC_MODE_COMBO, CB_SETCURSEL, 
                (WPARAM)0, 0L);
            ms_bFullscreen = false;

            iIndex = SendDlgItemMessage(pkDialog, IDC_MODE_COMBO,
                CB_ADDSTRING, 0, (LPARAM)"Fullscreen");
            SendDlgItemMessage(pkDialog, IDC_MODE_COMBO, CB_SETITEMDATA,
                (WPARAM)iIndex, (LPARAM)true);

            ms_bDialogOpen = true;

            // Find all of the Adapters and add them to the combo box
            // interface.
            EnumerateAdapters(pkDialog);
            EnumerateDisplays(pkDialog);
            EnumerateResolutions(pkDialog);
            EnumerateFormats(pkDialog);
            EnumerateMultisampleCounts(pkDialog);
            EnumerateMultisampleQuality(pkDialog);

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

                    // If the user has changed the screen mode, then the
                    // Adapter list box must be regenerated, and different
                    // sets of devices must be enumerated based on the
                    // settings.
                    int iIndex = SendDlgItemMessage(pkDialog, IDC_MODE_COMBO,
                        CB_GETCURSEL, 0, 0);

                    bool bOldMode = ms_bFullscreen;
                    if (iIndex >= 0)
                    {
                        ms_bFullscreen = SendDlgItemMessage(pkDialog, 
                            IDC_MODE_COMBO, CB_GETITEMDATA, iIndex, 0) ? 
                            true : false;
                    }

                    if (bOldMode != ms_bFullscreen)
                    {
                        EnumerateAdapters(pkDialog);
                        EnumerateDisplays(pkDialog);
                        EnumerateResolutions(pkDialog);
                        EnumerateFormats(pkDialog);
                        EnumerateMultisampleCounts(pkDialog);
                        EnumerateMultisampleQuality(pkDialog);
                    }

                    return TRUE;
                }
                case IDC_ADAPTER_COMBO:
                {
                    if (!ms_bDialogOpen)
                        return TRUE;

                    // Only regenerate displays, resolutions, formats,
                    // multisample count, and multisample quality if the
                    // Adapter has changed.
                    bool bNewAdapter = UpdateAdapter(pkDialog);

                    if (bNewAdapter)
                    {
                        EnumerateDisplays(pkDialog);
                        EnumerateResolutions(pkDialog);
                        EnumerateFormats(pkDialog);
                        EnumerateMultisampleCounts(pkDialog);
                        EnumerateMultisampleQuality(pkDialog);
                    }

                    return TRUE;
                }
                case IDC_DISPLAY_COMBO:
                {
                    if (!ms_bDialogOpen)
                        return TRUE;

                    // Only regenerate resolutions, formats, multisample
                    // count, and multisample quality if the Display has
                    // changed.
                    bool bNewDisplay = UpdateDisplay(pkDialog);

                    if (bNewDisplay)
                    {
                        EnumerateResolutions(pkDialog);
                        EnumerateFormats(pkDialog);
                        EnumerateMultisampleCounts(pkDialog);
                        EnumerateMultisampleQuality(pkDialog);
                    }

                    return TRUE;
                }
                case IDC_RESOLUTION_COMBO:
                {
                    if (!ms_bDialogOpen)
                        return TRUE;

                    // Only regenerate formats, multisample count, and
                    // multisample quality if the Resolution has changed.
                    bool bNewResolution = UpdateResolution(pkDialog);

                    if (bNewResolution)
                    {
                        EnumerateFormats(pkDialog);
                        EnumerateMultisampleCounts(pkDialog);
                        EnumerateMultisampleQuality(pkDialog);
                    }

                    return TRUE;
                }
                case IDC_FORMAT_COMBO:
                {
                    if (!ms_bDialogOpen)
                        return TRUE;

                    // Only regenerate multisample count and multisample
                    // quality if the Format has changed.
                    bool bNewFormat = UpdateFormat(pkDialog);

                    if (bNewFormat)
                    {
                        EnumerateMultisampleCounts(pkDialog);
                        EnumerateMultisampleQuality(pkDialog);
                    }

                    return TRUE;
                }
                case IDC_MULTISAMPLECOUNT_COMBO:
                {
                    if (!ms_bDialogOpen)
                        return TRUE;

                    // Only regenerate multisample quality if the multisample
                    // count has changed.
                    bool bNewMultisampleCount = 
                        UpdateMultisampleCount(pkDialog);

                    if (bNewMultisampleCount)
                    {
                        EnumerateMultisampleQuality(pkDialog);
                    }

                    return TRUE;
                }
                case IDOK:
                {
                    // On acceptance, find the selected adapter.
                    UpdateAdapter(pkDialog);
                    UpdateDisplay(pkDialog);
                    UpdateResolution(pkDialog);
                    UpdateFormat(pkDialog);
                    UpdateMultisampleCount(pkDialog);
                    UpdateMultisampleQuality(pkDialog);

                    ms_bDialogOpen = false;

                    SendDlgItemMessage(pkDialog, IDC_MODE_COMBO,
                        CB_RESETCONTENT, 0, 0);
                    SendDlgItemMessage(pkDialog, IDC_ADAPTER_COMBO, 
                        CB_RESETCONTENT, 0, 0);
                    SendDlgItemMessage(pkDialog, IDC_DISPLAY_COMBO, 
                        CB_RESETCONTENT, 0, 0);
                    SendDlgItemMessage(pkDialog, IDC_RESOLUTION_COMBO, 
                        CB_RESETCONTENT, 0, 0);
                    SendDlgItemMessage(pkDialog, IDC_FORMAT_COMBO, 
                        CB_RESETCONTENT, 0, 0);
                    SendDlgItemMessage(pkDialog, IDC_MULTISAMPLECOUNT_COMBO, 
                        CB_RESETCONTENT, 0, 0);
                    SendDlgItemMessage(pkDialog, IDC_MULTISAMPLEQUALITY_COMBO,
                        CB_RESETCONTENT, 0, 0);

                    EndDialog(pkDialog, 1);
                    return TRUE;
                }
                case IDCANCEL:
                {
                    // On cancel, mark the box as cancelled and return.
                    ms_bDialogOpen = false;

                    SendDlgItemMessage(pkDialog, IDC_MODE_COMBO, 
                        CB_RESETCONTENT, 0, 0);
                    SendDlgItemMessage(pkDialog, IDC_ADAPTER_COMBO, 
                        CB_RESETCONTENT, 0, 0);
                    SendDlgItemMessage(pkDialog, IDC_DISPLAY_COMBO, 
                        CB_RESETCONTENT, 0, 0);
                    SendDlgItemMessage(pkDialog, IDC_RESOLUTION_COMBO, 
                        CB_RESETCONTENT, 0, 0);
                    SendDlgItemMessage(pkDialog, IDC_FORMAT_COMBO, 
                        CB_RESETCONTENT, 0, 0);
                    SendDlgItemMessage(pkDialog, IDC_MULTISAMPLECOUNT_COMBO, 
                        CB_RESETCONTENT, 0, 0);
                    SendDlgItemMessage(pkDialog, IDC_MULTISAMPLEQUALITY_COMBO,
                        CB_RESETCONTENT, 0, 0);

                    EndDialog(pkDialog, 0);
                    return TRUE;
                }
            }
            break;
        }
    }

    return FALSE;
}
//---------------------------------------------------------------------------
void NiD3D10Select::CopyToWideChar(unsigned short** ppusStrOut, 
    const char* pcStrIn)
{
    unsigned int uiLen  = lstrlen(pcStrIn);
    unsigned short* pStrOut = *ppusStrOut;

    if (uiLen)
    {
        uiLen = MultiByteToWideChar(CP_ACP, 0, pcStrIn, uiLen,
            (LPWSTR)pStrOut, uiLen);
    }
    pStrOut[uiLen++] = L'\0';   // Add the null terminator.
    *ppusStrOut += uiLen;
}
//---------------------------------------------------------------------------
void NiD3D10Select::AddDialogControl(unsigned short** ppusTemplate, 
    unsigned int uiStyle, short sSX, short sSY, short sCX, short sCY, 
    unsigned short usID, const char* pcStrClassName, const char* pcStrTitle)
{
    // DWORD align the current pointer.
    DLGITEMTEMPLATE* pkTemplate
        = (DLGITEMTEMPLATE*)(((((ULONG)(*ppusTemplate))+3)>>2)<<2);

    pkTemplate->style = uiStyle | WS_CHILD | WS_VISIBLE;
    pkTemplate->dwExtendedStyle = 0L;
    pkTemplate->x = sSX;
    pkTemplate->y = sSY;
    pkTemplate->cx = sCX;
    pkTemplate->cy = sCY;
    pkTemplate->id = usID;

    *ppusTemplate = (unsigned short*)(++pkTemplate);    // Advance pointer.

    CopyToWideChar((unsigned short**)ppusTemplate, pcStrClassName); //Set name
    CopyToWideChar((unsigned short**)ppusTemplate, pcStrTitle); //Set title

    (*ppusTemplate)++;  // Skip extra stuff.
}
//---------------------------------------------------------------------------
DLGTEMPLATE* NiD3D10Select::BuildAdapterSelectTemplate()
{
    // Allocate ample memory for building the template.
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

    pkDT->cdit = 16;    // Set the the total number of controls on the dialog.

    pkDT->x = 0;
    pkDT->y = 0;
    pkDT->cx = 224;
    pkDT->cy = 160;

    // Add menu array, class array, dialog title, font size, and font name.
    unsigned short* pusW = (unsigned short*)(++pkDT);
    *pusW++ = L'\0';    // Set Menu array to nothing.
    *pusW++ = L'\0';    // Set Class array to nothing.

    CopyToWideChar((unsigned short**)&pusW, 
        TEXT("Select Desired Direct3D 10 Device"));

    *pusW++ = 8;    // Font size.
    CopyToWideChar(&pusW, TEXT("Arial"));   // Font name.
 
    // Add the mode select combobox.
    AddDialogControl(&pusW, CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 
        54, 9, 160, 45, IDC_MODE_COMBO, TEXT("COMBOBOX"), TEXT(""));

    // Add the "Mode:" text.
    AddDialogControl(&pusW, SS_LEFT, 11, 11, 38, 13, IDC_STATIC,
        TEXT("STATIC"), TEXT("Mode:"));

    // Add the Adapter select combobox.
    AddDialogControl(&pusW, CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 54,
        31, 160, 45, IDC_ADAPTER_COMBO, TEXT("COMBOBOX"), TEXT(""));

    // Add the "Adapter:" text.
    AddDialogControl(&pusW, SS_LEFT, 11, 33, 38, 13, IDC_STATIC,
        TEXT("STATIC"), TEXT("Adapter:"));

    // Add the Display select combobox.
    AddDialogControl(&pusW, CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 54,
        52, 160, 45, IDC_DISPLAY_COMBO, TEXT("COMBOBOX"), TEXT(""));

    // Add the "Display:" text.
    AddDialogControl(&pusW, SS_LEFT, 11, 54, 38, 13, IDC_STATIC,
        TEXT("STATIC"), TEXT("Display:"));

    // Add the Resolution select combobox.
    AddDialogControl(&pusW, CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 54,
        73, 160, 45, IDC_RESOLUTION_COMBO, TEXT("COMBOBOX"), TEXT(""));

    // Add the "Resolution:" text.
    AddDialogControl(&pusW, SS_LEFT, 11, 75, 38, 13, IDC_STATIC,
        TEXT("STATIC"), TEXT("Resolution:"));

    // Add the Resolution select combobox.
    AddDialogControl(&pusW, CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 54,
        95, 160, 45, IDC_FORMAT_COMBO, TEXT("COMBOBOX"), TEXT(""));

    // Add the "Resolution:" text.
    AddDialogControl(&pusW, SS_LEFT, 11, 97, 38, 13, IDC_STATIC,
        TEXT("STATIC"), TEXT("Format:"));

    // Add the multisample select combobox.
    AddDialogControl(&pusW, CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 85,
        116, 37, 45, IDC_MULTISAMPLECOUNT_COMBO, TEXT("COMBOBOX"), TEXT(""));

    // Add the "MultiSample:" text.
    AddDialogControl(&pusW, SS_LEFT, 11, 118, 74, 13, IDC_STATIC,
        TEXT("STATIC"), TEXT("MultiSample    Count:"));

    // Add the multisample select combobox.
    AddDialogControl(&pusW, CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, 165,
        116, 37, 45, IDC_MULTISAMPLEQUALITY_COMBO, TEXT("COMBOBOX"),
        TEXT(""));

    // Add the "MultiSample:" text.
    AddDialogControl(&pusW, SS_LEFT, 134, 118, 30, 13, IDC_STATIC,
        TEXT("STATIC"), TEXT("Quality:"));

    // Add the cancel button.
    AddDialogControl(&pusW, BS_PUSHBUTTON | WS_TABSTOP, 130, 137, 51, 14,
        IDCANCEL, TEXT("BUTTON"), TEXT("CANCEL"));

    // Add the okay button.
    AddDialogControl(&pusW, BS_PUSHBUTTON | WS_TABSTOP, 42, 137, 51, 14, IDOK,
        TEXT("BUTTON"), TEXT("OK"));

    return pkDlgTemplate;
}
//---------------------------------------------------------------------------
NiD3D10Select::ReturnVal NiD3D10Select::Select(NiWindowRef pkWnd,
    unsigned int uiDefaultWidth, unsigned int uiDefaultHeight, bool bRefRast,
    bool& bFullscreen, const void*& pvAdapter, const void*& pvDevice,
    const void*& pvOutputDesc, const void*& pvDXGIModeDesc,
    DXGI_FORMAT& eDXGIFormat, unsigned int& uiMultisampleCount,
    unsigned int& uiMultisampleQuality)
{
    bFullscreen = false;

    ms_uiDefaultWidth = uiDefaultWidth;
    ms_uiDefaultHeight = uiDefaultHeight;
    ms_bRefRast = bRefRast;

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
        bFullscreen = ms_bFullscreen;

        pvAdapter = ms_pkAdapterDesc;
        pvDevice = ms_pkDeviceDesc;

        pvOutputDesc = ms_pkOutputDesc;

        if (ms_bFullscreen)
        {
            pvDXGIModeDesc = ms_pkDXGIModeDesc;
            eDXGIFormat = ms_eDXGIFormat;
        }
        else
        {
            pvDXGIModeDesc = NULL;
            eDXGIFormat = DXGI_FORMAT_UNKNOWN;
        }

        uiMultisampleCount = ms_uiMultisampleCount;
        uiMultisampleQuality = ms_uiMultisampleQuality;

        return RETURN_OK;
    }
    else
    {
        return RETURN_CANCEL;
    }
}
//---------------------------------------------------------------------------
NiRendererPtr NiD3D10Select::DialogCreateRenderer(NiWindowRef pkTopLevelWnd,
    unsigned int uiDefaultWidth, unsigned int uiDefaultHeight, bool bRefRast,
    unsigned int uiCreateFlags, bool& bFullscreen)
{
    NiD3D10RendererPtr spRenderer;

    // Save the position and size of the window in case we have to put the
    // window back there later.
    RECT kRect;
    GetWindowRect(pkTopLevelWnd, &kRect);

    while (!spRenderer)
    {
        const NiD3D10AdapterDesc* pkAdapter = NULL;
        const NiD3D10DeviceDesc* pkDevice = NULL;
        const NiD3D10OutputDesc* pkOutputDesc = NULL;
        const DXGI_MODE_DESC* pkDXGIModeDesc = NULL;
        DXGI_FORMAT eDXGIFormat = DXGI_FORMAT_UNKNOWN;
        unsigned int uiMultisampleCount = 1;
        unsigned int uiMultisampleQuality = 0;

        // Select and set up the user's desired DirectDraw object and return
        // the desired renderer mode and display mode.
        NiD3D10Select::ReturnVal eRet = NiD3D10Select::Select(pkTopLevelWnd,
            uiDefaultWidth, uiDefaultHeight, bRefRast, bFullscreen,
            (const void *&)pkAdapter, (const void *&)pkDevice,
            (const void *&)pkOutputDesc, (const void *&)pkDXGIModeDesc,
            eDXGIFormat, uiMultisampleCount, uiMultisampleQuality);

        if (eRet == NiD3D10Select::RETURN_CANCEL)
            return NULL;

        assert(pkAdapter && pkOutputDesc);
        if (!(pkAdapter && pkOutputDesc))
            return NULL;

        // Select fullscreen or windowed, based on user selection.
        if (bFullscreen)
        {
            NiD3D10Renderer::CreationParameters kCreationParameters(
                pkTopLevelWnd);
            kCreationParameters.m_uiCreateFlags = uiCreateFlags;

            kCreationParameters.m_eDriverType = ((bRefRast) ?
                (NiD3D10Renderer::DRIVER_REFERENCE) :
                (NiD3D10Renderer::DRIVER_HARDWARE));

            kCreationParameters.m_uiAdapterIndex = pkAdapter->GetIndex();
            kCreationParameters.m_uiOutputIndex
                = pkOutputDesc->GetIndex();


            assert(pkDXGIModeDesc);
            if (!pkDXGIModeDesc)
                return NULL;

            kCreationParameters.m_kSwapChain.BufferDesc = *pkDXGIModeDesc;

            kCreationParameters.m_kSwapChain.SampleDesc.Count
                = uiMultisampleCount;
            kCreationParameters.m_kSwapChain.SampleDesc.Quality
                = uiMultisampleQuality;
            kCreationParameters.m_kSwapChain.Windowed = false;

            // Create and return the renderer.
            NiD3D10Renderer::Create(kCreationParameters, spRenderer);
        }
        else
        {
            NiD3D10Renderer::CreationParameters kCreationParameters(
                pkTopLevelWnd);
            kCreationParameters.m_uiCreateFlags = uiCreateFlags;

            kCreationParameters.m_eDriverType = ((bRefRast) ?
                (NiD3D10Renderer::DRIVER_REFERENCE) :
                (NiD3D10Renderer::DRIVER_HARDWARE));

            kCreationParameters.m_uiAdapterIndex = pkAdapter->GetIndex();
            kCreationParameters.m_uiOutputIndex
                = pkOutputDesc->GetIndex();

            kCreationParameters.m_kSwapChain.SampleDesc.Count
                = uiMultisampleCount;
            kCreationParameters.m_kSwapChain.SampleDesc.Quality
                = uiMultisampleQuality;

            // Create and return the renderer.
            NiD3D10Renderer::Create(kCreationParameters, spRenderer);
        }

        if (!spRenderer)
        {
            // Put the window back where it was if the renderer creation
            // failed.  If this step is not done, then the window could be
            // left at the size of the last attempted fullscreen renderer.
            MoveWindow(pkTopLevelWnd, kRect.left, kRect.top, 
                kRect.right - kRect.left, kRect.bottom-kRect.top, TRUE);
        }
    }

    return NiSmartPointerCast(NiRenderer, spRenderer);
}
//---------------------------------------------------------------------------
NiRendererPtr NiD3D10Select::CreateRenderer(NiWindowRef pkTopLevelWnd, 
    bool bDialog, unsigned int uiWidth, unsigned int uiHeight,
    bool bMultiThread, bool bRefRast, bool& bFullscreen)
{
    NiD3D10SystemDescPtr spSysDesc;
    NiD3D10SystemDesc::GetSystemDesc(spSysDesc);
    if (spSysDesc == NULL)
        return NULL;
    if (spSysDesc->IsEnumerationValid() == false)
    {
        // Give system another chance to enumerate
        spSysDesc->Enumerate();
        if (spSysDesc->IsEnumerationValid() == false)
        {
            // Enumeration still not valid; can't create renderer.
            return NULL;
        }
    }

    // Verify that adapters exist or early out gracefully otherwise.  Such an
    // early out would most likely occur when running with the "-d3d10" switch
    // on a Vista PC with a non-D3D10 graphics card.  (Note that use of a
    // reference rasterizer in such a circumstance should still be possible,
    // however.)
    unsigned int uiAdapterCount = spSysDesc->GetAdapterCount();
    bool bAdaptersExist = false;
    for (unsigned int i = 0; i < uiAdapterCount; i++)
    {
        const NiD3D10AdapterDesc* pkAdapterDesc = spSysDesc->GetAdapterDesc(i);
        if(!pkAdapterDesc)
            return NULL;

        // Select rasterization:  hardware or reference.
        const NiD3D10DeviceDesc* pkDeviceDesc = ((ms_bRefRast) ?
            (pkAdapterDesc->GetRefDevice()) : (pkAdapterDesc->GetHWDevice()));
        if (!pkDeviceDesc)
            continue;

        bAdaptersExist = true;
    }
    if (!bAdaptersExist)    // If adapters don't exist, can't create renderer.
        return NULL;


    NiD3D10RendererPtr spRenderer;

    unsigned int uiCreateFlags = 0;
    if (!bMultiThread)
        uiCreateFlags |= NiD3D10Renderer::CREATE_DEVICE_SINGLETHREADED;

    if (bDialog)
    {
        return DialogCreateRenderer(pkTopLevelWnd, uiWidth, uiHeight,
            bRefRast, uiCreateFlags, bFullscreen);
    }
    else
    {
        if (bFullscreen)
        {
            NiD3D10Renderer::CreationParameters kCreationParameters;
            kCreationParameters.m_uiCreateFlags = uiCreateFlags;

            kCreationParameters.m_eDriverType = ((bRefRast) ?
                (NiD3D10Renderer::DRIVER_REFERENCE) :
                (NiD3D10Renderer::DRIVER_HARDWARE));

            // Create and return the renderer.
            NiD3D10Renderer::Create(kCreationParameters, spRenderer);
        }
        else
        {
            NiD3D10Renderer::CreationParameters kCreationParameters(
                pkTopLevelWnd);
            kCreationParameters.m_uiCreateFlags = uiCreateFlags;

            kCreationParameters.m_eDriverType = ((bRefRast) ?
                (NiD3D10Renderer::DRIVER_REFERENCE) :
                (NiD3D10Renderer::DRIVER_HARDWARE));

            // Create and return the renderer.
            NiD3D10Renderer::Create(kCreationParameters, spRenderer);
        }
    }    

    return NiSmartPointerCast(NiRenderer, spRenderer);
}
//---------------------------------------------------------------------------
