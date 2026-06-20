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

#include "FLTImportDialog.h"
#include "FLTImportPlugin.h"
#include "resource.h"

//---------------------------------------------------------------------------
FLTImportDialog::FLTImportDialog(NiModuleRef hInstance,
    NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo) :
    NiDialog(IDD_IMPORT_DIALOG, hInstance, hWndParent)
{
    // Since this is a cancelable dialog, we need to keep around the original
    // untouched copy of the plug-in info. Therefore, we clone the original
    // and store it in the results.
    NIASSERT(pkPluginInfo);
    m_spPluginInfoInitial = pkPluginInfo;
    m_spPluginInfoResults = pkPluginInfo->Clone();
}
//---------------------------------------------------------------------------
FLTImportDialog::~FLTImportDialog()
{
    m_spPluginInfoResults = NULL;
    m_spPluginInfoInitial = NULL;
}
//---------------------------------------------------------------------------
void FLTImportDialog::InitDialog()
{
    // Here is where the original plug-in info's values are read in and
    // the original state of the UI is set up.
    bool bInternalTextures = m_spPluginInfoInitial->GetBool(
        USE_INTERNAL_TEXTURES);

    if (bInternalTextures)
    {
        CheckDlgButton(m_hWnd, IDC_INTERNALTEXTURES, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_INTERNALTEXTURES, BST_UNCHECKED);
    }

    bool bAddDefaultLights = m_spPluginInfoInitial->GetBool(
        ADD_DEFAULT_LIGHTS);

    if (bAddDefaultLights)
    {
        CheckDlgButton(m_hWnd, IDC_ADD_DEFAULT_LIGHTS, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_ADD_DEFAULT_LIGHTS, BST_UNCHECKED);
    }

    bool bUseTrueDoubleSided = m_spPluginInfoInitial->GetBool(
        USE_TRUE_DOUBLESIDED);

    if (bUseTrueDoubleSided)
    {
        CheckDlgButton(m_hWnd, IDC_USE_TRUE_DOUBLESIDED, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_USE_TRUE_DOUBLESIDED, BST_UNCHECKED);
    }

    bool bForceMergeSiblingSwitchNodes = m_spPluginInfoInitial->GetBool(
        FORCE_MERGE_SIBLING_SWITCHNODES);

    if (bForceMergeSiblingSwitchNodes)
    {
        CheckDlgButton(m_hWnd, IDC_FORCE_MERGE_SIBLING_SWITCHNODES,
            BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_FORCE_MERGE_SIBLING_SWITCHNODES,
            BST_UNCHECKED);
    }

    bool bForceMergeSiblingLODNodes = m_spPluginInfoInitial->GetBool(
        FORCE_MERGE_SIBLING_LODNODES);

    if (bForceMergeSiblingLODNodes)
    {
        CheckDlgButton(m_hWnd, IDC_FORCE_MERGE_SIBLING_LODNODES,
            BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_hWnd, IDC_FORCE_MERGE_SIBLING_LODNODES,
            BST_UNCHECKED);
    }
}
//---------------------------------------------------------------------------
BOOL FLTImportDialog::OnCommand(int iWParamLow, int iWParamHigh,
    long lParam)
{
    switch (iWParamLow)
    {
    case IDOK:
        // Handle the OK button.
        HandleOK();
        EndDialog(m_hWnd, IDOK);
        return TRUE;
    case IDCANCEL:
        // Handle the cancel button.
        EndDialog(m_hWnd, IDCANCEL);
        return TRUE;
    // If you have any logic for updating any of the GUI, add the handlers
    // here. Return true for anything that you handle.
    }

    return FALSE;
}
//---------------------------------------------------------------------------
NiPluginInfoPtr FLTImportDialog::GetResults()
{
    // This is called after the dialog has exited and returns the values that
    // were set in HandleOK.
    return m_spPluginInfoResults;
}
//---------------------------------------------------------------------------
void FLTImportDialog::HandleOK()
{
    // Now that the user has accepted the changes that were made, we must set
    // the resulting plug-in info.

    // Set the sample bool value.
    m_spPluginInfoResults->SetBool(USE_INTERNAL_TEXTURES, 
        IsDlgButtonChecked(m_hWnd, IDC_INTERNALTEXTURES) == BST_CHECKED);
    m_spPluginInfoResults->SetBool(ADD_DEFAULT_LIGHTS, 
        IsDlgButtonChecked(m_hWnd, IDC_ADD_DEFAULT_LIGHTS) == BST_CHECKED);
    m_spPluginInfoResults->SetBool(USE_TRUE_DOUBLESIDED, 
        IsDlgButtonChecked(m_hWnd, IDC_USE_TRUE_DOUBLESIDED) == BST_CHECKED);
    m_spPluginInfoResults->SetBool(FORCE_MERGE_SIBLING_SWITCHNODES, 
        IsDlgButtonChecked(m_hWnd, IDC_FORCE_MERGE_SIBLING_SWITCHNODES)
        == BST_CHECKED);
    m_spPluginInfoResults->SetBool(FORCE_MERGE_SIBLING_LODNODES, 
        IsDlgButtonChecked(m_hWnd, IDC_FORCE_MERGE_SIBLING_LODNODES)
        == BST_CHECKED);
}
//---------------------------------------------------------------------------
      