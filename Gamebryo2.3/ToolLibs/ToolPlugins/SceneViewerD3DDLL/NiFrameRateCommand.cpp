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
#include "NiFrameRateCommand.h"
#include "NiMain.h"

NiCommandID NiFrameRateCommand::ms_kID = 0;
                         

//---------------------------------------------------------------------------
NiFrameRateCommand::NiFrameRateCommand()
{
}
//---------------------------------------------------------------------------
bool NiFrameRateCommand::Apply(NiSceneViewer* pkViewer)
{
    pkViewer->EnableFrameRateStats(!pkViewer->IsFrameRateStatsEnabled());
    return true;
}
//---------------------------------------------------------------------------
NiCommandID NiFrameRateCommand::GetClassID()
{
    return ms_kID;
}
//---------------------------------------------------------------------------
NiFrameRateCommand::~NiFrameRateCommand()
{

}
//---------------------------------------------------------------------------
NiSceneCommand* NiFrameRateCommand::Create(NiSceneCommandInfo* pkInfo)
{
    return NiNew NiFrameRateCommand();
}
//---------------------------------------------------------------------------
NiTPrimitiveArray<NiSceneCommandInfo*>* 
    NiFrameRateCommand::GetDefaultCommandParamaterizations()
{
    NiSceneCommandInfo* pkCommandInfo = NiNew NiSceneCommandInfo();
    pkCommandInfo->m_uiClassID = GetClassID();
    pkCommandInfo->m_eDeviceState = NiDevice::PRESSED;
    pkCommandInfo->m_iPriority = 0;
    pkCommandInfo->SetName("Toggle Frame Rate Indicator");
    
    NiTPrimitiveArray<NiSceneCommandInfo*>* pkArray = 
        NiNew NiTPrimitiveArray<NiSceneCommandInfo*>();
    pkArray->Add(pkCommandInfo);
    return pkArray;
}
//---------------------------------------------------------------------------
void NiFrameRateCommand::Register()
{
    ms_kID = NiSceneCommandRegistry::RegisterCommand("FRAMERATE",
        NiFrameRateCommand::Create);
}
//---------------------------------------------------------------------------
