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

#include <NiStream.h>
#include "NiFloatKey.h"

NiImplementAnimationStream(NiFloatKey,FLOATKEY,NOINTERP);
float NiFloatKey::ms_fDefault = 0.0f;

//---------------------------------------------------------------------------
void NiFloatKey::RegisterSupportedFunctions(KeyContent eContent,
    KeyType eType)
{
    SetCurvatureFunction(eContent, eType, NULL);
    SetInterpFunction(eContent, eType, Interpolate);
    SetInterpD1Function(eContent, eType, NULL);
    SetInterpD2Function(eContent, eType, NULL);
    SetEqualFunction(eContent, eType, Equal);
    SetFillDerivedValsFunction(eContent, eType, FillDerivedVals);
    SetInsertFunction(eContent, eType, NULL);
}
//---------------------------------------------------------------------------
float NiFloatKey::GenInterp(float fTime, NiFloatKey* pkKeys,
    NiFloatKey::KeyType eType, unsigned int uiNumKeys, unsigned int& uiLastIdx,
    unsigned char ucSize)
{
    NIASSERT(uiNumKeys != 0);
    if (uiNumKeys == 1)
        return pkKeys->GetKeyAt(0, ucSize)->GetValue();

    unsigned int uiNumKeysM1 = uiNumKeys - 1;

    // This code assumes that the time values in the keys are ordered by
    // increasing value.  The search can therefore begin at uiLastIdx rather
    // than zero each time.  The idea is to provide an O(1) lookup based on
    // time coherency of the keys.

    // Copy the last index to a stack variable here to ensure that each thread
    // has its own consistent copy of the value. The stack variable is copied
    // back to the reference variable at the end of this function.
    unsigned int uiStackLastIdx = uiLastIdx;

    float fLastTime = pkKeys->GetKeyAt(uiStackLastIdx, ucSize)->GetTime();
    if(fTime < fLastTime)
    {
        uiStackLastIdx = 0;
        fLastTime = pkKeys->GetKeyAt(0, ucSize)->GetTime();
    }

    unsigned int uiNextIdx;
    float fNextTime = 0.0f;
    for(uiNextIdx = uiStackLastIdx + 1; uiNextIdx <= uiNumKeysM1; uiNextIdx++)
    {
        fNextTime = pkKeys->GetKeyAt(uiNextIdx, ucSize)->GetTime();
        if(fTime <= fNextTime)
            break;

        uiStackLastIdx++;
        fLastTime = fNextTime;
    }
    
    NIASSERT(uiNextIdx < uiNumKeys);

    // interpolate the keys, requires that the time is normalized to [0,1]
    float fNormTime = (fTime - fLastTime)/(fNextTime - fLastTime);
    NiAnimationKey::InterpFunction interp = 
        NiFloatKey::GetInterpFunction(eType);
    NIASSERT(interp);
    float fReturn;
    interp(fNormTime, pkKeys->GetKeyAt(uiStackLastIdx, ucSize),
        pkKeys->GetKeyAt(uiNextIdx, ucSize), &fReturn);
    uiLastIdx = uiStackLastIdx;
    return fReturn;
}
//---------------------------------------------------------------------------
bool NiFloatKey::Equal(const NiAnimationKey& key0, const NiAnimationKey& key1)
{
    if ( !NiAnimationKey::Equal(key0,key1) )
        return false;

    const NiFloatKey& float0 = (const NiFloatKey&) key0;
    const NiFloatKey& float1 = (const NiFloatKey&) key1;

    return float0.m_fValue == float1.m_fValue;
}
//---------------------------------------------------------------------------
void NiFloatKey::Interpolate(float, const NiAnimationKey*, 
    const NiAnimationKey*, void*)
{
    // This routine should never be called.
    NIASSERT(false);
}
//---------------------------------------------------------------------------
float NiFloatKey::GenInterpDefault(float fTime, NiFloatKey* pkKeys,
    KeyType eType, unsigned int uiNumKeys, unsigned char ucSize)
{
    if (uiNumKeys)
    {
        // GenInterp no longer handles out-of-range times
        if (fTime < pkKeys->GetKeyAt(0, ucSize)->GetTime())
            return pkKeys->GetKeyAt(0, ucSize)->GetValue();
        else if (fTime > pkKeys->GetKeyAt(uiNumKeys-1, ucSize)->GetTime())
            return pkKeys->GetKeyAt(uiNumKeys-1, ucSize)->GetValue();
        else
        {
            unsigned int uiLastIdx = 0;
            return NiFloatKey::GenInterp(fTime, pkKeys, eType, uiNumKeys,
                uiLastIdx, ucSize);
        }
    }
    else
    {
        return ms_fDefault;
    }
}
//---------------------------------------------------------------------------
void NiFloatKey::SetDefault(float fDefault)
{
    ms_fDefault = fDefault;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiAnimationKey* NiFloatKey::CreateFromStream(NiStream&, unsigned int)
{
    // abstract class, cannot create an object dynamically
    NIASSERT(false);
    return 0;
}
//---------------------------------------------------------------------------
void NiFloatKey::LoadBinary(NiStream& stream)
{
    NiAnimationKey::LoadBinary(stream);

    NiStreamLoadBinary(stream, m_fValue);
}
//---------------------------------------------------------------------------
void NiFloatKey::SaveToStream(NiStream& stream, NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    // This function should never be called.
    NIASSERT(false);
}
//---------------------------------------------------------------------------
void NiFloatKey::SaveBinary(NiStream& stream, NiAnimationKey* pkKey)
{
    NiAnimationKey::SaveBinary(stream, pkKey);

    NiFloatKey* pkFloatKey = (NiFloatKey*) pkKey;
    NiStreamSaveBinary(stream, pkFloatKey->m_fValue);
}
//---------------------------------------------------------------------------
