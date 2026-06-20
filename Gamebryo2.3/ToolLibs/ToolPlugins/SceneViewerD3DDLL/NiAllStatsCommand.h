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

#ifndef NIALLSTATSCOMMAND_H
#define NIALLSTATSCOMMAND_H

//#include "NiSceneCommand.h"
//#include "NiSceneViewer.h"

// Command Class:   AllStats
// Command ClassID: 13
// Command Purpose: To toggle AllStats mode on or off
// Command String Syntax: AllStats() SINGLE

class NiAllStatsCommand : public NiSceneCommand
{
public:
    NiAllStatsCommand();
    static NiSceneCommand* Create(NiSceneCommandInfo* pkInfo);
    ~NiAllStatsCommand();
    bool Apply(NiSceneViewer* pkViewer);
    NiCommandID GetClassID();
    NiTPrimitiveArray<NiSceneCommandInfo*>* 
        GetDefaultCommandParamaterizations();
    static void Register();
protected:
    static NiCommandID ms_kID;
};

#endif