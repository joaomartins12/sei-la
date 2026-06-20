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
#include "DeprecatedProcessPlugin.h"

// RTTI implementation macro.
NiImplementRTTI(DeprecatedProcessPlugin, NiPlugin);

//---------------------------------------------------------------------------
DeprecatedProcessPlugin::DeprecatedProcessPlugin(const char* pcName) : 
    NiPlugin(pcName, "0.0",  NDL_DEPRECATED_PLUGIN_STRING,
        NDL_DEPRECATED_PLUGIN_STRING)
{
    
}
//---------------------------------------------------------------------------
NiPluginInfo* DeprecatedProcessPlugin::GetDefaultPluginInfo()
{
    return NULL;
}
//---------------------------------------------------------------------------
bool DeprecatedProcessPlugin::HasManagementDialog()
{
    return true;
}
//---------------------------------------------------------------------------
bool DeprecatedProcessPlugin::DoManagementDialog(NiPluginInfo* pkInitialInfo,
    NiWindowRef hWndParent)
{
    NiMessageBox(GetLongDescription(), GetName());
    return true;
}
//---------------------------------------------------------------------------
bool DeprecatedProcessPlugin::CanExecute(NiPluginInfo* pkInfo)
{
    if (m_strName.EqualsNoCase(pkInfo->GetName()))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiExecutionResultPtr DeprecatedProcessPlugin::Execute(
    const NiPluginInfo* pkInfo)
{
    return NiNew NiExecutionResult(NiExecutionResult::EXECUTE_SUCCESS);
}
//---------------------------------------------------------------------------
bool DeprecatedProcessPlugin::CanCreateDefaultInstance()
{
    return false;
}
//---------------------------------------------------------------------------
      