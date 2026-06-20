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

#include "NiMaterialLibrary.h"
#include "NiMaterial.h"

NiTPrimitiveSet<NiMaterialLibrary*> NiMaterialLibrary::ms_kMaterialLibraries;

//---------------------------------------------------------------------------
NiMaterialLibrary::NiMaterialLibrary(const NiFixedString& kName)
{
    SetName(kName);
    NIASSERT(GetMaterialLibrary(kName) == NULL);
    ms_kMaterialLibraries.Add(this);
}
//---------------------------------------------------------------------------
NiMaterialLibrary::~NiMaterialLibrary()
{
    RemoveMaterialLibrary(this);
}
//---------------------------------------------------------------------------
const NiFixedString& NiMaterialLibrary::GetName()
{
    return m_kName;
}
//---------------------------------------------------------------------------
void NiMaterialLibrary::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------
NiMaterial* NiMaterialLibrary::GetMaterial(const NiFixedString& kName)
{
    // Default implementation returns 0
    return 0;
}
//---------------------------------------------------------------------------
const char* NiMaterialLibrary::GetMaterialName(unsigned int uiMaterial)
{
    return NULL;
}
//---------------------------------------------------------------------------
unsigned int NiMaterialLibrary::GetMaterialCount() const
{
    return 0;
}
//---------------------------------------------------------------------------
void NiMaterialLibrary::_SDMInit()
{
}
//---------------------------------------------------------------------------
void NiMaterialLibrary::_SDMShutdown()
{
    ms_kMaterialLibraries.RemoveAll();
    ms_kMaterialLibraries.Realloc();
    
#if defined(_USRDLL)
    FreeAllModules();
#endif
}
//---------------------------------------------------------------------------
NiMaterialLibrary* NiMaterialLibrary::GetMaterialLibrary(
    const NiFixedString& kName)
{
    for (unsigned int ui = 0; ui < ms_kMaterialLibraries.GetSize(); ui++)
    {
        NiMaterialLibrary* pkLibrary = ms_kMaterialLibraries.GetAt(ui);
        if (pkLibrary && pkLibrary->GetName() == kName)
            return pkLibrary;
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiMaterial* NiMaterialLibrary::CreateMaterial(const NiFixedString& kName)
{
    NiMaterial* pkMaterial = NiMaterial::GetMaterial(kName);

    if (pkMaterial)
        return pkMaterial;

    for (unsigned int ui = 0; ui < GetMaterialLibraryCount(); ui++)
    {
        NiMaterialLibrary* pkLibrary = GetMaterialLibrary(ui);
        if (pkLibrary)
        {
            for (unsigned int ui = 0; ui < pkLibrary->GetMaterialCount(); ui++)
            {
                if (pkLibrary->GetMaterialName(ui) == kName)
                {
                    return pkLibrary->GetMaterial(kName);
                }
            }
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiMaterialLibrary* NiMaterialLibrary::GetMaterialLibrary(
    unsigned int uiLibrary)
{
    return ms_kMaterialLibraries.GetAt(uiLibrary);
}
//---------------------------------------------------------------------------
unsigned int NiMaterialLibrary::GetMaterialLibraryCount()
{
    return ms_kMaterialLibraries.GetSize();
}
//---------------------------------------------------------------------------
void NiMaterialLibrary::RemoveMaterialLibrary(NiMaterialLibrary* pkLibrary)
{
    int iIndex = ms_kMaterialLibraries.Find(pkLibrary);
    if (iIndex >= 0)
        ms_kMaterialLibraries.RemoveAt(iIndex);
}
//---------------------------------------------------------------------------

