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

#include "NiBlendAccumTransformInterpolator.h"

NiImplementRTTI(NiBlendAccumTransformInterpolator, NiBlendInterpolator);

//---------------------------------------------------------------------------
NiBlendAccumTransformInterpolator::~NiBlendAccumTransformInterpolator()
{
    NiDelete[] m_pkAccumArray;
}
//---------------------------------------------------------------------------
unsigned char NiBlendAccumTransformInterpolator::AddInterpInfo(
    NiInterpolator* pkInterpolator, float fWeight, char cPriority,
    float fEaseSpinner)
{
    unsigned char ucSingleIdx = m_ucSingleIdx;

    unsigned char ucIndex = NiBlendInterpolator::AddInterpInfo(
        pkInterpolator, fWeight, cPriority, fEaseSpinner);

    if (ucIndex != INVALID_INDEX)
    {
        m_pkAccumArray[ucIndex].ClearValues();

        if (m_ucInterpCount == 1)
        {
            // Set m_bReset to true here to unapply the start transform on the
            // first update.
            m_bReset = true;
        }
        else if (m_ucInterpCount == 2)
        {
            // If switching from one interpolator to multiple, set the
            // proper accumulated transform value here. We do not invalidate
            // the last transform if it is valid. To do so would result in
            // dropping the delta transform from one frame.
            AccumArrayItem& kAccumItem = m_pkAccumArray[ucSingleIdx];

            if (!kAccumItem.m_kLastValue.IsTransformInvalid())
            {
                if (!m_kAccumulatedTransformValue.IsTransformInvalid())
                {
                    m_kAccumulatedTransformValue = m_kAccumulatedTransformValue
                        .HierApply(kAccumItem.m_kLastValue);
                }

                // Set the reference frame to the accumulated transform
                // rotation minus the transform at first time.
                NiQuaternion kAccumRot(NiQuaternion::IDENTITY);
                if (m_kAccumulatedTransformValue.IsRotateValid())
                {
                    kAccumRot = m_kAccumulatedTransformValue.GetRotate();
                }
                NiQuaternion kInvLast(NiQuaternion::IDENTITY);
                if (kAccumItem.m_kLastValue.IsRotateValid())
                {
                    kInvLast = NiQuaternion::UnitInverse(
                        kAccumItem.m_kLastValue.GetRotate());
                }
                NiQuaternion kRefFrame = kAccumRot * kInvLast;
                kRefFrame.ToRotation(kAccumItem.m_kRefFrame);
            }
            else
            {
                kAccumItem.ClearValues();
            }
        }
    }
     
    return ucIndex;
}
//---------------------------------------------------------------------------
NiInterpolatorPtr NiBlendAccumTransformInterpolator::RemoveInterpInfo(
    unsigned char ucIndex)
{
    NIASSERT(ucIndex < m_ucArraySize);

    if (m_ucInterpCount == 1 && ucIndex == m_ucSingleIdx &&
        !m_kAccumulatedTransformValue.IsTransformInvalid() &&
        !m_pkAccumArray[m_ucSingleIdx].m_kLastValue.IsTransformInvalid())
    {
        m_kAccumulatedTransformValue = m_kAccumulatedTransformValue
            .HierApply(m_pkAccumArray[m_ucSingleIdx].m_kLastValue);
    }

    NiInterpolatorPtr spInterpolator = NiBlendInterpolator::RemoveInterpInfo(
        ucIndex);

    if (m_ucInterpCount == 1 && spInterpolator != NULL)
    {
        // Set m_bReset to true here to unapply the start transform on the
        // first update.
        m_bReset = true;

        // Clear the accum array values for the single interpolator.
        m_pkAccumArray[m_ucSingleIdx].ClearValues();
    }

    return spInterpolator;
}
//---------------------------------------------------------------------------
bool NiBlendAccumTransformInterpolator::GetValue(NiQuatTransform& kValue) 
    const
{
    if (m_kAccumulatedTransformValue.IsTransformInvalid())
    {
        return false;
    }

    if (m_ucInterpCount == 1)
    {
        // The global transform is applied to the last interpolator transform
        // and is returned here.
        kValue = m_kAccumulatedTransformValue.HierApply(
            m_pkAccumArray[m_ucSingleIdx].m_kLastValue);
    }
    else
    {
        kValue = m_kAccumulatedTransformValue;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiBlendAccumTransformInterpolator::IsTransformValueSupported() const
{
    return true;
}
//---------------------------------------------------------------------------
bool NiBlendAccumTransformInterpolator::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator) const
{
    return pkInterpolator->IsTransformValueSupported();
}
//---------------------------------------------------------------------------
bool NiBlendAccumTransformInterpolator::StoreSingleValue(float fTime, 
    NiObjectNET* pkInterpTarget, NiQuatTransform& kValue)
{
    NIASSERT(m_ucSingleIdx != INVALID_INDEX);
    if (!GetSingleUpdateTime(fTime))
        return false;

    CalculateSingleValue(fTime, pkInterpTarget);

    // If the global transform has not yet been set, initialize it to the
    // inverse of the last value for the interpolator. This is done to
    // prevent the interpolator from "popping" forward when a sequence is
    // activated.
    
    // Here we have a chicken and egg problem when using a physics-driven
    // interpolator. Physics needs an up-to-date accumulated transform
    // value in order to set the interpolator value so that the
    // accumulation * interpolator value = world physics value. But here
    // we set the accumulated transform based on the output of the
    // interpolator update, which needs the accumulated transform to already
    // be set, which needs the interpolator's start value to be known, which
    // requires the accumulated transform, ...
    // The solution is to require an application to explicitly set the
    // accumulation transform before activating the physics driven-sequence
    // (if it's the only sequence) or otherwise be certain that the
    // physics is positioned with any accumulated dimensions set at 0.
    if (m_bReset || m_kAccumulatedTransformValue.IsTransformInvalid())
    {
        NiQuatTransform kLastInv;
        m_pkAccumArray[m_ucSingleIdx].m_kLastValue.HierInvert(kLastInv);
        if (m_kAccumulatedTransformValue.IsTransformInvalid())
        {
            m_kAccumulatedTransformValue = kLastInv;
        }
        else
        {
            m_kAccumulatedTransformValue = m_kAccumulatedTransformValue
                .HierApply(kLastInv);
        }
        m_bReset = false;
    }
    
    if (m_kAccumulatedTransformValue.IsTransformInvalid())
    {
        return false;
    }

    // The global transform is applied to the last interpolator transform
    // and is returned here.
    kValue = m_kAccumulatedTransformValue.HierApply(
        m_pkAccumArray[m_ucSingleIdx].m_kLastValue);
    
    return true;
}
//---------------------------------------------------------------------------
bool NiBlendAccumTransformInterpolator::BlendValues(float fTime, 
    NiObjectNET* pkInterpTarget, NiQuatTransform& kValue)
{
    float fTotalTransWeight = 1.0f;
    float fTotalScaleWeight = 1.0f;

    NiPoint3 kFinalTranslate = NiPoint3::ZERO;
    NiQuaternion kFinalRotate = NiQuaternion(0.0f, 0.0f, 0.0f, 0.0f);
    float fFinalScale = 0.0f;

    bool bTransChanged = false;
    bool bRotChanged = false;
    bool bScaleChanged = false;

    bool bAccumTransformInvalid = m_kAccumulatedTransformValue
        .IsTransformInvalid();
    if (bAccumTransformInvalid)
    {
        m_kAccumulatedTransformValue.SetTranslate(NiPoint3::ZERO);
        m_kAccumulatedTransformValue.SetRotate(NiQuaternion::IDENTITY);
        m_kAccumulatedTransformValue.SetScale(1.0f);
    }

    bool bFirstRotation = true;
    for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
    {
        InterpArrayItem& kItem = m_pkInterpArray[uc];
        AccumArrayItem& kAccumItem = m_pkAccumArray[uc];
        if (kItem.m_spInterpolator && kItem.m_fNormalizedWeight > 0.0f)
        {
            NiQuatTransform kTransform;
            if (bAccumTransformInvalid)
            {
                kTransform = kAccumItem.m_kLastValue;
            }
            else
            {
                kTransform = kAccumItem.m_kDeltaValue;
            }

            // Add in the current interpolator's weighted
            // translation to the accumulated translation thus far.
            if (kTransform.IsTranslateValid())
            {
                kFinalTranslate += kTransform.GetTranslate() *
                    kItem.m_fNormalizedWeight;
                bTransChanged = true;
            }
            else
            {
                // This translate is invalid, so we need to
                // remove it's overall weight from the result
                // at the end
                fTotalTransWeight -= kItem.m_fNormalizedWeight;
            }

            // Add in the current interpolator's weighted
            // rotation to the accumulated rotation thus far.
            // Since quaternion SLERP is not commutative, we can
            // get away with accumulating weighted sums of the quaternions
            // as long as we re-normalize at the end.
            if (kTransform.IsRotateValid())
            {
                NiQuaternion kRotValue = kTransform.GetRotate();

                // Dot only represents the angle between quats when they
                // are unitized. However, we don't care about the 
                // specific angle. We only care about the sign of the angle
                // between the two quats. This is preserved when
                // quaternions are non-unit.
                if (!bFirstRotation)
                {
                    float fCos = NiQuaternion::Dot(kFinalRotate, kRotValue);

                    // If the angle is negative, we need to invert the
                    // quat to get the best path.
                    if (fCos < 0.0f)
                    {
                        kRotValue = -kRotValue;
                    }
                }
                else
                {
                    bFirstRotation = false;
                }

                // Multiply in the weights to the quaternions.
                // Note that this makes them non-rotations.
                kRotValue = kRotValue * kItem.m_fNormalizedWeight;

                // Accumulate the total weighted values into the 
                // rotation
                kFinalRotate.SetValues(
                    kRotValue.GetW() + kFinalRotate.GetW(), 
                    kRotValue.GetX() + kFinalRotate.GetX(), 
                    kRotValue.GetY() + kFinalRotate.GetY(),
                    kRotValue.GetZ() + kFinalRotate.GetZ());

                // Need to re-normalize quaternion.
                bRotChanged = true;
            }
            // we don't need to remove the weight of invalid rotations
            // since we are re-normalizing at the end. It's just extra work.

            // Add in the current interpolator's weighted
            // scale to the accumulated scale thus far.
            if (kTransform.IsScaleValid())
            {
                fFinalScale += kTransform.GetScale() *
                    kItem.m_fNormalizedWeight;
                bScaleChanged = true;
            }
            else 
            {
                // This scale is invalid, so we need to
                // remove it's overall weight from the result
                // at the end
                fTotalScaleWeight -= kItem.m_fNormalizedWeight;
            }
        }
    }

    // If any of the channels were animated, the final
    // transform needs to be updated
    if (bTransChanged || bRotChanged || bScaleChanged)
    {
        // Since channels may or may not actually have been
        // active during the blend, we can remove the weights for
        // channels that weren't active.
        float fTotalTransWeightInv = 1.0f / fTotalTransWeight;
        float fTotalScaleWeightInv = 1.0f / fTotalScaleWeight;

        NiQuatTransform kFinalTransform;
        if (bTransChanged)
        {
            // Remove the effect of invalid translations from the
            // weighted sum
            kFinalTranslate *= fTotalTransWeightInv;
            kFinalTransform.SetTranslate(kFinalTranslate);
        }
        if (bRotChanged)
        {
            // Since we summed quaternions earlier, we have
            // non-unit quaternions, which are not rotations.
            // To make the accumulated quaternion a rotation, we 
            // need to normalize.
            kFinalRotate.Normalize();
            kFinalTransform.SetRotate(kFinalRotate);
        }
        if (bScaleChanged)
        {
            // Remove the effect of invalid scales from the
            // weighted sum
            fFinalScale *= fTotalScaleWeightInv;
            kFinalTransform.SetScale(fFinalScale);
        }

        m_kAccumulatedTransformValue = m_kAccumulatedTransformValue *
            kFinalTransform;

        if (m_kAccumulatedTransformValue.IsTransformInvalid())
        {
            return false;
        }

        kValue = m_kAccumulatedTransformValue;
    
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiBlendAccumTransformInterpolator::Update(float fTime,
    NiObjectNET* pkInterpTarget, NiQuatTransform& kValue)
{
    // Do not use the TimeHasChanged check here, because blend interpolators
    // should always update their interpolators.

    bool bReturnValue = false;
    if (m_ucInterpCount == 1)
    {
        bReturnValue = StoreSingleValue(fTime, pkInterpTarget, kValue);
    }
    else if (m_ucInterpCount > 0)
    {
        ComputeNormalizedWeights();
        for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
        {
            float fUpdateTime = fTime;
            if (!GetUpdateTimeForItem(fUpdateTime,  m_pkInterpArray[uc]))
                continue;

            CalculateValue(uc, fUpdateTime, pkInterpTarget);
        }

        bReturnValue = BlendValues(fTime, pkInterpTarget, kValue);
    }
    
    m_fLastTime = fTime;
    return bReturnValue;
}
//---------------------------------------------------------------------------
bool NiBlendAccumTransformInterpolator::ReallocateArrays()
{
    unsigned char ucOldArraySize = m_ucArraySize;

    bool bReturn = NiBlendInterpolator::ReallocateArrays();

    if (bReturn)
    {
        AccumArrayItem* pkNewAccumArray = NiNew AccumArrayItem[m_ucArraySize];

        // Copy existing data.
        unsigned char uc;
        for (uc = 0; uc < ucOldArraySize; uc++)
        {
            pkNewAccumArray[uc] = m_pkAccumArray[uc];
        }

        NiDelete[] m_pkAccumArray;
        m_pkAccumArray = pkNewAccumArray;
    }

    return bReturn;
}
//---------------------------------------------------------------------------
void NiBlendAccumTransformInterpolator::CalculateSingleValue(float fTime,
    NiObjectNET* pkInterpTarget)
{
    // For single interpolators, a running accumulated transform is not kept
    // each frame because of floating point precision errors. Instead, a
    // global transform is stored in m_kAccumulatedTransformValue that is
    // applied each frame to the last computed interpolator transform stored
    // in kAccumItem.m_kLastValue. This global transform is updated whenever
    // the interpolator loops with the loop transform stored in
    // kAccumItem.m_kDeltaValue. GetValue applies m_kLastValue to
    // m_kAccumulatedTransformValue to obtain the final accumulated position.

    NiInterpolator* pkInterpolator = m_pkInterpArray[m_ucSingleIdx]
        .m_spInterpolator;
    AccumArrayItem& kAccumItem = m_pkAccumArray[m_ucSingleIdx];

    if (kAccumItem.m_fLastTime != fTime)
    {
        if (fTime < kAccumItem.m_fLastTime)
        {
            // Compute the interpolator loop transform, if necessary.
            if (kAccumItem.m_kDeltaValue.IsTransformInvalid())
            {
                // Get the interpolator time range.
                float fBeginTime, fEndTime;
                pkInterpolator->GetActiveTimeRange(fBeginTime, fEndTime);

                // Get the begin time transformation.
                NiQuatTransform kBeginTransform;
                NIVERIFY(pkInterpolator->Update(fBeginTime,
                    pkInterpTarget, kBeginTransform));

                // Get the end time transformation.
                NiQuatTransform kEndTransform;
                NIVERIFY(pkInterpolator->Update(fEndTime, pkInterpTarget,
                    kEndTransform));

                // Get the delta from the begin time to the end time.
                NiQuatTransform kInvBegin;
                kBeginTransform.HierInvert(kInvBegin);
                kAccumItem.m_kDeltaValue = kEndTransform.HierApply(kInvBegin);
            }

            // Apply the interpolator loop transform to the global transform.
            if(!m_kAccumulatedTransformValue.IsTransformInvalid())
            {
                m_kAccumulatedTransformValue = m_kAccumulatedTransformValue
                    .HierApply(kAccumItem.m_kDeltaValue);
            }
        }

        // Store the transform for the current time.
        NIVERIFY(pkInterpolator->Update(fTime, pkInterpTarget, 
            kAccumItem.m_kLastValue));
    }

    kAccumItem.m_fLastTime = fTime;
}
//---------------------------------------------------------------------------
void NiBlendAccumTransformInterpolator::CalculateValue(unsigned char 
    ucIndex, float fTime, NiObjectNET* pkInterpTarget)
{
    NiInterpolator* pkInterpolator =
        m_pkInterpArray[ucIndex].m_spInterpolator;
    AccumArrayItem& kAccumItem = m_pkAccumArray[ucIndex];

    if (pkInterpolator && kAccumItem.m_fLastTime != fTime)
    {
        // Check for first update.
        if (m_bReset || kAccumItem.m_kLastValue.IsTransformInvalid())
        {
            m_bReset = false;

            // If this is our first time through, set the last value to the
            // transformation at the first time.
            NIVERIFY(pkInterpolator->Update(fTime, pkInterpTarget, 
                kAccumItem.m_kLastValue));

            // Set the reference frame to the accumulated transform rotation
            // minus the transform at first time.
            NiQuaternion kAccumRot(NiQuaternion::IDENTITY);
            if (m_kAccumulatedTransformValue.IsRotateValid())
            {
                kAccumRot = m_kAccumulatedTransformValue.GetRotate();
            }
            NiQuaternion kInvLast(NiQuaternion::IDENTITY);
            if (kAccumItem.m_kLastValue.IsRotateValid())
            {
                kInvLast = NiQuaternion::UnitInverse(
                    kAccumItem.m_kLastValue.GetRotate());
            }
            NiQuaternion kRefFrame = kAccumRot * kInvLast;
            kRefFrame.ToRotation(kAccumItem.m_kRefFrame);
        }

        if (fTime < kAccumItem.m_fLastTime)
        {
            // Need to get the delta from last time to end of animation
            // and from begin of animation to now
            float fBeginTime, fEndTime;
            pkInterpolator->GetActiveTimeRange(fBeginTime, fEndTime);

            // Get the end time transformation
            NiQuatTransform kEndTransform;
            NIVERIFY(pkInterpolator->Update(fEndTime, pkInterpTarget,
                kEndTransform));

            // Get the begin time transformation
            NiQuatTransform kBeginTransform;
            NIVERIFY(pkInterpolator->Update(fBeginTime, pkInterpTarget,
                kBeginTransform));

            // Get the current time transformation
            NiQuatTransform kNowTransform;
            NIVERIFY(pkInterpolator->Update(fTime, pkInterpTarget,
                kNowTransform));

            // Get the delta from the last value to the end of the
            // animation
            NiQuatTransform kInvLast;
            kAccumItem.m_kLastValue.Invert(kInvLast);
            kAccumItem.m_kDeltaValue = kInvLast * kEndTransform;
            if (kAccumItem.m_kDeltaValue.IsTranslateValid())
            {
                kAccumItem.m_kDeltaValue.SetTranslate(kAccumItem.m_kRefFrame *
                    kAccumItem.m_kDeltaValue.GetTranslate());
            }

            // Compute the new reference frame.
            NiQuaternion kAccumRotate(NiQuaternion::IDENTITY);
            if (m_kAccumulatedTransformValue.IsRotateValid())
            {
                kAccumRotate = m_kAccumulatedTransformValue.GetRotate();
            }
            NiQuaternion kInvBegin(NiQuaternion::IDENTITY);
            if (kBeginTransform.IsRotateValid())
            {
                kInvBegin = NiQuaternion::UnitInverse(
                    kBeginTransform.GetRotate());
            }
            NiQuaternion kRefFrame = kAccumRotate * kInvBegin;
            kRefFrame.ToRotation(kAccumItem.m_kRefFrame);

            // Get the delta from the begin value to the current value of the
            // animation
            kBeginTransform.Invert(kInvLast);
            kAccumItem.m_kLastValue = kInvLast * kNowTransform;
            if (kAccumItem.m_kLastValue.IsTranslateValid())
            {
                kAccumItem.m_kLastValue.SetTranslate(kAccumItem.m_kRefFrame *
                    kAccumItem.m_kLastValue.GetTranslate());
            }

            // Get the total delta of the two animations
            // m_kDeltaValue will contain the delta
            kAccumItem.m_kDeltaValue = kAccumItem.m_kDeltaValue *
                kAccumItem.m_kLastValue;

            // The last value is now the current value of the animation
            kAccumItem.m_kLastValue = kNowTransform;
        }
        else
        {
            NiQuatTransform kNowTransform;
            // Get the current time transformation
            NIVERIFY(pkInterpolator->Update(fTime, pkInterpTarget,
                kNowTransform));

            // Get the delta between the last time and the current time
            // m_kDeltaValue will contain the delta
            NiQuatTransform kInvLast;
            kAccumItem.m_kLastValue.Invert(kInvLast);
            kAccumItem.m_kDeltaValue = kInvLast * kNowTransform;
            if (kAccumItem.m_kDeltaValue.IsTranslateValid())
            {
                kAccumItem.m_kDeltaValue.SetTranslate(kAccumItem.m_kRefFrame *
                    kAccumItem.m_kDeltaValue.GetTranslate());
            }

            // The last value is now the current value of the animation
            kAccumItem.m_kLastValue = kNowTransform;
        }
    }
    else
    {
        kAccumItem.m_kDeltaValue.SetTranslate(NiPoint3::ZERO);
        kAccumItem.m_kDeltaValue.SetRotate(NiQuaternion::IDENTITY);
        kAccumItem.m_kDeltaValue.SetScale(1.0f);
    }

    kAccumItem.m_fLastTime = fTime;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBlendAccumTransformInterpolator);
//---------------------------------------------------------------------------
void NiBlendAccumTransformInterpolator::CopyMembers(
    NiBlendAccumTransformInterpolator* pkDest, NiCloningProcess& kCloning)
{
    NiBlendInterpolator::CopyMembers(pkDest, kCloning);

    pkDest->m_pkAccumArray = NiNew AccumArrayItem[m_ucArraySize];
    unsigned char uc;
    for (uc = 0; uc < m_ucArraySize; uc++)
    {
        pkDest->m_pkAccumArray[uc] = m_pkAccumArray[uc];
    }

    pkDest->m_kAccumulatedTransformValue = m_kAccumulatedTransformValue;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBlendAccumTransformInterpolator);
//---------------------------------------------------------------------------
void NiBlendAccumTransformInterpolator::LoadBinary(NiStream& kStream)
{
    NiBlendInterpolator::LoadBinary(kStream);

    m_pkAccumArray = NiNew AccumArrayItem[m_ucArraySize];

    unsigned char uc;
    for (uc = 0; uc < m_ucArraySize; uc++)
    {
        m_pkAccumArray[uc].LoadBinary(kStream);
    }

    m_kAccumulatedTransformValue.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiBlendAccumTransformInterpolator::LinkObject(NiStream& kStream)
{
    NiBlendInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBlendAccumTransformInterpolator::RegisterStreamables(NiStream& kStream)
{
    return NiBlendInterpolator::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiBlendAccumTransformInterpolator::SaveBinary(NiStream& kStream)
{
    NiBlendInterpolator::SaveBinary(kStream);

    for (unsigned char uc = 0; uc < m_ucArraySize; uc++)
    {
        m_pkAccumArray[uc].SaveBinary(kStream);
    }

    m_kAccumulatedTransformValue.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiBlendAccumTransformInterpolator::IsEqual(NiObject* pkObject)
{
    if (!NiBlendInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiBlendAccumTransformInterpolator* pkDest = 
        (NiBlendAccumTransformInterpolator*) pkObject;


    if (m_kAccumulatedTransformValue != pkDest->m_kAccumulatedTransformValue)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBlendAccumTransformInterpolator::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiBlendInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiBlendAccumTransformInterpolator::ms_RTTI
        .GetName()));

    m_kAccumulatedTransformValue.GetViewerStrings(pkStrings);
}
//---------------------------------------------------------------------------
