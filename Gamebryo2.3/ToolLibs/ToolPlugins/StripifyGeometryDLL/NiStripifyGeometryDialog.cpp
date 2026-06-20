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

#include "stdafx.h"
#include "NiStripifyGeometryDialog.h"
#include "NiStripifyGeometry_resource.h"
#include "NiStripifyDLLDefines.h"

//---------------------------------------------------------------------------
NiStripifyGeometryDialog::NiStripifyGeometryDialog(NiModuleRef hInstance, 
    NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo) :
    NiDialog(IDD_OPT_STRIPIFY_OPTIONS, hInstance, hWndParent)
{
    m_spPluginInfoInitial = pkPluginInfo;
    m_spPluginInfoResults = pkPluginInfo->Clone();

}
//---------------------------------------------------------------------------
NiStripifyGeometryDialog::~NiStripifyGeometryDialog()
{
    m_spPluginInfoResults = NULL;
    m_spPluginInfoInitial = NULL;
}
//---------------------------------------------------------------------------
int NiStripifyGeometryDialog::DoModal()
{
    int iResult = NiDialog::DoModal();

    return iResult;
}
//---------------------------------------------------------------------------
void NiStripifyGeometryDialog::InitDialog()
{
    NiString strPCValue = m_spPluginInfoInitial->GetValue(STRIP_PC_SETTINGS);
    NiString strXboxValue = 
        m_spPluginInfoInitial->GetValue(STRIP_NEXTGENXBOX_SETTINGS);

    if (strPCValue == NULL)
        strPCValue = STRIP_PC_NEXTGEN_VALUE;
    
    if (strXboxValue == NULL)
        strXboxValue = STRIP_NEXTGENXBOX_VALUE;

    int idValue = IDC_PC_NEXTGEN;

    if (strPCValue == STRIP_PC_FIRSTGEN_VALUE)
        idValue = IDC_PC_FIRST_GEN;
    else if (strPCValue == STRIP_PC_UNACCELERATED_VALUE)
        idValue = IDC_PC_UNACCELERATED;
    else if (strPCValue == STRIP_DO_NOT_STRIPIFY_VALUE)
        idValue = IDC_PC_NOSTRIP;

    CheckRadioButton(m_hWnd, IDC_PC_NOSTRIP, IDC_PC_NEXTGEN, idValue);

    idValue = IDC_NEXTGENXBOX_STRIP;
    if (strPCValue == STRIP_DO_NOT_STRIPIFY_VALUE)
        idValue = IDC_NEXTGENXBOX_NOSTRIP;

    CheckRadioButton(m_hWnd, IDC_NEXTGENXBOX_NOSTRIP, IDC_NEXTGENXBOX_STRIP, 
        idValue);

}
//---------------------------------------------------------------------------
BOOL NiStripifyGeometryDialog::OnCommand(int iWParamLow, int iWParamHigh, 
                                         long lParam)
{
    switch(iWParamLow)
    {
    case IDOK:
    {
            // Save options.
        HandleOK();
        EndDialog(m_hWnd, IDOK);
        return TRUE;
    }
    case IDCANCEL:
    {
        EndDialog(m_hWnd, IDCANCEL);
        return TRUE;
    }
    }
    return FALSE;
}
//---------------------------------------------------------------------------
NiPluginInfoPtr NiStripifyGeometryDialog::GetResults()
{
    return m_spPluginInfoResults;
}
//---------------------------------------------------------------------------
void NiStripifyGeometryDialog::HandleOK()
{
    NiString strPCValue = STRIP_PC_NEXTGEN_VALUE;

    if (IsDlgButtonChecked(m_hWnd, IDC_PC_NOSTRIP) == BST_CHECKED)
        strPCValue = STRIP_DO_NOT_STRIPIFY_VALUE;
    else if (IsDlgButtonChecked(m_hWnd, IDC_PC_UNACCELERATED) == BST_CHECKED)
        strPCValue = STRIP_PC_UNACCELERATED_VALUE;
    else if (IsDlgButtonChecked(m_hWnd, IDC_PC_FIRST_GEN) == BST_CHECKED)
        strPCValue = STRIP_PC_FIRSTGEN_VALUE;
    else if (IsDlgButtonChecked(m_hWnd, IDC_PC_NEXTGEN) == BST_CHECKED)
        strPCValue = STRIP_PC_NEXTGEN_VALUE;

    NiString strXboxValue = STRIP_NEXTGENXBOX_VALUE;
    if (IsDlgButtonChecked(m_hWnd, IDC_NEXTGENXBOX_NOSTRIP) == BST_CHECKED)
        strXboxValue = STRIP_DO_NOT_STRIPIFY_VALUE;


    m_spPluginInfoResults->SetValue(STRIP_PC_SETTINGS, strPCValue);
    m_spPluginInfoResults->SetValue(STRIP_NEXTGENXBOX_SETTINGS, strXboxValue);
}
//---------------------------------------------------------------------------
