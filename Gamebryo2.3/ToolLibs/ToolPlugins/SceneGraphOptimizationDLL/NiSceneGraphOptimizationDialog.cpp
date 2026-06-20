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

#include <stdafx.h>
#include "NiSceneGraphOptimizationDialog.h"
#include "NiSceneGraphOptimization_resource.h"
#include "NiSceneGraphOptimizationDefines.h"

//---------------------------------------------------------------------------
NiSceneGraphOptimizationDialog::NiSceneGraphOptimizationDialog(
    NiModuleRef hInstance, NiWindowRef hWndParent, NiPluginInfo* pkPluginInfo)
    : NiDialog(IDD_OPTIMIZATION_OPTIONS, hInstance, hWndParent)
{
    m_spPluginInfoInitial = pkPluginInfo;
    m_spPluginInfoResults = pkPluginInfo->Clone();
}
//---------------------------------------------------------------------------
NiSceneGraphOptimizationDialog::~NiSceneGraphOptimizationDialog()
{
    m_spPluginInfoResults = NULL;
    m_spPluginInfoInitial = NULL;
}
//---------------------------------------------------------------------------
void NiSceneGraphOptimizationDialog::InitDialog()
{
    // Load options.
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEDUPPROPERTIES,
        m_spPluginInfoResults->GetBool(OPT_REMOVE_DUP_PROPERTIES));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEBADTEXTURINGPROPS,
        m_spPluginInfoResults->GetBool(OPT_REMOVE_BAD_TEXTURING_PROPS));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEMULTIMTLNODES,
        m_spPluginInfoResults->GetBool(OPT_REMOVE_MULTIMTL_NODES));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVECHILDLESSNODES,
        m_spPluginInfoResults->GetBool(OPT_REMOVE_CHILDLESS_NODES));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVESINGLECHILDNODES,
        m_spPluginInfoResults->GetBool(OPT_REMOVE_SINGLE_CHILD_NODES));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_MERGESIBLINGNODES,
        m_spPluginInfoResults->GetBool(OPT_MERGE_SIBLING_NODES));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_MERGESIBLINGTRISHAPES,
        m_spPluginInfoResults->GetBool(OPT_MERGE_SIBLING_TRISHAPES));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_NOMERGEONALPHA,
        m_spPluginInfoResults->GetBool(OPT_NO_MERGE_ON_ALPHA));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_OPTIMIZETRISHAPES,
        m_spPluginInfoResults->GetBool(OPT_OPTIMIZE_TRISHAPES));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEEXTRAUVSETS,
        m_spPluginInfoResults->GetBool(OPT_REMOVE_EXTRA_UVSETS));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYNORMALS,
        m_spPluginInfoResults->GetBool(OPT_REMOVE_UNNECESSARY_NORMALS));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYVISCONTROLLERS,
        m_spPluginInfoResults->GetBool(
        OPT_REMOVE_UNNECESSARY_VISCONTROLLERS));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYLOOKATCONTROLLERS,
        m_spPluginInfoResults->GetBool(
        OPT_REMOVE_UNNECESSARY_LOOKATCONTROLLERS));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEHIDDENBONEGEOMETRY,
        m_spPluginInfoResults->GetBool(OPT_REMOVE_HIDDEN_BONE_GEOMETRY));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REDUCEANIMATIONKEYS,
        m_spPluginInfoResults->GetBool(OPT_REDUCE_ANIMATION_KEYS));

    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_ANIMATIONKEYREDUCTIONTOLERANCE), m_spPluginInfoResults
        ->GetValue(OPT_ANIMATION_KEY_REDUCTION_TOLERANCE));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYEXTRADATA,
            m_spPluginInfoResults->GetBool(
            OPT_REMOVE_UNNECESSARY_EXTRA_DATA));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVESGOKEEPTAGS,
        m_spPluginInfoResults->GetBool(OPT_REMOVE_SGOKEEP_TAGS));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_SORTBYTEXTURINGPROP,
        m_spPluginInfoResults->GetBool(OPT_SORT_BY_TEXTURING_PROP));

    if (m_spPluginInfoResults->GetBool(OPT_REMOVE_NAMES))
    {
        CheckRadioButton(m_hWnd, IDC_OPT_RADIO_REMOVENAMES,
            IDC_OPT_RADIO_LEAVENAMES, IDC_OPT_RADIO_REMOVENAMES);
    }
    else 
    {
        if (m_spPluginInfoResults->GetBool(OPT_NAME_UNNAMED_OBJECTS))
        {
            CheckRadioButton(m_hWnd, IDC_OPT_RADIO_REMOVENAMES,
                IDC_OPT_RADIO_LEAVENAMES,
                IDC_OPT_RADIO_NAMEUNNAMEDOBJECTS);
        }
        else
        {
            CheckRadioButton(m_hWnd, IDC_OPT_RADIO_REMOVENAMES,
                IDC_OPT_RADIO_LEAVENAMES, IDC_OPT_RADIO_LEAVENAMES);
        }
    }
    
    EnableWindow(
        GetDlgItem(m_hWnd, IDC_OPT_CHECK_NOMERGEONALPHA),
        IsDlgButtonChecked(m_hWnd,
        IDC_OPT_CHECK_MERGESIBLINGTRISHAPES));
    EnableWindow(
        GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_ANIMATIONKEYREDUCTIONTOLERANCE),
        IsDlgButtonChecked(m_hWnd,
        IDC_OPT_CHECK_REDUCEANIMATIONKEYS));
    EnableWindow(
        GetDlgItem(m_hWnd, IDC_OPT_CHECK_REMOVESGOKEEPTAGS),
        IsDlgButtonChecked(m_hWnd,
        IDC_OPT_CHECK_REMOVEUNNECESSARYEXTRADATA));

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_WELDSKIN,
        m_spPluginInfoResults->GetBool(OPT_WELD_SKIN));

    if (m_spPluginInfoResults->GetBool(OPT_EXPORT_SOFTWARE_SKIN))
    {
        if (m_spPluginInfoResults->GetBool(OPT_EXPORT_HARDWARE_SKIN))
        {
            CheckRadioButton(m_hWnd, IDC_OPT_RADIO_SKINHW,
                IDC_OPT_RADIO_SKINBOTH, IDC_OPT_RADIO_SKINBOTH);
        }
        else
        {
            CheckRadioButton(m_hWnd, IDC_OPT_RADIO_SKINHW,
                IDC_OPT_RADIO_SKINBOTH, IDC_OPT_RADIO_SKINSW);
        }
    }
    else 
    {
        CheckRadioButton(m_hWnd, IDC_OPT_RADIO_SKINHW,
            IDC_OPT_RADIO_SKINBOTH, IDC_OPT_RADIO_SKINHW);
    }

    SetWindowText(GetDlgItem(m_hWnd, IDC_OPT_EDIT_BONESPERPARTITION),
        m_spPluginInfoResults->GetValue(OPT_BONES_PER_PARTITION));

    SetSkinAndBonesOptionsForPlatform();
}
//---------------------------------------------------------------------------
BOOL NiSceneGraphOptimizationDialog::OnCommand(int iWParamLow,
    int iWParamHigh, long lParam)
{
    switch(iWParamLow)
    {
    case IDOK:
        // Save options.
        HandleOK();
        EndDialog(m_hWnd, IDOK);
        return TRUE;
    case IDCANCEL:
        EndDialog(m_hWnd, IDCANCEL);
        return TRUE;
    case IDC_OPT_CHECK_MERGESIBLINGTRISHAPES:
        EnableWindow( GetDlgItem(m_hWnd, IDC_OPT_CHECK_NOMERGEONALPHA),
            IsDlgButtonChecked(m_hWnd,
            IDC_OPT_CHECK_MERGESIBLINGTRISHAPES) == 1);
        return TRUE;
    case IDC_OPT_CHECK_REDUCEANIMATIONKEYS:
        EnableWindow( GetDlgItem(m_hWnd,
            IDC_OPT_EDIT_ANIMATIONKEYREDUCTIONTOLERANCE),
            IsDlgButtonChecked(m_hWnd,
            IDC_OPT_CHECK_REDUCEANIMATIONKEYS) == 1);
        return TRUE;
    case IDC_OPT_CHECK_REMOVEUNNECESSARYEXTRADATA:
        EnableWindow( GetDlgItem(m_hWnd, IDC_OPT_CHECK_REMOVESGOKEEPTAGS),
            IsDlgButtonChecked(m_hWnd,
            IDC_OPT_CHECK_REMOVEUNNECESSARYEXTRADATA) == 1);
        return TRUE;
    case IDC_OPT_BUTTON_RESETDEFAULTS:
        ResetDefaultDialogOptions();
        return TRUE;
    case IDC_OPT_BUTTON_UNCHECKALL:
        // Uncheck all options.
        UncheckAllDialogOptions();
        return TRUE;
    }

    return FALSE;
}
//---------------------------------------------------------------------------
NiPluginInfoPtr NiSceneGraphOptimizationDialog::GetResults()
{
    return m_spPluginInfoResults;
}
//---------------------------------------------------------------------------
void NiSceneGraphOptimizationDialog::SetSkinAndBonesOptionsForPlatform()
{
    NiString strValue = m_spPluginInfoResults->GetValue("TexturePlatform");
    int iPlatform;
    if(strValue.ToInt(iPlatform))
    {
        switch (iPlatform)
        {
        case NiDevImageConverter::PLAYSTATION2:
            EnableWindow(GetDlgItem(m_hWnd, IDC_OPT_EDIT_BONESPERPARTITION),
                FALSE);
            break;
        default:
            EnableWindow(GetDlgItem(m_hWnd, IDC_OPT_EDIT_BONESPERPARTITION),
                TRUE);
            break;
        }
    }
}
//---------------------------------------------------------------------------
void NiSceneGraphOptimizationDialog::ResetDefaultDialogOptions()
{
    // Resets options in dialog to defaults.
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEDUPPROPERTIES, TRUE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEBADTEXTURINGPROPS, TRUE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEMULTIMTLNODES, TRUE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVECHILDLESSNODES, TRUE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVESINGLECHILDNODES, TRUE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_MERGESIBLINGNODES, TRUE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_MERGESIBLINGTRISHAPES, TRUE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_NOMERGEONALPHA, TRUE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_OPTIMIZETRISHAPES, TRUE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEEXTRAUVSETS, TRUE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYNORMALS, TRUE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYVISCONTROLLERS,
        TRUE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYLOOKATCONTROLLERS,
        TRUE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEHIDDENBONEGEOMETRY, TRUE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REDUCEANIMATIONKEYS, TRUE);
    char acTolerance[100];
    NiSprintf(acTolerance, 100, "%f", NiOptimize::ms_fEpsilon);
    SetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_ANIMATIONKEYREDUCTIONTOLERANCE), acTolerance);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYEXTRADATA, TRUE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVESGOKEEPTAGS, TRUE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_SORTBYTEXTURINGPROP, FALSE);
    CheckRadioButton(m_hWnd, IDC_OPT_RADIO_REMOVENAMES,
        IDC_OPT_RADIO_LEAVENAMES, IDC_OPT_RADIO_LEAVENAMES);

    EnableWindow(GetDlgItem(m_hWnd, IDC_OPT_CHECK_NOMERGEONALPHA), TRUE);
    EnableWindow(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_ANIMATIONKEYREDUCTIONTOLERANCE), TRUE);
    EnableWindow(GetDlgItem(m_hWnd, IDC_OPT_CHECK_REMOVESGOKEEPTAGS), TRUE);

    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_WELDSKIN, TRUE);
    CheckDlgButton(m_hWnd, IDC_OPT_RADIO_SKINBOTH, TRUE);
    CheckDlgButton(m_hWnd, IDC_OPT_RADIO_SKINHW, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_RADIO_SKINSW, FALSE);
    SetWindowText(GetDlgItem(m_hWnd, IDC_OPT_EDIT_BONESPERPARTITION), "4");
}
//---------------------------------------------------------------------------
void NiSceneGraphOptimizationDialog::UncheckAllDialogOptions()
{
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEDUPPROPERTIES, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEBADTEXTURINGPROPS, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEMULTIMTLNODES, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVECHILDLESSNODES, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVESINGLECHILDNODES, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_MERGESIBLINGNODES, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_MERGESIBLINGTRISHAPES, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_NOMERGEONALPHA, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_OPTIMIZETRISHAPES, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEEXTRAUVSETS, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYNORMALS, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYVISCONTROLLERS,
        FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYLOOKATCONTROLLERS,
        FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEHIDDENBONEGEOMETRY, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REDUCEANIMATIONKEYS, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYEXTRADATA, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_REMOVESGOKEEPTAGS, FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_SORTBYTEXTURINGPROP, FALSE);
    CheckRadioButton(m_hWnd, IDC_OPT_RADIO_REMOVENAMES,
        IDC_OPT_RADIO_LEAVENAMES, IDC_OPT_RADIO_LEAVENAMES);
    EnableWindow(GetDlgItem(m_hWnd, IDC_OPT_CHECK_NOMERGEONALPHA), FALSE);
    EnableWindow(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_ANIMATIONKEYREDUCTIONTOLERANCE), FALSE);
    EnableWindow(GetDlgItem(m_hWnd, IDC_OPT_CHECK_REMOVESGOKEEPTAGS), FALSE);
    CheckDlgButton(m_hWnd, IDC_OPT_CHECK_WELDSKIN, FALSE);
}
//---------------------------------------------------------------------------
void NiSceneGraphOptimizationDialog::HandleOK()
{
    m_spPluginInfoResults->SetBool(OPT_REMOVE_DUP_PROPERTIES,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REMOVEDUPPROPERTIES) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REMOVE_BAD_TEXTURING_PROPS,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REMOVEBADTEXTURINGPROPS) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REMOVE_MULTIMTL_NODES,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REMOVEMULTIMTLNODES) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REMOVE_CHILDLESS_NODES,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REMOVECHILDLESSNODES) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REMOVE_SINGLE_CHILD_NODES,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REMOVESINGLECHILDNODES) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_MERGE_SIBLING_NODES,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_MERGESIBLINGNODES) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_MERGE_SIBLING_TRISHAPES,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_MERGESIBLINGTRISHAPES) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_NO_MERGE_ON_ALPHA,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_NOMERGEONALPHA) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_OPTIMIZE_TRISHAPES,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_OPTIMIZETRISHAPES) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REMOVE_EXTRA_UVSETS,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REMOVEEXTRAUVSETS) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REMOVE_UNNECESSARY_NORMALS,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYNORMALS) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REMOVE_UNNECESSARY_VISCONTROLLERS,
        IsDlgButtonChecked(m_hWnd,
        IDC_OPT_CHECK_REMOVEUNNECESSARYVISCONTROLLERS) == BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REMOVE_UNNECESSARY_LOOKATCONTROLLERS,
        IsDlgButtonChecked(m_hWnd,
        IDC_OPT_CHECK_REMOVEUNNECESSARYLOOKATCONTROLLERS) == BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REMOVE_HIDDEN_BONE_GEOMETRY,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REMOVEHIDDENBONEGEOMETRY) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REDUCE_ANIMATION_KEYS,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REDUCEANIMATIONKEYS) ==
        BST_CHECKED);

    char acTolerance[100];
    GetWindowText(GetDlgItem(m_hWnd,
        IDC_OPT_EDIT_ANIMATIONKEYREDUCTIONTOLERANCE), &acTolerance[0], 100);
    float fNewTolerance = (float) atof(acTolerance);
    if (fNewTolerance > 0.0f)
    {
        m_spPluginInfoResults->SetFloat(OPT_ANIMATION_KEY_REDUCTION_TOLERANCE,
            fNewTolerance);
    }

    m_spPluginInfoResults->SetBool(OPT_REMOVE_UNNECESSARY_EXTRA_DATA,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REMOVEUNNECESSARYEXTRADATA)
        == BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_REMOVE_SGOKEEP_TAGS,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_REMOVESGOKEEPTAGS) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_SORT_BY_TEXTURING_PROP,
        IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_SORTBYTEXTURINGPROP) ==
        BST_CHECKED);

    if (IsDlgButtonChecked(m_hWnd, IDC_OPT_RADIO_REMOVENAMES) == BST_CHECKED)
    {
        m_spPluginInfoResults->SetBool(OPT_REMOVE_NAMES, true);
        m_spPluginInfoResults->SetBool(OPT_NAME_UNNAMED_OBJECTS, false);
    }
    else if (IsDlgButtonChecked(m_hWnd, IDC_OPT_RADIO_NAMEUNNAMEDOBJECTS) ==
        BST_CHECKED)
    {
        m_spPluginInfoResults->SetBool(OPT_REMOVE_NAMES, false);
        m_spPluginInfoResults->SetBool(OPT_NAME_UNNAMED_OBJECTS, true);
    }
    else
    {
        m_spPluginInfoResults->SetBool(OPT_REMOVE_NAMES, false);
        m_spPluginInfoResults->SetBool(OPT_NAME_UNNAMED_OBJECTS, false);
    }

    bool bWeld = (IsDlgButtonChecked(m_hWnd, IDC_OPT_CHECK_WELDSKIN) ==
        BST_CHECKED);
    m_spPluginInfoResults->SetBool(OPT_WELD_SKIN, bWeld);

    if (IsDlgButtonChecked(m_hWnd, IDC_OPT_RADIO_SKINSW) == BST_CHECKED)
    {
        m_spPluginInfoResults->SetBool(OPT_EXPORT_SOFTWARE_SKIN, true);
        m_spPluginInfoResults->SetBool(OPT_EXPORT_HARDWARE_SKIN, false);
    }
    else if (IsDlgButtonChecked(m_hWnd, IDC_OPT_RADIO_SKINHW) == BST_CHECKED)
    {
        m_spPluginInfoResults->SetBool(OPT_EXPORT_SOFTWARE_SKIN, false);
        m_spPluginInfoResults->SetBool(OPT_EXPORT_HARDWARE_SKIN, true);
    }
    else
    {
        m_spPluginInfoResults->SetBool(OPT_EXPORT_SOFTWARE_SKIN, true);
        m_spPluginInfoResults->SetBool(OPT_EXPORT_HARDWARE_SKIN, true);
    }
    
    char acBonesPerPartition[100];
    GetWindowText(GetDlgItem(m_hWnd, IDC_OPT_EDIT_BONESPERPARTITION),
        &acBonesPerPartition[0], 100);
    unsigned int uiNewBPP = atoi(acBonesPerPartition);
    if (uiNewBPP > 3 && uiNewBPP <= 256)
    {
        m_spPluginInfoResults->SetInt(OPT_BONES_PER_PARTITION,
            (int) uiNewBPP);
    }
}
//---------------------------------------------------------------------------
