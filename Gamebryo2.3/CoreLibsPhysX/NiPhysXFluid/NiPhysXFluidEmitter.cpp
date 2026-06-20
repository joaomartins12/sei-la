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

#include "NiPhysXFluidEmitter.h"
#include "NiPhysXFluidSystem.h"

NiImplementRTTI(NiPhysXFluidEmitter, NiPSysEmitter);

//---------------------------------------------------------------------------
NiPhysXFluidEmitter::NiPhysXFluidEmitter(const char* pcName,
    NiColorA kInitialColor, float fInitialRadius, float fRadiusVar) :
    NiPSysEmitter(pcName, ORDER_EMITTER, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, kInitialColor, fInitialRadius, NI_INFINITY, 0.0f,
    fRadiusVar)
{
}
//---------------------------------------------------------------------------
NiPhysXFluidEmitter::NiPhysXFluidEmitter()
{
}
//---------------------------------------------------------------------------
void NiPhysXFluidEmitter::Update(float fTime, NiPSysData* pkData)
{
    // Unlike other Gamebryo emitters, this on does its emission
    // now.
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
        NIASSERT(false && "NiPhysXFluidEmitter: Wrong type\n");
    }

    if (!kFluidData.bufferPos)
        return;

    if (pkData->GetNumParticles() < *(kFluidData.numParticlesPtr))
    {
        unsigned short usNumNew =
            *(kFluidData.numParticlesPtr) - pkData->GetNumParticles();
        float* pfAges = NiAlloc(float, usNumNew);
        for (unsigned short us = 0; us < usNumNew; us++)
            pfAges[us] = 0.0f;
        EmitParticles(fTime, usNumNew, pfAges);
        NiFree(pfAges);         
    }
}
//---------------------------------------------------------------------------
void NiPhysXFluidEmitter::ComputeInitialPositionAndVelocity(
    NiPoint3& kPosition, NiPoint3& kVelocity)
{
    // Does nothing. We don't have the index of the particle to know
    // what to set it to. We allow the Initialize function of
    // NiPhysXFluidUpdateModifier to do the job.
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXFluidEmitter);
//---------------------------------------------------------------------------
bool NiPhysXFluidEmitter::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXFluidEmitter::LoadBinary(NiStream& kStream)
{
    NiPSysEmitter::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXFluidEmitter::LinkObject(NiStream& kStream)
{
    NiPSysEmitter::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXFluidEmitter::RegisterStreamables(NiStream& kStream)
{
    return NiPSysEmitter::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPhysXFluidEmitter::SaveBinary(NiStream& kStream)
{
    NiPSysEmitter::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXFluidEmitter::IsEqual(NiObject* pkObject)
{
    if (!NiPSysEmitter::IsEqual(pkObject))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPhysXFluidEmitter::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysEmitter::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPhysXFluidEmitter::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
