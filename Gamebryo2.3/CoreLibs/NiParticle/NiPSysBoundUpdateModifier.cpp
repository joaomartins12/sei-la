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

#include "NiMeshPSysData.h"
#include "NiParticleSystem.h"
#include "NiPSysBoundUpdateModifier.h"
#include "NiPSysData.h"

NiImplementRTTI(NiPSysBoundUpdateModifier, NiPSysModifier);

//---------------------------------------------------------------------------
NiPSysBoundUpdateModifier::NiPSysBoundUpdateModifier(const char* pcName,
    short sUpdateSkip) : NiPSysModifier(pcName, ORDER_BOUNDUPDATE),
    m_sUpdateSkip(sUpdateSkip), m_usUpdateCount(0), m_pkSkipBounds(0)
{
    SetUpdateSkip(sUpdateSkip);
}
//---------------------------------------------------------------------------
NiPSysBoundUpdateModifier::~NiPSysBoundUpdateModifier()
{
    NiDelete [] m_pkSkipBounds;
}
//---------------------------------------------------------------------------
NiPSysBoundUpdateModifier::NiPSysBoundUpdateModifier() : m_sUpdateSkip(0),
    m_usUpdateCount(0), m_pkSkipBounds(0)
{
}
//---------------------------------------------------------------------------
void NiPSysBoundUpdateModifier::Update(float fTime, NiPSysData* pkData)
{
    unsigned short usNumParticles = pkData->GetNumParticles();

    // Check for no particles    
    if (usNumParticles <= 0)
    {
        pkData->GetBound().SetCenter(NiPoint3::ZERO);
        pkData->GetBound().SetRadius(0.0f);

        // Reset the Update Count
        m_usUpdateCount = 0;
        return;

    }

    // Check for a Mesh Bound Update
    if (NiIsKindOf(NiMeshPSysData, pkData))
    {
        UpdateMeshBounds(fTime, (NiMeshPSysData*)pkData);
        return;
    }

    // Check for Auto Setting the Update Skip
    if (m_sUpdateSkip == AUTO_SKIP_UPDATE)
    {
        SetUpdateSkip((pkData->GetMaxNumParticles() / 50) + 1);
    }

    // Set the Computed Skip based on the number of particles
    int iComputedSkip = NiMin(m_sUpdateSkip, (usNumParticles / 50) + 1);

    // A minimum of 1
    iComputedSkip = NiMax(iComputedSkip, 1);

    // compute the axis-aligned box containing the data
    const NiPoint3* pkVerts = pkData->GetVertices();
    
    float fMinX = pkVerts->x;
    float fMinY = pkVerts->y;
    float fMinZ = pkVerts->z;
    
    float fMaxX = fMinX;
    float fMaxY = fMinY;
    float fMaxZ = fMinZ;
    
    float* pkRadii = pkData->GetRadii();
    float* pkSizes = pkData->GetSizes();
    float fFinalSize;
    
    int i;
    for (i = m_usUpdateCount; i < usNumParticles; i += iComputedSkip) 
    {
        pkVerts = &pkData->GetVertices()[i];
        fFinalSize = pkRadii[i] * pkSizes[i];
        
        fMinX = NiMin(fMinX, pkVerts->x - fFinalSize);
        fMaxX = NiMax(fMaxX, pkVerts->x + fFinalSize);
        
        fMinY = NiMin(fMinY, pkVerts->y - fFinalSize);
        fMaxY = NiMax(fMaxY, pkVerts->y + fFinalSize);
        
        fMinZ = NiMin(fMinZ, pkVerts->z - fFinalSize);
        fMaxZ = NiMax(fMaxZ, pkVerts->z + fFinalSize);
    }
    
    NiPoint3 kMin(fMinX, fMinY, fMinZ);
    NiPoint3 kMax(fMaxX, fMaxY, fMaxZ);

    // Update the Skip Bounds
    m_pkSkipBounds[m_usUpdateCount].SetCenter(0.5f * (kMin + kMax));
    m_pkSkipBounds[m_usUpdateCount].SetRadius((kMax - kMin).Length() / 2.0f);
    
    // Zero the Unused bounds
    for (i = iComputedSkip; i < m_sUpdateSkip; i++)
    {
        m_pkSkipBounds[i].SetCenter(NiPoint3::ZERO);
        m_pkSkipBounds[i].SetRadius(0.0f);
    }

    // Merge the SkipBounds
    NiBound kTotal;
    kTotal.SetCenter(m_pkSkipBounds[m_usUpdateCount].GetCenter());
    kTotal.SetRadius(m_pkSkipBounds[m_usUpdateCount].GetRadius());

    for(i = 1; i < iComputedSkip; i++)
    {
        if (m_pkSkipBounds[i].GetRadius() != 0.0f)
            kTotal.Merge(&m_pkSkipBounds[i]);
    }

    // sphere center is the axis-aligned box center
    pkData->GetBound().SetCenter(kTotal.GetCenter());
    pkData->GetBound().SetRadius(kTotal.GetRadius());
    
    // Guarantee we have some type of bound    
    if (pkData->GetBound().GetRadius() == 0.0f)
    {
        pkData->GetBound().SetRadius(pkData->GetRadii()[0] *
            pkData->GetSizes()[0]);
    }
        
    if (++m_usUpdateCount >= iComputedSkip)
    {
        m_usUpdateCount = 0;
    }
}
//---------------------------------------------------------------------------
void NiPSysBoundUpdateModifier::UpdateMeshBounds(float fTime, 
    NiMeshPSysData* pkMeshData)
{
    unsigned short usNumParticles = pkMeshData->GetNumParticles();

    NiNode* pkContainer = pkMeshData->GetContainerNode();

    // Check for Auto Setting the Update Skip
    if (m_sUpdateSkip == AUTO_SKIP_UPDATE)
    {
        SetUpdateSkip((pkMeshData->GetMaxNumParticles() / 20) + 1);
    }

    // Set the Computed Skip based on the number of particles
    int iComputedSkip = NiMin(m_sUpdateSkip, (usNumParticles / 20) + 1);

    // A minimum of 1
    iComputedSkip = NiMax(iComputedSkip, 1);
    
    NiBound kTotal = pkContainer->GetAt(0)->GetWorldBound();

    int i;
    for (i = m_usUpdateCount; i < usNumParticles; i += iComputedSkip) 
    {
        kTotal.Merge(&pkContainer->GetAt(i)->GetWorldBound());
    }

    // Update the Skip Bounds
    m_pkSkipBounds[m_usUpdateCount].SetCenter(kTotal.GetCenter());
    m_pkSkipBounds[m_usUpdateCount].SetRadius(kTotal.GetRadius());

    // Zero the Unused bounds
    for (i = iComputedSkip; i < m_sUpdateSkip; i++)
    {
        m_pkSkipBounds[i].SetCenter(NiPoint3::ZERO);
        m_pkSkipBounds[i].SetRadius(0.0f);
    }

    // Merge the SkipBounds
    for(i = 1; i < iComputedSkip; i++)
    {
        if (m_pkSkipBounds[i].GetRadius() != 0.0f)
            kTotal.Merge(&m_pkSkipBounds[i]);
    }

    // Guarantee we have some type of bound    
    if (kTotal.GetRadius() == 0.0f)
    {
        kTotal.SetRadius(pkMeshData->GetRadii()[0] *
            pkMeshData->GetSizes()[0]);
    }

    // Update the Container Nodes World Bound
    pkContainer->SetWorldBound(kTotal);
    
    if (++m_usUpdateCount >= iComputedSkip)
    {
        m_usUpdateCount = 0;
    }
}
//---------------------------------------------------------------------------
void NiPSysBoundUpdateModifier::SetSystemPointer(NiParticleSystem* pkTarget)
{
    if (m_pkTarget)
    {
        m_pkTarget->SetDynamicBounds(false);
    }
    if (pkTarget)
    {
        pkTarget->SetDynamicBounds(true);
    }

    NiPSysModifier::SetSystemPointer(pkTarget);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysBoundUpdateModifier);
//---------------------------------------------------------------------------
void NiPSysBoundUpdateModifier::CopyMembers(NiPSysBoundUpdateModifier* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifier::CopyMembers(pkDest, kCloning);

    pkDest->SetUpdateSkip(m_sUpdateSkip);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysBoundUpdateModifier);
//---------------------------------------------------------------------------
void NiPSysBoundUpdateModifier::LoadBinary(NiStream& kStream)
{
    NiPSysModifier::LoadBinary(kStream);

    // Update Skip didn't previously allow for a AUTO_SKIP_UPDATE value
    if (kStream.GetFileVersion() <= NiStream::GetVersion(10, 1, 0, 100))
    {
        unsigned short usUpdateSkip;
        NiStreamLoadBinary(kStream, usUpdateSkip);

        SetUpdateSkip(usUpdateSkip);
    }
    else
    {
        NiStreamLoadBinary(kStream, m_sUpdateSkip);
        SetUpdateSkip(m_sUpdateSkip);
    }
}
//---------------------------------------------------------------------------
void NiPSysBoundUpdateModifier::LinkObject(NiStream& kStream)
{
    NiPSysModifier::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysBoundUpdateModifier::RegisterStreamables(NiStream& kStream)
{
    return NiPSysModifier::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysBoundUpdateModifier::SaveBinary(NiStream& kStream)
{
    NiPSysModifier::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_sUpdateSkip);
}
//---------------------------------------------------------------------------
bool NiPSysBoundUpdateModifier::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifier::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysBoundUpdateModifier* pkDest = (NiPSysBoundUpdateModifier*) pkObject;

    if (pkDest->m_sUpdateSkip != m_sUpdateSkip)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysBoundUpdateModifier::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiPSysModifier::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysBoundUpdateModifier::ms_RTTI
        .GetName()));

    pkStrings->Add(NiGetViewerString("Update Skip", m_sUpdateSkip));
}
//---------------------------------------------------------------------------
