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

#ifndef NIFRAMERATECOMMAND_H
#define NIFRAMERATECOMMAND_H

//#include "NiSceneCommand.h"
//#include "NiSceneViewer.h"

// Command Class:   FrameRate
// Command ClassID: 10
// Command Purpose: To toggle the FrameRate mode on or off
// Command String Syntax: FrameRate() SINGLE

class NiFrameRateCommand : public NiSceneCommand
{
public:
    NiFrameRateCommand();
    static NiSceneCommand* Create(NiSceneCommandInfo* pkInfo);
    ~NiFrameRateCommand();
    bool Apply(NiSceneViewer* pkViewer);
    NiCommandID GetClassID();
    static void Register();
    
    NiTPrimitiveArray<NiSceneCommandInfo*>* 
        GetDefaultCommandParamaterizations();

protected:
    static NiCommandID ms_kID;
};

#endif