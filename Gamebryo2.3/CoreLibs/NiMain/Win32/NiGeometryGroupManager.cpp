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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiGeometryGroupManager.h"

NiGeometryGroupManager* NiGeometryGroupManager::ms_pkManager = NULL;

//---------------------------------------------------------------------------
NiGeometryGroupManager::NiGeometryGroupManager()
{
    ms_pkManager = this;
}
//---------------------------------------------------------------------------
NiGeometryGroupManager::~NiGeometryGroupManager()
{
    ms_pkManager = 0;
}
//---------------------------------------------------------------------------
NiGeometryGroup* NiGeometryGroupManager::CreateGroup(RepackingBehavior)
{
    // stub function
    return NULL;
}
//---------------------------------------------------------------------------
bool NiGeometryGroupManager::DestroyGroup(NiGeometryGroup*)
{ 
    // stub function
    return false;
}
//---------------------------------------------------------------------------
bool NiGeometryGroupManager::IsGroupEmpty(NiGeometryGroup*) const
{ 
    // stub function
    return true;
}
//---------------------------------------------------------------------------
bool NiGeometryGroupManager::AddObjectToGroup(NiGeometryGroup*, 
    NiGeometryData*, NiSkinInstance*)
{ 
    // stub function
    return false;
}
//---------------------------------------------------------------------------
bool NiGeometryGroupManager::RemoveObjectFromGroup(NiGeometryData*, 
    NiSkinInstance*)
{ 
    // stub function
    return false;
}
//---------------------------------------------------------------------------
bool NiGeometryGroupManager::AddObjectToGroup(NiGeometryGroup*, 
    NiScreenTexture*)
{ 
    // stub function
    return false;
}
//---------------------------------------------------------------------------
bool NiGeometryGroupManager::RemoveObjectFromGroup(NiScreenTexture*)
{ 
    // stub function
    return false;
}
//---------------------------------------------------------------------------
