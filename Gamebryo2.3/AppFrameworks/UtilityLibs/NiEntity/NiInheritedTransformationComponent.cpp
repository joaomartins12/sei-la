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

#include "NiInheritedTransformationComponent.h"
#include "NiEntityErrorInterface.h"
#include <NiAVObject.h>

NiFixedString NiInheritedTransformationComponent::
    ERR_ATTACHMENT_POINT_NOT_FOUND;
NiFixedString NiInheritedTransformationComponent::ms_kClassName;
NiFixedString NiInheritedTransformationComponent::ms_kComponentName;
NiFixedString NiInheritedTransformationComponent::ms_kInheritTranslationName;
NiFixedString NiInheritedTransformationComponent::ms_kInheritRotationName;
NiFixedString NiInheritedTransformationComponent::ms_kInheritScaleName;
NiFixedString NiInheritedTransformationComponent::ms_kTranslationName;
NiFixedString NiInheritedTransformationComponent::ms_kRotationName;
NiFixedString NiInheritedTransformationComponent::ms_kScaleName;
NiFixedString NiInheritedTransformationComponent::ms_kSourceEntityName;
NiFixedString NiInheritedTransformationComponent::ms_kAttachmentPointNameName;
NiFixedString NiInheritedTransformationComponent::
    ms_kInheritTranslationDescription;
NiFixedString NiInheritedTransformationComponent::
    ms_kInheritRotationDescription;
NiFixedString NiInheritedTransformationComponent::ms_kInheritScaleDescription;
NiFixedString NiInheritedTransformationComponent::ms_kTranslationDescription;
NiFixedString NiInheritedTransformationComponent::ms_kRotationDescription;
NiFixedString NiInheritedTransformationComponent::ms_kScaleDescription;
NiFixedString NiInheritedTransformationComponent::ms_kSourceEntityDescription;
NiFixedString NiInheritedTransformationComponent::
    ms_kAttachmentPointNameDescription;
NiFixedString NiInheritedTransformationComponent::ms_kSceneRootPointerName;

//---------------------------------------------------------------------------
void NiInheritedTransformationComponent::_SDMInit()
{
    ERR_ATTACHMENT_POINT_NOT_FOUND = "The specified attachment point was not "
        "found; the base scene root will be used instead.";
    ms_kClassName = "NiInheritedTransformationComponent";
    ms_kComponentName = "Inherited Transformation";
    ms_kInheritTranslationName = "Inherit Translation";
    ms_kInheritRotationName = "Inherit Rotation";
    ms_kInheritScaleName = "Inherit Scale";
    ms_kTranslationName = "Translation";
    ms_kRotationName = "Rotation";
    ms_kScaleName = "Scale";
    ms_kSourceEntityName = "Source Entity";
    ms_kAttachmentPointNameName = "Attachment Point Name";
    ms_kInheritTranslationDescription = "Whether or not to inherit the "
        "translation of the source entity.";
    ms_kInheritRotationDescription = "Whether or not to inherit the "
        "rotation of the source entity.";
    ms_kInheritScaleDescription = "Whether or not to inherit the "
        "scale of the source entity.";
    ms_kTranslationDescription = "The world-space position of the entity.";
    ms_kRotationDescription = "The world-space orientation of the entity.";
    ms_kScaleDescription = "The world-space scale of the entity.";
    ms_kSourceEntityDescription = "The entity from which the translation, "
        "rotation, and scale properties are inherited.";
    ms_kAttachmentPointNameDescription = "The attachment point in the source "
        "entity whose transforms are inherited.";
    ms_kSceneRootPointerName = "Scene Root Pointer";
}
//---------------------------------------------------------------------------
void NiInheritedTransformationComponent::_SDMShutdown()
{
    ERR_ATTACHMENT_POINT_NOT_FOUND = NULL;
    ms_kClassName = NULL;
    ms_kComponentName = NULL;
    ms_kInheritTranslationName = NULL;
    ms_kInheritRotationName = NULL;
    ms_kInheritScaleName = NULL;
    ms_kTranslationName = NULL;
    ms_kRotationName = NULL;
    ms_kScaleName = NULL;
    ms_kSourceEntityName = NULL;
    ms_kAttachmentPointNameName = NULL;
    ms_kInheritTranslationDescription = NULL;
    ms_kInheritRotationDescription = NULL;
    ms_kInheritScaleDescription = NULL;
    ms_kTranslationDescription = NULL;
    ms_kRotationDescription = NULL;
    ms_kScaleDescription = NULL;
    ms_kSourceEntityDescription = NULL;
    ms_kAttachmentPointNameDescription = NULL;
    ms_kSceneRootPointerName = NULL;
}
//---------------------------------------------------------------------------
NiInheritedTransformationComponent::NiInheritedTransformationComponent() :
    m_uFlags(0), m_pkSourceEntity(NULL), m_kTranslation(NiPoint3::ZERO),
    m_kRotation(NiMatrix3::IDENTITY), m_fScale(1.0f),
    m_pkAttachmentPoint(NULL), m_bRetrievingTranslation(false),
    m_bRetrievingRotation(false), m_bRetrievingScale(false)
{
    SetInheritTranslation(true);
    SetInheritRotation(true);
    SetInheritScale(true);
}
//---------------------------------------------------------------------------
NiInheritedTransformationComponent::NiInheritedTransformationComponent(
    NiEntityInterface* pkSourceEntity,
    const NiFixedString& kAttachmentPointName) : m_uFlags(0),
    m_pkSourceEntity(pkSourceEntity),
    m_kAttachmentPointName(kAttachmentPointName),
    m_kTranslation(NiPoint3::ZERO), m_kRotation(NiMatrix3::IDENTITY),
    m_fScale(1.0f), m_pkAttachmentPoint(NULL),
    m_bRetrievingTranslation(false), m_bRetrievingRotation(false),
    m_bRetrievingScale(false)
{
    SetInheritTranslation(true);
    SetInheritRotation(true);
    SetInheritScale(true);
}
//---------------------------------------------------------------------------
NiInheritedTransformationComponent::NiInheritedTransformationComponent(
    NiEntityInterface* pkSourceEntity,
    const NiFixedString& kAttachmentPointName, bool bInheritTranslation,
    bool bInheritRotation, bool bInheritScale) : m_uFlags(0),
    m_pkSourceEntity(pkSourceEntity),
    m_kAttachmentPointName(kAttachmentPointName),
    m_kTranslation(NiPoint3::ZERO), m_kRotation(NiMatrix3::IDENTITY),
    m_fScale(1.0f), m_pkAttachmentPoint(NULL),
    m_bRetrievingTranslation(false), m_bRetrievingRotation(false),
    m_bRetrievingScale(false)
{
    SetInheritTranslation(bInheritTranslation);
    SetInheritRotation(bInheritRotation);
    SetInheritScale(bInheritScale);
}
//---------------------------------------------------------------------------
NiInheritedTransformationComponent::NiInheritedTransformationComponent(
    NiEntityInterface* pkSourceEntity,
    const NiFixedString& kAttachmentPointName, bool bInheritTranslation,
    bool bInheritRotation, bool bInheritScale, const NiPoint3& kTranslation,
    const NiMatrix3& kRotation, float fScale) : m_uFlags(0),
    m_pkSourceEntity(pkSourceEntity),
    m_kAttachmentPointName(kAttachmentPointName),
    m_kTranslation(kTranslation), m_kRotation(kRotation), m_fScale(fScale),
    m_pkAttachmentPoint(NULL), m_bRetrievingTranslation(false),
    m_bRetrievingRotation(false), m_bRetrievingScale(false)
{
    SetInheritTranslation(bInheritTranslation);
    SetInheritRotation(bInheritRotation);
    SetInheritScale(bInheritScale);
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::GetInheritTranslation() const
{
    return GetBit(INHERIT_TRANSLATION_MASK);
}
//---------------------------------------------------------------------------
void NiInheritedTransformationComponent::SetInheritTranslation(
    bool bInheritTranslation)
{
    SetBit(bInheritTranslation, INHERIT_TRANSLATION_MASK);
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::GetInheritRotation() const
{
    return GetBit(INHERIT_ROTATION_MASK);
}
//---------------------------------------------------------------------------
void NiInheritedTransformationComponent::SetInheritRotation(
    bool bInheritRotation)
{
    SetBit(bInheritRotation, INHERIT_ROTATION_MASK);
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::GetInheritScale() const
{
    return GetBit(INHERIT_SCALE_MASK);
}
//---------------------------------------------------------------------------
void NiInheritedTransformationComponent::SetInheritScale(bool bInheritScale)
{
    SetBit(bInheritScale, INHERIT_SCALE_MASK);
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::GetErrorResolvingAttachmentPoint()
    const
{
    return GetBit(ERROR_RESOLVING_ATTACHMENT_POINT_MASK);
}
//---------------------------------------------------------------------------
void NiInheritedTransformationComponent::SetErrorResolvingAttachmentPoint(
    bool bErrorResolvingAttachmentPoint)
{
    SetBit(bErrorResolvingAttachmentPoint,
        ERROR_RESOLVING_ATTACHMENT_POINT_MASK);
}
//---------------------------------------------------------------------------
// NiEntityComponentInterface overrides.
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiInheritedTransformationComponent::Clone(
    bool bInheritProperties)
{
    return NiNew NiInheritedTransformationComponent(m_pkSourceEntity,
        m_kAttachmentPointName, NIBOOL_IS_TRUE(GetInheritTranslation()),
        NIBOOL_IS_TRUE(GetInheritRotation()),
        NIBOOL_IS_TRUE(GetInheritScale()), m_kTranslation, m_kRotation,
        m_fScale);
}
//---------------------------------------------------------------------------
NiEntityComponentInterface*
    NiInheritedTransformationComponent::GetMasterComponent()
{
    // This component does not have a master component.
    return NULL;
}
//---------------------------------------------------------------------------
void NiInheritedTransformationComponent::SetMasterComponent(
    NiEntityComponentInterface* pkMasterComponent)
{
    // This component does not have a master component.
}
//---------------------------------------------------------------------------
void NiInheritedTransformationComponent::GetDependentPropertyNames(
    NiTObjectSet<NiFixedString>& kDependentPropertyNames)
{
    // This component has no dependent properties.
}
//---------------------------------------------------------------------------
// NiEntityPropertyInterface overrides.
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::SetTemplateID(
    const NiUniqueID& kID)
{
    // Not supported for custom components.
    return false;
}
//---------------------------------------------------------------------------
NiUniqueID  NiInheritedTransformationComponent::GetTemplateID()
{
    static const NiUniqueID kUniqueID = NiUniqueID(0x11, 0xDB, 0xC5, 0x17,
        0x9B, 0xDC, 0x90, 0x44, 0xA3, 0xFA, 0x7D, 0xCD, 0x6A, 0x5D, 0x7D,
        0xEE);
    return kUniqueID;
}
//---------------------------------------------------------------------------
void NiInheritedTransformationComponent::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiInheritedTransformationComponent::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiInheritedTransformationComponent::GetClassName() const
{
    return ms_kClassName;
}
//---------------------------------------------------------------------------
NiFixedString NiInheritedTransformationComponent::GetName() const
{
    return ms_kComponentName;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::SetName(const NiFixedString& kName)
{
    // This component does not allow its name to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::IsAnimated() const
{
    return true;
}
//---------------------------------------------------------------------------
void NiInheritedTransformationComponent::Update(
    NiEntityPropertyInterface* pkParentEntity, float fTime,
    NiEntityErrorInterface* pkErrors,
    NiExternalAssetManager* pkAssetManager)
{
    if (m_pkSourceEntity &&  !m_pkAttachmentPoint &&
        m_kAttachmentPointName.Exists() &&
        !GetErrorResolvingAttachmentPoint())
    {
        NiObject* pkObject;
        if (m_pkSourceEntity->GetPropertyData(ms_kSceneRootPointerName,
            pkObject, 0) && pkObject)
        {
            NiAVObject* pkSceneRoot = NiDynamicCast(NiAVObject, pkObject);
            if (pkSceneRoot)
            {
                m_pkAttachmentPoint = pkSceneRoot->GetObjectByName(
                    m_kAttachmentPointName);
                if (!m_pkAttachmentPoint)
                {
                    pkErrors->ReportError(ERR_ATTACHMENT_POINT_NOT_FOUND,
                        m_kAttachmentPointName, pkParentEntity->GetName(),
                        ms_kAttachmentPointNameName);
                    m_pkAttachmentPoint = pkSceneRoot;
                }
            }
            if (!m_pkAttachmentPoint)
            {
                SetErrorResolvingAttachmentPoint(true);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiInheritedTransformationComponent::BuildVisibleSet(
    NiEntityRenderingContext* pkRenderingContext,
    NiEntityErrorInterface* pkErrors)
{
    // This component has no work to do during BuildVisibleSet.
}
//---------------------------------------------------------------------------
void NiInheritedTransformationComponent::GetPropertyNames(
    NiTObjectSet<NiFixedString>& kPropertyNames) const
{
    kPropertyNames.Add(ms_kInheritTranslationName);
    kPropertyNames.Add(ms_kInheritRotationName);
    kPropertyNames.Add(ms_kInheritScaleName);
    kPropertyNames.Add(ms_kTranslationName);
    kPropertyNames.Add(ms_kRotationName);
    kPropertyNames.Add(ms_kScaleName);
    kPropertyNames.Add(ms_kSourceEntityName);
    kPropertyNames.Add(ms_kAttachmentPointNameName);
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::CanResetProperty(
    const NiFixedString& kPropertyName, bool& bCanReset) const
{
    if (kPropertyName == ms_kInheritTranslationName ||
        kPropertyName == ms_kInheritRotationName ||
        kPropertyName == ms_kInheritScaleName ||
        kPropertyName == ms_kTranslationName ||
        kPropertyName == ms_kRotationName ||
        kPropertyName == ms_kScaleName ||
        kPropertyName == ms_kSourceEntityName ||
        kPropertyName == ms_kAttachmentPointNameName)
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
NiBool NiInheritedTransformationComponent::ResetProperty(
    const NiFixedString& kPropertyName)
{
    // No properties can be reset.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::MakePropertyUnique(
    const NiFixedString& kPropertyName, bool& bMadeUnique)
{
    if (kPropertyName == ms_kInheritTranslationName ||
        kPropertyName == ms_kInheritRotationName ||
        kPropertyName == ms_kInheritScaleName ||
        kPropertyName == ms_kTranslationName ||
        kPropertyName == ms_kRotationName ||
        kPropertyName == ms_kScaleName ||
        kPropertyName == ms_kSourceEntityName ||
        kPropertyName == ms_kAttachmentPointNameName)        
    {
        // No properties are inherited, so they are all already unique.
        bMadeUnique = false;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::GetDisplayName(
    const NiFixedString& kPropertyName, NiFixedString& kDisplayName) const
{
    if (kPropertyName == ms_kInheritTranslationName)
    {
        kDisplayName = ms_kInheritTranslationName;
    }
    else if (kPropertyName == ms_kInheritRotationName)
    {
        kDisplayName = ms_kInheritRotationName;
    }
    else if (kPropertyName == ms_kInheritScaleName)
    {
        kDisplayName = ms_kInheritScaleName;
    }
    else if (kPropertyName == ms_kTranslationName)
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
    else if (kPropertyName == ms_kSourceEntityName)
    {
        kDisplayName = ms_kSourceEntityName;
    }
    else if (kPropertyName == ms_kAttachmentPointNameName)
    {
        kDisplayName = ms_kAttachmentPointNameName;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::SetDisplayName(
    const NiFixedString& kPropertyName, const NiFixedString& kDisplayName)
{
    // This component does not allow the display name to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::GetPrimitiveType(
    const NiFixedString& kPropertyName, NiFixedString& kPrimitiveType) const
{
    if (kPropertyName == ms_kInheritTranslationName)
    {
        kPrimitiveType = PT_BOOL;
    }
    else if (kPropertyName == ms_kInheritRotationName)
    {
        kPrimitiveType = PT_BOOL;
    }
    else if (kPropertyName == ms_kInheritScaleName)
    {
        kPrimitiveType = PT_BOOL;
    }
    else if (kPropertyName == ms_kTranslationName)
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
    else if (kPropertyName == ms_kSourceEntityName)
    {
        kPrimitiveType = PT_ENTITYPOINTER;
    }
    else if (kPropertyName == ms_kAttachmentPointNameName)
    {
        kPrimitiveType = PT_STRING;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::SetPrimitiveType(
    const NiFixedString& kPropertyName, const NiFixedString& kPrimitiveType)
{
    // This component does not allow the primitive type to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::GetSemanticType(
    const NiFixedString& kPropertyName, NiFixedString& kSemanticType) const
{
    if (kPropertyName == ms_kInheritTranslationName)
    {
        kSemanticType = PT_BOOL;
    }
    else if (kPropertyName == ms_kInheritRotationName)
    {
        kSemanticType = PT_BOOL;
    }
    else if (kPropertyName == ms_kInheritScaleName)
    {
        kSemanticType = PT_BOOL;
    }
    else if (kPropertyName == ms_kTranslationName)
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
    else if (kPropertyName == ms_kSourceEntityName)
    {
        kSemanticType = PT_ENTITYPOINTER;
    }
    else if (kPropertyName == ms_kAttachmentPointNameName)
    {
        kSemanticType = PT_STRING;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::SetSemanticType(
    const NiFixedString& kPropertyName, const NiFixedString& kSemanticType)
{
    // This component does not allow the semantic type to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::GetDescription(
    const NiFixedString& kPropertyName, NiFixedString& kDescription) const
{
    if (kPropertyName == ms_kInheritTranslationName)
    {
        kDescription = ms_kInheritTranslationDescription;
    }
    else if (kPropertyName == ms_kInheritRotationName)
    {
        kDescription = ms_kInheritRotationDescription;
    }
    else if (kPropertyName == ms_kInheritScaleName)
    {
        kDescription = ms_kInheritScaleDescription;
    }
    else if (kPropertyName == ms_kTranslationName)
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
    else if (kPropertyName == ms_kSourceEntityName)
    {
        kDescription = ms_kSourceEntityDescription;
    }
    else if (kPropertyName == ms_kAttachmentPointNameName)
    {
        kDescription = ms_kAttachmentPointNameDescription;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::SetDescription(
    const NiFixedString& kPropertyName, const NiFixedString& kDescription)
{
    // This component does not allow the description to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::GetCategory(
    const NiFixedString& kPropertyName, NiFixedString& kCategory) const
{
    if (kPropertyName == ms_kInheritTranslationName ||
        kPropertyName == ms_kInheritRotationName ||
        kPropertyName == ms_kInheritScaleName ||
        kPropertyName == ms_kTranslationName ||
        kPropertyName == ms_kRotationName ||
        kPropertyName == ms_kScaleName ||
        kPropertyName == ms_kSourceEntityName ||
        kPropertyName == ms_kAttachmentPointNameName)
    {
        kCategory = ms_kComponentName;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::IsPropertyReadOnly(
    const NiFixedString& kPropertyName, bool& bIsReadOnly)
{
    if (kPropertyName == ms_kTranslationName)
    {
        bIsReadOnly = NIBOOL_IS_TRUE(GetInheritTranslation());
    }
    else if (kPropertyName == ms_kRotationName)
    {
        bIsReadOnly = NIBOOL_IS_TRUE(GetInheritRotation());
    }
    else if (kPropertyName == ms_kScaleName)
    {
        bIsReadOnly = NIBOOL_IS_TRUE(GetInheritScale());
    }
    else if (kPropertyName == ms_kInheritTranslationName ||
        kPropertyName == ms_kInheritRotationName ||
        kPropertyName == ms_kInheritScaleName ||
        kPropertyName == ms_kSourceEntityName ||
        kPropertyName == ms_kAttachmentPointNameName)
    {
        bIsReadOnly = false;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::IsPropertyUnique(
    const NiFixedString& kPropertyName, bool& bIsUnique)
{
    if (kPropertyName == ms_kInheritTranslationName ||
        kPropertyName == ms_kInheritRotationName ||
        kPropertyName == ms_kInheritScaleName ||
        kPropertyName == ms_kTranslationName ||
        kPropertyName == ms_kRotationName ||
        kPropertyName == ms_kScaleName ||
        kPropertyName == ms_kSourceEntityName ||
        kPropertyName == ms_kAttachmentPointNameName)
    {
        bIsUnique = true;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::IsPropertySerializable(
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
NiBool NiInheritedTransformationComponent::IsPropertyInheritable(
    const NiFixedString& kPropertyName, bool& bIsInheritable)
{
    if (kPropertyName == ms_kInheritTranslationName ||
        kPropertyName == ms_kInheritRotationName ||
        kPropertyName == ms_kInheritScaleName ||
        kPropertyName == ms_kTranslationName ||
        kPropertyName == ms_kRotationName ||
        kPropertyName == ms_kScaleName ||
        kPropertyName == ms_kSourceEntityName ||
        kPropertyName == ms_kAttachmentPointNameName)
    {
        bIsInheritable = false;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::IsExternalAssetPath(
    const NiFixedString& kPropertyName, unsigned int uiIndex,
    bool& bIsExternalAssetPath) const
{
    if (kPropertyName == ms_kInheritTranslationName ||
        kPropertyName == ms_kInheritRotationName ||
        kPropertyName == ms_kInheritScaleName ||
        kPropertyName == ms_kTranslationName ||
        kPropertyName == ms_kRotationName ||
        kPropertyName == ms_kScaleName ||
        kPropertyName == ms_kSourceEntityName ||
        kPropertyName == ms_kAttachmentPointNameName)
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
NiBool NiInheritedTransformationComponent::GetElementCount(
    const NiFixedString& kPropertyName, unsigned int& uiCount) const
{
    if (kPropertyName == ms_kInheritTranslationName ||
        kPropertyName == ms_kInheritRotationName ||
        kPropertyName == ms_kInheritScaleName ||
        kPropertyName == ms_kTranslationName ||
        kPropertyName == ms_kRotationName ||
        kPropertyName == ms_kScaleName ||
        kPropertyName == ms_kSourceEntityName ||
        kPropertyName == ms_kAttachmentPointNameName)
    {
        uiCount = 1;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::SetElementCount(
    const NiFixedString& kPropertyName, unsigned int uiCount, bool& bCountSet)
{
    if (kPropertyName == ms_kInheritTranslationName ||
        kPropertyName == ms_kInheritRotationName ||
        kPropertyName == ms_kInheritScaleName ||
        kPropertyName == ms_kTranslationName ||
        kPropertyName == ms_kRotationName ||
        kPropertyName == ms_kScaleName ||
        kPropertyName == ms_kSourceEntityName ||
        kPropertyName == ms_kAttachmentPointNameName)
    {
        bCountSet = false;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::IsCollection(
    const NiFixedString& kPropertyName, bool& bIsCollection) const
{
    if (kPropertyName == ms_kInheritTranslationName ||
        kPropertyName == ms_kInheritRotationName ||
        kPropertyName == ms_kInheritScaleName ||
        kPropertyName == ms_kTranslationName ||
        kPropertyName == ms_kRotationName ||
        kPropertyName == ms_kScaleName ||
        kPropertyName == ms_kSourceEntityName ||
        kPropertyName == ms_kAttachmentPointNameName)
    {
        bIsCollection = false;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::GetPropertyData(
    const NiFixedString& kPropertyName, float& fData, unsigned int uiIndex)
    const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kScaleName)
    {
        if (GetInheritScale() && !m_bRetrievingScale)
        {
            m_bRetrievingScale = true;
            if (m_pkAttachmentPoint)
            {
                fData = m_pkAttachmentPoint->GetWorldScale();
            }
            else if (!m_pkSourceEntity || !m_pkSourceEntity->GetPropertyData(
                kPropertyName, fData, uiIndex))
            {
                fData = m_fScale;
            }
            m_bRetrievingScale = false;
        }
        else
        {
            fData = m_fScale;
        }
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::SetPropertyData(
    const NiFixedString& kPropertyName, float fData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kScaleName)
    {
        if (GetInheritScale())
        {
            return false;
        }
        else
        {
            m_fScale = fData;
        }
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::GetPropertyData(
    const NiFixedString& kPropertyName, bool& bData, unsigned int uiIndex)
    const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kInheritTranslationName)
    {
        bData = NIBOOL_IS_TRUE(GetInheritTranslation());
    }
    else if (kPropertyName == ms_kInheritRotationName)
    {
        bData = NIBOOL_IS_TRUE(GetInheritRotation());
    }
    else if (kPropertyName == ms_kInheritScaleName)
    {
        bData = NIBOOL_IS_TRUE(GetInheritScale());
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::SetPropertyData(
    const NiFixedString& kPropertyName, bool bData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kInheritTranslationName)
    {
        // If turning inheritance off, cache last inherited value.
        if (!bData)
        {
            GetPropertyData(ms_kTranslationName, m_kTranslation, 0);
        }
        SetInheritTranslation(bData);
    }
    else if (kPropertyName == ms_kInheritRotationName)
    {
        // If turning inheritance off, cache last inherited value.
        if (!bData)
        {
            GetPropertyData(ms_kRotationName, m_kRotation, 0);
        }
        SetInheritRotation(bData);
    }
    else if (kPropertyName == ms_kInheritScaleName)
    {
        // If turning inheritance off, cache last inherited value.
        if (!bData)
        {
            GetPropertyData(ms_kScaleName, m_fScale, 0);
        }
        SetInheritScale(bData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::GetPropertyData(
    const NiFixedString& kPropertyName, NiPoint3& kData, unsigned int uiIndex)
    const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kTranslationName)
    {
        if (GetInheritTranslation() && !m_bRetrievingTranslation)
        {
            m_bRetrievingTranslation = true;
            if (m_pkAttachmentPoint)
            {
                kData = m_pkAttachmentPoint->GetWorldTranslate();
            }
            else if (!m_pkSourceEntity || !m_pkSourceEntity->GetPropertyData(
                kPropertyName, kData, uiIndex))
            {
                kData = m_kTranslation;
            }
            m_bRetrievingTranslation = false;
        }
        else
        {
            kData = m_kTranslation;
        }
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::SetPropertyData(
    const NiFixedString& kPropertyName, const NiPoint3& kData,
    unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kTranslationName)
    {
        if (GetInheritTranslation())
        {
            return false;
        }
        else
        {
            m_kTranslation = kData;
        }
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::GetPropertyData(
    const NiFixedString& kPropertyName, NiMatrix3& kData,
    unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kRotationName)
    {
        if (GetInheritRotation() && !m_bRetrievingRotation)
        {
            m_bRetrievingRotation = true;
            if (m_pkAttachmentPoint)
            {
                kData = m_pkAttachmentPoint->GetWorldRotate();
            }
            else if (!m_pkSourceEntity || !m_pkSourceEntity->GetPropertyData(
                kPropertyName, kData, uiIndex))
            {
                kData = m_kRotation;
            }
            m_bRetrievingRotation = false;
        }
        else
        {
            kData = m_kRotation;
        }
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::SetPropertyData(
    const NiFixedString& kPropertyName, const NiMatrix3& kData,
    unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kRotationName)
    {
        if (GetInheritRotation())
        {
            return false;
        }
        else
        {
            m_kRotation = kData;
        }
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::GetPropertyData(
    const NiFixedString& kPropertyName, NiFixedString& kData,
    unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kAttachmentPointNameName)
    {
        kData = m_kAttachmentPointName;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::SetPropertyData(
    const NiFixedString& kPropertyName, const NiFixedString& kData,
    unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kAttachmentPointNameName)
    {
        m_kAttachmentPointName = kData;
        m_pkAttachmentPoint = NULL;
        SetErrorResolvingAttachmentPoint(false);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::GetPropertyData(
    const NiFixedString& kPropertyName, NiEntityInterface*& pkData,
    unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kSourceEntityName)
    {
        pkData = m_pkSourceEntity;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiInheritedTransformationComponent::SetPropertyData(
    const NiFixedString& kPropertyName, NiEntityInterface* pkData,
    unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kSourceEntityName)
    {
        m_pkSourceEntity = pkData;
        m_pkAttachmentPoint = NULL;
        SetErrorResolvingAttachmentPoint(false);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
