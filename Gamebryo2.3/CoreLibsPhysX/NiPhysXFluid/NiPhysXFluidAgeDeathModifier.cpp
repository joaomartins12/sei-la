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
#include "NiPhysXFluidPCH.h"

#include "NiPhysXFluidAgeDeathModifier.h"
#include "NiParticleSystem.h"
#include "NiPSysSpawnModifier.h"

NiImplementRTTI(NiPhysXFluidAgeDeathModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPhysXFluidAgeDeathModifier::NiPhysXFluidAgeDeathModifier(const char* pcName)
    : NiPSysModifier(pcName, ORDER_KILLOLDPARTICLES)
{
}
//---------------------------------------------------------------------------
NiPhysXFluidAgeDeathModifier::NiPhysXFluidAgeDeathModifier()
{
}
//---------------------------------------------------------------------------
void NiPhysXFluidAgeDeathModifier::Update(float fTime, NiPSysData* pkData)
{
    // Code related to spawning is similar to that in NiPSySAgeDeathModifier
    // from the NiParticles library. Changes here may need to be
    // reflected there.

    NxParticleData kFluidData; 
    if (NiIsKindOf(NiPhysXFluidData, pkData))
    {
        kFluidData = ((NiPhysXFluidData*)pkData)->GetFluidData();
    }
    else if (NiIsKindOf(NiPhysXMeshFluidData, pkData))
    {
        kFluidData = ((NiPhysXMeshFluidData*)pkData)->GetFluidData();
    }
    else
    {
        NIASSERT(false && "NiPhysXFluidAgeDeathModifier: Wrong type\n");
    }

    if (!kFluidData.bufferPos)
        return;

    // All particles have been emitted by now, so we can iterate over
    // number of particles in the fluid data, updating ages.
    for (unsigned short us = 0; us < *(kFluidData.numParticlesPtr); us++)
    {
        // Get particle.
        NiParticleInfo* pkCurrentParticle = &pkData->GetParticleInfo()[us];

        // Update age.
        pkCurrentParticle->m_fAge += fTime - pkCurrentParticle->m_fLastUpdate;
    }

    // Iterate over particles backwards to avoid removing already dead
    // particles.
    for (unsigned short us = pkData->GetNumParticles();
        us > *(kFluidData.numParticlesPtr); us--)
    {
        pkData->RemoveParticle(us - 1);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXFluidAgeDeathModifier);
//---------------------------------------------------------------------------
bool NiPhysXFluidAgeDeathModifier::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXFluidAgeDeathModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXFluidAgeDeathModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXFluidAgeDeathModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXFluidAgeDeathModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXFluidAgeDeathModifier::IsEqual(NiObject* pkObject)
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
void NiPhysXFluidAgeDeathModifier::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSysModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPhysXFluidAgeDeathModifier::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
