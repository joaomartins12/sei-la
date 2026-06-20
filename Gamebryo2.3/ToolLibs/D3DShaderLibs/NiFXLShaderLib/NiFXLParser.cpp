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
#include "NiFXLShaderLibPCH.h"

#include "NiFXLParser.h"
#include "NiFXLParameterDesc.h"

unsigned int NiFXLParser::ms_uiShaderMapIndex = 0;
unsigned int NiFXLParser::ms_uiGeneralLightIndex = 0;
unsigned int NiFXLParser::ms_uiPointLightIndex = 0;
unsigned int NiFXLParser::ms_uiDirectionalLightIndex = 0;
unsigned int NiFXLParser::ms_uiSpotLightIndex = 0;
unsigned int NiFXLParser::ms_uiShadowPointLightIndex = 0;
unsigned int NiFXLParser::ms_uiShadowDirectionalLightIndex = 0;
unsigned int NiFXLParser::ms_uiShadowSpotLightIndex = 0;
unsigned int NiFXLParser::ms_uiProjShadowIndex = 0;
unsigned int NiFXLParser::ms_uiProjLightIndex = 0;

static NiD3DShaderConstantMap::PredefinedMapping* s_pkObjectProperties = NULL;
static unsigned int s_uiNumObjectProperties = 18;

//---------------------------------------------------------------------------
void NiFXLParser::_SDMInit()
{
    s_pkObjectProperties = NiNew 
        NiD3DShaderConstantMap::PredefinedMapping[s_uiNumObjectProperties];

    unsigned int ui = 0;
    s_pkObjectProperties[ui++] = NiD3DShaderConstantMap::PredefinedMapping(
        NiD3DShaderConstantMap::SCM_OBJ_DIMMER, "Dimmer");
    s_pkObjectProperties[ui++] = NiD3DShaderConstantMap::PredefinedMapping(
        NiD3DShaderConstantMap::SCM_OBJ_AMBIENT, "Ambient");
    s_pkObjectProperties[ui++] = NiD3DShaderConstantMap::PredefinedMapping(
        NiD3DShaderConstantMap::SCM_OBJ_DIFFUSE, "Diffuse");
    s_pkObjectProperties[ui++] = NiD3DShaderConstantMap::PredefinedMapping(
        NiD3DShaderConstantMap::SCM_OBJ_SPECULAR, "Specular");
    s_pkObjectProperties[ui++] = NiD3DShaderConstantMap::PredefinedMapping(
        NiD3DShaderConstantMap::SCM_OBJ_WORLDPOSITION, "Position");
    s_pkObjectProperties[ui++] = NiD3DShaderConstantMap::PredefinedMapping(
        NiD3DShaderConstantMap::SCM_OBJ_WORLDDIRECTION, "Direction");
    s_pkObjectProperties[ui++] = NiD3DShaderConstantMap::PredefinedMapping(
        NiD3DShaderConstantMap::SCM_OBJ_WORLDTRANSFORM, "World");
    s_pkObjectProperties[ui++] = NiD3DShaderConstantMap::PredefinedMapping(
        NiD3DShaderConstantMap::SCM_OBJ_ATTENUATION, "Attenuation");
    s_pkObjectProperties[ui++] = NiD3DShaderConstantMap::PredefinedMapping(
        NiD3DShaderConstantMap::SCM_OBJ_SPOTATTENUATION, "SpotAttenuation");
    s_pkObjectProperties[ui++] = NiD3DShaderConstantMap::PredefinedMapping(
        NiD3DShaderConstantMap::SCM_OBJ_WORLDPROJECTIONMATRIX, "Projection");
    s_pkObjectProperties[ui++] = NiD3DShaderConstantMap::PredefinedMapping(
        NiD3DShaderConstantMap::SCM_OBJ_WORLDPROJECTIONTRANSLATION,
        "ProjectionTranslation");
    s_pkObjectProperties[ui++] = NiD3DShaderConstantMap::PredefinedMapping(
        NiD3DShaderConstantMap::SCM_OBJ_WORLDCLIPPINGPLANE, "ClippingPlane");
    s_pkObjectProperties[ui++] = NiD3DShaderConstantMap::PredefinedMapping(
        NiD3DShaderConstantMap::SCM_OBJ_TEXCOORDGEN, "TexCoordGen");
    s_pkObjectProperties[ui++] = NiD3DShaderConstantMap::PredefinedMapping(
        NiD3DShaderConstantMap::SCM_OBJ_WORLDTOSHADOWMAPMATRIX, 
        "WorldToShadowMapMatrix");
    s_pkObjectProperties[ui++] = NiD3DShaderConstantMap::PredefinedMapping(
        NiD3DShaderConstantMap::SCM_OBJ_SHADOWMAP, "ShadowMap");
    s_pkObjectProperties[ui++] = NiD3DShaderConstantMap::PredefinedMapping(
        NiD3DShaderConstantMap::SCM_OBJ_SHADOWMAPTEXSIZE, "TexSizeShadow");
    s_pkObjectProperties[ui++] = NiD3DShaderConstantMap::PredefinedMapping(
        NiD3DShaderConstantMap::SCM_OBJ_SHADOWBIAS, "ShadowBias");
    s_pkObjectProperties[ui++] = NiD3DShaderConstantMap::PredefinedMapping(
        NiD3DShaderConstantMap::SCM_OBJ_SHADOW_VSM_POWER_EPSILON, 
        "ShadowVSMPowerEpsilon");


    NIASSERT(s_uiNumObjectProperties == ui);
}
//---------------------------------------------------------------------------
void NiFXLParser::_SDMShutdown()
{
    NiDelete[] s_pkObjectProperties;
    s_pkObjectProperties = NULL;
}
//---------------------------------------------------------------------------
bool NiFXLParser::GetParametersAndTechniques(FXLEffect* pkEffect, 
    unsigned int& uiNumParameters, unsigned int& uiNumTechniques)
{
    FXLEFFECT_DESC kEffectDesc;
    pkEffect->GetEffectDesc(&kEffectDesc);

    uiNumParameters = kEffectDesc.Parameters;
    uiNumTechniques = kEffectDesc.Techniques;
    return true;
}
//---------------------------------------------------------------------------
NiFXLParameterDesc* NiFXLParser::GetParameter(FXLEffect* pkEffect, 
    unsigned int uiIndex)
{
    FXLHANDLE hParam = pkEffect->GetParameterHandleFromIndex(uiIndex);
    
    NiFXLParameterDesc* pkParamDesc = 
        NiFXLParser::GetParameterDesc(pkEffect, hParam);

    return pkParamDesc;
}
//---------------------------------------------------------------------------
NiFXLTechniqueDesc* NiFXLParser::GetTechnique(FXLEffect* pkEffect, 
    unsigned int uiIndex)
{
    FXLHANDLE hTech = pkEffect->GetTechniqueHandleFromIndex(uiIndex);

    NiFXLTechniqueDesc* pkTechDesc = 
        NiFXLParser::GetTechniqueDesc(pkEffect, hTech);

    return pkTechDesc;
}
//---------------------------------------------------------------------------
NiFXLParameterDesc* NiFXLParser::GetParameterDesc(
    FXLEffect* pkEffect, FXLHANDLE hParam)
{
    FXLPARAMETER_DESC kParamDesc;
    pkEffect->GetParameterDesc(hParam, &kParamDesc);

    const char* pcName = kParamDesc.pName;
    FXLDATA_CLASS eClass = kParamDesc.Class;
    FXLDATA_TYPE eType = kParamDesc.Type;

    unsigned int uiRows = kParamDesc.Rows;
    unsigned int uiCols = kParamDesc.Columns;
    unsigned int uiElements = kParamDesc.Elements;

    // We have an array or struct. In FXL, we have to check other data for
    // the necessary information to feed into the parsing code we inherited
    // from the D3DX framework.
    if (eClass == FXLDCLASS_CONTAINER && eType == FXLDTYPE_CONTAINER)
    {
        if (kParamDesc.Content == FXLPACONTENT_ARRAY)
        {
            // Get the handle for the 0th element and then get the class, type,
            // rows, and columns from that.
            FXLHANDLE hElement = pkEffect->GetElementHandle(hParam, 0);
            FXLPARAMETER_DESC kElementDesc;
            pkEffect->GetParameterDesc(hElement, &kElementDesc);

            // Populate our temporary variables with information from the
            // element. We retain the number of elements from the array
            // itself which plugs into our old parsing code which expects 
            // arrays to be defined as type, rows, cols, and elements.
            eClass = kElementDesc.Class;
            eType = kElementDesc.Type;
            uiRows = kElementDesc.Rows;
            uiCols = kElementDesc.Columns;
        }
        else 
        {
            // We only handle arrays at this time. We do not handle structs.
            return NULL;
        }
    }

    // First check for numerical parameters
    if (eType == FXLDTYPE_BOOL ||
        eType == FXLDTYPE_INT ||
        eType == FXLDTYPE_FLOAT)
    {
        // Check for "StandardsGlobal" parameter
        if (pcName && NiStricmp(pcName, "StandardsGlobal") == 0)
        {
            // Currently unable to do anything about it, really
            return NULL;
        }

        // Map semantic to either a predefined constant map value
        // or to an NiShader attribute

        NiFXLParameterDesc* pkDesc = NULL;

        bool bAttribute = false;
        bool bGlobal = false;
        bool bObject = false;
        bool bPredefined = false;

        // Get VarType from annotations.
        const char* pcVarType = NULL;
        FXLHANDLE hVarType = pkEffect->GetAnnotationHandle(hParam, "VarType");
        if (hVarType)
        {
            const char* pcAnnotation = GetStringReference(pkEffect, hVarType);

            bAttribute = (NiStricmp(pcAnnotation, "ATTRIBUTE") == 0);
            bGlobal = (NiStricmp(pcAnnotation, "GLOBAL") == 0);
            bObject = (NiStricmp(pcAnnotation, "OBJECT") == 0);
            bPredefined = (NiStricmp(pcAnnotation, "PREDEFINED") == 0);
        }

        // VarType is undefined.
        if (!bAttribute && !bGlobal)
        {
            pkDesc = QueryObject(pkEffect, hParam, &kParamDesc);
            if (!pkDesc)
            {
                pkDesc = QueryPredefined(pkEffect, hParam, 
                    (eClass == FXLDCLASS_CMATRIX), kParamDesc.Elements);
                if (pkDesc)
                {
                    bPredefined = true;
                    ParsePredefinedAnnotations(pkEffect, hParam, &kParamDesc,
                        (NiFXLParameterPredefinedDesc*)pkDesc);
                }
            }
            else
            {
                bObject = true;
            }
        }

        // Unknown var type and annotations haven't clarified. It's an 
        // attribute.
        if (!bGlobal && !bObject && !bPredefined)
            bAttribute = true;

        if (!pkDesc)
        {
            // Semantic must map to an attribute
            if ((eClass == FXLDCLASS_VECTOR && uiCols == 1) ||
                ((eClass == FXLDCLASS_RMATRIX || 
                eClass == FXLDCLASS_CMATRIX) && uiElements == 1))
            {
                eClass = FXLDCLASS_SCALAR;
            }

            if (eType  == FXLDTYPE_FLOAT && uiElements > 1 && 
               (eClass == FXLDCLASS_VECTOR || eClass == FXLDCLASS_SCALAR))
            {
                NiFXLParameterArrayDesc* pkArrayDesc = 
                    NiFXLParameterDesc::GetArrayDesc();

                pkArrayDesc->m_uiNumRows = uiRows;
                pkArrayDesc->m_uiNumCols = uiCols;
                pkArrayDesc->m_uiNumElements = uiElements;
                
                if (pkArrayDesc->m_uiNumElements >
                    NiFXLParameterArrayDesc::MAX_ROWS)
                {
                    NiOutputDebugString("Array has more than "
                        "NiFXLParameterArrayDesc::MAX_ROWS worth"
                        " of elements. Please use fewer elements.");
                    
                    return false;
                }

                NiFXLParameterArrayDesc::ArrayType eType = 
                    pkArrayDesc->GetArrayType();
                
                // Search for annotations
                ParseArrayAnnotations(pkEffect, hParam, &kParamDesc,  
                    pkArrayDesc);
                
                bool bCompactTempArray = false;
                if (eClass == FXLDCLASS_SCALAR)
                {
                    pkEffect->GetScalarArrayF(hParam, 
                        pkArrayDesc->m_afInitValue, 
                        pkArrayDesc->m_uiNumElements);
                }
                else if (eClass == FXLDCLASS_VECTOR)
                {
                    if (uiRows == 1 && uiCols == 4)
                    {
                        // Vector 4's are the same size as the 
                        // array, so no need to waste time on a temp 
                        // array.
                        pkEffect->GetVectorArrayF(hParam, 
                            pkArrayDesc->m_afInitValue, 
                            pkArrayDesc->m_uiNumElements);
                    }
                    else
                    {
                        // Read into the temporary buffer
                        // because D3DX forces the initial values to be
                        // D3DXVECTOR4's when they will often be smaller.
                        // The temp array will hold the values as 
                        // D3DXVECTOR4's and we will compact the values
                        // in a later method.
                        pkEffect->GetVectorArrayF(hParam, 
                            pkArrayDesc->m_afTempValues, 
                            pkArrayDesc->m_uiNumElements);
                        bCompactTempArray = true;
                    }
                }
                        
                if (bCompactTempArray)
                {
                    bool bCompacted = pkArrayDesc->CompactFromVector4Array(
                        (const D3DXVECTOR4*) pkArrayDesc->m_afTempValues, 
                        pkArrayDesc->m_afInitValue,
                        pkArrayDesc->m_uiNumRows, pkArrayDesc->m_uiNumCols,
                        pkArrayDesc->m_uiNumElements);
                    NIASSERT(bCompacted);
                }
                
                pkDesc = pkArrayDesc;
            }
            else if (eClass == FXLDCLASS_RMATRIX || 
                eClass == FXLDCLASS_CMATRIX)
            {
                // Can currently only handle float matrices
                if (eType != FXLDTYPE_FLOAT)
                    return false;

                NiFXLParameterMatrixDesc* pkMatrixDesc = 
                    NiFXLParameterDesc::GetMatrixDesc();

                pkMatrixDesc->m_uiNumRows = uiRows;
                pkMatrixDesc->m_uiNumCols = uiCols;

                // Search for annotations
                ParseMatrixAnnotations(pkEffect, hParam, &kParamDesc, 
                    pkMatrixDesc);

                // Check for initial value
                D3DXMATRIX* pkMatrix = 
                    (D3DXMATRIX*)(pkMatrixDesc->m_afInitValue);

                pkEffect->GetMatrixF4x4(hParam, 
                    pkMatrixDesc->m_afInitValue);

                if (eClass = FXLDCLASS_RMATRIX)
                    D3DXMatrixTranspose(pkMatrix, pkMatrix);

                pkDesc = pkMatrixDesc;
            }
            else if (eClass == FXLDCLASS_VECTOR)
            {
                // Can currently only handle float vectors/colors
                if (eType != FXLDTYPE_FLOAT)
                    return false;

                NiFXLParameterVectorDesc* pkVectorDesc = 
                    NiFXLParameterDesc::GetVectorDesc();

                pkVectorDesc->m_uiNumRows = uiRows;
                pkVectorDesc->m_uiNumCols = uiCols;
                pkVectorDesc->m_uiNumAtomicElements = uiCols;
                
                if (pkVectorDesc->m_uiNumAtomicElements >
                    NiFXLParameterVectorDesc::MAX_ENTRIES)
                {
                    return false;
                }

                // Search for annotations
                ParseVectorAnnotations(pkEffect, hParam, &kParamDesc, 
                    pkVectorDesc);
                
                pkEffect->GetVectorF(hParam, pkVectorDesc->m_afInitValue);
                    
                pkDesc = pkVectorDesc;
            }
            else if (eClass == FXLDCLASS_SCALAR)
            {
                // Can currently only handle float, bool, or int scalars
                if (eType == FXLDTYPE_FLOAT)
                {
                    NiFXLParameterFloatDesc* pkFloatDesc = 
                        NiFXLParameterDesc::GetFloatDesc();

                    // Search for annotations
                    ParseFloatAnnotations(pkEffect, hParam, &kParamDesc, 
                        pkFloatDesc);

                    // Check for initial value
                    float fFloat;
                    pkEffect->GetScalarF(hParam, &fFloat);
                    pkFloatDesc->m_fInitValue = fFloat;

                    pkDesc = pkFloatDesc;
                }
                else if (eType == FXLDTYPE_BOOL)
                {
                    NiFXLParameterBooleanDesc* pkBooleanDesc = 
                        NiFXLParameterDesc::GetBooleanDesc();

                    // Search for annotations
                    ParseBooleanAnnotations(pkEffect, hParam, &kParamDesc, 
                        pkBooleanDesc);

                    // Check for initial value
                    BOOL bBool;
                    pkEffect->GetScalarB(hParam, &bBool);
                    pkBooleanDesc->m_bInitValue = (bBool != FALSE);
                    
                    pkDesc = pkBooleanDesc;
                }
                else if (eType == FXLDTYPE_INT)
                {
                    NiFXLParameterIntegerDesc* pkIntegerDesc = 
                        NiFXLParameterDesc::GetIntegerDesc();

                    // Search for annotations
                    ParseIntegerAnnotations(pkEffect, hParam, &kParamDesc, 
                        pkIntegerDesc);

                    // Check for initial value
                    int iInt;
                    pkEffect->GetScalarI(hParam, &iInt);
                    pkIntegerDesc->m_uiInitValue = (unsigned int)iInt;

                    pkDesc = pkIntegerDesc;
                }
            }
            if (pkDesc)
                pkDesc->m_bGlobal = bGlobal;
        }

        if (pkDesc)
        {
            if (pkDesc->m_pcName == NULL ||
                pkDesc->m_pcName[0] == '\0')
            {
                pkDesc->m_pcName = kParamDesc.pName;
            }
            return pkDesc;
        }
        return NULL;
    }

    // Not a numeric parameter - continue on to check for texture
    if (eType == FXLDTYPE_SAMPLER)
    {
        // Search for annotations
        NiFXLParameterSamplerDesc* pkDesc = ParseSamplerAnnotations(pkEffect, 
            hParam, &kParamDesc);
        return pkDesc;
    }

    // No luck
    return NULL;
}
//---------------------------------------------------------------------------
NiFXLTechniqueDesc* NiFXLParser::GetTechniqueDesc(
    FXLEffect* pkEffect, FXLHANDLE hTech)
{
    FXLTECHNIQUE_DESC kTechniqueDesc;
    pkEffect->GetTechniqueDesc(hTech, &kTechniqueDesc);

    NiFXLTechniqueDesc* pkDesc = NiFXLTechniqueDesc::GetDesc();

    pkDesc->m_pcName = kTechniqueDesc.pName;

    if (kTechniqueDesc.Annotations)
    {
        ParseTechniqueAnnotations(pkEffect, hTech, &kTechniqueDesc, 
            pkDesc);
    }

    // Get pixel/vertex shader annotations
    for (unsigned int uiPass = 0; uiPass < kTechniqueDesc.Passes; uiPass++)
    {
        FXLHANDLE hPass = pkEffect->GetPassHandleFromIndex(hTech, uiPass);
        FXLPASS_DESC kPassDesc;
        pkEffect->GetPassDesc(hPass, &kPassDesc);

        unsigned int uiVSVersion = 0;
        unsigned int uiPSVersion = 0;

        if (kPassDesc.pVertexShaderFunction != NULL)
        {
            uiVSVersion = D3DXGetShaderVersion(
                kPassDesc.pVertexShaderFunction);
        }

        if (kPassDesc.pPixelShaderFunction != NULL)
        {
            uiPSVersion = D3DXGetShaderVersion(
                kPassDesc.pPixelShaderFunction);
        }

        if (pkDesc->m_uiVSVersion < uiVSVersion)
            pkDesc->m_uiVSVersion = uiVSVersion;
        if (pkDesc->m_uiPSVersion < uiPSVersion)
            pkDesc->m_uiPSVersion = uiPSVersion;
    }

    return pkDesc;
}
//---------------------------------------------------------------------------
void NiFXLParser::ResetIndices()
{
    ms_uiShaderMapIndex = 0;
    ms_uiGeneralLightIndex = 0;
    ms_uiPointLightIndex = 0;
    ms_uiDirectionalLightIndex = 0;
    ms_uiSpotLightIndex = 0;
    ms_uiShadowPointLightIndex = 0;
    ms_uiShadowDirectionalLightIndex = 0;
    ms_uiShadowSpotLightIndex = 0;
    ms_uiProjShadowIndex = 0;
    ms_uiProjLightIndex = 0;
}
//---------------------------------------------------------------------------
void NiFXLParser::ParseMatrixAnnotations(FXLEffect* pkEffect, 
    FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc, 
    NiFXLParameterMatrixDesc* pkDesc)
{
    // Query annotations
    for (unsigned int i = 0; i < pkParamDesc->Annotations; i++)
    {
        FXLHANDLE hAnnot = pkEffect->GetAnnotationHandleFromIndex(hParam, i);
        FXLANNOTATION_DESC kAnnotDesc;
        pkEffect->GetAnnotationDesc(hAnnot, &kAnnotDesc);
        BOOL bBool;
        if (NiStricmp(kAnnotDesc.pName, "desc") == 0 ||
            NiStricmp(kAnnotDesc.pName, "description") == 0 ||
            NiStricmp(kAnnotDesc.pName, "uihelp") == 0)
        {
            pkDesc->m_pcDesc = GetStringReference(pkEffect, hAnnot);
        }
        else if (NiStricmp(kAnnotDesc.pName, "hidden") == 0)
        {
            bBool = GetBool(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.pName, "uiwidget"))
        {
            const char* pcValue = NULL;
            pcValue = GetStringReference(pkEffect, hAnnot);

            if (pcValue && NiStricmp(pcValue, "none") == 0)
            {
                pkDesc->m_bHidden = true;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiFXLParser::ParseVectorAnnotations(FXLEffect* pkEffect, 
    FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc, 
    NiFXLParameterVectorDesc* pkDesc)
{
    // Query annotations
    for (unsigned int i = 0; i < pkParamDesc->Annotations; i++)
    {
        FXLHANDLE hAnnot = pkEffect->GetAnnotationHandleFromIndex(hParam, i);
        FXLANNOTATION_DESC kAnnotDesc;
        pkEffect->GetAnnotationDesc(hAnnot, &kAnnotDesc);
        BOOL bBool;
        if (NiStricmp(kAnnotDesc.pName, "desc") == 0 ||
            NiStricmp(kAnnotDesc.pName, "description") == 0 ||
            NiStricmp(kAnnotDesc.pName, "uihelp") == 0)
        {
            pkDesc->m_pcDesc = GetStringReference(pkEffect, hAnnot);
        }
        else if (NiStricmp(kAnnotDesc.pName, "hidden") == 0)
        {
            bBool = GetBool(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.pName, "uiwidget") == 0)
        {
            const char* pcValue = NULL;
            pcValue = GetStringReference(pkEffect, hAnnot);

            if (pcValue && 
                (NiStricmp(pcValue, "color") == 0 ||
                NiStricmp(pcValue, "colorswatch") == 0))
            {
                pkDesc->m_bColor = true;
            }
            else if (pcValue && NiStricmp(pcValue, "none") == 0)
            {
                pkDesc->m_bHidden = true;
            }
        }
        else if (NiStricmp(kAnnotDesc.pName, "color") == 0)
        {
            bBool = GetBool(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_bColor = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.pName, "uitype") == 0)
        {
            const char* pcValue = NULL;
            pcValue = GetStringReference(pkEffect, hAnnot);

            if (pcValue && 
                (NiStricmp(pcValue, "color") == 0 ||
                NiStricmp(pcValue, "colorswatch") == 0))
            {
                pkDesc->m_bColor = true;
            }

        }
        else if (NiStricmp(kAnnotDesc.pName, "min") == 0 ||
            NiStricmp(kAnnotDesc.pName, "uimin") == 0)
        {
            GetFloatVector(pkEffect, hAnnot, &kAnnotDesc, 
                &((D3DXVECTOR4)pkDesc->m_afMinValue));
        }
        else if (NiStricmp(kAnnotDesc.pName, "max") == 0 ||
            NiStricmp(kAnnotDesc.pName, "uimax") == 0)
        {
            GetFloatVector(pkEffect, hAnnot, &kAnnotDesc,
                &((D3DXVECTOR4)pkDesc->m_afMaxValue));
        }
    }
}
//---------------------------------------------------------------------------
void NiFXLParser::ParseArrayAnnotations(FXLEffect* pkEffect, 
    FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc, 
    NiFXLParameterArrayDesc* pkDesc)
{
    // Query annotations
    for (unsigned int i = 0; i < pkParamDesc->Annotations; i++)
    {
        FXLHANDLE hAnnot = pkEffect->GetAnnotationHandleFromIndex(hParam, i);
        FXLANNOTATION_DESC kAnnotDesc;
        pkEffect->GetAnnotationDesc(hAnnot, &kAnnotDesc);
        BOOL bBool;
        
        if (NiStricmp(kAnnotDesc.pName, "desc") == 0 ||
            NiStricmp(kAnnotDesc.pName, "description") == 0 ||
            NiStricmp(kAnnotDesc.pName, "uihelp") == 0)
        {
            pkDesc->m_pcDesc = GetStringReference(pkEffect, hAnnot);
        }
        else if (NiStricmp(kAnnotDesc.pName, "hidden") == 0)
        {
            bBool = GetBool(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.pName, "uiwidget") == 0)
        {
            const char* pcValue = NULL;
            pcValue = GetStringReference(pkEffect, hAnnot);

            if (pcValue && 
                (NiStricmp(pcValue, "color") == 0 ||
                NiStricmp(pcValue, "colorswatch") == 0))
            {
                if (pkDesc->m_uiNumCols != 4 && 
                    pkDesc->m_uiNumRows != 1)
                {
                    NiOutputDebugString("Annotation indicates \"color\", "
                        "but the variable is not a float4. UI will not be"
                        " a color.");
                    continue;
                }
                pkDesc->m_bColor = true;
            }
            else if (pcValue && NiStricmp(pcValue, "none") == 0)
            {
                pkDesc->m_bHidden = true;
            }
        }
        else if (NiStricmp(kAnnotDesc.pName, "color") == 0)
        {
            if (pkDesc->m_uiNumCols != 4 && 
                pkDesc->m_uiNumRows != 1)
            {
                NiOutputDebugString("Annotation indicates \"color\", but the"
                    " variable is not a float4. UI will not be a color.");
                continue;
            }
            bBool = GetBool(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_bColor = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.pName, "uitype") == 0)
        {
            const char* pcValue = NULL;
            pcValue = GetStringReference(pkEffect, hAnnot);

            if (pcValue && 
                (NiStricmp(pcValue, "color") == 0 ||
                NiStricmp(pcValue, "colorswatch") == 0))
            {
                if (pkDesc->m_uiNumCols != 4 && 
                    pkDesc->m_uiNumRows != 1)
                {
                    NiOutputDebugString("Annotation indicates \"color\", "
                        "but the variable is not a float4. UI will not be"
                        " a color.");
                    continue;
                }
                pkDesc->m_bColor = true;
            }

        }
        else if (NiStricmp(kAnnotDesc.pName, "min") == 0 ||
            NiStricmp(kAnnotDesc.pName, "uimin") == 0)
        {
            if (kAnnotDesc.Columns != pkDesc->m_uiNumCols ||
                kAnnotDesc.Rows != pkDesc->m_uiNumRows ||
                kAnnotDesc.Elements != pkDesc->m_uiNumElements)
            {
                NiOutputDebugString("Array annotation \"min\" does not match"
                    " parent in either number of columns, rows, or elements");
                continue;
            }

            bool bCompactTempArray = false;
            if (kAnnotDesc.Class == FXLDCLASS_SCALAR)
            {
                GetFloatArray(pkEffect, hAnnot, &kAnnotDesc, 
                    pkDesc->m_afInitValue);
            }
            else if (kAnnotDesc.Class == FXLDCLASS_VECTOR)
            {
                if (kAnnotDesc.Rows == 1 && kAnnotDesc.Columns == 4)
                {
                    // Vector 4's are the same size as the 
                    // array, so no need to waste time on a temp 
                    // array.
                    GetFloatVector(pkEffect, hAnnot, &kAnnotDesc, 
                        pkDesc->m_afMinValue);
                }
                else
                {
                    // Read into the temporary buffer
                    // because D3DX forces the initial values to be
                    // D3DXVECTOR4's when they will often be smaller.
                    // The temp array will hold the values as D3DXVECTOR4's
                    // and we will compact the values in a later method.
                    
                    GetFloatVectorArray(pkEffect, hAnnot, &kAnnotDesc,
                        pkDesc->m_afTempValues);
                    bCompactTempArray = true;
                }
            }
                    
            if (bCompactTempArray)
            {
                bool bCompacted = pkDesc->CompactFromVector4Array(
                    (const D3DXVECTOR4*) pkDesc->m_afTempValues, 
                    pkDesc->m_afMinValue,
                    pkDesc->m_uiNumRows, pkDesc->m_uiNumCols,
                    pkDesc->m_uiNumElements);
                NIASSERT(bCompacted);
            }
                
        }
        else if (NiStricmp(kAnnotDesc.pName, "max") == 0 ||
            NiStricmp(kAnnotDesc.pName, "uimax") == 0)
        {
            if (kAnnotDesc.Columns != pkDesc->m_uiNumCols ||
                kAnnotDesc.Rows != pkDesc->m_uiNumRows ||
                kAnnotDesc.Elements != pkDesc->m_uiNumElements)
            {
                NiOutputDebugString("Array annotation \"max\" does not match"
                    " parent in either number of columns, rows, or elements");
                continue;
            }

            bool bCompactTempArray = false;
            if (kAnnotDesc.Class == FXLDCLASS_SCALAR)
            {
                GetFloatArray(pkEffect, hAnnot, &kAnnotDesc, 
                    pkDesc->m_afMaxValue);
            }
            else if (kAnnotDesc.Class == FXLDCLASS_VECTOR)
            {
                if (kAnnotDesc.Rows == 1 && kAnnotDesc.Columns == 4)
                {
                    // Vector 4's are the same size as the 
                    // array, so no need to waste time on a temp 
                    // array.
                    GetFloatVectorArray(pkEffect, hAnnot, &kAnnotDesc,
                        pkDesc->m_afMaxValue);
                }
                else
                {
                    // Read into the temporary buffer
                    // because D3DX forces the initial values to be
                    // D3DXVECTOR4's when they will often be smaller.
                    // The temp array will hold the values as D3DXVECTOR4's
                    // and we will compact the values in a later method.
                    
                    GetFloatVectorArray(pkEffect, hAnnot, &kAnnotDesc,
                        pkDesc->m_afTempValues);
                    bCompactTempArray = true;
                }
            }
                    
            if (bCompactTempArray)
            {
                bool bCompacted = pkDesc->CompactFromVector4Array(
                    (const D3DXVECTOR4*) pkDesc->m_afTempValues, 
                    pkDesc->m_afMaxValue,
                    pkDesc->m_uiNumRows, pkDesc->m_uiNumCols,
                    pkDesc->m_uiNumElements);
                NIASSERT(bCompacted);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiFXLParser::ParseFloatAnnotations(FXLEffect* pkEffect, 
    FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc, 
    NiFXLParameterFloatDesc* pkDesc)
{
    // Query annotations
    for (unsigned int i = 0; i < pkParamDesc->Annotations; i++)
    {
        FXLHANDLE hAnnot = pkEffect->GetAnnotationHandleFromIndex(hParam, i);
        FXLANNOTATION_DESC kAnnotDesc;
        pkEffect->GetAnnotationDesc(hAnnot, &kAnnotDesc);
        BOOL bBool;
        if (NiStricmp(kAnnotDesc.pName, "desc") == 0 ||
            NiStricmp(kAnnotDesc.pName, "description") == 0 ||
            NiStricmp(kAnnotDesc.pName, "uihelp") == 0)
        {
            pkDesc->m_pcDesc = GetStringReference(pkEffect, hAnnot);
        }
        else if (NiStricmp(kAnnotDesc.pName, "hidden") == 0)
        {
            bBool = GetBool(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.pName, "uiwidget") == 0)
        {
            const char* pcValue = NULL;
            pcValue = GetStringReference(pkEffect, hAnnot);
            if (pcValue && NiStricmp(pcValue, "none") == 0)
            {
                pkDesc->m_bHidden = true;
            }
        }
        else if (NiStricmp(kAnnotDesc.pName, "min") == 0 ||
            NiStricmp(kAnnotDesc.pName, "uimin") == 0)
        {
            pkDesc->m_fMinValue = GetFloat(pkEffect, hAnnot, &kAnnotDesc);
        }
        else if (NiStricmp(kAnnotDesc.pName, "max") == 0 ||
            NiStricmp(kAnnotDesc.pName, "uimax") == 0)
        {
            pkDesc->m_fMaxValue = GetFloat(pkEffect, hAnnot, &kAnnotDesc);
        }
    }
}
//---------------------------------------------------------------------------
void NiFXLParser::ParseBooleanAnnotations(FXLEffect* pkEffect, 
    FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc, 
    NiFXLParameterBooleanDesc* pkDesc)
{
    BOOL bBool = FALSE;

    // Query annotations
    for (unsigned int i = 0; i < pkParamDesc->Annotations; i++)
    {
        FXLHANDLE hAnnot = pkEffect->GetAnnotationHandleFromIndex(hParam, i);
        FXLANNOTATION_DESC kAnnotDesc;
        pkEffect->GetAnnotationDesc(hAnnot, &kAnnotDesc);
        if (NiStricmp(kAnnotDesc.pName, "desc") == 0 ||
            NiStricmp(kAnnotDesc.pName, "description") == 0 ||
            NiStricmp(kAnnotDesc.pName, "uihelp") == 0)
        {
            pkDesc->m_pcDesc = GetStringReference(pkEffect, hAnnot);
        }
        else if (NiStricmp(kAnnotDesc.pName, "hidden") == 0)
        {
            bBool = GetBool(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.pName, "uiwidget") == 0)
        {
            const char* pcValue = NULL;
            pcValue = GetStringReference(pkEffect, hAnnot);

            if (pcValue && NiStricmp(pcValue, "none") == 0)
            {
                pkDesc->m_bHidden = true;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiFXLParser::ParseIntegerAnnotations(FXLEffect* pkEffect, 
    FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc,  
    NiFXLParameterIntegerDesc* pkDesc)
{
    int iInt = 0;

    // Query annotations
    for (unsigned int i = 0; i < pkParamDesc->Annotations; i++)
    {
        FXLHANDLE hAnnot = pkEffect->GetAnnotationHandleFromIndex(hParam, i);
        FXLANNOTATION_DESC kAnnotDesc;
        pkEffect->GetAnnotationDesc(hAnnot, &kAnnotDesc);
        BOOL bBool;
        
        if (NiStricmp(kAnnotDesc.pName, "desc") == 0 ||
            NiStricmp(kAnnotDesc.pName, "description") == 0 ||
            NiStricmp(kAnnotDesc.pName, "uihelp") == 0)
        {
            pkDesc->m_pcDesc = GetStringReference(pkEffect, hAnnot);
        }
        else if (NiStricmp(kAnnotDesc.pName, "hidden") == 0)
        {
            bBool = GetBool(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.pName, "uiwidget") == 0)
        {
            const char* pcValue = NULL;
            pcValue = GetStringReference(pkEffect, hAnnot);

            if (pcValue && NiStricmp(pcValue, "none") == 0)
            {
                pkDesc->m_bHidden = true;
            }
        }
        else if (NiStricmp(kAnnotDesc.pName, "min") == 0 ||
            NiStricmp(kAnnotDesc.pName, "uimin") == 0)
        {
            iInt = GetInt(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_uiMinValue = (unsigned int)iInt;
        }
        else if (NiStricmp(kAnnotDesc.pName, "max") == 0 ||
            NiStricmp(kAnnotDesc.pName, "uimax") == 0)
        {
            iInt = GetInt(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_uiMaxValue = (unsigned int)iInt;
        }
    }
}
//---------------------------------------------------------------------------
NiFXLParameterSamplerDesc* NiFXLParser::ParseSamplerAnnotations(
    FXLEffect* pkEffect, FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc)
{
    NiFXLParameterSamplerDesc* pkDesc = NiFXLParameterDesc::GetSamplerDesc();

    int iInt = 0;
    BOOL bBool = FALSE;
    const char* pcTextureType = NULL;

    pkDesc->m_pcName = pkParamDesc->pName;
    pkDesc->m_uiTextureWidth = D3DX_DEFAULT;
    pkDesc->m_uiTextureHeight = D3DX_DEFAULT;
    pkDesc->m_uiTextureDepth = D3DX_DEFAULT;

    pkDesc->m_uiTextureIndex = 0xFFFFFFFF;
    pkDesc->m_uiObjectType = NiShaderAttributeDesc::OT_UNDEFINED;

    // Query annotations
    const char* pcObjectType = NULL;
    unsigned int uiObjectIndex = 0;
    bool bFoundObjectIndex = false;
    for (unsigned int i = 0; i < pkParamDesc->Annotations; i++)
    {
        FXLHANDLE hAnnot = pkEffect->GetAnnotationHandleFromIndex(hParam, i);
        FXLANNOTATION_DESC kAnnotDesc;
        pkEffect->GetAnnotationDesc(hAnnot, &kAnnotDesc);
        if (NiStricmp(kAnnotDesc.pName, "desc") == 0 ||
            NiStricmp(kAnnotDesc.pName, "description") == 0 ||
            NiStricmp(kAnnotDesc.pName, "uihelp") == 0)
        {
            pkDesc->m_pcDesc = GetStringReference(pkEffect, hAnnot);
        }
        else if (NiStricmp(kAnnotDesc.pName, "hidden") == 0)
        {
            bBool = GetBool(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.pName, "uiwidget") == 0)
        {
            const char* pcValue = NULL;
            pcValue = GetStringReference(pkEffect, hAnnot);
            
            if (pcValue && NiStricmp(pcValue, "none") == 0)
            {
                pkDesc->m_bHidden = true;
            }
        }
        else if (NiStricmp(kAnnotDesc.pName, "name") == 0)
        {
            pkDesc->m_pcTextureName = GetStringReference(pkEffect, hAnnot);
        }
        else if (NiStricmp(kAnnotDesc.pName, "function") == 0)
        {
            pkDesc->m_pcTextureFunction = GetStringReference(pkEffect, hAnnot);
        }
        else if (NiStricmp(kAnnotDesc.pName, "target") == 0) 
        {
            pkDesc->m_pcTextureTarget = GetStringReference(pkEffect, hAnnot);
        }
        else if (NiStricmp(kAnnotDesc.pName, "width") == 0)
        {
            iInt = GetInt(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_uiTextureWidth = (unsigned int)iInt;
        }
        else if (NiStricmp(kAnnotDesc.pName, "height") == 0)
        {
            iInt = GetInt(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_uiTextureHeight = (unsigned int)iInt;
        }
        else if (NiStricmp(kAnnotDesc.pName, "depth") == 0)
        {
            iInt = GetInt(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_uiTextureDepth = (unsigned int)iInt;
        }
        else if (NiStricmp(kAnnotDesc.pName, "type") == 0)
        {
            pcTextureType = GetStringReference(pkEffect, hAnnot);
        }
        else if (NiStricmp(kAnnotDesc.pName, "NTM") == 0)
        {
            pkDesc->m_pcTextureMap = GetStringReference(pkEffect, hAnnot);
        }
        else if (NiStricmp(kAnnotDesc.pName, "NTMIndex") == 0)
        {
            iInt = GetInt(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_uiTextureIndex = (unsigned int)iInt;
        }
        else if (NiStricmp(kAnnotDesc.pName, "Object") == 0)
        {
            pcObjectType = GetStringReference(pkEffect, hAnnot);
        }
        else if (NiStricmp(kAnnotDesc.pName, "ObjectIndex") == 0)
        {
            iInt = GetInt(pkEffect, hAnnot, &kAnnotDesc);
            uiObjectIndex = (unsigned int) iInt;
            bFoundObjectIndex = true;
        }
    }

    if (pcObjectType)
    {
        pkDesc->m_uiObjectType = DetermineObjectType(pcObjectType,
            uiObjectIndex, bFoundObjectIndex);
        pkDesc->m_uiObjectIndex = uiObjectIndex;
        pkDesc->m_pcTextureMap = NULL;
    }

    if (pcTextureType != NULL) 
    {
        if (NiStricmp(pcTextureType, "volume") == 0)
        {
            pkDesc->m_eTextureType = 
                NiFXLParameterSamplerDesc::TT_3D;
        }
        else if (NiStricmp(pcTextureType, "cube") == 0)
        {
            pkDesc->m_eTextureType = 
                NiFXLParameterSamplerDesc::TT_CUBE;
        }
    }
    else
    {
        pkDesc->m_eTextureType = NiFXLParameterSamplerDesc::TT_2D;
    }

    // First try and locate the texture if it is given by name
    if (pkDesc->m_pcTextureName)
    {
        // Only look in current directory for now
        HANDLE hFile = CreateFile(pkDesc->m_pcTextureName, GENERIC_READ, 
            FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile == INVALID_HANDLE_VALUE) 
            pkDesc->m_pcTextureName = NULL;
        else
            CloseHandle(hFile);
    }

    // Indicate shader map if no other clues are given
    if (pkDesc->m_pcTextureName == NULL &&
        pkDesc->m_pcTextureFunction == NULL &&
        !pcObjectType)
    {
        if (pkDesc->m_pcTextureMap == NULL)
            pkDesc->m_pcTextureMap = "shader";

        if (strcmp(pkDesc->m_pcTextureMap, "shader") == 0 &&
            pkDesc->m_uiTextureIndex == 0xFFFFFFFF)
        {
            pkDesc->m_uiTextureIndex = ms_uiShaderMapIndex++;
        }
    }

    return pkDesc;
}
//---------------------------------------------------------------------------
void NiFXLParser::ParsePredefinedAnnotations(FXLEffect* pkEffect, 
    FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc, 
    NiFXLParameterPredefinedDesc* pkDesc)
{
    int iInt = 0;

    // Query annotations
    for (unsigned int i = 0; i < pkParamDesc->Annotations; i++)
    {
        FXLHANDLE hAnnot = pkEffect->GetAnnotationHandleFromIndex(hParam, i);
        FXLANNOTATION_DESC kAnnotDesc;
        pkEffect->GetAnnotationDesc(hAnnot, &kAnnotDesc);
        if (NiStricmp(kAnnotDesc.pName, "NTMIndex") == 0)
        {
            iInt = GetInt(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_uiExtraData = (unsigned int)iInt;
        }
    }
}
//---------------------------------------------------------------------------
void NiFXLParser::ParseTechniqueAnnotations(FXLEffect* pkEffect, 
    FXLHANDLE hTech, FXLTECHNIQUE_DESC* pkTechDesc, NiFXLTechniqueDesc* pkDesc)
{
    int iInt;
    BOOL bBool = FALSE;
    const char* pcNBT = NULL;

    // Get name, description, implementation, and user version annotations
    for (unsigned int i = 0; i < pkTechDesc->Annotations; i++)
    {
        FXLHANDLE hAnnot = 
            pkEffect->GetAnnotationHandleFromIndex(hTech, i);
        FXLANNOTATION_DESC kAnnotDesc;
        pkEffect->GetAnnotationDesc(hAnnot, &kAnnotDesc);
        if (NiStricmp(kAnnotDesc.pName, "desc") == 0 ||
            NiStricmp(kAnnotDesc.pName, "description") == 0 ||
            NiStricmp(kAnnotDesc.pName, "uihelp") == 0)
        {
            pkDesc->m_pcDesc = GetStringReference(pkEffect, hAnnot);
        }
        else if (NiStricmp(kAnnotDesc.pName, "implementation") == 0)
        {
            iInt = GetInt(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_uiImplementation = (unsigned int)iInt;
        }
        else if (NiStricmp(kAnnotDesc.pName, "userversion") == 0)
        {
            iInt = GetInt(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_uiUserVersion = (unsigned int)iInt;
        }
        else if (NiStricmp(kAnnotDesc.pName, "bonesperpartition") == 0)
        {
            iInt = GetInt(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_uiBonesPerPartition = (unsigned int)iInt;
        }
        else if (NiStricmp(kAnnotDesc.pName, "nbtmethod") == 0)
        {
            pcNBT = GetStringReference(pkEffect, hAnnot);
        }
        else if (NiStricmp(kAnnotDesc.pName, "usesnirenderstate") == 0)
        {
            bBool = GetBool(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_bUsesNiRenderState = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.pName, "usesnilightstate") == 0)
        {
            bBool = GetBool(pkEffect, hAnnot, &kAnnotDesc);
            pkDesc->m_bUsesNiLightState = (bBool != FALSE);
        }
    }

    pkDesc->m_eNBTMethod = NiGeometryData::NBT_METHOD_NONE;
    if (pcNBT)
    {
        if (NiStricmp(pcNBT, "NDL") == 0)
        {
            pkDesc->m_eNBTMethod = NiGeometryData::NBT_METHOD_NDL;
        }
        else if (NiStricmp(pcNBT, "MAX") == 0)
        {
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, true,
                "NiFXLParser Warning: deprecated \"MAX\" value "
                "assigned to the \"NBTMethod\" annotation specified on "
                "\"%s\" technique", pkTechDesc->pName);
            pkDesc->m_eNBTMethod = NiGeometryData::NBT_METHOD_NDL;
        }
        else if (NiStricmp(pcNBT, "ATI") == 0)
        {
            pkDesc->m_eNBTMethod = NiGeometryData::NBT_METHOD_ATI;
        }
    }
}
//---------------------------------------------------------------------------
NiFXLParameterDesc* NiFXLParser::QueryPredefined(FXLEffect* pkEffect,
    FXLHANDLE hParam, bool bColumnMatrix, unsigned int uiArrayElements)
{
    NIASSERT(hParam);

    // Grab the DefinedMapping annotation now.
    const char* pcDefMap = NULL;
    FXLHANDLE hAnnot = pkEffect->GetAnnotationHandle(hParam, "DefinedMapping");
    if (hAnnot)
    {
        pcDefMap = GetStringReference(pkEffect, hAnnot);
        
    }

    // If the VarType is "Predefined" but there's no "DefinedMapping"
    // annotation, there's nothing we can do.
    if (pcDefMap == NULL)
        return NULL;

    // First check the unmodified version
    unsigned int uiMapping = 0;
    unsigned int uiNumRegisters = 0;
    bool bSuccess = NiD3DShaderConstantMap::LookUpPredefinedMapping(pcDefMap, 
        uiMapping, uiNumRegisters);

    if (!bSuccess)
    {
        // Create new copy of annotation for mangling
        unsigned int uiLen = strlen(pcDefMap);
        char* pcLowerDefMap = NiAlloc(char, uiLen + 1);
        unsigned int i = 0;
        for (; i < uiLen; i++)
            pcLowerDefMap[i] = (char)tolower(pcDefMap[i]);
        char* pcLastChar = pcLowerDefMap + i;
        *pcLastChar = '\0';

        // Attempt to support a number of variations based off of other 
        // packages smenatics:
        //   - 3ds Max semantics:
        //     [XXX]
        //     [XXX]I   (inverse)
        //     [XXX]IT  (inverse transpose)
        //       where XXX = WORLD, VIEW, PROJECTION, WORLDVIEW, or
        //                   WORLDVIEWPROJ
        //   - DirectX Standard Semantics:
        //     [XXX]
        //     [XXX]Inverse             (inverse)
        //     [XXX]InverseTranspose    (inverse transpose)
        //       where XXX = World, View, Projection, WorldView, 
        //         WorldViewProjection, ViewProjection, Joint, JointWorld,
        //         JointWorldView, or JointWorldViewProjection
        bool bInverse = false;
        bool bTranspose = false;

        // Check for a Transpose matrix
        //   - [  ]IT (inverse as well!)    (3ds Max)
        //   - [  ]Transpose                (Gamebryo, DirectX)

        if (uiLen > 2 && strncmp(pcLastChar - 2, "it", 2) == 0)
        {
            bInverse = true;
            bTranspose = true;
            uiLen -= 2;
            pcLastChar -= 2;
            *pcLastChar = '\0';
        }
        else if (uiLen > 5 && strncmp(pcLastChar - 5, "trans", 5) == 0)
        {
            bTranspose = true;
            uiLen -= 5;
            pcLastChar -= 5;
            *pcLastChar = '\0';
        }
        else if (uiLen > 9 && strncmp(pcLastChar - 9, "transpose", 9) == 0)
        {
            bTranspose = true;
            uiLen -= 9;
            pcLastChar -= 9;
            *pcLastChar = '\0';
        }

        // Check for an Inverse matrix
        //   - Inv[]                (Gamebryo)
        //   - []I                  (3ds Max)
        //   - []Inverse            (DirectX)
        if (uiLen > 3 && strncmp(pcLowerDefMap, "inv", 3) == 0)
        {
            bInverse = true;
            for (i = 0; i < uiLen - 3; i++)
                pcLowerDefMap[i] = pcLowerDefMap[i + 3];
            uiLen -= 3;
            pcLastChar -= 3;
            *pcLastChar = '\0';
        }
        else if (uiLen > 1 && strncmp(pcLastChar - 1, "i", 1) == 0)
        {
            bInverse = true;
            uiLen -= 1;
            pcLastChar -= 1;
            *pcLastChar = '\0';
        }
        else if (uiLen > 7 && strncmp(pcLastChar - 7, "inverse", 7) == 0)
        {
            bInverse = true;
            uiLen -= 7;
            pcLastChar -= 7;
            *pcLastChar = '\0';
        }

        // Assemble components into new semantic;
        unsigned int uiNewLen = uiLen + 1 +
            (bInverse ? 3 : 0) + (bTranspose ? 9 : 0);
        char* pcNewDefMap = NiAlloc(char, uiNewLen);
        NiSprintf(pcNewDefMap, uiNewLen, "%s%s%s",
            (bInverse ? "Inv" : ""),
            pcLowerDefMap,
            (bTranspose ? "Transpose" : ""));
        NiFree(pcLowerDefMap);

        bSuccess = NiD3DShaderConstantMap::LookUpPredefinedMapping(pcNewDefMap,
            uiMapping, uiNumRegisters);

        NiFree(pcNewDefMap);
    }

    if (bSuccess)
    {
        // Semantic corresponds to a Gamebryo constant map value

        NiFXLParameterPredefinedDesc* pkDesc = 
            NiFXLParameterDesc::GetPredefinedDesc();
        pkDesc->m_pcPredefined = pcDefMap;
        pkDesc->m_uiMapping = uiMapping;
        // Handle bone matrices
        if (uiMapping >= NiD3DShaderConstantMap::SCM_DEF_BONE_MATRIX_3 &&
            uiMapping <= NiD3DShaderConstantMap::SCM_DEF_SKINBONE_MATRIX_4)
        {
            pkDesc->m_uiArrayCount = uiArrayElements;
        }
        else
        {
            pkDesc->m_uiArrayCount = 1;
        }

        return pkDesc;
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiFXLParameterDesc* NiFXLParser::QueryObject(FXLEffect* pkEffect,
    FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc)
{
    NIASSERT(pkEffect && hParam && pkParamDesc);

    // First, look for the required "Object" annotation.
    const char* pcObjectValue = LookupObjectAnnotation(pkEffect,
        hParam, pkParamDesc, "Object");
    if (!pcObjectValue)
    {
        // This parameter is not an object.
        return NULL;
    }

    // Get the specified object index, if any.
    unsigned int uiObjectIndex = 0;
    bool bFoundObjectIndex = false;
    for (unsigned int ui = 0; ui < pkParamDesc->Annotations; ui++)
    {
        FXLHANDLE hAnnot = pkEffect->GetAnnotationHandleFromIndex(hParam, ui);
        FXLANNOTATION_DESC kAnnotDesc;
        pkEffect->GetAnnotationDesc(hAnnot, &kAnnotDesc);
        if (NiStricmp(kAnnotDesc.pName, "ObjectIndex") == 0)
        {
            NIASSERT(kAnnotDesc.Type == FXLDTYPE_INT);
            int iIndex = 0;
            iIndex = GetInt(pkEffect, hAnnot, &kAnnotDesc);
            uiObjectIndex = iIndex;
            bFoundObjectIndex = true;
            break;
        }
    }

    // Identify the object type from the appropriate annotation.
    NiShaderAttributeDesc::ObjectType eObjectType =
        (NiShaderAttributeDesc::ObjectType) DetermineObjectType(
        pcObjectValue, uiObjectIndex, bFoundObjectIndex);
    if (eObjectType == NiShaderAttributeDesc::OT_UNDEFINED)
    {
        return NULL;
    }

    // Identify object mapping from ObjectProperty annotation.
    unsigned int uiMapping;
    const char* pcObjectProperty = NULL;
    FXLHANDLE hAnnot = pkEffect->GetAnnotationHandle(hParam,
        "ObjectProperty");
    if (hAnnot)
    {
        pcObjectProperty = GetStringReference(pkEffect, hAnnot);
    }

    if (!LookupObjectProperty(pcObjectProperty, uiMapping))
    {
        return NULL;
    }

    // Verify that the mapping is appropriate for the object type. Also,
    // read any supported additional annotations for the semantic, adjusting
    // the object mapping as necessary. uiMapping may be changed by this
    // function. This function returns false if the mapping is not
    // appropriate for the object type.
    if (!VerifyAndAdjustMapping(pkEffect, hParam, pkParamDesc,
        eObjectType, uiMapping))
    {
        return NULL;
    }

    // Create the parameter desc with the appropriate object type and mapping.
    NiFXLParameterObjectDesc* pkDesc =
        NiFXLParameterDesc::GetObjectDesc();
    pkDesc->m_uiMapping = uiMapping;
    pkDesc->m_eObjectType = eObjectType;
    pkDesc->m_uiObjectIndex = uiObjectIndex;

    return pkDesc;
}
//---------------------------------------------------------------------------
const char* NiFXLParser::LookupObjectAnnotation(FXLEffect* pkEffect,
    FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc, const char* pcAnnotation)
{
    NIASSERT(pkEffect && hParam && pkParamDesc && pcAnnotation);

    const char* pcValue = NULL;
    for (unsigned int ui = 0; ui < pkParamDesc->Annotations; ui++)
    {
        FXLHANDLE hAnnot = pkEffect->GetAnnotationHandleFromIndex(
            hParam, ui);
        FXLANNOTATION_DESC kAnnotDesc;
        pkEffect->GetAnnotationDesc(hAnnot, &kAnnotDesc);
        if (NiStricmp(kAnnotDesc.pName, pcAnnotation) == 0)
        {
            pcValue = GetStringReference(pkEffect, hAnnot);
            break;
        }
    }

    return pcValue;
}
//---------------------------------------------------------------------------
unsigned int NiFXLParser::DetermineObjectType(
    const char* pcObjectValue, unsigned int& uiObjectIndex,
    bool bFoundObjectIndex)
{
    unsigned int uiObjectType = NiShaderAttributeDesc::OT_UNDEFINED;
    if (NiStricmp(pcObjectValue, "GeneralLight") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_GENERALLIGHT;
        if (!bFoundObjectIndex)
        {
            uiObjectIndex = ms_uiGeneralLightIndex++;
        }
    }
    else if (NiStricmp(pcObjectValue, "PointLight") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_POINTLIGHT;
        if (!bFoundObjectIndex)
        {
            uiObjectIndex = ms_uiPointLightIndex++;
        }
    }
    else if (NiStricmp(pcObjectValue, "DirectionalLight") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT;
        if (!bFoundObjectIndex)
        {
            uiObjectIndex = ms_uiDirectionalLightIndex++;
        }
    }
    else if (NiStricmp(pcObjectValue, "SpotLight") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_SPOTLIGHT;
        if (!bFoundObjectIndex)
        {
            uiObjectIndex = ms_uiSpotLightIndex++;
        }
    }
    else if (NiStricmp(pcObjectValue, "ShadowPointLight") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT;
        if (!bFoundObjectIndex)
        {
            uiObjectIndex = ms_uiShadowPointLightIndex++;
        }
    }
    else if (NiStricmp(pcObjectValue, "ShadowDirectionalLight") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_SHADOWDIRECTIONALLIGHT;
        if (!bFoundObjectIndex)
        {
            uiObjectIndex = ms_uiShadowDirectionalLightIndex++;
        }
    }
    else if (NiStricmp(pcObjectValue, "ShadowSpotLight") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT;
        if (!bFoundObjectIndex)
        {
            uiObjectIndex = ms_uiShadowSpotLightIndex++;
        }
    }

    else if (NiStricmp(pcObjectValue, "EnvironmentMap") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP;
    }
    else if (NiStricmp(pcObjectValue, "ProjectedShadowMap") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_PROJECTEDSHADOWMAP;
        if (!bFoundObjectIndex)
        {
            uiObjectIndex = ms_uiProjShadowIndex++;
        }
    }
    else if (NiStricmp(pcObjectValue, "ProjectedLightMap") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_PROJECTEDLIGHTMAP;
        if (!bFoundObjectIndex)
        {
            uiObjectIndex = ms_uiProjLightIndex++;
        }
    }
    else if (NiStricmp(pcObjectValue, "FogMap") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_FOGMAP;
    }

    return uiObjectType;
}
//---------------------------------------------------------------------------
bool NiFXLParser::LookupObjectProperty(const char* pcObjectProperty,
    unsigned int& uiPropertyID)
{
    for (unsigned int ui = 0; ui < s_uiNumObjectProperties; ui++)
    {
        NiD3DShaderConstantMap::PredefinedMapping* pkProperty =
            &s_pkObjectProperties[ui];

        if (NiStricmp(pcObjectProperty, pkProperty->m_kMappingName) == 0)
        {
            uiPropertyID = pkProperty->m_uiMappingID;
            return true;
        }
    }
    
    return false;
}
//---------------------------------------------------------------------------
bool NiFXLParser::VerifyAndAdjustMapping(FXLEffect* pkEffect,
    FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc, 
    unsigned int uiObjectType, unsigned int& uiMapping)
{
    // This function reads any supported additional annotations for the input
    // mapping, adjusting the mapping as necessary. uiMapping may be changed
    // by this function. If the mapping is not appropriate for the input
    // object type, this function returns false.

    if (!NiD3DShaderConstantMap::IsObjectMappingValidForType(
        (NiD3DShaderConstantMap::ObjectMappings) uiMapping,
        (NiShaderAttributeDesc::ObjectType) uiObjectType))
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false,
            "NiFXLParser Error: Property specified for incompatible "
            "object type on \"%s\" parameter\n",
            pkParamDesc->pName);
        return false;
    }

    const char* pcAnnotation = NULL;
    switch (uiMapping)
    {
        case NiD3DShaderConstantMap::SCM_OBJ_DIMMER:
            // No additional annotations supported.
            break;
        case NiD3DShaderConstantMap::SCM_OBJ_AMBIENT:
            pcAnnotation = LookupObjectAnnotation(pkEffect, hParam,
                pkParamDesc, "Usage");
            if (pcAnnotation && NiStricmp(pcAnnotation, "Undimmed") == 0)
            {
                uiMapping = NiD3DShaderConstantMap::SCM_OBJ_UNDIMMEDAMBIENT;
            }
            break;
        case NiD3DShaderConstantMap::SCM_OBJ_DIFFUSE:
            pcAnnotation = LookupObjectAnnotation(pkEffect, hParam,
                pkParamDesc, "Usage");
            if (pcAnnotation && NiStricmp(pcAnnotation, "Undimmed") == 0)
            {
                uiMapping = NiD3DShaderConstantMap::SCM_OBJ_UNDIMMEDDIFFUSE;
            }
            break;
        case NiD3DShaderConstantMap::SCM_OBJ_SPECULAR:
            pcAnnotation = LookupObjectAnnotation(pkEffect, hParam,
                pkParamDesc, "Usage");
            if (pcAnnotation && NiStricmp(pcAnnotation, "Undimmed") == 0)
            {
                uiMapping = NiD3DShaderConstantMap::SCM_OBJ_UNDIMMEDSPECULAR;
            }
            break;
        case NiD3DShaderConstantMap::SCM_OBJ_WORLDPOSITION:
            pcAnnotation = LookupObjectAnnotation(pkEffect, hParam,
                pkParamDesc, "Space");
            if (pcAnnotation)
            {
                if (NiStricmp(pcAnnotation, "Model") == 0)
                {
                    uiMapping = NiD3DShaderConstantMap::SCM_OBJ_MODELPOSITION;
                }
            }
            break;
        case NiD3DShaderConstantMap::SCM_OBJ_WORLDDIRECTION:
            pcAnnotation = LookupObjectAnnotation(pkEffect, hParam,
                pkParamDesc, "Space");
            if (pcAnnotation)
            {
                if (NiStricmp(pcAnnotation, "Model") == 0)
                {
                    uiMapping = NiD3DShaderConstantMap::SCM_OBJ_MODELDIRECTION;
                }
            }
            break;
        case NiD3DShaderConstantMap::SCM_OBJ_WORLDTRANSFORM:
            pcAnnotation = LookupObjectAnnotation(pkEffect, hParam,
                pkParamDesc, "Space");
            if (pcAnnotation)
            {
                if (NiStricmp(pcAnnotation, "Model") == 0)
                {
                    uiMapping = NiD3DShaderConstantMap::SCM_OBJ_MODELTRANSFORM;
                }
            }
            break;
        case NiD3DShaderConstantMap::SCM_OBJ_ATTENUATION:
            pcAnnotation = LookupObjectAnnotation(pkEffect, hParam,
                pkParamDesc, "Attenuation");
            if (pcAnnotation && NiStricmp(pcAnnotation, "Spot") == 0)
            {
                uiMapping = NiD3DShaderConstantMap::SCM_OBJ_SPOTATTENUATION;
            }
            break;
        case NiD3DShaderConstantMap::SCM_OBJ_WORLDPROJECTIONMATRIX:
            pcAnnotation = LookupObjectAnnotation(pkEffect, hParam,
                pkParamDesc, "Space");
            if (pcAnnotation)
            {
                if (NiStricmp(pcAnnotation, "Model") == 0)
                {
                    uiMapping =
                        NiD3DShaderConstantMap::SCM_OBJ_MODELPROJECTIONMATRIX;
                }
            }
            break;
        case NiD3DShaderConstantMap::SCM_OBJ_WORLDPROJECTIONTRANSLATION:
            pcAnnotation = LookupObjectAnnotation(pkEffect, hParam,
                pkParamDesc, "Space");
            if (pcAnnotation)
            {
                if (NiStricmp(pcAnnotation, "Model") == 0)
                {
                    uiMapping = NiD3DShaderConstantMap::
                        SCM_OBJ_MODELPROJECTIONTRANSLATION;
                }
            }
            break;
        case NiD3DShaderConstantMap::SCM_OBJ_WORLDCLIPPINGPLANE:
            pcAnnotation = LookupObjectAnnotation(pkEffect, hParam,
                pkParamDesc, "Space");
            if (pcAnnotation)
            {
                if (NiStricmp(pcAnnotation, "Model") == 0)
                {
                    uiMapping =
                        NiD3DShaderConstantMap::SCM_OBJ_MODELCLIPPINGPLANE;
                }
            }
            break;
        case NiD3DShaderConstantMap::SCM_OBJ_TEXCOORDGEN:
            // No additional annotations supported.
            break;
    }

    return true;
}
//---------------------------------------------------------------------------
const char* NiFXLParser::GetStringReference(FXLEffect* pkEffect, 
    FXLHANDLE hAnnotation)
{
#if defined(_DEBUG)
    FXLANNOTATION_DESC kDesc;
    pkEffect->GetAnnotationDesc(hAnnotation, &kDesc);
    NIASSERT(kDesc.Type == FXLDTYPE_STRING);
#endif
    
    // This is bad, I'm breaking encapsulation here, but it keeps
    // me from having to heap allocate strings.
    DWORD uiOffset = hAnnotation >> FXLHANDLE_ANNOTATION_OFFSET_SHIFT;
    FXLPARAMETERENTRY &kAnno = pkEffect->m_pAnnotationList[uiOffset];
    const char* pcValue = 
        (const char*)(pkEffect->m_pAnnotationData + kAnno.Offset);
    return pcValue;
}
//---------------------------------------------------------------------------
float NiFXLParser::GetFloat(FXLEffect* pkEffect, FXLHANDLE hAnnot,
    FXLANNOTATION_DESC* pkDesc)
{
    NIASSERT(pkDesc->Type == FXLDTYPE_FLOAT);
    float fValue = 0.0f;
    pkEffect->GetAnnotation(hAnnot, &fValue);
    return fValue;
}
//---------------------------------------------------------------------------
int NiFXLParser::GetInt(FXLEffect* pkEffect, FXLHANDLE hAnnot,
    FXLANNOTATION_DESC* pkDesc)
{
    NIASSERT(pkDesc->Type == FXLDTYPE_INT);
    int iValue = 0;
    pkEffect->GetAnnotation(hAnnot, &iValue);
    return iValue;
}
//---------------------------------------------------------------------------
bool NiFXLParser::GetBool(FXLEffect* pkEffect, FXLHANDLE hAnnot,
    FXLANNOTATION_DESC* pkDesc)
{
    NIASSERT(pkDesc->Type == FXLDTYPE_BOOL);
    int iValue = 0;;
    pkEffect->GetAnnotation(hAnnot, &iValue);
    return iValue != 0;
}
//---------------------------------------------------------------------------
void NiFXLParser::GetFloatVector(FXLEffect* pkEffect, FXLHANDLE hAnnot,
    FXLANNOTATION_DESC* pkDesc, void* pkVector)
{
    NIASSERT(pkDesc->Type == FXLDTYPE_FLOAT);
    NIASSERT(pkDesc->Class == FXLDCLASS_VECTOR);
    pkEffect->GetAnnotation(hAnnot, pkVector);
}
//---------------------------------------------------------------------------
void NiFXLParser::GetFloatVectorArray(FXLEffect* pkEffect, FXLHANDLE hAnnot,
    FXLANNOTATION_DESC* pkDesc, void* pkVector)
{
    NIASSERT(pkDesc->Class == FXLDCLASS_VECTOR);
    NIASSERT(pkDesc->Content == FXLPACONTENT_ARRAY);
    NIASSERT(pkDesc->Type == FXLDTYPE_FLOAT);
    pkEffect->GetAnnotation(hAnnot, pkVector);
}
//---------------------------------------------------------------------------
void NiFXLParser::GetFloatArray(FXLEffect* pkEffect, FXLHANDLE hAnnot,
    FXLANNOTATION_DESC* pkDesc, float* pfFloats)
{
    NIASSERT(pkDesc->Class == FXLDCLASS_SCALAR);
    NIASSERT(pkDesc->Content == FXLPACONTENT_ARRAY);
    NIASSERT(pkDesc->Type == FXLDTYPE_FLOAT);
    pkEffect->GetAnnotation(hAnnot, pfFloats);
}
//---------------------------------------------------------------------------

