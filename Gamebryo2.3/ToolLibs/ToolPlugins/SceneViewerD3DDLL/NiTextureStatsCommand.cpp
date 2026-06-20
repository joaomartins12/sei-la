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
#include "NiTextureStatsCommand.h"
#include "NiMain.h"

NiCommandID NiTextureStatsCommand::ms_kID = 0;
//---------------------------------------------------------------------------
NiTextureStatsCommand::NiTextureStatsCommand()
{
}
//---------------------------------------------------------------------------
bool NiTextureStatsCommand::Apply(NiSceneViewer* pkViewer)
{
    pkViewer->EnableTextureStats(!pkViewer->IsTextureStatsEnabled());
    return true;
}
//---------------------------------------------------------------------------
NiCommandID NiTextureStatsCommand::GetClassID()
{
    return ms_kID;
}
//---------------------------------------------------------------------------
NiTextureStatsCommand::~NiTextureStatsCommand()
{

}
//---------------------------------------------------------------------------
NiSceneCommand* NiTextureStatsCommand::Create(NiSceneCommandInfo* pkInfo)
{
    return NiNew NiTextureStatsCommand();
}
//---------------------------------------------------------------------------
NiTPrimitiveArray<NiSceneCommandInfo*>* 
    NiTextureStatsCommand::GetDefaultCommandParamaterizations()
{
    NiSceneCommandInfo* pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->SetName("Toggle Texture Stats Indicator");
    
    NiTPrimitiveArray<NiSceneCommandInfo*>* pkArray = 
        NiNew NiTPrimitiveArray<NiSceneCommandInfo*>();
    pkArray->Add(pkCommandInfo);
    return pkArray;
}
//---------------------------------------------------------------------------
void NiTextureStatsCommand::Register()
{
    ms_kID = NiSceneCommandRegistry::RegisterCommand("TEXTURE_STATS",
        NiTextureStatsCommand::Create);
}
//---------------------------------------------------------------------------
