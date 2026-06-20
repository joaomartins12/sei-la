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

//---------------------------------------------------------------------------
inline unsigned int NiPhysXNxVec3HashFunctor::KeyToHashIndex(NxVec3* key,
    unsigned int uiTableSize)
{
    return (unsigned int)((unsigned int)key->x | (unsigned int)key->y
        | (unsigned int)key->z) % uiTableSize;
}
//---------------------------------------------------------------------------
inline bool NiPhysXNxVec3EqualsFunctor::IsKeysEqual(NxVec3* key1, NxVec3* key2)
{
    return (*key1) == (*key2);
}
//---------------------------------------------------------------------------
