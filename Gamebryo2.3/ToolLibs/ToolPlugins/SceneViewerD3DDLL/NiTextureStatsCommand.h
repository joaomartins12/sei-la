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

#ifndef NITEXTURESTATSCOMMAND_H
#define NITEXTURESTATSCOMMAND_H

//#include "NiSceneCommand.h"
//#include "NiSceneViewer.h"

// Command Class:   TextureStats
// Command ClassID: 11
// Command Purpose: To toggle the TextureStats mode on or off
// Command String Syntax: TextureStats() SINGLE

class NiTextureStatsCommand : public NiSceneCommand
{
public:
    NiTextureStatsCommand();
    static NiSceneCommand* Create(NiSceneCommandInfo* pkInfo);
    ~NiTextureStatsCommand();
    bool Apply(NiSceneViewer* pkViewer);
    NiCommandID GetClassID();
    NiTPrimitiveArray<NiSceneCommandInfo*>* 
        GetDefaultCommandParamaterizations();
    static void Register();
protected:
    static NiCommandID ms_kID;
};

#endif