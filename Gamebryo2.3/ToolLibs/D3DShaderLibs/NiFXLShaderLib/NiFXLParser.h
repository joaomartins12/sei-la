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

#ifndef NIFXLPARSER_H
#define NIFXLPARSER_H

#include "NiFXLShaderLibType.h"

class NiFXLParameterDesc;
class NiFXLParameterMatrixDesc;
class NiFXLParameterVectorDesc;
class NiFXLParameterFloatDesc;
class NiFXLParameterBooleanDesc;
class NiFXLParameterIntegerDesc;
class NiFXLParameterTextureDesc;
class NiFXLParameterPredefinedDesc;
class NiFXLTechniqueDesc;
class NiFXLParameterArrayDesc;

class NIFXLSHADER_ENTRY NiFXLParser : public NiMemObject
{
public:
    static bool GetParametersAndTechniques(FXLEffect* pkEffect, 
        unsigned int& uiNumParameters, unsigned int& uiNumTechniques);
    static NiFXLParameterDesc* GetParameter(FXLEffect* pkEffect, 
        unsigned int uiIndex);
    static NiFXLTechniqueDesc* GetTechnique(FXLEffect* pkEffect, 
        unsigned int uiIndex);

    static NiFXLParameterDesc* GetParameterDesc(FXLEffect* pkEffect, 
        FXLHANDLE hParam);

    static NiFXLTechniqueDesc* GetTechniqueDesc(FXLEffect* pkEffect, 
        FXLHANDLE hParam);
    
    static void ResetIndices();

    // Begin Emergent internal use only
    static void _SDMInit();
    static void _SDMShutdown();
    // End Emergent internal use only

protected:
    static void ParseMatrixAnnotations(FXLEffect* pkEffect, 
        FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc, 
        NiFXLParameterMatrixDesc* pkDesc);
    static void ParseVectorAnnotations(FXLEffect* pkEffect, 
        FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc, 
        NiFXLParameterVectorDesc* pkDesc);
    static void ParseFloatAnnotations(FXLEffect* pkEffect, 
        FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc, 
        NiFXLParameterFloatDesc* pkDesc);
    static void ParseBooleanAnnotations(FXLEffect* pkEffect, 
        FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc, 
        NiFXLParameterBooleanDesc* pkDesc);
    static void ParseIntegerAnnotations(FXLEffect* pkEffect, 
        FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc, 
        NiFXLParameterIntegerDesc* pkDesc);
    static NiFXLParameterSamplerDesc* ParseSamplerAnnotations(
        FXLEffect* pkEffect, FXLHANDLE hParam, 
        FXLPARAMETER_DESC* pkParamDesc);
    static void ParsePredefinedAnnotations(FXLEffect* pkEffect, 
        FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc, 
        NiFXLParameterPredefinedDesc* pkDesc);
    static void ParseTechniqueAnnotations(FXLEffect* pkEffect, 
        FXLHANDLE hTech, FXLTECHNIQUE_DESC* pkTechDesc, 
        NiFXLTechniqueDesc* pkDesc);
    static void ParseArrayAnnotations(FXLEffect* pkEffect, 
        FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc, 
        NiFXLParameterArrayDesc* pkDesc);
    
    static NiFXLParameterDesc* QueryPredefined(FXLEffect* pkEffect,
        FXLHANDLE hParam, bool bColumnMatrix, unsigned int uiArrayElements);

    static NiFXLParameterDesc* QueryObject(FXLEffect* pkEffect,
        FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc);
    static const char* LookupObjectAnnotation(FXLEffect* pkEffect,
        FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc, 
        const char* pcAnnotation);
    static unsigned int DetermineObjectType(const char* pcObjectValue,
        unsigned int& uiObjectIndex, bool bFoundObjectIndex);
    static bool LookupObjectProperty(const char* pcObjectProperty,
        unsigned int& uiPropertyID);
    static bool VerifyAndAdjustMapping(FXLEffect* pkEffect,
        FXLHANDLE hParam, FXLPARAMETER_DESC* pkParamDesc, 
        unsigned int uiObjectType, unsigned int& uiMapping);

    static const char* GetStringReference(FXLEffect* pkEffect, 
        FXLHANDLE hAnnotation);
    static float GetFloat(FXLEffect* pkEffect, FXLHANDLE hAnnot,
        FXLANNOTATION_DESC* pkDesc);
    static int GetInt(FXLEffect* pkEffect, FXLHANDLE hAnnot,
        FXLANNOTATION_DESC* pkDesc);
    static bool GetBool(FXLEffect* pkEffect, FXLHANDLE hAnnot,
        FXLANNOTATION_DESC* pkDesc);

    // We use D3DXVECTOR objects for the float vectors, but since the FXL
    // functions use void*, we'll just pass those.
    static void GetFloatVector(FXLEffect* pkEffect, FXLHANDLE hAnnot,
        FXLANNOTATION_DESC* pkDesc, void* pkVector);
    static void GetFloatVectorArray(FXLEffect* pkEffect, FXLHANDLE hAnnot,
        FXLANNOTATION_DESC* pkDesc, void* pkVector);

    static void GetFloatArray(FXLEffect* pkEffect, FXLHANDLE hAnnot,
        FXLANNOTATION_DESC* pkDesc, float* pfFloats);

    static unsigned int ms_uiShaderMapIndex;
    static unsigned int ms_uiGeneralLightIndex;
    static unsigned int ms_uiPointLightIndex;
    static unsigned int ms_uiDirectionalLightIndex;
    static unsigned int ms_uiSpotLightIndex;
    static unsigned int ms_uiShadowPointLightIndex;
    static unsigned int ms_uiShadowDirectionalLightIndex;
    static unsigned int ms_uiShadowSpotLightIndex;
    static unsigned int ms_uiProjShadowIndex;
    static unsigned int ms_uiProjLightIndex;
};

#endif // NIFXLPARSER_H
