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

#include "NiCamera.h"
#include "NiParticles.h"

NiImplementRTTI(NiParticles, NiGeometry);

//---------------------------------------------------------------------------
NiParticles::NiParticles(unsigned short usVertices, NiPoint3* pVertex, 
    NiPoint3* pNormal, NiColorA* pColor, NiQuaternion* pkRots,
    float* pfRotationAngles, NiPoint3* pkRotationAxes) : 
    NiGeometry(NiNew NiParticlesData(usVertices, pVertex, pNormal, pColor, 
    pkRots, pfRotationAngles, pkRotationAxes))
{
    /* */
}
//---------------------------------------------------------------------------
NiParticles::NiParticles(NiParticlesData* pModelData)
    : NiGeometry(pModelData)
{
    /* */
}
//---------------------------------------------------------------------------
NiParticles::NiParticles()
{
    /* */
}
//---------------------------------------------------------------------------
void NiParticles::RenderImmediate(NiRenderer* pkRenderer)
{
    NiGeometry::RenderImmediate(pkRenderer);

    // render the object
    pkRenderer->RenderPoints(this);
}
//---------------------------------------------------------------------------
void NiParticles::UpdateEffectsDownward(NiDynamicEffectState* pParentState)
{
    if(pParentState)
    {
        // Make sure NiLines and NiParticles only have lights
        if (pParentState->GetProjLightHeadPos() ||
            pParentState->GetProjShadowHeadPos() ||
            pParentState->GetEnvironmentMap() ||
            pParentState->GetFogMap())
        {
            m_spEffectState = NiNew NiDynamicEffectState;
            NiDynEffectStateIter pIter = pParentState->GetLightHeadPos();

            while(pIter)
            {
                NiLight* pLight = pParentState->GetNextLight(pIter);
                m_spEffectState->AddEffect((NiDynamicEffect*)pLight);
            }
        }
        else
        {
            m_spEffectState = pParentState;
        }
    }
    else
    {
        // DO NOT change the cached effect state - there is no point in
        // ever having this be NULL - all NULL's are the same as-is
        m_spEffectState = NULL;   
    }
}
//---------------------------------------------------------------------------
void NiParticles::CalculateConsistency(bool bTool)
{
    m_spModelData->SetConsistency(NiGeometryData::VOLATILE);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiParticles);
//---------------------------------------------------------------------------
void NiParticles::CopyMembers(NiParticles* pDest,
    NiCloningProcess& kCloning)
{
    NiGeometry::CopyMembers(pDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiParticles);
//---------------------------------------------------------------------------
void NiParticles::LoadBinary(NiStream& stream)
{
    NiGeometry::LoadBinary(stream);
}
//---------------------------------------------------------------------------
void NiParticles::LinkObject(NiStream& stream)
{
    NiGeometry::LinkObject(stream);
}
//---------------------------------------------------------------------------
bool NiParticles::RegisterStreamables(NiStream& stream)
{
    return NiGeometry::RegisterStreamables(stream);
}
//---------------------------------------------------------------------------
void NiParticles::SaveBinary(NiStream& stream)
{
    NiGeometry::SaveBinary(stream);
}
//---------------------------------------------------------------------------
bool NiParticles::IsEqual(NiObject* pObject)
{
    return NiGeometry::IsEqual(pObject);
}
//---------------------------------------------------------------------------
void NiParticles::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiGeometry::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiParticles::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
