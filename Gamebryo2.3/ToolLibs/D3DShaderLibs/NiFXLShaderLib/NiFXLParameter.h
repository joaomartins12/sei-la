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

#ifndef NIFXLPARAMETER_H
#define NIFXLPARAMETER_H

#include "NiFXLShaderLibType.h"
#include "NiFXLParameterDesc.h"

#include <NiD3DTextureStage.h>
#include <NiD3DShaderConstantMap.h>
#include <NiFixedString.h>

class NiFXLAttributeTable;

class NIFXLSHADER_ENTRY NiFXLParameter : public NiMemObject
{
public:
    NiFXLParameter();
    ~NiFXLParameter();

    bool Initialize(NiFXLParameterDesc* pkParamDesc, 
        NiFXLAttributeTable* pkAttribTable);

    NiD3DError SetParameter(FXLEffect* pkEffect, NiGeometry* pkGeometry, 
        const NiSkinInstance* pkSkin, 
        const NiSkinPartition::Partition* pkPartition, 
        NiGeometryBufferData* pkBuffData, const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects, const NiTransform& kWorld, 
        const NiBound& kWorldBound, unsigned int uiPass);

    // *** begin Emergent internal use only ***

    const NiFixedString& GetName() const;

    void SetParameterPtr(FXLParameterPtr pkParam);
    FXLParameterPtr GetParameterPtr() const;

    void LinkHandle(FXLEffect* pkEffect);

    bool RequiresTime() const;

    void DestroyRendererData();

    NiD3DShaderConstantMap::DefinedMappings GetPredefinedMapping() const;
    unsigned int GetArrayCount() const;

    static void ReleaseBoneArray();

    // *** end Emergent internal use only ***

protected:
    void Construct2DTexture();
    void Construct3DTexture();
    void ConstructCubeTexture();

    bool FillPredefinedConstantValue(FXLEffect* pkEffect, 
        NiGeometry* pkGeometry, const NiSkinInstance* pkSkin, 
        const NiSkinPartition::Partition* pkPartition, 
        NiGeometryBufferData* pkBuffData, const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects, const NiTransform& kWorld, 
        const NiBound& kWorldBound, unsigned int uiPass);
    NiD3DError FillObjectConstantValue(FXLEffect* pkD3DXEffect,
        NiGeometry* pkGeometry, const NiSkinInstance* pkSkin, 
        const NiSkinPartition::Partition* pkPartition, 
        NiGeometryBufferData* pkBuffData, const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects, const NiTransform& kWorld, 
        const NiBound& kWorldBound, unsigned int uiPass);
    D3DBaseTexturePtr GetNiTexture(NiTexturingProperty* pkTexProp, 
        unsigned int uiTextureFlags);

    FXLParameterPtr m_pkParam;
    NiFixedString m_kName;
    bool m_bGlobal;

    NiD3DShaderConstantMap::DefinedMappings m_ePredefinedMapping;
    NiD3DShaderConstantMap::ObjectMappings m_eObjectMapping;
    NiFXLParameterDesc::ParameterType m_eParameterType;
    NiFXLParameterSamplerDesc::TextureType m_eTextureType;
    unsigned int m_uiParameterCount;
    unsigned int m_uiArrayCount;

    unsigned int m_uiExtraData;

    // Texture-specific variables
    char* m_pcTextureSource;
    char* m_pcTextureTarget;
    unsigned int m_uiTextureFlags;
    D3DBaseTexturePtr m_pkTexture;
    unsigned int m_uiWidth;
    unsigned int m_uiHeight;
    unsigned int m_uiDepth;

    // Object-specific variables.
    NiShaderAttributeDesc::ObjectType m_eObjectType;

    static D3DXVECTOR4 
        ms_akVector4Array[NiFXLParameterArrayDesc::MAX_ROWS];

    static D3DXMATRIX* ms_pkMatrixArray;
    static unsigned int ms_uiMatrixArraySize;
    static float ms_afObjectData[16];
};

#include "NiFXLParameter.inl"

#endif //NIFXLPARAMETER_H
