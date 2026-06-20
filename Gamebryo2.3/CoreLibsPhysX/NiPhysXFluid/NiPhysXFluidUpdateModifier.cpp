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

#include "NiPhysXFluidUpdateModifier.h"
#include "NiPhysXFluidSystem.h"
#include "fluids/NxFluid.h"

NiImplementRTTI(NiPhysXFluidUpdateModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPhysXFluidUpdateModifier::NiPhysXFluidUpdateModifier(const char* pcName) :
    NiPSysModifier(pcName, ORDER_POSTPOSUPDATE)
{
}
//---------------------------------------------------------------------------
NiPhysXFluidUpdateModifier::NiPhysXFluidUpdateModifier()
{
}
//---------------------------------------------------------------------------
void NiPhysXFluidUpdateModifier::Update(float fTime, NiPSysData* pkData)
{
    // Actually add any particles created by previous modifiers to get their
    // last update time modified.
    pkData->ResolveAddedParticles();

    NiParticleInfo* pkParticles = pkData->GetParticleInfo();

    for (unsigned short us = 0; us < pkData->GetNumParticles(); us++)
    {
        // Set last update time.
        pkParticles[us].m_fLastUpdate = fTime;
    }
}
//---------------------------------------------------------------------------
void NiPhysXFluidUpdateModifier::Initialize(NiPSysData* pkData,
    unsigned short usNewParticle)
{
    NxParticleData kFluidData;
    bool bPhysXSpace;
    NiTransform kPhysXToFluid;
    if (NiIsKindOf(NiPhysXFluidData, pkData))
    {
        kFluidData = ((NiPhysXFluidData*)pkData)->GetFluidData();
        bPhysXSpace = ((NiPhysXFluidSystem*)m_pkTarget)->GetPhysXSpace();
        if (!bPhysXSpace)
        {
            kPhysXToFluid =
                ((NiPhysXFluidSystem*)m_pkTarget)->GetPhysXToFluid();
        }
    }
    else if (NiIsKindOf(NiPhysXMeshFluidData, pkData))
    {
        kFluidData = ((NiPhysXMeshFluidData*)pkData)->GetFluidData();
        bPhysXSpace = ((NiPhysXMeshFluidSystem*)m_pkTarget)->GetPhysXSpace();
        if (!bPhysXSpace)
        {
            kPhysXToFluid =
                ((NiPhysXMeshFluidSystem*)m_pkTarget)->GetPhysXToFluid();
        }
    }
    else
    {
        NIASSERT(false && "NiPhysXFluidUpdateModifier: Wrong type\n");
    }

    if (!kFluidData.bufferPos)
        return;
  
    NxF32* pfPosn = kFluidData.bufferPos +
        (usNewParticle * (kFluidData.bufferPosByteStride / sizeof(NxF32)));
    NxF32* pfVelocity = kFluidData.bufferVel +
        (usNewParticle * (kFluidData.bufferVelByteStride / sizeof(NxF32)));
    
    if (bPhysXSpace)
    {
        NiPoint3 kPosn;
        kPosn.x = *pfPosn;
        kPosn.y = *(pfPosn + 1);
        kPosn.z = *(pfPosn + 2);

        pkData->GetVertices()[usNewParticle] = kPosn;

        if (kFluidData.bufferVel)
        {
            NiPoint3 kVelocity;
            kVelocity.x = *pfVelocity;
            kVelocity.y = *(pfVelocity + 1);
            kVelocity.z = *(pfVelocity + 2);
            
            pkData->GetParticleInfo()[usNewParticle].m_kVelocity = kVelocity;
        }
        
        return;
    }

    NiPoint3 kPosn;
    kPosn.x = *pfPosn;
    kPosn.y = *(pfPosn + 1);
    kPosn.z = *(pfPosn + 2);

    pkData->GetVertices()[usNewParticle] = kPhysXToFluid * kPosn;              

    if (kFluidData.bufferVel)
    {
        NiPoint3 kVelocity;
        kVelocity.x = *pfVelocity;
        kVelocity.y = *(pfVelocity + 1);
        kVelocity.z = *(pfVelocity + 2);
        
        pkData->GetParticleInfo()[usNewParticle].m_kVelocity =
            kPhysXToFluid.m_Rotate * kVelocity;    
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXFluidUpdateModifier);
//---------------------------------------------------------------------------
bool NiPhysXFluidUpdateModifier::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXFluidUpdateModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    m_uiOrder = ORDER_POSTPOSUPDATE;
}
//---------------------------------------------------------------------------
void NiPhysXFluidUpdateModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXFluidUpdateModifier::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSysModifier::RegisterStreamables(kStream))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXFluidUpdateModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXFluidUpdateModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
        return false;

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPhysXFluidUpdateModifier::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiPSysModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPhysXFluidUpdateModifier::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
