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

#ifndef NISCREENCONSOLECOMMAND_H
#define NISCREENCONSOLECOMMAND_H

//#include "NiSceneCommand.h"
//#include "NiSceneViewer.h"

// Command Class:   ScreenConsole
// Command ClassID: 14
// Command Purpose: To toggle the ScreenConsole mode on or off
// Command String Syntax: ScreenConsole() SINGLE

class NiScreenConsoleCommand : public NiSceneCommand
{
public:
    NiScreenConsoleCommand();
    static NiSceneCommand* Create(NiSceneCommandInfo* pkInfo);
    ~NiScreenConsoleCommand();
    bool Apply(NiSceneViewer* pkViewer);
    NiCommandID GetClassID();
    NiTPrimitiveArray<NiSceneCommandInfo*>* 
        GetDefaultCommandParamaterizations();
    static void Register();

protected:
    static NiCommandID ms_kID;
};

#endif //NISCREENCONSOLECOMMAND_H