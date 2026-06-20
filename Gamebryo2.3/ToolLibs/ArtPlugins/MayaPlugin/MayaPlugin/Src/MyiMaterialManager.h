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

// MyiMaterialManager.h

#ifndef MYIMATERIALMANAGER_H
#define MYIMATERIALMANAGER_H

#include <NiSmartPointer.h>
#include <NiMaterialProperty.h>
#include <NiAlphaProperty.h>
#include <NiShader.h>
#include <NiTriShape.h>

#include "MyiObject.h"

NiSmartPointer(NiMaterialProperty);
NiSmartPointer(NiSpecularProperty);
NiSmartPointer(NiVertexColorProperty);
class MyiPixelShader;

class MyiMaterialManager : public MyiObject
{
public:
    static const char *m_pTag;

    MyiMaterialManager(void);
    ~MyiMaterialManager(void);
    void GetMaterial(int iMaterialID, NiMaterialProperty *&pMaterial,
        NiSpecularProperty *&pSpecular, 
        NiVertexColorProperty *&pVertexColor,
        NiAlphaProperty *&pAlphaProperty);

    void AddPixelShaderAndAttributes(unsigned int iMaterialID, 
        NiGeometry* pkGeometry, unsigned int& uiNBTUVSource);

protected:
    bool IsSpecular(unsigned int i) const;
    bool SpecularInit();
    bool VertexColorInit();

    unsigned int m_uiMaterials;
    NiMaterialPropertyPtr *m_sppMaterials;
    MyiPixelShader** m_ppPixelShaders;

    NiSpecularPropertyPtr       m_spSpecularOn;
    NiSpecularPropertyPtr       m_spSpecularOff;
    NiVertexColorPropertyPtr    m_spVertexProperty;
};

#endif