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
#include "NiGeometryStatsCommand.h"
#include "NiMain.h"

NiCommandID NiGeometryStatsCommand::ms_kID = 0;

//---------------------------------------------------------------------------
NiGeometryStatsCommand::NiGeometryStatsCommand()
{
}
//---------------------------------------------------------------------------
bool NiGeometryStatsCommand::Apply(NiSceneViewer* pkViewer)
{
   pkViewer->EnableGeometryStats(!pkViewer->IsGeometryStatsEnabled());
   return true;
}
//---------------------------------------------------------------------------
NiCommandID NiGeometryStatsCommand::GetClassID()
{
    return ms_kID;
}
//---------------------------------------------------------------------------
NiGeometryStatsCommand::~NiGeometryStatsCommand()
{

}
//---------------------------------------------------------------------------
NiSceneCommand* NiGeometryStatsCommand::Create(NiSceneCommandInfo* pkInfo)
{
    return NiNew NiGeometryStatsCommand();
}
//---------------------------------------------------------------------------
NiTPrimitiveArray<NiSceneCommandInfo*>* 
    NiGeometryStatsCommand::GetDefaultCommandParamaterizations()
{
    NiSceneCommandInfo* pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->SetName("Toggle Geometry Stats Indicator");
    
    NiTPrimitiveArray<NiSceneCommandInfo*>* pkArray = 
        NiNew NiTPrimitiveArray<NiSceneCommandInfo*>();
    pkArray->Add(pkCommandInfo);
    return pkArray;
}
//---------------------------------------------------------------------------
void NiGeometryStatsCommand::Register()
{
    ms_kID = NiSceneCommandRegistry::RegisterCommand("GEOMETRY_STATS",
        NiGeometryStatsCommand::Create);
}
//---------------------------------------------------------------------------
