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

#include "NiPhysXSharedData.h"

NiImplementRTTI(NiPhysXSharedData, NiSharedData);

//---------------------------------------------------------------------------
NiPhysXSharedData::NiPhysXSharedData()
{
    m_spScene = 0; 
}
//---------------------------------------------------------------------------
NiPhysXSharedData::~NiPhysXSharedData()
{
    m_kProps.RemoveAll();
    m_spScene = 0; 
}
//---------------------------------------------------------------------------
unsigned int NiPhysXSharedData::GetPropCount()
{
    return m_kProps.GetSize();
}
//---------------------------------------------------------------------------
NiPhysXProp* NiPhysXSharedData::GetPropAt(unsigned int uiIndex)
{
    return m_kProps.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
void NiPhysXSharedData::AddProp(NiPhysXProp* pkProp)
{
    m_kProps.Add(pkProp);
}
//---------------------------------------------------------------------------
void NiPhysXSharedData::RemoveProp(NiPhysXProp* pkProp)
{
    m_kProps.Remove(pkProp);
}
//---------------------------------------------------------------------------
NiPhysXScene* NiPhysXSharedData::GetScene()
{
    return m_spScene;
}
//---------------------------------------------------------------------------
void NiPhysXSharedData::SetScene(NiPhysXScene* pkScene)
{
    NIASSERT(pkScene);
    m_spScene = pkScene;
}
//---------------------------------------------------------------------------
