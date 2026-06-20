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

#include "NiEntityPropertyInterface.h"

NiFixedString NiEntityPropertyInterface::PT_FLOAT;
NiFixedString NiEntityPropertyInterface::PT_BOOL;
NiFixedString NiEntityPropertyInterface::PT_INT;
NiFixedString NiEntityPropertyInterface::PT_UINT;
NiFixedString NiEntityPropertyInterface::PT_SHORT;
NiFixedString NiEntityPropertyInterface::PT_USHORT;
NiFixedString NiEntityPropertyInterface::PT_STRING;
NiFixedString NiEntityPropertyInterface::PT_POINT2;
NiFixedString NiEntityPropertyInterface::PT_POINT3;
NiFixedString NiEntityPropertyInterface::PT_QUATERNION;
NiFixedString NiEntityPropertyInterface::PT_MATRIX3;
NiFixedString NiEntityPropertyInterface::PT_COLOR;
NiFixedString NiEntityPropertyInterface::PT_COLORA;
NiFixedString NiEntityPropertyInterface::PT_NIOBJECTPOINTER;
NiFixedString NiEntityPropertyInterface::PT_ENTITYPOINTER;

//---------------------------------------------------------------------------
void NiEntityPropertyInterface::_SDMInit()
{
    PT_FLOAT            = "Float";
    PT_BOOL             = "Boolean";
    PT_INT              = "Integer";
    PT_UINT             = "Unsigned Integer";
    PT_SHORT            = "Short";
    PT_USHORT           = "Unsigned Short";
    PT_STRING           = "String";
    PT_POINT2           = "Point2";
    PT_POINT3           = "Point3";
    PT_QUATERNION       = "Quaternion";
    PT_MATRIX3          = "Matrix3";
    PT_COLOR            = "Color (RGB)";
    PT_COLORA           = "Color (RGBA)";
    PT_NIOBJECTPOINTER  = "NiObject Pointer";
    PT_ENTITYPOINTER    = "Entity Pointer";
}
//---------------------------------------------------------------------------
void NiEntityPropertyInterface::_SDMShutdown()
{
    PT_FLOAT            = NULL;
    PT_BOOL             = NULL;
    PT_INT              = NULL;
    PT_UINT             = NULL;
    PT_SHORT            = NULL;
    PT_USHORT           = NULL;
    PT_STRING           = NULL;
    PT_POINT2           = NULL;
    PT_POINT3           = NULL;
    PT_QUATERNION       = NULL;
    PT_MATRIX3          = NULL;
    PT_COLOR            = NULL;
    PT_COLORA           = NULL;
    PT_NIOBJECTPOINTER  = NULL;
    PT_ENTITYPOINTER    = NULL;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::IsAnimated() const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::IsAddPropertySupported()
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::AddProperty(
    const NiFixedString& kPropertyName, const NiFixedString& kDisplayName,
    const NiFixedString& kPrimitiveType, const NiFixedString& kSemanticType,
    const NiFixedString& kDescription)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::MakeCollection(
    const NiFixedString& kPropertyName, bool bCollection)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::RemoveProperty(
    const NiFixedString& kPropertyName)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(
    const NiFixedString& kPropertyName, float& fData, unsigned int uiIndex)
    const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(
    const NiFixedString& kPropertyName, bool& bData, unsigned int uiIndex)
    const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(
    const NiFixedString& kPropertyName, int& iData, unsigned int uiIndex)
    const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(
    const NiFixedString& kPropertyName, unsigned int& uiData,
    unsigned int uiIndex) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(
    const NiFixedString& kPropertyName, short& sData, unsigned int uiIndex)
    const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(
    const NiFixedString& kPropertyName, unsigned short& usData,
    unsigned int uiIndex) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(
    const NiFixedString& kPropertyName, NiFixedString& kData,
    unsigned int uiIndex) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(
    const NiFixedString& kPropertyName, NiPoint2& kData,
    unsigned int uiIndex) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(
    const NiFixedString& kPropertyName, NiPoint3& kData,
    unsigned int uiIndex) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(
    const NiFixedString& kPropertyName, NiQuaternion& kData,
    unsigned int uiIndex) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(
    const NiFixedString& kPropertyName, NiMatrix3& kData,
    unsigned int uiIndex) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(
    const NiFixedString& kPropertyName, NiColor& kData, unsigned int uiIndex)
    const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(
    const NiFixedString& kPropertyName, NiColorA& kData, unsigned int uiIndex)
    const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(
    const NiFixedString& kPropertyName, NiObject*& pkData,
    unsigned int uiIndex) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(
    const NiFixedString& kPropertyName, NiEntityInterface*& pkData,
    unsigned int uiIndex) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::GetPropertyData(
    const NiFixedString& kPropertyName, void*& pvData,
    size_t& stDataSizeInBytes, unsigned int uiIndex) const
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(
    const NiFixedString& kPropertyName, float fData, unsigned int uiIndex)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(
    const NiFixedString& kPropertyName, bool bData, unsigned int uiIndex)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(
    const NiFixedString& kPropertyName, int iData, unsigned int uiIndex)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(
    const NiFixedString& kPropertyName, unsigned int uiData,
    unsigned int uiIndex)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(
    const NiFixedString& kPropertyName, short sData, unsigned int uiIndex)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(
    const NiFixedString& kPropertyName, unsigned short usData,
    unsigned int uiIndex)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(
    const NiFixedString& kPropertyName, const NiFixedString& kData,
    unsigned int uiIndex)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(
    const NiFixedString& kPropertyName, const NiPoint2& kData,
    unsigned int uiIndex)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(
    const NiFixedString& kPropertyName, const NiPoint3& kData,
    unsigned int uiIndex)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(
    const NiFixedString& kPropertyName, const NiQuaternion& kData,
    unsigned int uiIndex)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(
    const NiFixedString& kPropertyName, const NiMatrix3& kData,
    unsigned int uiIndex)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(
    const NiFixedString& kPropertyName, const NiColor& kData,
    unsigned int uiIndex)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(
    const NiFixedString& kPropertyName, const NiColorA& kData,
    unsigned int uiIndex)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(
    const NiFixedString& kPropertyName, NiObject* pkData,
    unsigned int uiIndex)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(
    const NiFixedString& kPropertyName, NiEntityInterface* pkData,
    unsigned int uiIndex)
{
    return false;
}
//---------------------------------------------------------------------------
NiBool NiEntityPropertyInterface::SetPropertyData(
    const NiFixedString& kPropertyName, const void* pvData,
    size_t stDataSizeInBytes, unsigned int uiIndex)
{
    return false;
}
//---------------------------------------------------------------------------
