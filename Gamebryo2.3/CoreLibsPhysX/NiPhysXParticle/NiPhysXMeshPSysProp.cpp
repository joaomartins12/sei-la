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

NiImplementRTTI(NiPhysXMeshPSysProp, NiPhysXProp);    
        
//---------------------------------------------------------------------------
NiPhysXMeshPSysProp::NiPhysXMeshPSysProp()
{
}
//---------------------------------------------------------------------------
NiPhysXMeshPSysProp::~NiPhysXMeshPSysProp()
{
    m_kSystems.RemoveAll();
}
//---------------------------------------------------------------------------
unsigned int NiPhysXMeshPSysProp::GetSystemCount() const
{
    return m_kSystems.GetSize();
}
//---------------------------------------------------------------------------
NiPhysXMeshParticleSystem* NiPhysXMeshPSysProp::GetSystemAt(
    unsigned int uiIndex)
{
    return m_kSystems.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysProp::AddSystem(NiPhysXMeshParticleSystem* pkSystem)
{
    m_kSystems.AddFirstEmpty(pkSystem);
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysProp::RemoveSystem(NiPhysXMeshParticleSystem* pkSystem)
{
    m_kSystems.Remove(pkSystem);
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysProp::AttachSceneCallback(NiPhysXScene* pkScene)
{
    NiPhysXProp::AttachSceneCallback(pkScene);

    unsigned int uiSize = m_kSystems.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiPhysXMeshParticleSystem* pkSystem = m_kSystems.GetAt(ui);
        if (pkSystem)
            pkSystem->AttachToScene(pkScene);
    }
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysProp::DetachSceneCallback(NiPhysXScene* pkScene)
{
    unsigned int uiSize = m_kSystems.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiPhysXMeshParticleSystem* pkSystem = m_kSystems.GetAt(ui);
        if (pkSystem)
            pkSystem->DetachFromScene();
    }

    NiPhysXProp::DetachSceneCallback(pkScene);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXMeshPSysProp);
//---------------------------------------------------------------------------
void NiPhysXMeshPSysProp::CopyMembers(NiPhysXMeshPSysProp* pkDest,
    NiCloningProcess& kCloning)
{
    NiPhysXProp::CopyMembers(pkDest, kCloning);

    unsigned int uiSize = m_kSystems.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiPhysXMeshParticleSystem* pkSystem = m_kSystems.GetAt(ui);
        if (pkSystem)
        {
            NiPhysXMeshParticleSystem* pkNewSystem =
                (NiPhysXMeshParticleSystem*)
                pkSystem->CreateSharedClone(kCloning);
            pkDest->AddSystem(pkNewSystem);
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysProp::ProcessClone(NiCloningProcess& kCloning)
{
    NiPhysXProp::ProcessClone(kCloning);

    bool bCloned;
    NiObject* pkCloneObj = 0;
    bCloned = kCloning.m_pkCloneMap->GetAt(this, pkCloneObj);
    if (!bCloned)
        return;

    unsigned int uiSize = m_kSystems.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiPhysXMeshParticleSystem* pkSystem = m_kSystems.GetAt(ui);
        if (pkSystem)
            pkSystem->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXMeshPSysProp::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXProp::RegisterStreamables(kStream))
        return false;

    unsigned int uiSize = m_kSystems.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiPhysXMeshParticleSystem* pkSystem = m_kSystems.GetAt(ui);
        if (pkSystem)
            pkSystem->RegisterStreamables(kStream);
    }
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysProp::SaveBinary(NiStream& kStream)
{
    NiPhysXProp::SaveBinary(kStream);

    unsigned int uiSize = m_kSystems.GetSize();
    NiStreamSaveBinary(kStream, m_kSystems.GetEffectiveSize());
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiPhysXMeshParticleSystem* pkSystem = m_kSystems.GetAt(ui);
        if (pkSystem)
            kStream.SaveLinkID(pkSystem);
    }
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXMeshPSysProp);
//---------------------------------------------------------------------------
void NiPhysXMeshPSysProp::LoadBinary(NiStream& kStream)
{
    NiPhysXProp::LoadBinary(kStream);

    unsigned int uiSize;
    NiStreamLoadBinary(kStream, uiSize);
    m_kSystems.SetSize(uiSize);
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        m_kSystems.SetAt(ui,
            (NiPhysXMeshParticleSystem*)kStream.ResolveLinkID());
    }
}
//---------------------------------------------------------------------------
void NiPhysXMeshPSysProp::LinkObject(NiStream& kStream)
{
    NiPhysXProp::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXMeshPSysProp::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXMeshPSysProp::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXMeshPSysProp, pkObject));
    if(!NiPhysXProp::IsEqual(pkObject))
        return false;
        
    NiPhysXMeshPSysProp* pkThatObject = (NiPhysXMeshPSysProp*)pkObject;
        
    unsigned int uiEffectiveThis = m_kSystems.GetEffectiveSize();
    unsigned int uiEffectiveThat = pkThatObject->m_kSystems.GetEffectiveSize();
    if (uiEffectiveThis != uiEffectiveThat)
        return false;

    unsigned int uiSizeThis = m_kSystems.GetSize(); 
    unsigned int uiSizeThat = pkThatObject->m_kSystems.GetSize();
    unsigned int uiThis = 0;
    unsigned int uiThat = 0;
    bool bDone = false;
    do
    {
        NiPhysXMeshParticleSystem* pkSystemThis = NULL;
        while (uiThis < uiSizeThis &&
            !(pkSystemThis = m_kSystems.GetAt(uiThis)))
        {
            uiThis++;
        }

        NiPhysXMeshParticleSystem* pkSystemThat = NULL;
        while (uiThat < uiSizeThat &&
            !(pkSystemThat = pkThatObject->m_kSystems.GetAt(uiThat)))
        {
            uiThat++;
        }
        if (uiThis == uiSizeThis && uiThat == uiSizeThat)
            bDone = true; // All is good
        else if (uiThis == uiSizeThis && uiThat != uiSizeThat)
            return false; // More systems in That than in This
        else if (uiThis != uiSizeThis && uiThat == uiSizeThat)
            return false; // More systems in This than in That
        else if (!pkSystemThis->IsEqual(pkSystemThat))
            return false;
            
        uiThis++;
        uiThat++;
    } while (!bDone);

    return true;
}
//---------------------------------------------------------------------------
