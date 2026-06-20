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

#include "NiPhysXMeshFluidSystem.h"
#include "NiPhysXFluidDest.h"

NiImplementRTTI(NiPhysXMeshFluidSystem, NiMeshParticleSystem);

//---------------------------------------------------------------------------
NiPhysXMeshFluidSystem::NiPhysXMeshFluidSystem(NxParticleData& kFluidData,
    unsigned short usMaxNumParticles, bool bHasColors, bool bHasRotations,
    bool bWorldSpace, unsigned int uiPoolSize, bool bFillPoolsOnLoad,
    NiPhysXProp* pkProp, bool bPhysXSpace) :
    NiMeshParticleSystem(NiNew NiPhysXMeshFluidData(kFluidData,
    usMaxNumParticles, bHasColors, bHasRotations, uiPoolSize,
    bFillPoolsOnLoad), bWorldSpace)
{
    m_bPhysXSpace = bPhysXSpace;

    // Set up destinations to get data into and out of PhysX
    if (pkProp)
    {
        AttachToProp(pkProp);
    }
}
//---------------------------------------------------------------------------
NiPhysXMeshFluidSystem::NiPhysXMeshFluidSystem(NiMeshParticleSystem* pkPSys,
    NiPhysXProp* pkProp, bool bPhysXSpace,
    NiTMap<NiAVObject*, NiPhysXActorDescPtr>& kActorMap)
    : NiMeshParticleSystem()
{
    // Clone to copy all the modifiers, controllers, shaders, etc.
    NiCloningProcess kCloning;
    kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;
    kCloning.m_cAppendChar = '%';

    pkPSys->CopyMembers(this, kCloning);
    pkPSys->ProcessClone(kCloning);
        
    // Set the stuff the clone didn't catch
    m_bPhysXSpace = bPhysXSpace;

    // Replace the model data.
    m_spModelData = NiNew NiPhysXMeshFluidData(
        NiDynamicCast(NiMeshPSysData, m_spModelData));

    //
    // Create a fluid descriptor and add it to the scene
    //
    // First get the scene descriptor.
    NIASSERT(pkProp->GetSnapshot() &&
        NiIsKindOf(NiPhysXFluidPropDesc, pkProp->GetSnapshot()));
    NiPhysXFluidPropDesc* pkPropDesc =
        (NiPhysXFluidPropDesc*)pkProp->GetSnapshot();

    // Create the Fluid descriptor
    NiPhysXFluidDescPtr spFluidDesc = NiNew NiPhysXFluidDesc();
    spFluidDesc->SetName(GetName());
    spFluidDesc->SetParticleSystem(this);
    
    NiPhysXFluidSystem::ConvertModifiers(
        this, pkProp, spFluidDesc, kActorMap);
    
    pkPropDesc->AddFluidDesc(spFluidDesc);

    // Set up the destinations to get data out of PhysX
    if (pkProp)
    {
        AttachToProp(pkProp);
    }
    else
    {
        m_spDestination = 0;
    }
}
//---------------------------------------------------------------------------
NiPhysXMeshFluidSystem::NiPhysXMeshFluidSystem()
{
}
//---------------------------------------------------------------------------
void NiPhysXMeshFluidSystem::SetModelData(NiGeometryData* pkModelData,
    NiPhysXProp* pkProp)
{
    // The model data must be of type NiPhysXFluidData.
    NIASSERT(NiIsKindOf(NiPhysXMeshFluidData, pkModelData));
    NiParticles::SetModelData(pkModelData);
    
    AttachToProp(pkProp);    
}
//---------------------------------------------------------------------------
void NiPhysXMeshFluidSystem::AttachToProp(NiPhysXProp* pkProp)
{
    m_spDestination = NiNew NiPhysXFluidDest(this);
    pkProp->AddDestination(m_spDestination);
}
//---------------------------------------------------------------------------
void NiPhysXMeshFluidSystem::DetachFromProp(NiPhysXProp* pkProp)
{
    pkProp->DeleteDestination(m_spDestination);
    m_spDestination = 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXMeshFluidSystem);
//---------------------------------------------------------------------------
bool NiPhysXMeshFluidSystem::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMeshFluidSystem::LoadBinary(NiStream& kStream)
{
    NiMeshParticleSystem::LoadBinary(kStream);

    m_spDestination = (NiPhysXFluidDest*)kStream.ResolveLinkID();

    NiBool bVal;
    NiStreamLoadBinary(kStream, bVal);
    m_bPhysXSpace = ( bVal != 0 );
}
//---------------------------------------------------------------------------
void NiPhysXMeshFluidSystem::LinkObject(NiStream& kStream)
{
    NiMeshParticleSystem::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXMeshFluidSystem::RegisterStreamables(NiStream& kStream)
{
    if (!NiMeshParticleSystem::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spDestination)
        m_spDestination->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMeshFluidSystem::SaveBinary(NiStream& kStream)
{
    NiMeshParticleSystem::SaveBinary(kStream);

    kStream.SaveLinkID(m_spDestination);

    NiStreamSaveBinary(kStream, NiBool(m_bPhysXSpace));
}
//---------------------------------------------------------------------------
bool NiPhysXMeshFluidSystem::IsEqual(NiObject* pkObject)
{
    if (!NiMeshParticleSystem::IsEqual(pkObject))
    {
        return false;
    }

    NiPhysXMeshFluidSystem* pkNewObject = (NiPhysXMeshFluidSystem*)pkObject;
    
    if (m_bPhysXSpace != pkNewObject->m_bPhysXSpace)
        return false;

    return true;
}
//---------------------------------------------------------------------------
