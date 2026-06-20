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

#include "MaxImmerse.h"
#include "NiMAXExportSettingsDialog.h"
#include "MiResource.h"
#include <NiScriptInfoDialogs.h>

// these correspond directly to the dropdown box options
#define GENERIC 0   // "Default value", preserved for legacy interpretation
                    // (was PC in Gb 2.1).
#define XENON 1 // XBox 360 value, preserved for legacy interpretation (was
                // XENON in Gb 2.1).
#define PS3 2
#define DX9 3

//---------------------------------------------------------------------------
const char * 
NiMAXExportSettingsDialog::ms_aPlatforms[NUM_PLATFORMS_SUPPORTED] =
{
    "Generic",
    "Xbox 360",
    "Playstation 3",
    "DX 9"
};

//---------------------------------------------------------------------------
NiMAXExportSettingsDialog::NiMAXExportSettingsDialog(NiModuleRef hInstance, 
    NiWindowRef hWndParent):
    NiDialog(IDD_NIF_EXPORT, hInstance, hWndParent)
{
}
//---------------------------------------------------------------------------
NiMAXExportSettingsDialog::~NiMAXExportSettingsDialog()
{
}
//---------------------------------------------------------------------------
int NiMAXExportSettingsDialog::DoModal()
{
    m_spNewProcessScript = NiMAXOptions::GetProcessScript();
    m_spNewExportScript = NiMAXOptions::GetExportScript();

    m_spAvailableExportScripts = 
        NiMAXOptions::GenerateAvailableExportScripts();
    m_spAvailableProcessScripts = 
        NiMAXOptions::GenerateAvailableProcessScripts();
    m_bOptionsChanged = false;
    int iResult = NiDialog::DoModal();
    return iResult;
}
//---------------------------------------------------------------------------
void FillDropDownList(HWND hList, NiScriptInfoSet* pkSet, 
    NiScriptInfo* pkCurrentScript)
{
    NIASSERT(pkSet);
    NIASSERT(pkCurrentScript);

    SendMessage(hList, CB_RESETCONTENT , 0, 0);

    for (unsigned int ui = 0; ui < pkSet->GetScriptCount(); ui++)
    {
        NiScriptInfo* pkInfo = pkSet->GetScriptAt(ui);
        if (pkInfo)
        {
            SendMessage(hList, CB_ADDSTRING, 0, (LPARAM) (const char*)
                pkInfo->GetName());
        }
    }

    SendMessage(hList, CB_SELECTSTRING, -1, (LPARAM) (const char*) 
        pkCurrentScript->GetName());
}
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::InitDialog()
{    
    CHECK_MEMORY();
    // Script options
    FillDropDownList(GetDlgItem(m_hWnd, IDC_SELECTED_EXPORT_SCRIPT), 
        m_spAvailableExportScripts, m_spNewExportScript);
    FillDropDownList(GetDlgItem(m_hWnd, IDC_SELECTED_PROCESS_SCRIPT), 
        m_spAvailableProcessScripts, m_spNewProcessScript);
    CheckDlgButton(m_hWnd, IDC_USE_TEMPLATE_SCRIPTS_CHECK, 
        NiMAXOptions::GetBool(NI_USE_TEMPLATE_FOR_SCRIPT));

    // export options
    CheckDlgButton(m_hWnd, IDC_CAM, 
        NiMAXOptions::GetBool(NI_CONVERT_CAMERAS));
    CheckDlgButton(m_hWnd, IDC_LIGHTS, 
        NiMAXOptions::GetBool(NI_CONVERT_LIGHTS));

    // animation options
    CheckDlgButton(m_hWnd, IDC_APP_TIME,  
        NiMAXOptions::GetBool(NI_USE_APPTIME));
    CheckDlgButton(m_hWnd, IDC_EXPORT_CSASKEYFRAME, 
        NiMAXOptions::GetBool(NI_CONVERT_CSASKEYFRAMES));
    SetDlgItemText(m_hWnd, IDC_SKIN_PERCENTAGE_EDIT, 
        NiMAXOptions::GetValue(NI_SKINNING_THRESHOLD));

    // texture options
    NiString strTextureExport = NiMAXOptions::GetValue(NI_TEXTURES);
    int iID = IDC_TEX_MAINNIF;
    if (strTextureExport == NI_SEPERATE_NIFS)
        iID = IDC_TEX_SEPARATE;
    else if(strTextureExport == NI_ORIGINAL_IMAGES)
        iID = IDC_TEX_ORIG;
    else if (strTextureExport == NI_ORIGINAL_IMAGES_VRAM)
        iID = IDC_TEX_ORIG_VRAM;
    else
        iID = IDC_TEX_MAINNIF;
    CheckTextureExportRadio(iID);
    
    // Platform specific settings
    NiPluginToolkitRenderer ePlatform = 
        (NiPluginToolkitRenderer) NiMAXOptions::GetInt(NI_EXPORT_PLATFORM);
    HWND hPlatform = GetDlgItem(m_hWnd, IDC_PLATFORM);
    SendMessage(hPlatform, CB_RESETCONTENT, 0L, 0L);

    SendMessage(hPlatform, CB_ADDSTRING, 0L, (LPARAM)ms_aPlatforms[GENERIC] );
    SendMessage(hPlatform, CB_ADDSTRING, 0L, (LPARAM)ms_aPlatforms[XENON]);
    SendMessage(hPlatform, CB_ADDSTRING, 0L, (LPARAM)ms_aPlatforms[PS3]);
    SendMessage(hPlatform, CB_ADDSTRING, 0L, (LPARAM)ms_aPlatforms[DX9]);

    int iCurSel = 0;
    switch (ePlatform)
    {
        case RENDER_XENON:
            iCurSel = XENON;
            break;
        case RENDER_PS3:
            iCurSel = PS3;
            break;
        case RENDER_DX9:
            iCurSel = DX9;
            break;
        default:    // RENDER_GENERIC or other value.
            iCurSel = GENERIC;
            break;
    }

    SendMessage(hPlatform, CB_SETCURSEL, (WPARAM) iCurSel, 0);
    OnPlatform();
    OnTextureExport();

    CheckDlgButton(m_hWnd, IDC_PER_TEXTURE_OVERRIDE, 
        NiMAXOptions::GetBool(NI_PER_TEXTURE_OVERRIDE));
    SetDlgItemText(m_hWnd, IDC_IMAGE_FILE_DIR, 
        NiMAXOptions::GetValue(NI_IMAGE_FILE_DIR));
    CheckDlgButton(m_hWnd, IDC_ALLOW_NPOT_TEXTURES,
        NiMAXOptions::GetBool(NI_ALLOW_NPOT_TEXTURES));
    
    NiTexture::FormatPrefs::PixelLayout eLayout = 
        (NiTexture::FormatPrefs::PixelLayout) 
        (NiMAXOptions::GetInt(NI_PIXEL_LAYOUT));
      
    int iUISelection = IDC_TEX_QUAL_32;
    switch(eLayout)
    {
        case NiTexture::FormatPrefs::PALETTIZED_8 :
            iUISelection = IDC_TEX_QUAL_PAL;
            break;
        case NiTexture::FormatPrefs::HIGH_COLOR_16 :
            iUISelection = IDC_TEX_QUAL_16;
            break;
        case NiTexture::FormatPrefs::TRUE_COLOR_32 :
            iUISelection = IDC_TEX_QUAL_32;
            break;
        case NiTexture::FormatPrefs::COMPRESSED :
            iUISelection = IDC_TEX_QUAL_COMP;
            break;
        case NiTexture::FormatPrefs::BUMPMAP :
            iUISelection = IDC_TEX_QUAL_32;
            break;
        case NiTexture::FormatPrefs::PALETTIZED_4 :
            iUISelection = IDC_TEX_QUAL_PAL;
            break;
        case NiTexture::FormatPrefs::PIX_DEFAULT :
            iUISelection = IDC_TEX_QUAL_32;
            break;
    }

    CheckPixelLayoutRadio(iUISelection);
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
NiPluginToolkitRenderer NiMAXExportSettingsDialog::GetPlatform()
{
    CHECK_MEMORY();
    int iSel = SendDlgItemMessage(m_hWnd, IDC_PLATFORM, 
        CB_GETCURSEL, 0, 0);
    switch (iSel)
    {
        case GENERIC:
            return RENDER_GENERIC;
        case XENON:
            return RENDER_XENON;
        case PS3:
            return RENDER_PS3;
        case DX9:
            return RENDER_DX9;
        default:
            return RENDER_UNKNOWN;
    };
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::RefreshImageFormatOptions()
{
    CHECK_MEMORY();
    NiPluginToolkitRenderer ePlatform = GetPlatform();
    
    bool bOrigImages = IsChecked(IDC_TEX_ORIG) || 
        IsChecked(IDC_TEX_ORIG_VRAM);
    EnableImageFormatOptions(m_hWnd, !bOrigImages);
    if (bOrigImages)
        return;

    // enable/disable content quality options
    // no content quality conversion for generic platform
    EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_COMP), true);
    EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_PAL), true);
    EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_16), true);
    EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_32), true);
    EnableWindow(GetDlgItem(m_hWnd, IDC_PER_TEXTURE_OVERRIDE), true);
}
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::OnPlatform()
{
    CHECK_MEMORY();
    NiPluginToolkitRenderer ePlatform = GetPlatform();
    RefreshImageFormatOptions();

    // enable/disable content quality options
    switch (ePlatform)
    {
    case RENDER_XENON:
        SetDlgItemText(m_hWnd, IDC_IMAGE_FILE_DIR, "Xbox360");
        break;
    case RENDER_PS3:
        SetDlgItemText(m_hWnd, IDC_IMAGE_FILE_DIR, "PS3");
        break;
    case RENDER_DX9:
        SetDlgItemText(m_hWnd, IDC_IMAGE_FILE_DIR, "DX9");
        break;
    default:
        SetDlgItemText(m_hWnd, IDC_IMAGE_FILE_DIR, "");
        break;
    }
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::EnableImageFormatOptions(HWND m_hWnd, 
    bool bEnable)
{
    CHECK_MEMORY();
    NiString strTextureExport = GetTextureExport();
    if (strTextureExport == NI_SEPERATE_NIFS)
        EnableWindow(GetDlgItem(m_hWnd, IDC_IMAGE_FILE_DIR), true);
    else
        EnableWindow(GetDlgItem(m_hWnd, IDC_IMAGE_FILE_DIR), false);
    
    if (bEnable)
    {
        EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_COMP),   true);
        EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_16),     true);
        EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_32),     true);
        EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_PAL),    true);
        EnableWindow(GetDlgItem(m_hWnd, IDC_PER_TEXTURE_OVERRIDE), true);
    }
    else
    {
        EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_COMP),   false);
        EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_16),     false);
        EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_32),     false);
        EnableWindow(GetDlgItem(m_hWnd, IDC_TEX_QUAL_PAL),    false);
        EnableWindow(GetDlgItem(m_hWnd, IDC_PER_TEXTURE_OVERRIDE), false);
    }
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::OnTextureExport()
{
    CHECK_MEMORY();
    NiString strTextureExport = GetTextureExport();
    if (strTextureExport == NI_SEPERATE_NIFS)
    {
        RefreshImageFormatOptions();
        EnableWindow(GetDlgItem(m_hWnd, IDC_IMAGE_FILE_DIR), true);
    }
    else if (strTextureExport == NI_ORIGINAL_IMAGES)
    {
        RefreshImageFormatOptions();
        EnableWindow(GetDlgItem(m_hWnd, IDC_IMAGE_FILE_DIR), false);
    }
    else if (strTextureExport == NI_ORIGINAL_IMAGES_VRAM)
    {
        RefreshImageFormatOptions();
        EnableWindow(GetDlgItem(m_hWnd, IDC_IMAGE_FILE_DIR), false);
    }
    else if (strTextureExport == NI_KEEP_IN_MAIN_NIF)
    {
        RefreshImageFormatOptions();
        EnableWindow(GetDlgItem(m_hWnd, IDC_IMAGE_FILE_DIR), false);
    }
    else
    {
        NIASSERT(0);
    }
    CHECK_MEMORY();
}
            
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::OnSkinPercentageEdit()
{   
    CHECK_MEMORY();
    char acString[32];
    int iReturn = GetDlgItemText(m_hWnd, IDC_SKIN_PERCENTAGE_EDIT,
        acString, 32);
    NIASSERT(iReturn != 0);
    NiString strPercent(acString);

    if (strPercent != NiMAXOptions::GetValue(NI_SKINNING_THRESHOLD))
    {
        m_bOptionsChanged = true;
    }

    int iSkinPercent = 0;
    bool bChanged = false;
    if (strPercent.ToInt(iSkinPercent))
    {
        if (iSkinPercent < 1)
        {
            iSkinPercent = 1;
            bChanged = true;
        }
        if (iSkinPercent > 50)
        {
            iSkinPercent = 50;
            bChanged = true;
        }
    }
    else
    {
        bChanged = true;
        iSkinPercent = NiMAXOptions::GetInt(NI_SKINNING_THRESHOLD);
    }

    if (bChanged)
    {
        HWND hwnd = GetDlgItem(m_hWnd, IDC_SKIN_PERCENTAGE_EDIT);
        NiSprintf(acString, 32, "%d", iSkinPercent);
        SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR) acString);
    }
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::OnEditExportScriptButton()
{
    CHECK_MEMORY();
    NiScriptInfoSet* pkSet = m_spAvailableExportScripts;
    NIASSERT(pkSet);
    NiScriptTemplateManager* pkManager = 
        NiScriptTemplateManager::GetInstance();
    NiScriptInfoPtr spScript = 
        NiScriptInfoDialogs::DoManagementDialog(m_spNewExportScript, 
        pkSet, m_hWnd, "EXPORT");

    NiScriptInfoSet& kInfoSet = pkManager->GetScriptInfoSet();
    
    if (spScript)
        m_spNewExportScript = spScript;

    if (kInfoSet.IsTemplate(spScript) && !pkSet->IsTemplate(spScript))
    {   
        CheckDlgButton(m_hWnd, IDC_USE_TEMPLATE_SCRIPTS_CHECK, FALSE);
    }

    if (spScript && pkSet && 
        pkSet->GetScriptIndex(spScript->GetName()) == NIPT_INVALID_INDEX)
    {
        pkSet->AddScript(spScript);
        SendDlgItemMessage(m_hWnd, IDC_SELECTED_EXPORT_SCRIPT, CB_ADDSTRING ,
            0, (LPARAM) (const char*) spScript->GetName());
    }

    FillDropDownList(GetDlgItem(m_hWnd, IDC_SELECTED_EXPORT_SCRIPT), 
        m_spAvailableExportScripts, 
        m_spNewExportScript);

    m_bOptionsChanged = true;
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::OnEditProcessScriptButton()
{
    CHECK_MEMORY();
    NiScriptInfoSet* pkSet = m_spAvailableProcessScripts;
    NIASSERT(pkSet);
    NiScriptTemplateManager* pkManager = 
        NiScriptTemplateManager::GetInstance();

    NiScriptInfoPtr spScript = 
        NiScriptInfoDialogs::DoManagementDialog(m_spNewProcessScript, 
        pkSet, m_hWnd, "PROCESS");

    NiScriptInfoSet& kInfoSet = pkManager->GetScriptInfoSet();
    if (spScript)
        m_spNewProcessScript = spScript;

    if (kInfoSet.IsTemplate(spScript) && !pkSet->IsTemplate(spScript))
    {   
        CheckDlgButton(m_hWnd, IDC_USE_TEMPLATE_SCRIPTS_CHECK, FALSE);
    }

    if (spScript && pkSet &&
        pkSet->GetScriptIndex(spScript->GetName()) == NIPT_INVALID_INDEX)
    {
        pkSet->AddScript(spScript);
        SendDlgItemMessage(m_hWnd, IDC_SELECTED_PROCESS_SCRIPT, 
            CB_ADDSTRING , 0, (LPARAM) (const char*) spScript->GetName());
    }
    
    FillDropDownList(GetDlgItem(m_hWnd, IDC_SELECTED_PROCESS_SCRIPT), 
        m_spAvailableProcessScripts, 
        m_spNewProcessScript);

    m_bOptionsChanged = true;
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::OnSelectedExportScript()
{
   CHECK_MEMORY();
   int iSelIndex = SendDlgItemMessage(m_hWnd,
        IDC_SELECTED_EXPORT_SCRIPT, CB_GETCURSEL, 0, 0);
    if (iSelIndex != CB_ERR)
    {
        char acScriptName[NI_MAX_PATH];
        SendDlgItemMessage(m_hWnd, IDC_SELECTED_EXPORT_SCRIPT,
            CB_GETLBTEXT, (WPARAM) iSelIndex, (LPARAM) (LPCSTR)
            &acScriptName[0]);

        NiScriptInfoSet* pkSet = 
            m_spAvailableExportScripts;
        NIASSERT(pkSet);
        NiScriptInfo* pkSelScript = pkSet->GetScript(
            acScriptName);

        if (pkSelScript)
        {
            m_spNewExportScript = pkSelScript;
            m_bOptionsChanged = true;
        }
    }
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::OnSelectedProcessScript()
{                
    CHECK_MEMORY();
    int iSelIndex = SendDlgItemMessage(m_hWnd,
        IDC_SELECTED_PROCESS_SCRIPT, CB_GETCURSEL, 0, 0);
    if (iSelIndex != CB_ERR)
    {
        char acScriptName[NI_MAX_PATH];
        SendDlgItemMessage(m_hWnd, IDC_SELECTED_PROCESS_SCRIPT,
            CB_GETLBTEXT, (WPARAM) iSelIndex, (LPARAM) (LPCSTR)
            &acScriptName[0]);

        NiScriptInfoSet* pkSet = 
            m_spAvailableProcessScripts;
        NIASSERT(pkSet);
        NiScriptInfo* pkSelScript = pkSet->GetScript(
            acScriptName);

        if (pkSelScript)
        {
            m_spNewProcessScript = pkSelScript;
            m_bOptionsChanged = true;
        }
    }
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
BOOL NiMAXExportSettingsDialog::OnCommand(int iWParamLow, int iWParamHigh,
    long lParam)
{
    CHECK_MEMORY();
    switch(iWParamLow)
    {
        case IDOK:
            OnOK();
            EndDialog(m_hWnd, IDOK);
            break;
        case IDCANCEL:
            EndDialog(m_hWnd, IDCANCEL);
            break;
        case IDC_PLATFORM:
            if (iWParamHigh == CBN_SELCHANGE)
            {
                m_bOptionsChanged = true;
                OnPlatform();
            }
            return TRUE;
        case IDC_SKIN_PERCENTAGE_EDIT:
            if (iWParamHigh == EN_CHANGE)
            {
                OnSkinPercentageEdit();
            }
            return TRUE;
        case IDC_CAM:
        case IDC_LIGHTS:
        case IDC_TEX_QUAL_32:
        case IDC_TEX_QUAL_COMP:
        case IDC_TEX_QUAL_16:
        case IDC_TEX_QUAL_PAL:
        case IDC_KF_ONE:
        case IDC_KF_SEPARATE:
        case IDC_APP_TIME:
        case IDC_EXPORT_CSASKEYFRAME:
        case IDC_USE_TEMPLATE_SCRIPTS_CHECK:
        case IDC_PER_TEXTURE_OVERRIDE:
            m_bOptionsChanged = true;
            break;
        case IDC_TEX_MAINNIF:
        case IDC_TEX_SEPARATE:
        case IDC_TEX_ORIG:
        case IDC_TEX_ORIG_VRAM:
            m_bOptionsChanged = true;
            OnTextureExport();
            break;
        case IDC_EDIT_PROCESS_SCRIPT_BUTTON:
            OnEditProcessScriptButton();
            m_bOptionsChanged = true;
            break;
        case IDC_EDIT_EXPORT_SCRIPT_BUTTON:
            OnEditExportScriptButton();
            m_bOptionsChanged = true;
            break;
        case IDC_SELECTED_PROCESS_SCRIPT:
            if (iWParamHigh == CBN_SELCHANGE)
            {
                OnSelectedProcessScript();
                m_bOptionsChanged = true;
            }
            break;
        case IDC_SELECTED_EXPORT_SCRIPT:
            if (iWParamHigh == CBN_SELCHANGE)
            {
                OnSelectedExportScript();
                m_bOptionsChanged = true;
            }
            break;

        default:
            break;
    }
            
    return FALSE;
}

//---------------------------------------------------------------------------
bool NiMAXExportSettingsDialog::IsChecked(int iResource)
{
    CHECK_MEMORY();
    int iState = IsDlgButtonChecked(m_hWnd, iResource);
    switch (iState)
    {
        case BST_CHECKED:
            return true; 
        case BST_INDETERMINATE:
            NIASSERT(iState != BST_INDETERMINATE);
            return false;
        case BST_UNCHECKED:
            return false;
    }
    return false;
}
//---------------------------------------------------------------------------
NiString NiMAXExportSettingsDialog::GetTextureExport()
{
    CHECK_MEMORY();
    NiString strTextureExport = NI_SEPERATE_NIFS;
    bool bSeperateNifs = IsChecked(IDC_TEX_SEPARATE);
    bool bOriginalTextures = IsChecked(IDC_TEX_ORIG);
    bool bOriginalTexturesVRAM = IsChecked(IDC_TEX_ORIG_VRAM);
    bool bMainNif = IsChecked(IDC_TEX_MAINNIF);

    if (bSeperateNifs)
    {    
        NIASSERT(!bOriginalTextures);
        NIASSERT(!bMainNif);
        NIASSERT(!bOriginalTexturesVRAM);
        strTextureExport = NI_SEPERATE_NIFS;
    }
    else if(bOriginalTextures)
    {    
        NIASSERT(!bSeperateNifs);
        NIASSERT(!bMainNif);
        NIASSERT(!bOriginalTexturesVRAM);
        strTextureExport = NI_ORIGINAL_IMAGES;
    }
    else if (bOriginalTexturesVRAM)
    {    
        NIASSERT(!bSeperateNifs);
        NIASSERT(!bOriginalTextures);
        NIASSERT(!bMainNif);
        strTextureExport = NI_ORIGINAL_IMAGES_VRAM;
    }
    else if (bMainNif)
    {    
        NIASSERT(!bSeperateNifs);
        NIASSERT(!bOriginalTextures);
        NIASSERT(!bOriginalTexturesVRAM);
        strTextureExport = NI_KEEP_IN_MAIN_NIF;
    }
    else
    {
        NIASSERT(!bSeperateNifs);
        NIASSERT(!bOriginalTextures);
        NIASSERT(!bMainNif);
        NIASSERT(0);
    }

    return strTextureExport;
}

//---------------------------------------------------------------------------
NiTexture::FormatPrefs::PixelLayout 
NiMAXExportSettingsDialog::GetPixelLayout()
{
    CHECK_MEMORY();
    NiTexture::FormatPrefs::PixelLayout eLayout = 
        NiTexture::FormatPrefs::PIX_DEFAULT;

    bool bTrueColor = IsChecked(IDC_TEX_QUAL_32); 
    bool bHighColor = IsChecked(IDC_TEX_QUAL_16);
    bool bCompressed = IsChecked(IDC_TEX_QUAL_COMP);
    bool bPaletted = IsChecked(IDC_TEX_QUAL_PAL);

    if (bTrueColor)
    {
        NIASSERT(!bHighColor);
        NIASSERT(!bCompressed);
        NIASSERT(!bPaletted);
        return NiTexture::FormatPrefs::PIX_DEFAULT;
    }
    if (bHighColor)
    {
        NIASSERT(!bTrueColor);
        NIASSERT(!bCompressed);
        NIASSERT(!bPaletted);
        return NiTexture::FormatPrefs::HIGH_COLOR_16;
    }
    if (bCompressed)
    {
        NIASSERT(!bTrueColor);
        NIASSERT(!bHighColor);
        NIASSERT(!bPaletted);
        return NiTexture::FormatPrefs::COMPRESSED;
    }
    if (bPaletted)
    {
        NIASSERT(!bTrueColor);
        NIASSERT(!bHighColor);
        NIASSERT(!bCompressed);
        return NiTexture::FormatPrefs::PALETTIZED_8;
    }

    return eLayout;
}
        
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::OnOK()
{
    CHECK_MEMORY();
    bool bChecked = false;

    // Script options
    {
        bChecked = IsChecked(IDC_USE_TEMPLATE_SCRIPTS_CHECK);
        NiMAXOptions::SetBool(NI_USE_TEMPLATE_FOR_SCRIPT, bChecked);

        NIASSERT(m_spNewProcessScript);
        NiMAXOptions::SetProcessScript(m_spNewProcessScript);
        
        NIASSERT(m_spNewExportScript);
        NiMAXOptions::SetExportScript(m_spNewExportScript);
    }


    // Export options
    {
        bChecked = IsChecked(IDC_CAM);
        NiMAXOptions::SetBool(NI_CONVERT_CAMERAS, bChecked);

        bChecked = IsChecked(IDC_LIGHTS);
        NiMAXOptions::SetBool(NI_CONVERT_LIGHTS, bChecked);
    }

    // Animation options
    {
        bChecked = IsChecked(IDC_APP_TIME);
        NiMAXOptions::SetBool(NI_USE_APPTIME, bChecked);

        bChecked = IsChecked(IDC_EXPORT_CSASKEYFRAME);
        NiMAXOptions::SetBool(NI_CONVERT_CSASKEYFRAMES, bChecked);

        char acString[32];
        int iReturn = GetDlgItemText(m_hWnd, IDC_SKIN_PERCENTAGE_EDIT,
            acString, 32);
        NIASSERT(iReturn != 0);
        NiString strPercent(acString);
        int iSkinPercent = 0;
        if (strPercent.ToInt(iSkinPercent))
            NiMAXOptions::SetInt(NI_SKINNING_THRESHOLD, iSkinPercent);
    }

    
    // Texturing options
    {
        NiString strTextureExport = GetTextureExport();
        NiMAXOptions::SetValue(NI_TEXTURES, strTextureExport);
    }

    // Platform SPecific options
    NiPluginToolkitRenderer eRenderer = GetPlatform();
    NiMAXOptions::SetInt(NI_EXPORT_PLATFORM, eRenderer);

    char acImageFileDir[MAX_PATH];
    GetDlgItemText(m_hWnd, IDC_IMAGE_FILE_DIR, 
        (LPTSTR) acImageFileDir, MAX_PATH + 1);

    NiMAXOptions::SetValue(NI_IMAGE_FILE_DIR, acImageFileDir);

    NiTexture::FormatPrefs::PixelLayout ePixelLayout = GetPixelLayout();
    NiMAXOptions::SetInt(NI_PIXEL_LAYOUT, ePixelLayout);

    bChecked = IsChecked(IDC_PER_TEXTURE_OVERRIDE);
    NiMAXOptions::SetBool(NI_PER_TEXTURE_OVERRIDE, bChecked);
    bChecked = IsChecked(IDC_ALLOW_NPOT_TEXTURES);
    NiMAXOptions::SetBool(NI_ALLOW_NPOT_TEXTURES, bChecked);

    if (m_bOptionsChanged)
        SetSaveRequiredFlag(TRUE);
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
BOOL  NiMAXExportSettingsDialog::OnMessage(unsigned int uiMessage, 
    int iWParamLow, int iWParamHigh, long lParam)
{
    return NiDialog::OnMessage(uiMessage, iWParamLow, iWParamHigh, lParam);
}
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::CheckTextureExportRadio(int iSelection)
{
    CHECK_MEMORY();
    int iState = BST_UNCHECKED;
    CheckDlgButton(m_hWnd, IDC_TEX_SEPARATE, iState);
    CheckDlgButton(m_hWnd, IDC_TEX_ORIG, iState);
    CheckDlgButton(m_hWnd, IDC_TEX_MAINNIF, iState);
    CheckDlgButton(m_hWnd, IDC_TEX_ORIG_VRAM, iState);

    CheckDlgButton(m_hWnd, iSelection, BST_CHECKED);    
}
//---------------------------------------------------------------------------
void NiMAXExportSettingsDialog::CheckPixelLayoutRadio(int iSelection)
{
    CHECK_MEMORY();
    int iState = BST_UNCHECKED;
    CheckDlgButton(m_hWnd, IDC_TEX_QUAL_PAL, iState);
    CheckDlgButton(m_hWnd, IDC_TEX_QUAL_16, iState);
    CheckDlgButton(m_hWnd, IDC_TEX_QUAL_32, iState);
    CheckDlgButton(m_hWnd, IDC_TEX_QUAL_COMP, iState);

    CheckDlgButton(m_hWnd, iSelection, BST_CHECKED);
}
