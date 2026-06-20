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

#include "NiRotKey.h"
#include <NiStream.h>

NiImplementAnimationStream(NiRotKey,ROTKEY,NOINTERP);
NiQuaternion NiRotKey::ms_kDefault = NiQuaternion::IDENTITY;

//---------------------------------------------------------------------------
void NiRotKey::RegisterSupportedFunctions(KeyContent eContent,
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
NiQuaternion NiRotKey::GenInterp(float fTime, NiRotKey* pkKeys,
    KeyType eType, unsigned int uiNumKeys, unsigned int& uiLastIdx,
    unsigned char ucSize)
{
    NIASSERT(uiNumKeys != 0);
    if (uiNumKeys == 1)
    {
        if (eType != EULERKEY)
            return pkKeys->GetKeyAt(0, ucSize)->GetQuaternion();
    }

    // If rotation keys are specified as Euler angles then execution is
    // directed to another routine due to the special requirements in
    // handling Euler angles.
    if ( eType == EULERKEY )
    {
        NiRotKey::InterpFunction interp = NiRotKey::GetInterpFunction(eType);
        NIASSERT( interp );
        NiQuaternion kQuat;
        interp(fTime, pkKeys->GetKeyAt(0, ucSize), 0, &kQuat);
        return kQuat;
    }

    unsigned int uiNumKeysM1 = uiNumKeys - 1;

    // This code assumes that the time values in the keys are ordered by
    // increasing value.  The search can therefore begin at uiLastIdx rather
    // than zero each time.  The idea is to provide an O(1) lookup based on
    // time coherency of the keys.

    float fLastTime = pkKeys->GetKeyAt(uiLastIdx, ucSize)->GetTime();
    if ( fTime < fLastTime )
    {
        uiLastIdx = 0;
        fLastTime = pkKeys->GetKeyAt(0, ucSize)->GetTime();
    }
    
    unsigned int uiNextIdx;
    float fNextTime = 0.0f;
    for (uiNextIdx = uiLastIdx + 1; uiNextIdx <= uiNumKeysM1; uiNextIdx++)
    {
        fNextTime = pkKeys->GetKeyAt(uiNextIdx, ucSize)->GetTime();
        if ( fTime <= fNextTime )
            break;

        uiLastIdx++;
        fLastTime = fNextTime;
    }

    NIASSERT(uiNextIdx < uiNumKeys);

    // interpolate the keys, requires that the time is normalized to [0,1]
    float fNormTime = (fTime - fLastTime)/(fNextTime - fLastTime);
    NiRotKey::InterpFunction interp = NiRotKey::GetInterpFunction(eType);
    NIASSERT( interp );
    NiQuaternion kQuat;
    interp(fNormTime, pkKeys->GetKeyAt(uiLastIdx, ucSize),
        pkKeys->GetKeyAt(uiNextIdx, ucSize), &kQuat);
    return kQuat;
}
//---------------------------------------------------------------------------
NiQuaternion NiRotKey::GenInterpDefault(float fTime, NiRotKey* pkKeys,
    KeyType eType, unsigned int uiNumKeys, unsigned char ucSize)
{
    if (uiNumKeys)
    {
        // GenInterp no longer handles out-of-range times
        if (fTime < pkKeys->GetKeyAt(0, ucSize)->GetTime())
            return pkKeys->GetKeyAt(0, ucSize)->GetQuaternion();
        else if (fTime > pkKeys->GetKeyAt(uiNumKeys-1, ucSize)->GetTime())
            return pkKeys->GetKeyAt(uiNumKeys-1, ucSize)->GetQuaternion();
        else
        {
            unsigned int uiLastIdx = 0;
            return NiRotKey::GenInterp(fTime, pkKeys, eType, uiNumKeys,
                uiLastIdx, ucSize);
        }
    }
    else
    {
        return ms_kDefault;
    }
}
//---------------------------------------------------------------------------
void NiRotKey::SetDefault(const NiQuaternion& kDefault)
{
    ms_kDefault = kDefault;
}
//---------------------------------------------------------------------------
void NiRotKey::Interpolate(float, const NiAnimationKey*, 
    const NiAnimationKey*, void* pResult)
{
    // This routine should never be called.
    NIASSERT( false );
    *(NiQuaternion*)pResult = NiQuaternion(1.0f,0.0f,0.0f,0.0f);
}
//---------------------------------------------------------------------------
bool NiRotKey::Equal(const NiAnimationKey& key0, const NiAnimationKey& key1)
{
    if ( !NiAnimationKey::Equal(key0,key1) )
        return false;

    const NiRotKey& rot0 = (const NiRotKey&) key0;
    const NiRotKey& rot1 = (const NiRotKey&) key1;

    return rot0.m_quat == rot1.m_quat;
}
//---------------------------------------------------------------------------
void NiRotKey::FillDerivedVals(NiAnimationKey* pkKeys,
    unsigned int uiNumKeys, unsigned char ucSize)
{
    // Eliminate any non-acute angles between successive quaternions.  This
    // is done to prevent potential discontinuities that are the result of
    // invalid intermediate value quaternions.
    unsigned int i;
    for (i = 0; i < uiNumKeys-1; i++)
    {
        NiRotKey* pkKey0 = (NiRotKey*) pkKeys->GetKeyAt(i, ucSize);
        NiRotKey* pkKey1 = (NiRotKey*) pkKeys->GetKeyAt(i+1, ucSize);
        if ( NiQuaternion::Dot(pkKey0->m_quat, pkKey1->m_quat) < 0.0f )
            pkKey1->m_quat = -pkKey1->m_quat;
    }

    // Clamp identity quaternions so that |w| <= 1 (avoids problems with
    // call to acos in SlerpExtraSpins).
    for (i = 0; i < uiNumKeys; i++)
    {
        NiRotKey* pkKey = (NiRotKey*) pkKeys->GetKeyAt(i, ucSize);
        float fW = pkKey->m_quat.GetW();
        if ( fW < -1.0f )
            pkKey->m_quat.SetW(-1.0f);
        else if ( fW > 1.0f )
            pkKey->m_quat.SetW(1.0f);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiAnimationKey* NiRotKey::CreateFromStream(NiStream&, unsigned int)
{
    // This routine should never be called.
    NIASSERT(false);
    return 0;
}
//---------------------------------------------------------------------------
void NiRotKey::LoadBinary(NiStream& stream)
{
    NiAnimationKey::LoadBinary(stream);

    m_quat.LoadBinary(stream);
}
//---------------------------------------------------------------------------
void NiRotKey::SaveToStream(NiStream& stream, NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    // This routine should never be called.
    NIASSERT(false);
}
//---------------------------------------------------------------------------
void NiRotKey::SaveBinary(NiStream& stream, NiAnimationKey* pkKey)
{
    NiAnimationKey::SaveBinary(stream, pkKey);

    NiRotKey* pkRot = (NiRotKey*) pkKey;
    pkRot->m_quat.SaveBinary(stream);
}
//---------------------------------------------------------------------------
