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

// Precompiled Headers
#include "MayaPluginPCH.h"


//---------------------------------------------------------------------------
MyiMaterialManager::MyiMaterialManager(void)
{
    m_uiMaterials           = 0;
    m_sppMaterials          = NULL;
    m_ppPixelShaders        = NULL;
    m_spVertexProperty      = NULL;
    int iMaterialCnt;

    DtMtlGetSceneCount( &iMaterialCnt );
    if (iMaterialCnt > 0)
    {
        if (!SpecularInit())
            return;

        // The Info we need is not available from the material.
        // So the following two lines are commented out.
        //  if (!VertexColorInit())
        //      return;

        m_sppMaterials = NiNew NiMaterialPropertyPtr[iMaterialCnt];
        m_ppPixelShaders = NiAlloc(MyiPixelShader*, iMaterialCnt);

        for (int i = 0; i < iMaterialCnt; i++)
        {
            MyiMaterial kMyiMaterial( i );
            
            if (kMyiMaterial.Fail())
                return;

            m_sppMaterials[i] = kMyiMaterial.GetMaterial();
            m_ppPixelShaders[i] = kMyiMaterial.CreatePixelShader();

            // progressively increment m_uiMaterials so that, if there is a
            // failure before all materials are initialized, the destructot
            // will delete the right number of user data elements.
            m_uiMaterials++;
        }
    }
    m_bFail = false;
}
//---------------------------------------------------------------------------
MyiMaterialManager::~MyiMaterialManager()
{
    // Delete the materials
    if (m_sppMaterials)
        NiDelete [] m_sppMaterials;
    m_sppMaterials = NULL;

    // Delete the PixelShaders
    for (unsigned int uiLoop = 0; uiLoop < m_uiMaterials; uiLoop++)
    {
        if (m_ppPixelShaders[uiLoop] != NULL)
            NiDelete m_ppPixelShaders[uiLoop];

        m_ppPixelShaders[uiLoop] = NULL;
    }

    if (m_ppPixelShaders)
        NiFree(m_ppPixelShaders);
    m_ppPixelShaders = NULL;

    // Free the Specular Values
    m_spSpecularOn = NULL;
    m_spSpecularOff = NULL;
}
//---------------------------------------------------------------------------
void MyiMaterialManager::GetMaterial(int iMaterialID, 
    NiMaterialProperty *&pMaterial,
    NiSpecularProperty *&pSpecular, 
    NiVertexColorProperty *&pVertexColor,
    NiAlphaProperty *&pAlphaProperty)
{
    // return the NiMaterialProperty corresponding to Material. If needed,
    // also return a specular property and a vertex color property. If a 
    // specular property is not needed, set pSpecular to NULL. Likewise for 
    // vertex color properties and pVertexColor.

    pAlphaProperty  = NULL;
    pVertexColor    = NULL;
    pSpecular       = NULL;

    pMaterial = m_sppMaterials[iMaterialID];
    
    // Determine if we have should add an alpha property.
    if ((pMaterial->GetAlpha() < 1.0f) ||
        (pMaterial->GetController(&NiAlphaController::ms_RTTI) != NULL))
    {
        NiAlphaProperty* pAlpha = NiNew NiAlphaProperty;
        pAlpha->SetSrcBlendMode( NiAlphaProperty::ALPHA_SRCALPHA);
        pAlpha->SetDestBlendMode(NiAlphaProperty::ALPHA_INVSRCALPHA);
        pAlpha->SetAlphaBlending(true);
        pAlphaProperty = pAlpha;
    }

    // Turn on or off the specular Since there are just two possibilities
    // All object share simular properties
    if (IsSpecular(iMaterialID))
    {
        pSpecular = m_spSpecularOn;
    }
    else
    {
        pSpecular = m_spSpecularOff;
    }

    // This information can not be queryed from the material manager... 
    // We must assume and add the vertex property
    pVertexColor = m_spVertexProperty;
}
//---------------------------------------------------------------------------
bool MyiMaterialManager::SpecularInit()
{
    // Create the Specular values
    m_spSpecularOn = NiNew NiSpecularProperty;
    if (m_spSpecularOn == NULL)
        return false;
    m_spSpecularOn->SetSpecular(true);

    // Create the Specular values
    m_spSpecularOff = NiNew NiSpecularProperty;
    if (m_spSpecularOff == NULL)
        return false;
    m_spSpecularOff->SetSpecular(false);

    return true;
}
//---------------------------------------------------------------------------
bool MyiMaterialManager::VertexColorInit()
{
    m_spVertexProperty = NiNew NiVertexColorProperty;
    if (m_spVertexProperty == NULL)
        return false;

    m_spVertexProperty->SetSourceMode(
        NiVertexColorProperty::SOURCE_AMB_DIFF);
    m_spVertexProperty->SetLightingMode(
        NiVertexColorProperty::LIGHTING_E_A_D);

    return true;
}
//---------------------------------------------------------------------------
bool MyiMaterialManager::IsSpecular(unsigned int i) const
{
    NIASSERT(i < m_uiMaterials);
    if (m_sppMaterials[i]->GetSpecularColor() != NiColor::BLACK)
        return true;

    // Check to see if the Specular Color is animated
    NiTimeController* pkTController = m_sppMaterials[i]->GetControllers();

    // Scan each controller looking for an NiMaterialColor Controller
    while(pkTController != NULL)
    {
        NiMaterialColorController* pkColorController = 
            NiDynamicCast(NiMaterialColorController, pkTController);

        if ((pkColorController != NULL) &&
           (pkColorController->GetType() == NiMaterialColorController::SPEC))
           return true;

        pkTController = pkTController->GetNext();
    }

    return false;
}
//---------------------------------------------------------------------------
void MyiMaterialManager::AddPixelShaderAndAttributes(unsigned int iMaterialID,
        NiGeometry* pkGeometry, unsigned int& uiNBTUVSource)
{
    // Fill in data
    if (m_ppPixelShaders[iMaterialID] == NULL)
        return;

    // Set the Shader
    pkGeometry->ApplyAndSetActiveMaterial(
        m_ppPixelShaders[iMaterialID]->m_spMaterial,
        m_ppPixelShaders[iMaterialID]->m_uiMaterialExtraData);

    NiNodePtr spNode = m_ppPixelShaders[iMaterialID]->m_spAttributeNode;

    // Copy over any Extra Data Attributes
    unsigned int uiLoop;
    for (uiLoop = 0; uiLoop < spNode->GetExtraDataSize(); uiLoop++)
    {
        NiExtraData* pkData = spNode->GetExtraDataAt(uiLoop);

        NiExtraData* pkClone = (NiExtraData* )pkData->Clone();

        pkGeometry->AddExtraData(pkClone->GetName(), pkClone);

        if ((strcmp(pkClone->GetName(), "NiBonesPerPartition") == 0))
        {
            // Check for Skinning shader being applied to a nonskined object
            if ((((NiIntegerExtraData*)pkClone)->GetValue() > 0) &&
                (pkGeometry->GetSkinInstance() == NULL))
            {
                if (!m_ppPixelShaders[iMaterialID]->m_bConfigurableMaterial)
                {
                    DtExt_Err(
                        "Error:: Geometry %s is not skinned but the shader %s"
                        " IS a skinning shader.\n", pkGeometry->GetName(), 
                        m_ppPixelShaders[ iMaterialID ]->m_spMaterial->
                        GetName());
                }
            }

            // Check for NON Skinning shader being applied to a skined object
            if ((((NiIntegerExtraData*)pkClone)->GetValue() == 0) &&
                (pkGeometry->GetSkinInstance() != NULL))
            {
                DtExt_Err(
                    "Error:: Geometry %s is skinned but the shader %s "
                    "is NOT a skinning shader.\n", pkGeometry->GetName(), 
                    m_ppPixelShaders[ iMaterialID ]->m_spMaterial->GetName());
            }
        }
    }

    // Copy over any Controllers to the TriShape
    NiExtraDataController* pkCtrl = 
        (NiExtraDataController*)spNode->GetControllers();

    while(pkCtrl != NULL)
    {
        NiExtraDataController* pkClone = 
            (NiExtraDataController*)pkCtrl->Clone();

        const char* pcName = pkCtrl->GetExtraDataName();

        pkClone->SetTarget( (NiObjectNET*)pkGeometry);

        pkCtrl = (NiExtraDataController*)pkCtrl->GetNext();
    }

    // Create the BiNormal Tangent Data
    NiGeometryData* pkGeomData = pkGeometry->GetModelData();
    
    if (pkGeomData)
    {
        pkGeomData->StoreNormalBinormalTangent(
            m_ppPixelShaders[ iMaterialID ]->m_eNBTMethod);
        uiNBTUVSource = m_ppPixelShaders[iMaterialID]->m_uiNBTUVSource;
    }
    
}
//---------------------------------------------------------------------------