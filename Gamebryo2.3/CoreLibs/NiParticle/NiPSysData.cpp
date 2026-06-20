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

#include "NiParticleMetrics.h"
#include "NiPSysData.h"
#include <NiBool.h>

NiImplementRTTI(NiPSysData, NiParticlesData);

const unsigned short NiPSysData::INVALID_PARTICLE = (unsigned short) -1;

//---------------------------------------------------------------------------
NiPSysData::NiPSysData(unsigned short usMaxNumParticles, bool bHasColors,
    bool bHasRotations) : NiParticlesData(
    usMaxNumParticles, NULL, NULL, NULL, NULL, NULL, NULL),
    m_pkParticleInfo(NULL), m_pfRotationSpeeds(NULL),
    m_usNumAddedParticles(0), m_usAddedParticlesBase(0)
{
    // Allocate data arrays here rather than having them passed in.

    NIASSERT(usMaxNumParticles > 0);

    m_pkVertex = NiNew NiPoint3[usMaxNumParticles];
    memset(m_pkVertex, 0, usMaxNumParticles * sizeof(*m_pkVertex));

    if (bHasColors)
    {
        m_pkColor = NiNew NiColorA[usMaxNumParticles];
        memset(m_pkColor, 0, usMaxNumParticles * sizeof(*m_pkColor));
    }
    if (bHasRotations)
    {
        m_pfRotationAngles = NiAlloc(float, usMaxNumParticles);
        memset(m_pfRotationAngles, 0, usMaxNumParticles *
            sizeof(*m_pfRotationAngles));
        m_pfRotationSpeeds = NiAlloc(float, usMaxNumParticles);
        memset(m_pfRotationSpeeds, 0, usMaxNumParticles *
            sizeof(*m_pfRotationSpeeds));
    }

    m_pkParticleInfo = NiNew NiParticleInfo[usMaxNumParticles];

    SetActiveVertexCount(0);
}
//---------------------------------------------------------------------------
NiPSysData::NiPSysData() : m_pkParticleInfo(NULL), m_pfRotationSpeeds(NULL),
    m_usNumAddedParticles(0), m_usAddedParticlesBase(0)
{
}
//---------------------------------------------------------------------------
NiPSysData::~NiPSysData()
{
    NiDelete[] m_pkParticleInfo;
    NiFree(m_pfRotationSpeeds);
}
//---------------------------------------------------------------------------
void NiPSysData::Replace(NiParticleInfo* pkParticleInfo)
{
    NiDelete[] m_pkParticleInfo;
    m_pkParticleInfo = pkParticleInfo;
}
//---------------------------------------------------------------------------
void NiPSysData::ReplaceRotationSpeeds(float* pfRotationSpeeds)
{
    NiFree(m_pfRotationSpeeds);
    m_pfRotationSpeeds = pfRotationSpeeds;
}
//---------------------------------------------------------------------------
void NiPSysData::RemoveParticle(unsigned short usParticle)
{
    unsigned short usLastParticle = GetNumParticles() - 1;

    NiParticlesData::RemoveParticle(usParticle);

    if (usParticle != usLastParticle)
    {
        NIASSERT(m_pkParticleInfo);
        m_pkParticleInfo[usParticle] = m_pkParticleInfo[usLastParticle];
        if (m_pfRotationSpeeds)
        {
            m_pfRotationSpeeds[usParticle] =
                m_pfRotationSpeeds[usLastParticle];
        }
    }
}
//---------------------------------------------------------------------------
void NiPSysData::ResolveAddedParticles()
{
    if (m_usNumAddedParticles > 0)
    {
        NIMETRICS_PARTICLE_ADDVALUE(PARTICLES_SPAWNED, m_usNumAddedParticles);
    }

    if (m_usActiveVertices < m_usAddedParticlesBase)
    {
        NIMETRICS_PARTICLE_ADDVALUE(PARTICLES_DESTROYED, 
            m_usAddedParticlesBase - m_usActiveVertices);

        // Some particles were removed; fill array so that it is packed.
        unsigned short usIndex = m_usActiveVertices;
        m_usActiveVertices = m_usAddedParticlesBase + m_usNumAddedParticles;
        while (usIndex < m_usAddedParticlesBase &&
            m_usActiveVertices > m_usAddedParticlesBase)
        {
            RemoveParticle(usIndex++);
        }

        // If more particles were removed than were added, adjust number
        // of particles appropriately.
        if (usIndex < m_usAddedParticlesBase)
        {
            m_usActiveVertices = usIndex;
        }
    }
    else
    {
        // m_usActive should never be greater than m_usAddedParticlesBase.
        NIASSERT(m_usActiveVertices == m_usAddedParticlesBase);
        m_usActiveVertices += m_usNumAddedParticles;
    }

    m_usNumAddedParticles = 0;
    m_usAddedParticlesBase = m_usActiveVertices;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysData);
//---------------------------------------------------------------------------
void NiPSysData::CopyMembers(NiPSysData* pkDest, NiCloningProcess& kCloning)
{
    NiParticlesData::CopyMembers(pkDest, kCloning);

    // NiGeometryData does not implement this function, so copy its data here.
    NiPoint3* pkVertex = NiNew NiPoint3[m_usVertices];
    unsigned int uiByteSize = m_usVertices * sizeof(NiPoint3);
    NiMemcpy(pkVertex, m_pkVertex, uiByteSize);
    NiPoint3* pkNormal = NULL;
    if (m_pkNormal)
    {
        pkNormal = NiNew NiPoint3[m_usVertices];
        NiMemcpy(pkNormal, m_pkNormal, uiByteSize);
    }
    NiColorA* pkColor = NULL;
    if (m_pkColor)
    {
        pkColor = NiNew NiColorA[m_usVertices];
        uiByteSize = m_usVertices * sizeof(*pkColor);
        NiMemcpy(pkColor, m_pkColor, uiByteSize);
    }
    ((NiGeometryData*) pkDest)->Replace(m_usVertices, pkVertex, pkNormal,
        pkColor, NULL, 0, NiGeometryData::NBT_METHOD_NONE);

    // NiParticlesData does not implement this function, so copy its data
    // here.
    NiQuaternion* pkRotations = NULL;
    if (m_pkRotations)
    {
        pkRotations = NiNew NiQuaternion[m_usVertices];
        uiByteSize = m_usVertices * sizeof(*pkRotations);
        NiMemcpy(pkRotations, m_pkRotations, uiByteSize);
    }
    ((NiParticlesData*) pkDest)->ReplaceRotations(pkRotations);
    float* pfSizes = NiAlloc(float, m_usVertices);
    uiByteSize = m_usVertices * sizeof(float);
    NiMemcpy(pfSizes, m_pfSizes, uiByteSize);

    ((NiParticlesData*) pkDest)->ReplaceSizes(pfSizes);
    float* pfRadii = NiAlloc(float, m_usVertices);   
    NiMemcpy(pfRadii, m_pfRadii, uiByteSize);

    ((NiParticlesData*) pkDest)->ReplaceRadii(pfRadii);
    pkDest->m_usActiveVertices = m_usActiveVertices;
    if (m_pfRotationAngles)
    {
        float* pfAngles = NiAlloc(float, m_usVertices);
        NiMemcpy(pfAngles, m_pfRotationAngles, uiByteSize);
        ((NiParticlesData*) pkDest)->ReplaceRotationAngles(pfAngles);
    }
    if (m_pkRotationAxes)
    {
        NiPoint3* pfAxes = NiNew NiPoint3[m_usVertices];
        uiByteSize = m_usVertices * sizeof(*pfAxes);
        NiMemcpy(pfAxes, m_pkRotationAxes, uiByteSize);
        ((NiParticlesData*) pkDest)->ReplaceRotationAxes(pfAxes);
    }

    // Copy data for NiPSysData here.
    NiParticleInfo* pkParticleInfo = NULL;
    if (m_pkParticleInfo)
    {
        pkParticleInfo = NiNew NiParticleInfo[m_usVertices];
        uiByteSize = m_usVertices * sizeof(*pkParticleInfo);
        NiMemcpy(pkParticleInfo, m_pkParticleInfo, uiByteSize);
    }
    pkDest->Replace(pkParticleInfo);
    float* pfRotationSpeeds = NULL;
    if (m_pfRotationSpeeds)
    {
        pfRotationSpeeds = NiAlloc(float, m_usVertices);
        uiByteSize = m_usVertices * sizeof(*pfRotationSpeeds);
        NiMemcpy(pfRotationSpeeds, m_pfRotationSpeeds, uiByteSize);
    }
    pkDest->ReplaceRotationSpeeds(pfRotationSpeeds);
    pkDest->m_usNumAddedParticles = m_usNumAddedParticles;
    pkDest->m_usAddedParticlesBase = m_usAddedParticlesBase;
    pkDest->m_kBound = m_kBound;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysData);
//---------------------------------------------------------------------------
void NiPSysData::LoadBinary(NiStream& kStream)
{
    NiParticlesData::LoadBinary(kStream);

    m_pkParticleInfo = NiNew NiParticleInfo[m_usVertices];
    for (unsigned short us = 0; us < m_usVertices; us++)
    {
        m_pkParticleInfo[us].LoadBinary(kStream);
    }

    if (kStream.GetFileVersion() >= NiStream::GetVersion(20, 0, 0, 2))
    {
        NiBool bHasRotationSpeeds;
        NiStreamLoadBinary(kStream, bHasRotationSpeeds);
        if (bHasRotationSpeeds)
        {
            m_pfRotationSpeeds = NiAlloc(float, m_usVertices);
            NiStreamLoadBinary(kStream, m_pfRotationSpeeds, m_usVertices);
        }
    }
    else
    {
        if (m_pfRotationAngles)
        {
            m_pfRotationSpeeds = NiAlloc(float, m_usVertices);
            memset(m_pfRotationSpeeds, 0, m_usVertices *
                sizeof(m_pfRotationSpeeds[0]));
        }
    }

    NiStreamLoadBinary(kStream, m_usNumAddedParticles);
    NiStreamLoadBinary(kStream, m_usAddedParticlesBase);
}
//---------------------------------------------------------------------------
void NiPSysData::LinkObject(NiStream& kStream)
{
    NiParticlesData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSysData::RegisterStreamables(NiStream& kStream)
{
    return NiParticlesData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSysData::SaveBinary(NiStream& kStream)
{
    NiParticlesData::SaveBinary(kStream);

    NIASSERT(m_pkParticleInfo);
    for (unsigned short us = 0; us < m_usVertices; us++)
    {
        m_pkParticleInfo[us].SaveBinary(kStream);
    }

    NiBool bHasRotationSpeeds = (m_pfRotationSpeeds != NULL);
    NiStreamSaveBinary(kStream, bHasRotationSpeeds);
    if (bHasRotationSpeeds)
    {
        NiStreamSaveBinary(kStream, m_pfRotationSpeeds, m_usVertices);
    }

    NiStreamSaveBinary(kStream, m_usNumAddedParticles);
    NiStreamSaveBinary(kStream, m_usAddedParticlesBase);
}
//---------------------------------------------------------------------------
bool NiPSysData::IsEqual(NiObject* pkObject)
{
    if (!NiParticlesData::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysData* pkDest = (NiPSysData*) pkObject;

    if ((m_pfRotationSpeeds && !pkDest->m_pfRotationSpeeds) ||
        (!m_pfRotationSpeeds && pkDest->m_pfRotationSpeeds))
    {
        return false;
    }

    for (unsigned short us = 0; us < m_usActiveVertices; us++)
    {
        if (!m_pkParticleInfo[us].IsEqual(pkDest->m_pkParticleInfo[us]))
        {
            return false;
        }

        if (m_pfRotationSpeeds &&
            m_pfRotationSpeeds[us] != pkDest->m_pfRotationSpeeds[us])
        {
            return false;
        }
    }

    if (m_usNumAddedParticles != pkDest->m_usNumAddedParticles ||
        m_usAddedParticlesBase != pkDest->m_usAddedParticlesBase)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysData::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiParticlesData::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysData::ms_RTTI.GetName()));
    pkStrings->Add(NiGetViewerString("m_pkParticleInfo", m_pkParticleInfo));
    pkStrings->Add(NiGetViewerString("m_pfRotationSpeeds",
        m_pfRotationSpeeds));
    pkStrings->Add(NiGetViewerString("NumAddedParticles",
        m_usNumAddedParticles));
    pkStrings->Add(NiGetViewerString("AddedParticlesBase",
        m_usAddedParticlesBase));
}
//---------------------------------------------------------------------------
