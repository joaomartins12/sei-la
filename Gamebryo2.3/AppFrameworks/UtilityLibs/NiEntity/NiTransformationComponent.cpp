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
#include "NiEntityPCH.h"

#include "NiTransformationComponent.h"
#include "NiEntityErrorInterface.h"

NiFixedString NiTransformationComponent::ms_kClassName;
NiFixedString NiTransformationComponent::ms_kComponentName;
NiFixedString NiTransformationComponent::ms_kTranslationName;
NiFixedString NiTransformationComponent::ms_kRotationName;
NiFixedString NiTransformationComponent::ms_kScaleName;
NiFixedString NiTransformationComponent::ms_kTranslationDescription;
NiFixedString NiTransformationComponent::ms_kRotationDescription;
NiFixedString NiTransformationComponent::ms_kScaleDescription;

//---------------------------------------------------------------------------
void NiTransformationComponent::_SDMInit()
{
    ms_kClassName = "NiTransformationComponent";
    ms_kComponentName = "Transformation";
    ms_kTranslationName = "Translation";
    ms_kRotationName = "Rotation";
    ms_kScaleName = "Scale";
    ms_kTranslationDescription = "The world-space position of the entity.";
    ms_kRotationDescription = "The world-space orientation of the entity.";
    ms_kScaleDescription = "The world-space scale of the entity.";
}
//---------------------------------------------------------------------------
void NiTransformationComponent::_SDMShutdown()
{
    ms_kClassName = NULL;
    ms_kComponentName = NULL;
    ms_kTranslationName = NULL;
    ms_kRotationName = NULL;
    ms_kScaleName = NULL;
    ms_kTranslationDescription = NULL;
    ms_kRotationDescription = NULL;
    ms_kScaleDescription = NULL;
}
//---------------------------------------------------------------------------
// NiEntityComponentInterface overrides.
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiTransformationComponent::Clone(
    bool bInheritProperties)
{
    return NiNew NiTransformationComponent(m_kTranslation, m_kRotation,
        m_fScale);
}
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiTransformationComponent::GetMasterComponent()
{
    // This component does not have a master component.
    return NULL;
}
//---------------------------------------------------------------------------
void NiTransformationComponent::SetMasterComponent(
    NiEntityComponentInterface* pkMasterComponent)
{
    // This component does not have a master component.
}
//---------------------------------------------------------------------------
void NiTransformationComponent::GetDependentPropertyNames(
    NiTObjectSet<NiFixedString>& kDependentPropertyNames)
{
    // This component has no dependent properties.
}
//---------------------------------------------------------------------------
// NiEntityPropertyInterface overrides.
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetTemplateID(const NiUniqueID& kID)
{
    //Not supported for custom components
    return false;
}
//---------------------------------------------------------------------------
NiUniqueID  NiTransformationComponent::GetTemplateID()
{
    static const NiUniqueID kUniqueID = 
        NiUniqueID(0x70,0x76,0x3E,0x14,0x2B,0x83,0x2,0x4D,0xAE,0xE1,0x76,0x99,
        0x64,0x93,0x22,0xF2);
    return kUniqueID;
}
//---------------------------------------------------------------------------
void NiTransformationComponent::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiTransformationComponent::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiTransformationComponent::GetClassName() const
{
    return ms_kClassName;
}
//---------------------------------------------------------------------------
NiFixedString NiTransformationComponent::GetName() const
{
    return ms_kComponentName;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetName(const NiFixedString& kName)
{
    // This component does not allow its name to be set.
    return false;
}
//---------------------------------------------------------------------------
void NiTransformationComponent::Update(
    NiEntityPropertyInterface* pkParentEntity, float fTime,
    NiEntityErrorInterface* pkErrors,
    NiExternalAssetManager* pkAssetManager)
{
    // This component has no work to do during Update.
}
//---------------------------------------------------------------------------
void NiTransformationComponent::BuildVisibleSet(
    NiEntityRenderingContext* pkRenderingContext,
    NiEntityErrorInterface* pkErrors)
{
    // This component has no work to do during BuildVisibleSet.
}
//---------------------------------------------------------------------------
void NiTransformationComponent::GetPropertyNames(
    NiTObjectSet<NiFixedString>& kPropertyNames) const
{
    kPropertyNames.Add(ms_kTranslationName);
    kPropertyNames.Add(ms_kRotationName);
    kPropertyNames.Add(ms_kScaleName);
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::CanResetProperty(
    const NiFixedString& kPropertyName, bool& bCanReset) const
{
    if (kPropertyName == ms_kTranslationName ||
        kPropertyName == ms_kRotationName ||
        kPropertyName == ms_kScaleName)
    {
        bCanReset = false;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::ResetProperty(
    const NiFixedString& kPropertyName)
{
    // No properties can be reset.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::MakePropertyUnique(
    const NiFixedString& kPropertyName, bool& bMadeUnique)
{
    bool bCanReset;
    if (!CanResetProperty(kPropertyName, bCanReset))
    {
        return false;
    }

    // No properties are inherited, so they are all already unique.
    bMadeUnique = false;

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetDisplayName(
    const NiFixedString& kPropertyName, NiFixedString& kDisplayName) const
{
    if (kPropertyName == ms_kTranslationName)
    {
        kDisplayName = ms_kTranslationName;
    }
    else if (kPropertyName == ms_kRotationName)
    {
        kDisplayName = ms_kRotationName;
    }
    else if (kPropertyName == ms_kScaleName)
    {
        kDisplayName = ms_kScaleName;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetDisplayName(
    const NiFixedString& kPropertyName, const NiFixedString& kDisplayName)
{
    // This component does not allow the display name to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetPrimitiveType(
    const NiFixedString& kPropertyName, NiFixedString& kPrimitiveType) const
{
    if (kPropertyName == ms_kTranslationName)
    {
        kPrimitiveType = PT_POINT3;
    }
    else if (kPropertyName == ms_kRotationName)
    {
        kPrimitiveType = PT_MATRIX3;
    }
    else if (kPropertyName == ms_kScaleName)
    {
        kPrimitiveType = PT_FLOAT;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetPrimitiveType(
    const NiFixedString& kPropertyName, const NiFixedString& kPrimitiveType)
{
    // This component does not allow the primitive type to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetSemanticType(
    const NiFixedString& kPropertyName, NiFixedString& kSemanticType) const
{
    if (kPropertyName == ms_kTranslationName)
    {
        kSemanticType = PT_POINT3;
    }
    else if (kPropertyName == ms_kRotationName)
    {
        kSemanticType = PT_MATRIX3;
    }
    else if (kPropertyName == ms_kScaleName)
    {
        kSemanticType = PT_FLOAT;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetSemanticType(
    const NiFixedString& kPropertyName, const NiFixedString& kSemanticType)
{
    // This component does not allow the semantic type to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetDescription(
    const NiFixedString& kPropertyName, NiFixedString& kDescription) const
{
    if (kPropertyName == ms_kTranslationName)
    {
        kDescription = ms_kTranslationDescription;
    }
    else if (kPropertyName == ms_kRotationName)
    {
        kDescription = ms_kRotationDescription;
    }
    else if (kPropertyName == ms_kScaleName)
    {
        kDescription = ms_kScaleDescription;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetDescription(
    const NiFixedString& kPropertyName, const NiFixedString& kDescription)
{
    // This component does not allow the description to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetCategory(
    const NiFixedString& kPropertyName, NiFixedString& kCategory) const
{
    if (kPropertyName == ms_kTranslationName ||
        kPropertyName == ms_kRotationName ||
        kPropertyName == ms_kScaleName)
    {
        kCategory = ms_kComponentName;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::IsPropertyReadOnly(
    const NiFixedString& kPropertyName, bool& bIsReadOnly)
{
    if (kPropertyName == ms_kTranslationName ||
        kPropertyName == ms_kRotationName ||
        kPropertyName == ms_kScaleName)
    {
        bIsReadOnly = false;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::IsPropertyUnique(
    const NiFixedString& kPropertyName, bool& bIsUnique)
{
    if (kPropertyName == ms_kTranslationName ||
        kPropertyName == ms_kRotationName ||
        kPropertyName == ms_kScaleName)
    {
        bIsUnique = true;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::IsPropertySerializable(
    const NiFixedString& kPropertyName, bool& bIsSerializable)
{
    bool bIsUnique;
    NiBool bSuccess = IsPropertyUnique(kPropertyName, bIsUnique);
    if (bSuccess)
    {
        bool bIsReadOnly;
        bSuccess = IsPropertyReadOnly(kPropertyName, bIsReadOnly);
        NIASSERT(bSuccess);

        bIsSerializable = bIsUnique && !bIsReadOnly;
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::IsPropertyInheritable(
    const NiFixedString& kPropertyName, bool& bIsInheritable)
{
    if (kPropertyName == ms_kTranslationName ||
        kPropertyName == ms_kRotationName ||
        kPropertyName == ms_kScaleName)
    {
        bIsInheritable = false;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::IsExternalAssetPath(
    const NiFixedString& kPropertyName, unsigned int uiIndex,
    bool& bIsExternalAssetPath) const
{
    if (kPropertyName == ms_kTranslationName ||
        kPropertyName == ms_kRotationName ||
        kPropertyName == ms_kScaleName)
    {
        bIsExternalAssetPath = false;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetElementCount(
    const NiFixedString& kPropertyName, unsigned int& uiCount) const
{
    if (kPropertyName == ms_kTranslationName ||
        kPropertyName == ms_kRotationName ||
        kPropertyName == ms_kScaleName)
    {
        uiCount = 1;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetElementCount(
    const NiFixedString& kPropertyName, unsigned int uiCount, bool& bCountSet)
{
    if (kPropertyName == ms_kTranslationName ||
        kPropertyName == ms_kRotationName ||
        kPropertyName == ms_kScaleName)
    {
        bCountSet = false;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::IsCollection(
    const NiFixedString& kPropertyName, bool& bIsCollection) const
{
    if (kPropertyName == ms_kTranslationName ||
        kPropertyName == ms_kRotationName ||
        kPropertyName == ms_kScaleName)
    {
        bIsCollection = false;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetPropertyData(
    const NiFixedString& kPropertyName, float& fData, unsigned int uiIndex)
    const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kScaleName)
    {
        fData = m_fScale;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetPropertyData(
    const NiFixedString& kPropertyName, float fData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kScaleName)
    {
       m_fScale = fData;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetPropertyData(
    const NiFixedString& kPropertyName, NiPoint3& kData, unsigned int uiIndex)
    const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kTranslationName)
    {
        kData = m_kTranslation;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetPropertyData(
    const NiFixedString& kPropertyName, const NiPoint3& kData,
    unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kTranslationName)
    {
        m_kTranslation = kData;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetPropertyData(
    const NiFixedString& kPropertyName, NiMatrix3& kData,
    unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kRotationName)
    {
        kData = m_kRotation;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetPropertyData(
    const NiFixedString& kPropertyName, const NiMatrix3& kData,
    unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kRotationName)
    {
        m_kRotation = kData;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
