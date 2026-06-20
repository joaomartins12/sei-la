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
#include "NiAnimationPCH.h"

#include "NiAnimation.h"
#include "NiAnimationSDM.h"

NiImplementSDMConstructor(NiAnimation);

#ifdef NIANIMATION_EXPORT
NiImplementDllMain(NiAnimation);
#endif

//---------------------------------------------------------------------------
void NiAnimationSDM::Init()
{
    NiImplementSDMInitCheck();

    NiRegisterStream(NiAlphaController);
    NiRegisterStream(NiBlendAccumTransformInterpolator);
    NiRegisterStream(NiBlendBoolInterpolator);
    NiRegisterStream(NiBlendColorInterpolator);
    NiRegisterStream(NiBlendFloatInterpolator);
    NiRegisterStream(NiBlendPoint3Interpolator);
    NiRegisterStream(NiBlendQuaternionInterpolator);
    NiRegisterStream(NiBlendTransformInterpolator);
    NiRegisterStream(NiBoneLODController);
    NiRegisterStream(NiBoolData);
    NiRegisterStream(NiBoolInterpolator);
    NiRegisterStream(NiBoolTimelineInterpolator);
    NiRegisterStream(NiBSplineBasisData);
    NiRegisterStream(NiBSplineData);
    NiRegisterStream(NiBSplineColorInterpolator);
    NiRegisterStream(NiBSplineCompColorInterpolator);
    NiRegisterStream(NiBSplineCompFloatInterpolator);
    NiRegisterStream(NiBSplineCompPoint3Interpolator);
    NiRegisterStream(NiBSplineCompTransformInterpolator);
    NiRegisterStream(NiBSplineFloatInterpolator);
    NiRegisterStream(NiBSplinePoint3Interpolator);
    NiRegisterStream(NiBSplineTransformInterpolator);
    NiRegisterStream(NiColorData);
    NiRegisterStream(NiColorExtraDataController);
    NiRegisterStream(NiColorInterpolator);
    NiRegisterStream(NiControllerManager);
    NiRegisterStream(NiControllerSequence);
    NiRegisterStream(NiFlipController);
    NiRegisterStream(NiFloatData);
    NiRegisterStream(NiFloatExtraDataController);
    NiRegisterStream(NiFloatInterpolator);
    NiRegisterStream(NiFloatsExtraDataController);
    NiRegisterStream(NiFloatsExtraDataPoint3Controller);
    NiRegisterStream(NiGeomMorpherController);

    // In Gamebryo 1.2, NiKeyframeController was deprecated.
    // NiTransformController took its place.
    NiStream::RegisterLoader("NiKeyframeController", 
        NiTransformController::CreateObject);

    // In Gamebryo 1.2, NiKeyframeData was renamed NiTransformData.
    NiStream::RegisterLoader("NiKeyframeData",
        NiTransformData::CreateObject);

    NiRegisterStream(NiLightColorController);
    NiRegisterStream(NiLightDimmerController);
    NiRegisterStream(NiLookAtController);
    NiRegisterStream(NiLookAtInterpolator);
    NiRegisterStream(NiMaterialColorController);
    NiRegisterStream(NiMorphData);
    NiRegisterStream(NiMultiTargetTransformController);
    NiRegisterStream(NiPathController);
    NiRegisterStream(NiPathInterpolator);
    NiRegisterStream(NiPoint3Interpolator);
    NiRegisterStream(NiPosData);
    NiRegisterStream(NiQuaternionInterpolator);
    NiRegisterStream(NiRollController);
    NiRegisterStream(NiRotData);
    NiRegisterStream(NiStringPalette);
    NiRegisterStream(NiTextKeyExtraData);
    NiRegisterStream(NiTextureTransformController);
    NiRegisterStream(NiTransformController);
    NiRegisterStream(NiTransformData);
    NiRegisterStream(NiTransformInterpolator);
    NiRegisterStream(NiVisController);

    // In Gamebryo 1.2, NiVisData was deprecated.
    // NiBoolData took its place.
    NiStream::RegisterLoader("NiVisData", 
        NiBoolData::CreateObject);

    NiStream::RegisterPostProcessFunction(NiOldAnimationConverter::Convert);

    NiActorManager::_SDMInit();
    NiGeomMorpherUpdateTask::_SDMInit();
}
//---------------------------------------------------------------------------
void NiAnimationSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiUnregisterStream(NiAlphaController);
    NiUnregisterStream(NiBlendAccumTransformInterpolator);
    NiUnregisterStream(NiBlendBoolInterpolator);
    NiUnregisterStream(NiBlendColorInterpolator);
    NiUnregisterStream(NiBlendFloatInterpolator);
    NiUnregisterStream(NiBlendPoint3Interpolator);
    NiUnregisterStream(NiBlendQuaternionInterpolator);
    NiUnregisterStream(NiBlendTransformInterpolator);
    NiUnregisterStream(NiBoneLODController);
    NiUnregisterStream(NiBoolData);
    NiUnregisterStream(NiBoolInterpolator);
    NiUnregisterStream(NiBoolTimelineInterpolator);
    NiUnregisterStream(NiBSplineBasisData);
    NiUnregisterStream(NiBSplineData);
    NiUnregisterStream(NiBSplineColorInterpolator);
    NiUnregisterStream(NiBSplineCompColorInterpolator);
    NiUnregisterStream(NiBSplineCompFloatInterpolator);
    NiUnregisterStream(NiBSplineCompPoint3Interpolator);
    NiUnregisterStream(NiBSplineCompTransformInterpolator);
    NiUnregisterStream(NiBSplineFloatInterpolator);
    NiUnregisterStream(NiBSplinePoint3Interpolator);
    NiUnregisterStream(NiBSplineTransformInterpolator);
    NiUnregisterStream(NiColorData);
    NiUnregisterStream(NiColorExtraDataController);
    NiUnregisterStream(NiColorInterpolator);
    NiUnregisterStream(NiControllerManager);
    NiUnregisterStream(NiControllerSequence);
    NiUnregisterStream(NiFlipController);
    NiUnregisterStream(NiFloatData);
    NiUnregisterStream(NiFloatExtraDataController);
    NiUnregisterStream(NiFloatInterpolator);
    NiUnregisterStream(NiFloatsExtraDataController);
    NiUnregisterStream(NiFloatsExtraDataPoint3Controller);
    NiUnregisterStream(NiGeomMorpherController);

    // In Gamebryo 1.2, NiKeyframeController was deprecated.
    // NiTransformController took its place.
    NiUnregisterStream(NiKeyframeController);

    // In Gamebryo 1.2, NiKeyframeData was renamed NiTransformData.
    NiUnregisterStream(NiKeyframeData);

    NiUnregisterStream(NiLightColorController);
    NiUnregisterStream(NiLightDimmerController);
    NiUnregisterStream(NiLookAtController);
    NiUnregisterStream(NiLookAtInterpolator);
    NiUnregisterStream(NiMaterialColorController);
    NiUnregisterStream(NiMorphData);
    NiUnregisterStream(NiMultiTargetTransformController);
    NiUnregisterStream(NiPathController);
    NiUnregisterStream(NiPathInterpolator);
    NiUnregisterStream(NiPoint3Interpolator);
    NiUnregisterStream(NiPosData);
    NiUnregisterStream(NiQuaternionInterpolator);
    NiUnregisterStream(NiRollController);
    NiUnregisterStream(NiRotData);
    NiUnregisterStream(NiStringPalette);
    NiUnregisterStream(NiTextKeyExtraData);
    NiUnregisterStream(NiTextureTransformController);
    NiUnregisterStream(NiTransformController);
    NiUnregisterStream(NiTransformData);
    NiUnregisterStream(NiTransformInterpolator);
    NiUnregisterStream(NiVisController);

    // In Gamebryo 1.2, NiVisData was deprecated.
    // NiBoolData took its place.
    NiUnregisterStream(NiVisData);

    NiStream::UnregisterPostProcessFunction(NiOldAnimationConverter::Convert);

    NiAllocatorShutdown(NiActorManager::TimelineData);

    NiActorManager::_SDMShutdown();
    NiGeomMorpherUpdateTask::_SDMShutdown();
}
//---------------------------------------------------------------------------
