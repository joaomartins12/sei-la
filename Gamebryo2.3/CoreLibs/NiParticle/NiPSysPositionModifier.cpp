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
#include "NiParticlePCH.h"

#include "NiPSysPositionModifier.h"
#include "NiParticleSystem.h"

NiImplementRTTI(NiPSysPositionModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysPositionModifier::NiPSysPositionModifier(const char* pcName) :
    NiPSysModifier(pcName, ORDER_POSUPDATE)
{
}
//---------------------------------------------------------------------------
NiPSysPositionModifier::NiPSysPositionModifier()
{
}
//---------------------------------------------------------------------------
void NiPSysPositionModifier::Update(float fTime, NiPSysData* pkData)
{
    // Actually add any particles created by previous modifiers so their
    // positions can be updated.
    pkData->ResolveAddedParticles();

    NiParticleInfo* pkParticles = pkData->GetParticleInfo();

    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        NiParticleInfo* pkCurrentParticle = &pkParticles[us];

        // Update position based on velocity.
        NiPoint3 kDeltaPosition = pkCurrentParticle->m_kVelocity * 
            (fTime - pkCurrentParticle->m_fLastUpdate);
        pkData->GetVertices()[us] += kDeltaPosition;

        // Set last update time.
        pkCurrentParticle->m_fLastUpdate = fTime;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysPositionModifier);
//---------------------------------------------------------------------------
void NiPSysPositionModifier::CopyMembers(NiPSysPositionModifier* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifier::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysPositionModifier);
//---------------------------------------------------------------------------
void NiPSysPositionModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSysPositionModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysPositionModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysPositionModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysPositionModifier::IsEqual(NiObject* pkObject)
{
    return NiPSysModifier::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysPositionModifier::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysPositionModifier::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
