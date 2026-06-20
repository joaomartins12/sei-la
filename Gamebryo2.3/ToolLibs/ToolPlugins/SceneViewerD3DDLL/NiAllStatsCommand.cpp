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
#include "NiAllStatsCommand.h"

NiCommandID NiAllStatsCommand::ms_kID = 0;

//---------------------------------------------------------------------------
NiAllStatsCommand::NiAllStatsCommand()
{
}
//---------------------------------------------------------------------------
bool NiAllStatsCommand::Apply(NiSceneViewer* pkViewer)
{
    pkViewer->EnableAllStats(!pkViewer->IsAllStatsEnabled());
    return true;
}
//---------------------------------------------------------------------------
NiCommandID NiAllStatsCommand::GetClassID()
{
    return ms_kID;
}
//---------------------------------------------------------------------------
NiAllStatsCommand::~NiAllStatsCommand()
{

}
//---------------------------------------------------------------------------
NiSceneCommand* NiAllStatsCommand::Create(NiSceneCommandInfo* pkInfo)
{
    return NiNew NiAllStatsCommand();
}
//---------------------------------------------------------------------------
NiTPrimitiveArray<NiSceneCommandInfo*>* 
    NiAllStatsCommand::GetDefaultCommandParamaterizations()
{
    NiSceneCommandInfo* pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->SetName("Toggle All Stats");
    
    NiTPrimitiveArray<NiSceneCommandInfo*>* pkArray = 
        NiNew NiTPrimitiveArray<NiSceneCommandInfo*>();
    pkArray->Add(pkCommandInfo);
    return pkArray;
}
//---------------------------------------------------------------------------
void NiAllStatsCommand::Register()
{
     ms_kID = NiSceneCommandRegistry::RegisterCommand("ALL_STATS", 
         NiAllStatsCommand::Create);
}
//---------------------------------------------------------------------------
