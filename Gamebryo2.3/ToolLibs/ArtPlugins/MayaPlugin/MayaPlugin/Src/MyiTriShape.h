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

#ifndef MYITRISHAPE_H
#define MYITRISHAPE_H

#include <NiTriShape.h>
#include <NiSmartPointer.h>

#include "MyiObject.h"
#include "MyiMaterialManager.h"
#include "MyiTextureManager.h"
#include "NiOptVertex.h"
#include <maya/MFnMesh.h>
#include <maya/MFnBlendShapeDeformer.h>

NiSmartPointer(NiTriShape);
class NiPoint3;
class NiColorA;
class NiTextureProperty;

class MyiTriShape : public MyiObject
{
public:
    MyiTriShape( int iShapeNum, int iGroupID,
        MyiMaterialManager* pkMaterialManager,
        MyiTextureManager* pkTextureManager);

    ~MyiTriShape( void );

    NiTriShape *GetTriShape( void );

    static bool GetMaterialProperties( int iShapeID,int iGroupID,
        int iMaterialID,
        MyiMaterialManager *pkMaterialManager, 
        NiTexturingProperty* pTexturingProperty, bool bNeedsAlphaProperty,
        bool bHasVertexColors,
        NiAVObject* pkAVObject);

    bool CreateMultiTexture( int iShapeID, int iGroupID, 
        MyiTextureManager* pkTextureManager,
        NiTexturingProperty *&pTexturingProperty,
        bool &bNeedsAlphaProperty);

    static bool CreateMultiTexture( int iMaterialID, 
        MyiTextureManager* pkTextureManager,
        NiTexturingProperty *&pTexturingProperty,
        bool &bNeedsAlphaProperty);

    bool CreateMultiTextureForType( int iShapeID, int iGroupID,
        MyiTextureManager* pkTextureManager,
        NiTexturingProperty *&pTexturingProperty,
        char *szType,
        bool &bNeedsAlphaProperty);
    
    static bool CreateMultiTextureForType( int iMaterialID,
        MyiTextureManager* pkTextureManager,
        NiTexturingProperty *&pTexturingProperty,
        char *szType,
        bool &bNeedsAlphaProperty);
    static bool HasAnimatedTextureTransform(NiTexturingProperty* pkProperty,
        NiTexturingProperty::Map* pkMap);
    
    bool GetShapeAttributesForNi( int iShapeID, int iGroupID );

    bool GetVertsForNi(int iShapeID, int iGroupID);

    bool GetTriIndexListForNi(int iShapeID, int iGroupID);

    bool GetNormsForNi(int iShapeID, int iGroupID );

    bool GetUVsForNi(int iShapeID, int iGroupID );

    bool GetVertexColorsForNi(int iShapeID, int iGroupID);

    
protected:

    int AssignUVs(int iShapeID, int iGroupID, char *pTextureName, 
        int iTextureID);
    void BakeUVs(NiTexturingProperty* pkTexProp);

    void AttachMorpherControllers(int iShapeID,int iGroupID, 
        const unsigned int* puiNewToOld, MStatus* pkStatus = NULL);
    MFnMesh GetBaseMesh(const MFnBlendShapeDeformer& kBlendShape);
    NiPoint3* GetUndeformedBaseVertices(
        const MFnBlendShapeDeformer& kBlendShape, 
        const unsigned int* puiNewToOld, const int* piFaceList);
    NiPoint3* GetCollapsedVertices(const MPointArray& kPointArray,
        const unsigned int* puiNewToOld, const int* piFaceList);
    
    void AttachProperties(int iShapeID, int iGroupID, 
        NiTexturingProperty* pTexturingProperty);

    static NiTexturingProperty::ClampMode GetClampMode(char* pTextureName);

    static void CreateFlipBookProperty(int iShapeID, int iGroupID, 
        int iTextureID, char* szType, MyiTextureManager* pkTextureManager, 
        NiTexturingProperty::Map *pMap, 
        NiTexturingProperty *pTexturingProperty);

    static void CreateFlipBookProperty(int iMaterialID, int iTextureID, 
        char* szType, MyiTextureManager* pkTextureManager, 
        NiTexturingProperty::Map *pMap, 
        NiTexturingProperty *pTexturingProperty);

    bool GetBlendShape(int iShapeID, int iGroupID, MObject& BlendShape);

    bool GetMorphUpdateNormals(int iShapeID);

    int OptimizeUVs(int iUVIndex);

    void AddNoStripifyAttributes(MFnDagNode& dgNode);

    void AddUniqueName(NiMorphData::MorphTarget* pkNiTargetArray,
        const char *szTargetName, int iTarget);

    const char *m_pName;
    float     m_fScale[3];
    bool      m_bHasVertexColors;
    bool      m_bHasAlpha;
    
    NiTriShapePtr m_spTriShape;
    bool m_bGotTriangles;
    
    unsigned int m_uiNiVertices;
    unsigned int m_uiNiTriangles;
    unsigned int m_uiNumIndices;
    
    NiOptVertex* m_pkVertices;

    unsigned int m_uiTriListLength;
    unsigned int* m_puiTriList;

    unsigned int m_uiMultiIndex;
   
};

//---------------------------------------------------------------------------
inline NiTriShape* MyiTriShape::GetTriShape()
{
    return m_spTriShape;
}
//---------------------------------------------------------------------------

#endif