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

#ifndef NIGEOMETRYSTATSCOMMAND_H
#define NIGEOMETRYSTATSCOMMAND_H

//#include "NiSceneCommand.h"
//#include "NiSceneViewer.h"

// Command Class:   GeometryStats
// Command ClassID: 12
// Command Purpose: To toggle the GeometryStats mode on or off
// Command String Syntax: GeometryStats() SINGLE

class NiGeometryStatsCommand : public NiSceneCommand
{
public:
    NiGeometryStatsCommand();
    static NiSceneCommand* Create(NiSceneCommandInfo* pkInfo);
    ~NiGeometryStatsCommand();
    bool Apply(NiSceneViewer* pkViewer);
    NiCommandID GetClassID();
    static void Register();
    
    NiTPrimitiveArray<NiSceneCommandInfo*>* 
        GetDefaultCommandParamaterizations();

protected:
    static NiCommandID ms_kID;
};

#endif