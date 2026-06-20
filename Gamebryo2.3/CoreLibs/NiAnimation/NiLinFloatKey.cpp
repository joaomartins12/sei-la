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

#include "NiInterpScalar.h"
#include "NiLinFloatKey.h"
#include <NiRTLib.h>
#include <NiStream.h>

NiImplementAnimationStream(NiLinFloatKey,FLOATKEY,LINKEY);

//---------------------------------------------------------------------------
void NiLinFloatKey::RegisterSupportedFunctions(KeyContent eContent,
    KeyType eType)
{
    SetCurvatureFunction(eContent, eType, NULL);
    SetInterpFunction(eContent, eType, Interpolate);
    SetInterpD1Function(eContent, eType, NULL);
    SetInterpD2Function(eContent, eType, NULL);
    SetEqualFunction(eContent, eType, Equal);
    SetFillDerivedValsFunction(eContent, eType, FillDerivedVals);
    SetInsertFunction(eContent, eType, Insert);
}
//---------------------------------------------------------------------------
void NiLinFloatKey::FillDerivedVals(NiAnimationKey*, unsigned int,
    unsigned char)
{
    // no derived values to fill in
}
//---------------------------------------------------------------------------
bool NiLinFloatKey::Equal(const NiAnimationKey& key0, 
    const NiAnimationKey& key1)
{
    return NiFloatKey::Equal(key0,key1);
}
//---------------------------------------------------------------------------
void NiLinFloatKey::Interpolate(float fTime, const NiAnimationKey* pKey0,
    const NiAnimationKey* pKey1, void* pResult)
{
    const NiLinFloatKey* pLin0 = (const NiLinFloatKey*) pKey0;
    const NiLinFloatKey* pLin1 = (const NiLinFloatKey*) pKey1;

    // interpolate between this key and pQ
    *(float*)pResult = NiInterpScalar::Linear(fTime, pLin0->GetValue(), 
        pLin1->GetValue());
}
//---------------------------------------------------------------------------
void NiLinFloatKey::Copy(NiAnimationKey* pkNewKey,
    const NiAnimationKey* pkOrigKey)
{
    NiLinFloatKey* pkNewLin = (NiLinFloatKey*) pkNewKey;
    NiLinFloatKey* pkOrigLin = (NiLinFloatKey*) pkOrigKey;

    pkNewLin->m_fTime = pkOrigLin->m_fTime;
    pkNewLin->m_fValue = pkOrigLin->m_fValue;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiLinFloatKey::CreateArray(unsigned int uiNumKeys)
{
    return NiNew NiLinFloatKey[uiNumKeys];
}
//---------------------------------------------------------------------------
void NiLinFloatKey::DeleteArray(NiAnimationKey* pkKeyArray)
{
    NiLinFloatKey* pkLinFloatKeyArray = (NiLinFloatKey*) pkKeyArray;
    NiDelete[] pkLinFloatKeyArray;
}
//---------------------------------------------------------------------------
bool NiLinFloatKey::Insert(float fTime, NiAnimationKey*& pkKeys, 
    unsigned int& uiNumKeys)
{
    NiLinFloatKey* pkLinFloatKeys = (NiLinFloatKey*) pkKeys;

    unsigned int uiDestSize;
    unsigned int uiInsertAt;
    if (OkayToInsert(fTime, pkLinFloatKeys, uiNumKeys, uiInsertAt,
        sizeof(NiLinFloatKey)))
    {
        NiLinFloatKey* pkNewKeys = NiNew NiLinFloatKey[uiNumKeys + 1];
        uiDestSize = (uiNumKeys + 1) * sizeof(NiLinFloatKey);
        NiMemcpy(pkNewKeys, uiDestSize, pkLinFloatKeys,
            uiInsertAt * sizeof(NiLinFloatKey));
        if (uiNumKeys > uiInsertAt)
        {
            uiDestSize = (uiNumKeys - uiInsertAt) * sizeof(NiLinFloatKey);
            NiMemcpy(&pkNewKeys[uiInsertAt + 1], &pkLinFloatKeys[uiInsertAt],
                uiDestSize);
        }

        float f = NiFloatKey::GenInterpDefault(fTime, pkLinFloatKeys,
            LINKEY, uiNumKeys, sizeof(NiLinFloatKey));

        NiLinFloatKey* pkNewKey = &pkNewKeys[uiInsertAt];
        pkNewKey->m_fTime = fTime;
        pkNewKey->m_fValue = f;

        uiNumKeys++;
        NiDelete[] pkLinFloatKeys;
        pkKeys = pkNewKeys;
        FillDerivedVals(pkKeys, uiNumKeys, sizeof(NiLinFloatKey));
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiAnimationKey* NiLinFloatKey::CreateFromStream(NiStream& stream,
    unsigned int uiNumKeys)
{
    NiLinFloatKey* pkLinFloatKeys = NiNew NiLinFloatKey[uiNumKeys];
    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        pkLinFloatKeys[ui].LoadBinary(stream);
    }
    return pkLinFloatKeys;
}
//---------------------------------------------------------------------------
void NiLinFloatKey::LoadBinary(NiStream& stream)
{
    NiFloatKey::LoadBinary(stream);
}
//---------------------------------------------------------------------------
void NiLinFloatKey::SaveToStream(NiStream& stream, NiAnimationKey* pkKeys,
    unsigned int uiNumKeys)
{
    NiLinFloatKey* pkLinFloatKeys = (NiLinFloatKey*) pkKeys;

    for (unsigned int ui = 0; ui < uiNumKeys; ui++)
    {
        SaveBinary(stream, &pkLinFloatKeys[ui]);
    }
}
//---------------------------------------------------------------------------
void NiLinFloatKey::SaveBinary(NiStream& stream, NiAnimationKey* pkKey)
{
    NiFloatKey::SaveBinary(stream, pkKey);
}
//---------------------------------------------------------------------------
