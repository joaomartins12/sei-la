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
#include "NiLines.h"
#include "NiRenderer.h"

NiImplementRTTI(NiLines,NiGeometry);

//---------------------------------------------------------------------------
NiLines::NiLines(unsigned short usVertices, NiPoint3* pkVertex,
    NiColorA* pkColor, NiPoint2* pkTexture, unsigned short usNumTextureSets, 
    NiGeometryData::DataFlags eNBTMethod, NiBool* pkFlags) :
    NiGeometry(NiNew NiLinesData(usVertices, pkVertex, pkColor, pkTexture, 
        usNumTextureSets, eNBTMethod, pkFlags))
{
}
//---------------------------------------------------------------------------
NiLines::NiLines(NiLinesData* pkModelData) :
    NiGeometry(pkModelData)
{
}
//---------------------------------------------------------------------------
NiLines::NiLines()
{
    // called by NiLines::CreateObject
}
//---------------------------------------------------------------------------
void NiLines::RenderImmediate(NiRenderer* pkRenderer)
{
    NiGeometry::RenderImmediate(pkRenderer);

    // render the object
    pkRenderer->RenderLines(this);
}
//---------------------------------------------------------------------------
void NiLines::UpdateEffectsDownward(NiDynamicEffectState* pkParentState)
{
    if (pkParentState)
    {
        // Make sure NiLines and NiParticles only have lights
        if (pkParentState->GetProjLightHeadPos() ||
            pkParentState->GetProjShadowHeadPos() ||
            pkParentState->GetEnvironmentMap() ||
            pkParentState->GetFogMap())
        {
            m_spEffectState = NiNew NiDynamicEffectState;
            NiDynEffectStateIter pkIter = pkParentState->GetLightHeadPos();

            while(pkIter)
            {
                NiLight* pkLight = pkParentState->GetNextLight(pkIter);
                m_spEffectState->AddEffect((NiDynamicEffect*)pkLight);
            }
        }
        else
        {
            m_spEffectState = pkParentState;
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

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiLines);
//---------------------------------------------------------------------------
void NiLines::CopyMembers(NiLines* pkDest,
    NiCloningProcess& kCloning)
{
    NiGeometry::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiLines);
//---------------------------------------------------------------------------
void NiLines::LoadBinary(NiStream& kStream)
{
    NiGeometry::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiLines::LinkObject(NiStream& kStream)
{
    NiGeometry::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiLines::RegisterStreamables(NiStream& kStream)
{
    return NiGeometry::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiLines::SaveBinary(NiStream& kStream)
{
    NiGeometry::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiLines::IsEqual(NiObject* pkObject)
{
    return NiGeometry::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
void NiLines::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiGeometry::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiLines::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
