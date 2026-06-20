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

#ifndef NIMAXPLATFORMMATERIALCLASSDESC_H
#define NIMAXPLATFORMMATERIALCLASSDESC_H

//---------------------------------------------------------------------------
class NiMAXPlatformMaterialClassDesc : public ClassDesc2 
{
public:
    int             IsPublic();
    void *          Create(BOOL loading);
    const TCHAR *   ClassName();
    SClass_ID       SuperClassID();
    Class_ID        ClassID();
    const TCHAR*    Category();

    const TCHAR*    InternalName();
    HINSTANCE       HInstance();
};

//---------------------------------------------------------------------------
//  Inline include
#include "NiMAXPlatformMaterialClassDesc.inl"
//---------------------------------------------------------------------------
#endif