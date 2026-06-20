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

#ifndef MYIOBJECT_H
#define MYIOBJECT_H


enum eGlobalConstantEnums
{
    NI_MAX_MAYA_OBJECT_NAME_LENGTH = 256
};

class MyiObject : public NiMemObject
{
public:
    MyiObject();
    bool Fail() const;

protected:
    bool m_bFail;
};

//---------------------------------------------------------------------------
inline MyiObject::MyiObject()
{
    m_bFail = true;
}
//---------------------------------------------------------------------------
inline bool MyiObject::Fail() const
{
    return m_bFail;
}
//---------------------------------------------------------------------------

#endif