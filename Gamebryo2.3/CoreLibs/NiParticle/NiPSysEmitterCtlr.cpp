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

#include "NiPSysEmitterCtlr.h"
#include "NiPSysEmitter.h"
#include "NiPSysEmitterCtlrData.h"
#include <NiFloatInterpolator.h>
#include <NiBlendFloatInterpolator.h>
#include <NiBoolInterpolator.h>
#include <NiBlendBoolInterpolator.h>
#include <NiFloatInterpolator.h>
#include <NiBoolInterpolator.h>

NiImplementRTTI(NiPSysEmitterCtlr, NiPSysModifierCtlr);
const char* NiPSysEmitterCtlr::ms_pcBirthRate = "BirthRate";
const char* NiPSysEmitterCtlr::ms_pcEmitterActive = "EmitterActive";
#define BIRTH_RATE_INDEX 0
#define EMITTER_ACTIVE_INDEX 1

//---------------------------------------------------------------------------
NiPSysEmitterCtlr::NiPSysEmitterCtlr(const char* pcModifierName) : 
    NiPSysModifierCtlr(pcModifierName),
    m_pkLastBirthRateInterpolator(NULL), 
    m_fLastScaledTime(-NI_INFINITY),
    m_bLastActive(false)
{
}
//---------------------------------------------------------------------------
NiPSysEmitterCtlr::NiPSysEmitterCtlr() : m_fLastScaledTime(-NI_INFINITY),
    m_bLastActive(false)
{
}
//---------------------------------------------------------------------------
void NiPSysEmitterCtlr::Stop()
{
    m_fLastScaledTime = -NI_INFINITY;
    NiPSysModifierCtlr::Stop();
}
//---------------------------------------------------------------------------
void NiPSysEmitterCtlr::Update(float fTime)
{
    // Detect negative time.
    if (m_fLastTime > fTime)
    {
        m_fLastScaledTime = -NI_INFINITY;
    }

    if (!m_pkTarget || !m_pkModifier)
    {
        return;
    }

    if (GetManagerControlled())
    {
        m_fScaledTime = INVALID_TIME;
    }

    if (DontDoUpdate(fTime))
    {
        return;
    }

    // We completely override the animation of the interpolators here
    // because we have a special case. Emitters are really simulations and 
    // as such, they need to have finer control over time.

    // We directly walk the key arrays here so that we can catch
    // all of the subtleties of the particle emission

    if (!m_spEmitterActiveInterpolator || !m_spInterpolator)
    {
        return;
    }
    
    // Set scaled time to -NI_INFINITY for the EmitterActive track. 
    // This default value lets us know later that the interpolator
    // does not exist or is not valid.
    float fBoolScaledTime = -NI_INFINITY;
    
    // We also do not blend particle systems together. Instead, we only 
    // use the current highest weighted item in the blend interpolators.
    if (NiIsKindOf(NiBlendBoolInterpolator, m_spEmitterActiveInterpolator))
    {
        NiBlendBoolInterpolator* pkBlendBool = NiSmartPointerCast(
            NiBlendBoolInterpolator, m_spEmitterActiveInterpolator);
        pkBlendBool->ComputeNormalizedWeights();
        unsigned char ucIndex = pkBlendBool->GetHighestWeightedIndex();
        if (ucIndex != NiBlendInterpolator::INVALID_INDEX)
        {
            fBoolScaledTime = pkBlendBool->GetTime(ucIndex);
        }
    }

    // GetEmitterActiveInterpolator will either return the
    // m_spEmitterActiveInterpolator cast as an NiBoolInterpolator if we
    // aren't being controlled by a manager or it will return the highest
    // weighted interpolator as calculated in the previous code block.
    NiBoolInterpolator* pkBoolInterp = GetEmitterActiveInterpolator();
    bool bBoolScaledTime = (fBoolScaledTime != -NI_INFINITY);

    // Set scaled time to -NI_INFINITY for the BirthRate track. 
    // This default value lets us know later that the interpolator
    // does not exist or is not valid.
    float fFloatScaledTime = -NI_INFINITY;

    // Repeat the same process as the EmitterActive track. Choose the
    // highest weighted NiFloatInterpolator from the blend interpolator.
    if (NiIsKindOf(NiBlendFloatInterpolator, m_spInterpolator))
    {
        NiBlendFloatInterpolator* pkBlendFloat = NiSmartPointerCast(
            NiBlendFloatInterpolator, m_spInterpolator);
        pkBlendFloat->ComputeNormalizedWeights();
        unsigned char ucIndex = pkBlendFloat->GetHighestWeightedIndex();
        if (ucIndex != NiBlendInterpolator::INVALID_INDEX)
        {
            fFloatScaledTime = pkBlendFloat->GetTime(ucIndex);
        }
    }

    // GetBirthRateInterpolator will either return the m_spInterpolator 
    // cast as an NiFloatInterpolator if we aren't being controlled by a
    // manager or it will return the highest weighted interpolator as
    // calculated in the previous code block.
    NiFloatInterpolator* pkFloatInterp = GetBirthRateInterpolator();
    bool bFloatScaledTime = (fFloatScaledTime != -NI_INFINITY);

    // If neither interpolator exist, we have no particles to emit.
    if (!pkBoolInterp || !pkFloatInterp)
    {
        return;
    }

    // Store the local version of the scaled time here. In code below,
    // we may manipulate the scaled time, so we don't want to use 
    // the internal variable.
    float fScaledTime = m_fScaledTime;
    if (bBoolScaledTime || bFloatScaledTime)
    {
        // Assert that the two values are the same.
        NIASSERT((bBoolScaledTime && !bFloatScaledTime) ||
            (!bBoolScaledTime && bFloatScaledTime) ||
            (fBoolScaledTime == fFloatScaledTime));
        fScaledTime = fFloatScaledTime;
    }
    
    // If we have changed interpolators, we need to reset our internal counts
    // when blending. This is because code below tracks through the keys
    // using the last time the interpolator was updated as a starting point.
    // This could potentially lead to crashes when interpolators change
    // between NiPSysEmitterCtlr::Update calls, because the keys may not be
    // in the same time range.
    if (GetManagerControlled() && 
        m_pkLastBirthRateInterpolator != pkFloatInterp)
    {
        m_fLastScaledTime = fScaledTime;
        pkBoolInterp->Update(fScaledTime, m_pkTarget, m_bLastActive);
        m_pkLastBirthRateInterpolator = pkFloatInterp;
        // Since this is the first update, don't do anything.
        return;
    }

    m_pkLastBirthRateInterpolator = pkFloatInterp;

    // As mentioned in a comment above, we are manually traversing the 
    // key arrays for the EmitterActive track and potentially the BirthRate
    // track.
    unsigned int uiNumEmitterActiveKeys;
    NiBoolKey::KeyType eEmitterActiveKeyType;
    unsigned char ucEmitterActiveSize;
    NiBoolKey* pkEmitterActiveKeys = pkBoolInterp->GetKeys(
        uiNumEmitterActiveKeys, eEmitterActiveKeyType, ucEmitterActiveSize);

    // If no keys exist and the interpolator is not posed, we can do nothing.
    // If, however, the active track is posed and active, emit using the 
    // BirthRate
    if (uiNumEmitterActiveKeys == 0 && !pkBoolInterp->GetChannelPosed(0))
    {
        return;
    }
    else if (pkBoolInterp->GetChannelPosed(0))
    {
        bool bActive;
        NIVERIFY(pkBoolInterp->Update(0.0f, m_pkTarget, bActive));

        // Since the emitter active interpolator is posed, we can return if
        // it is not active.
        if (!bActive)
        {
            return;
        }

        // If it is active, we can emit particles and skip the ensuing logic.
        float fLastScaledTime = m_fLastScaledTime;
        float fPSysLastTime = ((NiParticleSystem*) m_pkTarget)->GetLastTime();
        m_fLastScaledTime = fScaledTime;
        float fBirthRate;

        // If the float interpolator is also posed, we can simply
        // emit.
        if (pkFloatInterp->GetChannelPosed(0))
        {
            float fDeltaTime = fScaledTime - fLastScaledTime;
            if (fLastScaledTime > fScaledTime) // Catch the loop condition
            {
                fLastScaledTime = 0.0f;
                fDeltaTime = fScaledTime;
            }

            if (pkFloatInterp->Update(0.0f, m_pkTarget, fBirthRate))
            {
                // Both interpolators are posed, so we don't have an
                // appropriate start time to use for fEmitStart. Thus, we use
                // 0.0f.

                // Emit Particles requires that the data obey the following 
                // constraints:
                //  fBirthRate > 0.0f
                //  fEmitStart < fEmitStop 
                //  fCurTime > fEmitStart
                //  fLastTime < fEmitStop
                EmitParticles(fPSysLastTime + fDeltaTime, // fTime
                    fScaledTime, // fCurTime
                    fLastScaledTime, // fLastTime
                    0.0f, // fEmitStart
                    fScaledTime, // fEmitStop
                    fBirthRate); // fBirthRate
            }
            return;
        }

        // The birth rate interpolator is not posed. Get the key arrays and
        // begin and end key times here.
        unsigned int uiNumBirthRateKeys;
        NiFloatKey::KeyType eBirthRateKeyType;
        unsigned char ucBirthRateSize;
        NiFloatKey* pkBirthRateKeys = pkFloatInterp->GetKeys(
            uiNumBirthRateKeys, eBirthRateKeyType, ucBirthRateSize);

        NiFloatKey* pkBeginBirthRateKey = pkBirthRateKeys->GetKeyAt(0,
            ucBirthRateSize);
        float fBeginKeyTime = pkBeginBirthRateKey->GetTime();

        NiFloatKey* pkEndBirthRateKey = pkBirthRateKeys->GetKeyAt(
            uiNumBirthRateKeys - 1, ucBirthRateSize);
        float fEndKeyTime = pkEndBirthRateKey->GetTime();

        // The birth rate is the halfway point between the current time
        // and the last time. However, if we have looped in time, we 
        // need to emit from the last time to the end of the animation
        float fLoopDeltaTime = 0.0f;
        if (fLastScaledTime > fScaledTime)
        {   
            pkFloatInterp->Update(fLastScaledTime + 
                (fEndKeyTime - fLastScaledTime) * 0.5f, m_pkTarget,
                fBirthRate);
            
            fLoopDeltaTime = fEndKeyTime - fLastScaledTime;

            // Emit Particles requires that the data obey the following 
            // constraints:
            //  fBirthRate > 0.0f
            //  fEmitStart < fEmitStop 
            //  fCurTime > fEmitStart
            //  fLastTime < fEmitStop
            EmitParticles(fPSysLastTime + fLoopDeltaTime, //fTime
                fEndKeyTime, //fCurTime
                fLastScaledTime, // fLastTime
                fBeginKeyTime, //fEmitStart
                fEndKeyTime, // fEmitStop
                fBirthRate); // fBirthRate

            fLastScaledTime = 
                pkBirthRateKeys->GetKeyAt(0, ucBirthRateSize)->GetTime();
        }
        
        NIVERIFY(pkFloatInterp->Update(fLastScaledTime + (fScaledTime -
                fLastScaledTime) * 0.5f, m_pkTarget, fBirthRate));

        // Emit Particles requires that the data obey the following 
        // constraints:
        //  fBirthRate > 0.0f
        //  fEmitStart < fEmitStop 
        //  fCurTime > fEmitStart
        //  fLastTime < fEmitStop
        EmitParticles(fPSysLastTime + (fScaledTime - fLastScaledTime) + 
            fLoopDeltaTime, //fTime
            fScaledTime, //fCurTime
            fLastScaledTime, //fLastTime
            fBeginKeyTime, //fEmitStart
            fScaledTime, // fEmitStop
            fBirthRate); // fBirthRate
        return;
    }

    // Get emitter active value for fScaledTime;
    unsigned int uiLastEmitterActiveIdx = 0;
    bool bCurActive = NiBoolKey::GenInterp(fScaledTime, pkEmitterActiveKeys,
        eEmitterActiveKeyType, uiNumEmitterActiveKeys,
        uiLastEmitterActiveIdx, ucEmitterActiveSize);
    
    // If the last scaled time has reset or this is the first time through
    // we need to skip this update call b/c we need a time range to emit
    // particles.
    if (m_fLastScaledTime == -NI_INFINITY)
    {
        m_fLastScaledTime = fScaledTime;
        m_bLastActive = bCurActive;

        // Since this is the first update, don't do anything.
        return;
    }

    // Set the stored last values here so we can return from the function
    // whenever we want in the ensuing logic.
    float fLastScaledTime = m_fLastScaledTime;
    bool bLastActive = m_bLastActive;
    m_fLastScaledTime = fScaledTime;
    m_bLastActive = bCurActive;

    // Now we get to the meat of the computation. Because the emitter
    // can start and stop an arbitrary number of times between the last
    // time and the current time and we may have looped in that range
    // we need to walk through the key array. We call "EmitParticles" 
    // for each paired set of true/false EmitterActive keys. The birth rate
    // is the midpoint of time between the keys if the time range includes
    // the full start/stop. If the time range does not include the full 
    // start/stop, the birth rate is the midpoint between the last time
    // and the current time. We loop until we have covered the entire
    // time range encompassed by fLastScaledTime to fScaledTime.
    bool bDone = false;
    while (!bDone)
    {
        // Detect loop.
        bool bHasLooped = false;
        if (fLastScaledTime > fScaledTime)
        {
            bHasLooped = true;
        }

        // Determine start key.
        // If the last time through the while loop we were active, it means 
        // that a start key occured prior to the last scaled time. If 
        // we were not active, it means that we need to look for
        // the next start key.
        unsigned int uiStartKey;
        if (bLastActive)
        {
            // Find start key before fLastScaledTime.
            for (uiStartKey = uiNumEmitterActiveKeys; uiStartKey > 0;
                uiStartKey--)
            {
                NiBoolKey* pkKey = pkEmitterActiveKeys->GetKeyAt(
                    uiStartKey - 1, ucEmitterActiveSize);
                if (pkKey->GetTime() <= fLastScaledTime && !pkKey->GetBool())
                {
                    break;
                }
            }
        }
        else
        {
            // Find start key after fLastScaledTime.
            for (uiStartKey = 0; uiStartKey < uiNumEmitterActiveKeys;
                uiStartKey++)
            {
                NiBoolKey* pkKey = pkEmitterActiveKeys->GetKeyAt(uiStartKey,
                    ucEmitterActiveSize);
                if (pkKey->GetTime() > fLastScaledTime &&
                    (bHasLooped || pkKey->GetTime() <= fScaledTime) &&
                    pkKey->GetBool())
                {
                    break;
                }
            }
            if (uiStartKey == uiNumEmitterActiveKeys)
            {
                if (bHasLooped)
                {
                    // Reached the end of the key array. Find the first key
                    // with a value of true in the array. If we reach the
                    // current time, then bail out, there is nothing else
                    // to do this update.
                    for (uiStartKey = 0; uiStartKey< uiNumEmitterActiveKeys;
                        uiStartKey++)
                    {
                        NiBoolKey* pkKey = pkEmitterActiveKeys->GetKeyAt(
                            uiStartKey, ucEmitterActiveSize);
                        if (pkKey->GetTime() < fScaledTime &&
                            pkKey->GetBool())
                        {
                            break;
                        }
                    }

                    // If no start key was found, bail out.
                    if (uiStartKey == uiNumEmitterActiveKeys)
                    {
                        return;
                    }
                }
                else
                {
                    // No active keys between fLastScaledTime and fScaledTime.
                    // Don't emit any particles this frame.
                    return;
                }
            }
        }

        // Now that we have located the start key, we need to 
        // locate the next stop key and emit particles.
        unsigned int uiStopKey;
        for (uiStopKey = uiStartKey; uiStopKey < uiNumEmitterActiveKeys;
            uiStopKey++)
        {
            NiBoolKey* pkKey = pkEmitterActiveKeys->GetKeyAt(uiStopKey, 
                ucEmitterActiveSize);
            if (!pkKey->GetBool()) // We have the stop key
            {
                break;
            }
        }

        // If we have not found a stop key, we just use the last 
        // key in the array as a stop key. 
        if (uiStopKey == uiNumEmitterActiveKeys)
        {
            uiStopKey--;
        }

        // Grab the keys now that we have the indices.
        NiBoolKey* pkStartKey = pkEmitterActiveKeys->GetKeyAt(uiStartKey,
            ucEmitterActiveSize);
        NiBoolKey* pkStopKey = pkEmitterActiveKeys->GetKeyAt(uiStopKey,
            ucEmitterActiveSize);

        float fCurTime, fLastTime, fEmitStart, fEmitStop, fBirthRate;

        // Now we need to calculate the birth rate we are going to use in
        // this iteration of the while loop.  Check if the time range
        // indicates that the keys have looped.
        if (bHasLooped)
        {
            // If the stop key that was found is later than the
            // last scaled time, we have not officially looped through 
            // the key array yet.
            if (pkStopKey->GetTime() > fLastScaledTime)
            {
                fCurTime = pkEmitterActiveKeys->GetKeyAt(
                    uiNumEmitterActiveKeys - 1, ucEmitterActiveSize)
                    ->GetTime();
                fLastTime = fLastScaledTime;
                fEmitStart = pkStartKey->GetTime();
                fEmitStop = pkStopKey->GetTime();

                NIVERIFY(pkFloatInterp->Update(fEmitStart +
                    (fEmitStop - fEmitStart) * 0.5f, m_pkTarget, fBirthRate));
            }
            else
            {
                // If we have officially looped through the key array,
                // just consider the birth rate range from the 
                // first key to the current stop key.
                fCurTime = fScaledTime;
                fLastTime = pkEmitterActiveKeys->GetKeyAt(0, 
                    ucEmitterActiveSize)->GetTime();
                fEmitStart = pkStartKey->GetTime();
                if (pkStopKey->GetTime() < fScaledTime)
                {
                    fEmitStop = pkStopKey->GetTime();
                }
                else
                {
                    fEmitStop = fScaledTime;
                }

                NIVERIFY(pkFloatInterp->Update(fEmitStart +
                    (fEmitStop - fEmitStart) * 0.5f, m_pkTarget, fBirthRate));
            }
        }
        else
        {
            // If this start/stop pair doesn't go past the 
            // current time range, get the birth rate by evaluating the
            // midpoint between the start/stop keys. Otherwise,
            // just get the birth rate at the current time.
            if (pkStopKey->GetTime() < fScaledTime)
            {
                fCurTime = pkStopKey->GetTime();
                fLastTime = fLastScaledTime;
                fEmitStart = pkStartKey->GetTime();
                fEmitStop = pkStopKey->GetTime();

                NIVERIFY(pkFloatInterp->Update(fEmitStart +
                    (fEmitStop - fEmitStart) * 0.5f, m_pkTarget, fBirthRate));
            }
            else
            {
                fCurTime = fScaledTime;
                fLastTime = fLastScaledTime;
                fEmitStart = pkStartKey->GetTime();
                fEmitStop = pkStopKey->GetTime();

                NIVERIFY(pkFloatInterp->Update(fScaledTime, m_pkTarget,
                    fBirthRate));
            }
        }

        // We have to parameterize the current time relative to the particle 
        // system's notion of time.
        float fPSysLastTime = ((NiParticleSystem*)m_pkTarget)->GetLastTime();
        if (bHasLooped)
            fPSysLastTime += m_fHiKeyTime - m_fLoKeyTime;

        // Emit Particles requires that the data obey the following 
        // constraints:
        //  fBirthRate > 0.0f
        //  fEmitStart < fEmitStop 
        //  fCurTime > fEmitStart
        //  fLastTime < fEmitStop
        EmitParticles(fPSysLastTime + fScaledTime - fLastScaledTime, 
            fCurTime, fLastTime, fEmitStart, fEmitStop, fBirthRate);

        // Set up the next loop. If we didn't go past the end 
        // of the time range this time around,
        // just slide the last scaled time up to the current time for
        // the next loop. 
        if (pkStopKey->GetTime() < fScaledTime)
        {
            fLastScaledTime = fCurTime;
            bLastActive = NiBoolKey::GenInterp(fLastScaledTime,
                pkEmitterActiveKeys, eEmitterActiveKeyType, 
                uiNumEmitterActiveKeys, uiLastEmitterActiveIdx, 
                ucEmitterActiveSize);
        }
        else
        {
            bDone = true;
        }
    }
}
//---------------------------------------------------------------------------
void NiPSysEmitterCtlr::EmitParticles(float fTime, float fCurTime,
    float fLastTime, float fEmitStart, float fEmitStop, float fBirthRate)
{
    if (fBirthRate > 0.0f && fEmitStart < fEmitStop && 
        fCurTime > fEmitStart && fLastTime < fEmitStop)
    {
        float fCurDelta;
        if (fCurTime <= fEmitStop)
        {
            fCurDelta = fCurTime - fEmitStart;
        }
        else
        {
            fCurDelta = fEmitStop - fEmitStart;
        }

        float fLastDelta = 0.0f;
        if (fLastTime >= fEmitStart)
        {
            fLastDelta = fLastTime - fEmitStart;
        }

        unsigned short usCurNumParticles = (unsigned short) (fBirthRate *
            fCurDelta);
        unsigned short usLastNumParticles = (unsigned short) (fBirthRate *
            fLastDelta);
        unsigned short usNewParticles = usCurNumParticles -
            usLastNumParticles;
        float fParticleAge = 1.0f / fBirthRate;

        m_kParticleAges.RemoveAll();
        for (unsigned short us = 0; us < usNewParticles; us++)
        {
            m_kParticleAges.Add(fCurDelta - (++usLastNumParticles *
                fParticleAge));
        }

        ((NiPSysEmitter*) m_pkModifier)->EmitParticles(fTime,
            usNewParticles, m_kParticleAges.GetBase());
    }
}
//---------------------------------------------------------------------------
bool NiPSysEmitterCtlr::TargetIsRequiredType() const
{
    if (!NiPSysModifierCtlr::TargetIsRequiredType())
    {
        return false;
    }

    NiPSysModifier* pkModifier = ((NiParticleSystem*) m_pkTarget)
        ->GetModifierByName(m_kModifierName);
    NIASSERT(pkModifier);

    return NiIsKindOf(NiPSysEmitter, pkModifier);
}
//---------------------------------------------------------------------------
bool NiPSysEmitterCtlr::InterpolatorIsCorrectType(NiInterpolator * pkInterp,
    unsigned short usIndex) const
{
    if (usIndex == BIRTH_RATE_INDEX && 
        (NiIsKindOf(NiFloatInterpolator, pkInterp) ||
        NiIsKindOf(NiBlendFloatInterpolator, pkInterp)))
    {
        return true;
    }
    else if (usIndex == EMITTER_ACTIVE_INDEX && 
        (NiIsKindOf(NiBoolInterpolator, pkInterp) ||
        NiIsKindOf(NiBlendBoolInterpolator, pkInterp)))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiPSysEmitterCtlr::InterpTargetIsCorrectType(NiObjectNET * pkTarget)
    const
{
    if (!NiPSysModifierCtlr::InterpTargetIsCorrectType(pkTarget))
    {
        return false;
    }
    NiPSysModifier* pkModifier = ((NiParticleSystem*) pkTarget)
        ->GetModifierByName(m_kModifierName);
    NIASSERT(pkModifier);

    return NiIsKindOf(NiPSysEmitter, pkModifier);
}
//---------------------------------------------------------------------------
void NiPSysEmitterCtlr::GuaranteeTimeRange(float fStartTime, float fEndTime)
{
    NiPSysModifierCtlr::GuaranteeTimeRange(fStartTime, fEndTime);

    if (m_spEmitterActiveInterpolator)
    {
        m_spEmitterActiveInterpolator->GuaranteeTimeRange(fStartTime,
            fEndTime);
    }
}
//---------------------------------------------------------------------------
void NiPSysEmitterCtlr::ResetTimeExtrema()
{
    if (GetManagerControlled())
    {
        // Do nothing if this controller is being controlled by a manager.
        return;
    }

    m_spInterpolator->GetActiveTimeRange(m_fLoKeyTime, m_fHiKeyTime);

    float fLoKeyTime, fHiKeyTime;
    m_spEmitterActiveInterpolator->GetActiveTimeRange(fLoKeyTime,
        fHiKeyTime);
    if (fLoKeyTime < m_fLoKeyTime)
    {
        m_fLoKeyTime = fLoKeyTime;
    }
    if (fHiKeyTime > m_fHiKeyTime)
    {
        m_fHiKeyTime = fHiKeyTime;
    }
}
//---------------------------------------------------------------------------
unsigned short NiPSysEmitterCtlr::GetInterpolatorCount() const
{
    return 2;
}
//---------------------------------------------------------------------------
const char* NiPSysEmitterCtlr::GetInterpolatorID(unsigned short usIndex)
{
    NIASSERT(usIndex < 2);

    if (usIndex == BIRTH_RATE_INDEX)
    {
        return ms_pcBirthRate;
    }
    else if (usIndex == EMITTER_ACTIVE_INDEX)
    {
        return ms_pcEmitterActive;
    }

    return NULL;
}
//---------------------------------------------------------------------------
unsigned short NiPSysEmitterCtlr::GetInterpolatorIndex(const char* pcID) const
{
    if (NiStricmp(pcID, ms_pcBirthRate) == 0)
    {
        return BIRTH_RATE_INDEX;
    }
    else if (NiStricmp(pcID, ms_pcEmitterActive) == 0)
    {
        return EMITTER_ACTIVE_INDEX;
    }

    return INVALID_INDEX;
}
//---------------------------------------------------------------------------
NiInterpolator* NiPSysEmitterCtlr::GetInterpolator(unsigned short usIndex)
    const
{
    if (usIndex == BIRTH_RATE_INDEX)
    {
        return m_spInterpolator;
    }
    else if (usIndex == EMITTER_ACTIVE_INDEX)
    {
        return m_spEmitterActiveInterpolator;
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiPSysEmitterCtlr::SetInterpolator(NiInterpolator* pkInterpolator,
    unsigned short usIndex)
{
    if (usIndex == BIRTH_RATE_INDEX)
    {
        NIASSERT(!pkInterpolator || InterpolatorIsCorrectType(pkInterpolator,
            usIndex));
        m_spInterpolator = pkInterpolator;
    }
    else if (usIndex == EMITTER_ACTIVE_INDEX)
    {
        NIASSERT(!pkInterpolator || InterpolatorIsCorrectType(pkInterpolator,
            usIndex));
        m_spEmitterActiveInterpolator = pkInterpolator;
    }
}
//---------------------------------------------------------------------------
NiInterpolator* NiPSysEmitterCtlr::CreatePoseInterpolator(
    unsigned short usIndex)
{
    if (usIndex == BIRTH_RATE_INDEX)
    {
        return NiNew NiFloatInterpolator(0.0f);
    }
    else if (usIndex == EMITTER_ACTIVE_INDEX)
    {
        return NiNew NiBoolInterpolator(false);
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiPSysEmitterCtlr::SynchronizePoseInterpolator(
    NiInterpolator* pkInterp, unsigned short usIndex)
{
    // Do nothing here... we don't support this operation.
}
//---------------------------------------------------------------------------
NiBlendInterpolator* NiPSysEmitterCtlr::CreateBlendInterpolator(
    unsigned short usIndex, bool bManagerControlled,
    bool bAccumulateAnimations, float fWeightThreshold,
    unsigned char ucArraySize) const
{
    if (usIndex == BIRTH_RATE_INDEX)
    {
        return NiNew NiBlendFloatInterpolator(bManagerControlled, 
            fWeightThreshold, ucArraySize);
    }
    else if (usIndex == EMITTER_ACTIVE_INDEX)
    {
        return NiNew NiBlendBoolInterpolator(bManagerControlled, 
            fWeightThreshold, ucArraySize);
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiPSysEmitterCtlr::SetTargetData()
{
    // Do nothing here... we don't support this operation.
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSysEmitterCtlr);
//---------------------------------------------------------------------------
void NiPSysEmitterCtlr::CopyMembers(NiPSysEmitterCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSysModifierCtlr::CopyMembers(pkDest, kCloning);

    // NOTE.  By calling CreateClone() rather than passing m_spData
    // directly, this allows any change in what "clone" means to be
    // encapsulated by the CreateClone() call and does not require changing
    // the code here.
    if (m_spEmitterActiveInterpolator)
    {
        pkDest->m_spEmitterActiveInterpolator = ((NiInterpolator*) 
            m_spEmitterActiveInterpolator->CreateClone(kCloning));
    }
}
//---------------------------------------------------------------------------
void NiPSysEmitterCtlr::ProcessClone(NiCloningProcess& kCloning)
{
    NiPSysModifierCtlr::ProcessClone(kCloning);

    if (m_spEmitterActiveInterpolator)
    {
        m_spEmitterActiveInterpolator->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSysEmitterCtlr);
//---------------------------------------------------------------------------
void NiPSysEmitterCtlr::LoadBinary(NiStream& kStream)
{
    NiPSysModifierCtlr::LoadBinary(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        kStream.ReadLinkID();   // m_spData
        return;
    }
    kStream.ReadLinkID();       //m_spEmitterActiveInterpolator
        
}
//---------------------------------------------------------------------------
void NiPSysEmitterCtlr::LinkObject(NiStream& kStream)
{
    NiPSysModifierCtlr::LinkObject(kStream);

    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 104))
    {
        NiPSysEmitterCtlrDataPtr spData = (NiPSysEmitterCtlrData*) 
            kStream.GetObjectFromLinkID();
        if (spData)
        {
            NiFloatInterpolator* pkFloatInterp = NiNew NiFloatInterpolator();
            unsigned int uiNumKeys;
            NiFloatKey::KeyType eBirthRateKeyType;
            unsigned char ucSize;
            NiFloatKey* pkBirthRateKeys = spData->GetBirthRateKeys(uiNumKeys,
                eBirthRateKeyType, ucSize);
            pkFloatInterp->ReplaceKeys(pkBirthRateKeys, uiNumKeys, 
                eBirthRateKeyType);
            spData->SetBirthRateKeys(NULL, 0, NiFloatKey::NOINTERP); 
            SetInterpolator(pkFloatInterp, BIRTH_RATE_INDEX);
            
            NiBoolInterpolator* pkBoolInterp = NiNew NiBoolInterpolator();
            
            NiBoolKey::KeyType eEmitterActiveKeyType;
            NiBoolKey* pkEmitterActiveKeys = 
                spData->GetEmitterActiveKeys(uiNumKeys, 
                eEmitterActiveKeyType, ucSize);
            pkBoolInterp->ReplaceKeys(pkEmitterActiveKeys,
                eEmitterActiveKeyType, uiNumKeys);
            spData->SetEmitterActiveKeys(NULL, 0, NiAnimationKey::NOINTERP); 
            SetInterpolator(pkBoolInterp, EMITTER_ACTIVE_INDEX);
        }
        return;
    }
    m_spEmitterActiveInterpolator = (NiInterpolator*) 
        kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPSysEmitterCtlr::RegisterStreamables(NiStream& kStream)
{
    if (!NiPSysModifierCtlr::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spEmitterActiveInterpolator)
         m_spEmitterActiveInterpolator->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPSysEmitterCtlr::SaveBinary(NiStream& kStream)
{
    NiPSysModifierCtlr::SaveBinary(kStream);

    kStream.SaveLinkID(m_spEmitterActiveInterpolator);
}
//---------------------------------------------------------------------------
bool NiPSysEmitterCtlr::IsEqual(NiObject* pkObject)
{
    if (!NiPSysModifierCtlr::IsEqual(pkObject))
    {
        return false;
    }

    NiPSysEmitterCtlr* pkDest = (NiPSysEmitterCtlr*) pkObject;

    if ((pkDest->m_spEmitterActiveInterpolator == NULL &&
        m_spEmitterActiveInterpolator != NULL) ||
        (pkDest->m_spEmitterActiveInterpolator != NULL &&
        m_spEmitterActiveInterpolator == NULL))
    {
        return false;
    }
    else if (pkDest->m_spEmitterActiveInterpolator &&
        !pkDest->m_spEmitterActiveInterpolator->IsEqual(
        m_spEmitterActiveInterpolator))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSysEmitterCtlr::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSysModifierCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSysEmitterCtlr::ms_RTTI.GetName()));

    if (m_spEmitterActiveInterpolator)
    {
        m_spEmitterActiveInterpolator->GetViewerStrings(pkStrings);
    }
}
//---------------------------------------------------------------------------
