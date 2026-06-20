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

NiImplementRTTI(NiPhysXPSysProp, NiPhysXProp);    
        
//---------------------------------------------------------------------------
NiPhysXPSysProp::NiPhysXPSysProp()
{
}
//---------------------------------------------------------------------------
NiPhysXPSysProp::~NiPhysXPSysProp()
{
    m_kSystems.RemoveAll();
}
//---------------------------------------------------------------------------
unsigned int NiPhysXPSysProp::GetSystemCount() const
{
    return m_kSystems.GetSize();
}
//---------------------------------------------------------------------------
NiPhysXParticleSystem* NiPhysXPSysProp::GetSystemAt(unsigned int uiIndex)
{
    return m_kSystems.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
void NiPhysXPSysProp::AddSystem(NiPhysXParticleSystem* pkSystem)
{
    m_kSystems.AddFirstEmpty(pkSystem);
}
//---------------------------------------------------------------------------
void NiPhysXPSysProp::RemoveSystem(NiPhysXParticleSystem* pkSystem)
{
    m_kSystems.Remove(pkSystem);
}
//---------------------------------------------------------------------------
void NiPhysXPSysProp::AttachSceneCallback(NiPhysXScene* pkScene)
{
    NiPhysXProp::AttachSceneCallback(pkScene);

    unsigned int uiSize = m_kSystems.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiPhysXParticleSystem* pkSystem = m_kSystems.GetAt(ui);
        if (pkSystem)
            pkSystem->AttachToScene(pkScene);
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSysProp::DetachSceneCallback(NiPhysXScene* pkScene)
{
    unsigned int uiSize = m_kSystems.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiPhysXParticleSystem* pkSystem = m_kSystems.GetAt(ui);
        if (pkSystem)
            pkSystem->DetachFromScene();
    }

    NiPhysXProp::DetachSceneCallback(pkScene);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning Functions
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPhysXPSysProp);
//---------------------------------------------------------------------------
void NiPhysXPSysProp::CopyMembers(NiPhysXPSysProp* pkDest,
    NiCloningProcess& kCloning)
{
    NiPhysXProp::CopyMembers(pkDest, kCloning);

    unsigned int uiSize = m_kSystems.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiPhysXParticleSystem* pkSystem = m_kSystems.GetAt(ui);
        if (pkSystem)
        {
            NiPhysXParticleSystem* pkNewSystem =
                (NiPhysXParticleSystem*)
                pkSystem->CreateSharedClone(kCloning);
            pkDest->AddSystem(pkNewSystem);
        }
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSysProp::ProcessClone(NiCloningProcess& kCloning)
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
        NiPhysXParticleSystem* pkSystem = m_kSystems.GetAt(ui);
        if (pkSystem)
            pkSystem->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming Functions
//---------------------------------------------------------------------------
bool NiPhysXPSysProp::RegisterStreamables(NiStream& kStream)
{
    if (!NiPhysXProp::RegisterStreamables(kStream))
        return false;

    unsigned int uiSize = m_kSystems.GetSize();
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiPhysXParticleSystem* pkSystem = m_kSystems.GetAt(ui);
        if (pkSystem)
            pkSystem->RegisterStreamables(kStream);
    }
    
    return true;
}
//---------------------------------------------------------------------------
void NiPhysXPSysProp::SaveBinary(NiStream& kStream)
{
    NiPhysXProp::SaveBinary(kStream);

    unsigned int uiSize = m_kSystems.GetSize();
    NiStreamSaveBinary(kStream, m_kSystems.GetEffectiveSize());
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        NiPhysXParticleSystem* pkSystem = m_kSystems.GetAt(ui);
        if (pkSystem)
            kStream.SaveLinkID(pkSystem);
    }
}
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPhysXPSysProp);
//---------------------------------------------------------------------------
void NiPhysXPSysProp::LoadBinary(NiStream& kStream)
{
    NiPhysXProp::LoadBinary(kStream);

    unsigned int uiSize;
    NiStreamLoadBinary(kStream, uiSize);
    m_kSystems.SetSize(uiSize);
    for (unsigned int ui = 0; ui < uiSize; ui++)
    {
        m_kSystems.SetAt(ui, (NiPhysXParticleSystem*)kStream.ResolveLinkID());
    }
}
//---------------------------------------------------------------------------
void NiPhysXPSysProp::LinkObject(NiStream& kStream)
{
    NiPhysXProp::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPhysXPSysProp::StreamCanSkip()
{
    return true;
}
//---------------------------------------------------------------------------
bool NiPhysXPSysProp::IsEqual(NiObject* pkObject)
{
    NIASSERT(NiIsKindOf(NiPhysXPSysProp, pkObject));
    if(!NiPhysXProp::IsEqual(pkObject))
        return false;
        
    NiPhysXPSysProp* pkThatObject = (NiPhysXPSysProp*)pkObject;
        
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
        NiPhysXParticleSystem* pkSystemThis = NULL;
        while (uiThis < uiSizeThis &&
            !(pkSystemThis = m_kSystems.GetAt(uiThis)))
        {
            uiThis++;
        }

        NiPhysXParticleSystem* pkSystemThat = NULL;
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
