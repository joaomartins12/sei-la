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
#include "NiMainPCH.h"

#include "NiParticlesData.h"
#include "NiBool.h"

NiImplementRTTI(NiParticlesData, NiGeometryData);

//---------------------------------------------------------------------------
NiParticlesData::NiParticlesData(unsigned short usVertices,
    NiPoint3* pkVertex, NiPoint3* pkNormal, NiColorA* pkColor,
    NiQuaternion* pkRots, float* pfRotationAngles, NiPoint3* pkRotationAxes) :
    NiGeometryData(usVertices, pkVertex, pkNormal, pkColor, NULL, 0, 
        NiGeometryData::NBT_METHOD_NONE), m_pkRotations(pkRots), 
        m_pfRotationAngles(pfRotationAngles), m_pkRotationAxes(pkRotationAxes)
{
    // Particles data must have room for at least one particle.
    NIASSERT(usVertices > 0);

    m_pfRadii = NiAlloc(float, usVertices);
    m_pfSizes = NiAlloc(float, usVertices);
    for (unsigned short us = 0; us < usVertices; us++)
    {
        m_pfRadii[us] = 1.0f;
        m_pfSizes[us] = 1.0f;
    }

    SetConsistency(NiGeometryData::VOLATILE);
}
//---------------------------------------------------------------------------
NiParticlesData::NiParticlesData() : m_pfRadii(NULL), m_pfSizes(NULL), 
    m_pkRotations(NULL), m_pfRotationAngles(NULL), m_pkRotationAxes(NULL)
{
    SetConsistency(NiGeometryData::VOLATILE);
}
//---------------------------------------------------------------------------
NiParticlesData::~NiParticlesData ()
{
    NiFree(m_pfRadii);
    NiFree(m_pfSizes);
    NiDelete [] m_pkRotations;
    NiFree(m_pfRotationAngles);
    NiDelete [] m_pkRotationAxes;
}
//---------------------------------------------------------------------------
void NiParticlesData::ReplaceRadii(float* pfRadii)
{
    NiFree(m_pfRadii);
    m_pfRadii = pfRadii;
}
//---------------------------------------------------------------------------
void NiParticlesData::ReplaceSizes(float* pfSizes)
{
    NiFree(m_pfSizes);
    m_pfSizes = pfSizes;
}
//---------------------------------------------------------------------------
void NiParticlesData::ReplaceRotations(NiQuaternion* pkRotations)
{
    NiDelete [] m_pkRotations;
    m_pkRotations = pkRotations;
}
//---------------------------------------------------------------------------
void NiParticlesData::ReplaceRotationAngles(float* pfRotationAngles)
{
    NiFree(m_pfRotationAngles);
    m_pfRotationAngles = pfRotationAngles;
}
//---------------------------------------------------------------------------
void NiParticlesData::ReplaceRotationAxes(NiPoint3* pkRotationAxes)
{
    NiDelete [] m_pkRotationAxes;
    m_pkRotationAxes = pkRotationAxes;
}
//---------------------------------------------------------------------------
void NiParticlesData::RemoveParticle(unsigned short usParticle)
{
    unsigned short usLastParticle = m_usActiveVertices - 1;
    if (usParticle != usLastParticle)
    {
        m_pkVertex[usParticle] = m_pkVertex[usLastParticle];
        if (m_pkColor)
        {
            m_pkColor[usParticle] = m_pkColor[usLastParticle];
        }
        if (m_pkNormal)
        {
            m_pkNormal[usParticle] = m_pkNormal[usLastParticle];
        }
        if (m_pfRadii)
        {
            m_pfRadii[usParticle] = m_pfRadii[usLastParticle];
        }
        if (m_pfSizes)
        {
            m_pfSizes[usParticle] = m_pfSizes[usLastParticle];
        }
        if (m_pkRotations)
        {
            m_pkRotations[usParticle] = m_pkRotations[usLastParticle];
        }
        if (m_pfRotationAngles)
        {
            m_pfRotationAngles[usParticle] =
                m_pfRotationAngles[usLastParticle];
        }
        if (m_pkRotationAxes)
        {
            m_pkRotationAxes[usParticle] = m_pkRotationAxes[usLastParticle];
        }
    }

    m_usActiveVertices--;
}
//---------------------------------------------------------------------------
#ifdef _PS3
bool NiParticlesData::ContainsVertexData(
    NiShaderDeclaration::ShaderParameter eParameter) const
{
    if (NiGeometryData::ContainsVertexData(eParameter))
        return true;

    // Tex coords, normals, binormals, and tangents will not exist on 
    // particles but can be generated
    switch (eParameter)
    {
    case NiShaderDeclaration::SHADERPARAM_NI_NORMAL:
    case NiShaderDeclaration::SHADERPARAM_NI_BINORMAL:
    case NiShaderDeclaration::SHADERPARAM_NI_TANGENT:
    case NiShaderDeclaration::SHADERPARAM_NI_TEXCOORD0:
        return true;
    default:
        break;
    }

    return false;

}
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiParticlesData);
//---------------------------------------------------------------------------
void NiParticlesData::LoadBinary(NiStream& kStream)
{
    NiGeometryData::LoadBinary(kStream);

    m_pfRadii = NiAlloc(float,m_usVertices);
    NiBool bRadii;
    NiStreamLoadBinary(kStream, bRadii);
    if (bRadii)
    {
        NiStreamLoadBinary(kStream, m_pfRadii, m_usVertices);
    }
    else
    {
        for (unsigned short us = 0; us < m_usVertices; us++)
        {
            m_pfRadii[us] = 1.0f;
        }
    }

    NiStreamLoadBinary(kStream, m_usActiveVertices);

    NiBool bSize;
    NiStreamLoadBinary(kStream, bSize);

    m_pfSizes = NiAlloc(float,m_usVertices);

    if (bSize)
    {
        NiStreamLoadBinary(kStream,m_pfSizes,m_usVertices);
    }
    else
    {
        for (unsigned short i = 0; i < m_usVertices; i++)
            m_pfSizes[i] = 1.0f;
    }

    NiBool bHasRotation;
    NiStreamLoadBinary(kStream, bHasRotation);

    if (bHasRotation)
    {
        m_pkRotations = NiNew NiQuaternion[m_usVertices];
        for (unsigned short us = 0; us < m_usVertices; us++)
        {
            m_pkRotations[us].LoadBinary(kStream);
        }
    }

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 3, 0, 5))
    {
        if (m_pkRotations)
        {
            m_pfRotationAngles = NiAlloc(float, m_usVertices);
            m_pkRotationAxes = NiNew NiPoint3[m_usVertices];
            for (unsigned short us = 0; us < m_usVertices; us++)
            {
                m_pkRotations[us].ToAngleAxis(m_pfRotationAngles[us],
                    m_pkRotationAxes[us]);
            }
            ReplaceRotations(NULL);
        }
    }
    else
    {
        NiBool bHasRotationAngles;
        NiStreamLoadBinary(kStream, bHasRotationAngles);
        if (bHasRotationAngles)
        {
            m_pfRotationAngles = NiAlloc(float, m_usVertices);
            for (unsigned short us = 0; us < m_usVertices; us++)
            {
                NiStreamLoadBinary(kStream, m_pfRotationAngles[us]);
            }
        }

        NiBool bHasRotationAxes;
        NiStreamLoadBinary(kStream, bHasRotationAxes);
        if (bHasRotationAxes)
        {
            m_pkRotationAxes = NiNew NiPoint3[m_usVertices];
            for (unsigned short us = 0; us < m_usVertices; us++)
            {
                m_pkRotationAxes[us].LoadBinary(kStream);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiParticlesData::LinkObject(NiStream& kStream)
{
    NiGeometryData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiParticlesData::RegisterStreamables(NiStream& kStream)
{
    return NiGeometryData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiParticlesData::SaveBinary(NiStream& kStream)
{
    NiGeometryData::SaveBinary(kStream);

    NiBool bRadii = (m_pfRadii != NULL);
    NiStreamSaveBinary(kStream, bRadii);
    if (bRadii)
    {
        NiStreamSaveBinary(kStream, m_pfRadii, m_usVertices);
    }

    NiStreamSaveBinary(kStream, m_usActiveVertices);

    NiBool bSize = (m_pfSizes != NULL);
    NiStreamSaveBinary(kStream, bSize);
    if (bSize)
    {
        NiStreamSaveBinary(kStream, m_pfSizes, m_usVertices);
    }

    NiBool bHasRotation = (m_pkRotations != NULL);
    NiStreamSaveBinary(kStream, bHasRotation);
    if (bHasRotation)
    {
        for (unsigned int i = 0; i < m_usVertices; i++)
            m_pkRotations[i].SaveBinary(kStream);
    }

    NiBool bHasRotationAngles = (m_pfRotationAngles != NULL);
    NiStreamSaveBinary(kStream, bHasRotationAngles);
    if (bHasRotationAngles)
    {
        for (unsigned int ui = 0; ui < m_usVertices; ui++)
        {
            NiStreamSaveBinary(kStream, m_pfRotationAngles[ui]);
        }
    }

    NiBool bHasRotationAxes = (m_pkRotationAxes != NULL);
    NiStreamSaveBinary(kStream, bHasRotationAxes);
    if (bHasRotationAxes)
    {
        for (unsigned int ui = 0; ui < m_usVertices; ui++)
        {
            m_pkRotationAxes[ui].SaveBinary(kStream);
        }
    }
}
//---------------------------------------------------------------------------
bool NiParticlesData::IsEqual(NiObject* pkObject)
{
    if (!NiGeometryData::IsEqual(pkObject))
        return false;

    NiParticlesData* pkPart = (NiParticlesData*) pkObject;

    if ((m_pfRadii && !pkPart->m_pfRadii) ||
        (!m_pfRadii && pkPart->m_pfRadii))
    {
        return false;
    }
    if (m_pfRadii)
    {
        for (unsigned short us = 0; us < m_usVertices; us++)
        {
            if (m_pfRadii[us] != pkPart->m_pfRadii[us])
            {
                return false;
            }
        }
    }

    if ((m_pfSizes && !pkPart->m_pfSizes) || (!m_pfSizes && pkPart->m_pfSizes))
        return false;

    if (m_pfSizes)
    {
        for (unsigned short i = 0; i < m_usActiveVertices; i++)
        {
            if (m_pfSizes[i] != pkPart->m_pfSizes[i])
                return false;
        }
    }

    if ((m_pkRotations && !pkPart->m_pkRotations) || 
        (!m_pkRotations && pkPart->m_pkRotations))
    {
        return false;
    }

    if (m_pkRotations)
    {
        for (unsigned int i = 0; i < m_usActiveVertices; i++)
        {
            if (m_pkRotations[i] != pkPart->m_pkRotations[i])
                return false;
        }
    }

    if (m_pfRotationAngles)
    {
        for (unsigned int ui = 0; ui < m_usActiveVertices; ui++)
        {
            if (m_pfRotationAngles[ui] != pkPart->m_pfRotationAngles[ui])
            {
                return false;
            }
        }
    }

    if (m_pkRotationAxes)
    {
        for (unsigned int ui = 0; ui < m_usActiveVertices; ui++)
        {
            if (m_pkRotationAxes[ui] != pkPart->m_pkRotationAxes[ui])
            {
                return false;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiParticlesData::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiGeometryData::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiParticlesData::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_pfRadii", m_pfRadii));
    pkStrings->Add(NiGetViewerString("m_pfSizes",m_pfSizes));
    pkStrings->Add(NiGetViewerString("m_pkRotations", m_pkRotations));
    pkStrings->Add(NiGetViewerString("m_pfRotationAngles",
        m_pfRotationAngles));
    pkStrings->Add(NiGetViewerString("m_pkRotationAxes", m_pkRotationAxes));
}
//---------------------------------------------------------------------------
