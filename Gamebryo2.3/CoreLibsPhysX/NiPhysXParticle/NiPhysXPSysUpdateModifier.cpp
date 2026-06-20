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
#include "NiPhysXParticlePCH.h"

#include "NiPhysXParticle.h"

NiImplementRTTI(NiPhysXPSysUpdateModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPhysXPSysUpdateModifier::NiPhysXPSysUpdateModifier(const char* pcName) :
    NiPSysModifier(pcName, ORDER_POSTPOSUPDATE)
{
}
//---------------------------------------------------------------------------
NiPhysXPSysUpdateModifier::NiPhysXPSysUpdateModifier()
{
}
//---------------------------------------------------------------------------
void NiPhysXPSysUpdateModifier::Update(float fTime, NiPSysData* pkData)
{
    // Actually add any particles created by previous modifiers to get their
    // last update time modified. They are not in the simulation, however.
    // That happens when the PhysXPSysSrc gets hold of them. The motivation
    // is one of information availability. This class does not have access
    // to the transformation information necessary to correctly set the
    // initial conditions of the PhysX actors.
    pkData->ResolveAddedParticles();

    NiParticleInfo* pkParticles = pkData->GetParticleInfo();

    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        // Set last update time.
        pkParticles[us].m_fLastUpdate = fTime;
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSysUpdateModifier::Initialize(NiPSysData* pkData,
    unsigned short usNewParticle)
{
    if (NiIsKindOf(NiPhysXPSysData, pkData))
    {
        NiPhysXPSysData* pkPhysXData = (NiPhysXPSysData*) pkData;
        pkPhysXData->SetActorAt(usNewParticle, 0);
    }
    else if (NiIsKindOf(NiPhysXMeshPSysData, pkData))
    {
        NiPhysXMeshPSysData* pkPhysXData = (NiPhysXMeshPSysData*) pkData;
        pkPhysXData->SetActorAt(usNewParticle, 0);
    }
    else
    {
        NIASSERT(!"NiPhysXPSysUpdateModifier::Initialize with wrong data\n");
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXPSysUpdateModifier);
//---------------------------------------------------------------------------
void NiPhysXPSysUpdateModifier::CopyMembers(NiPhysXPSysUpdateModifier* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifier::CopyMembers(pkDest, kCloning);
}

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPSysUpdateModifier);
//---------------------------------------------------------------------------
bool NiPhysXPSysUpdateModifier::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSysUpdateModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    m_uiOrder = ORDER_POSTPOSUPDATE;
}
//---------------------------------------------------------------------------
void NiPhysXPSysUpdateModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPSysUpdateModifier::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSysModifier::RegisterStreamables(kStream))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSysUpdateModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPSysUpdateModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPhysXPSysUpdateModifier::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiPSysModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPhysXPSysUpdateModifier::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
