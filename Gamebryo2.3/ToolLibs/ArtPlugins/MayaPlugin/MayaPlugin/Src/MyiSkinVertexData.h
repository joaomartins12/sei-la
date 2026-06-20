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

// MyiSkinVertexData.h

#ifndef MYISKINVERTEXDATA_H
#define MYISKINVERTEXDATA_H

#include <NiTransform.h>
#include <NiNode.h>
#include "MyiObject.h"
class NiOptVertex;

class MyiSkinVertexData : public MyiObject
{
    public:
         MyiSkinVertexData(int iShapeNum, int iGroupID, int iNumVertices, 
             NiOptVertex *pVertexList);

        ~MyiSkinVertexData( void );
         void AddSVDataFromThisJoint(int iJoint);

         unsigned short** GetVertexIndices( void );
         
// SKINBRANCH        NiBone** GetBones( void );
         NiNode** GetBones( void );
         int GetNumberOfBones(void);
         int GetNumberOfVerts(void);

         NiNode* GetRootBone(void);
         NiTransform* GetSkinToBoneXforms(void);
         NiTransform* GetRootToSkinXform(void);

    protected:
// SKINBRANCH       NiBone**    m_ppBones;
        NiNode** m_ppBones;
        int m_iNumBones;
        int m_iNumVerts;

        NiNode*         m_pRootBone;
        NiTransform*    m_pSkinToBoneXforms;
        NiTransform*    m_pRootToSkinXform;
};

//---------------------------------------------------------------------------
inline int MyiSkinVertexData::GetNumberOfBones(void)
{
    return m_iNumBones;
}
//---------------------------------------------------------------------------
inline int MyiSkinVertexData::GetNumberOfVerts(void)
{
    return m_iNumVerts;
}
//---------------------------------------------------------------------------
inline NiNode** MyiSkinVertexData::GetBones(void)
{
     return m_ppBones;
}
//---------------------------------------------------------------------------
inline NiNode* MyiSkinVertexData::GetRootBone(void)
{
    return m_pRootBone;
}
//---------------------------------------------------------------------------
inline NiTransform* MyiSkinVertexData::GetSkinToBoneXforms(void)
{
    return m_pSkinToBoneXforms;
}
//---------------------------------------------------------------------------
inline NiTransform* MyiSkinVertexData::GetRootToSkinXform(void)
{
    return m_pRootToSkinXform;
}
//---------------------------------------------------------------------------

#endif