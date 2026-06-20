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
#include "NiMainPCH.h"

#include "NiStandardMaterialDescriptor.h"
#include "NiShadowManager.h"
#include "NiShadowGenerator.h"
#include "NiShadowTechnique.h"
#include "NiAmbientLight.h"
#include "NiDirectionalLight.h"
#include "NiPointLight.h"
#include "NiSpotLight.h"

//---------------------------------------------------------------------------
NiStandardMaterialDescriptor::NiStandardMaterialDescriptor()
{
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::AddLight(NiLight* pkLight, 
    unsigned int uiWhichLight, const NiGeometry* pkGeometry)
{
    if (uiWhichLight < NiStandardMaterial::STANDARD_PIPE_MAX_LIGHTS)
    {
        bool bFound = false;
        if (NiIsKindOf(NiSpotLight, pkLight))
        {
            unsigned int uiSpotLights = GetSPOTLIGHTCOUNT() + 1;
            SetSPOTLIGHTCOUNT(uiSpotLights);
            bFound = true;
        }
        else if (NiIsKindOf(NiPointLight, pkLight))
        {
            unsigned int uiPointLights = GetPOINTLIGHTCOUNT() + 1;
            SetPOINTLIGHTCOUNT(uiPointLights);
            bFound = true;
        }
        else if (NiIsKindOf(NiDirectionalLight, pkLight))
        {
            unsigned int uiDirLights = GetDIRLIGHTCOUNT() + 1;
            SetDIRLIGHTCOUNT(uiDirLights);
            bFound = true;
        }

        NiShadowGenerator* pkShadowGen = pkLight->GetShadowGenerator();
        if (bFound && pkShadowGen && NiShadowManager::GetShadowManager() &&
            NiShadowManager::GetActive())
        {
            if (!pkShadowGen->GetActive())
                return true;

            if (pkShadowGen->IsUnaffectedReceiverNode(pkGeometry))
                return true;

            bool bFoundShadowMap = false;
            const unsigned int uiShadowMapCount =
                pkShadowGen->GetShadowMapCount();
            for (unsigned int uiShadowMap = 0; uiShadowMap < uiShadowMapCount;
                uiShadowMap++)
            {
                if (pkShadowGen->GetOwnedShadowMap(uiShadowMap))
                {
                    bFoundShadowMap = true;
                    break;
                }
            }
            if (!bFoundShadowMap)
            {
                return true;
            }

            NiShadowTechnique* pkTechnique = pkShadowGen->GetShadowTechnique();

            if (pkTechnique)
            {
                unsigned short usActiveSlot = 
                    pkTechnique->GetActiveTechniqueSlot();

                // If this assert is hit it means the light is assigned a 
                // NiShadowTechnique that is not one of the active 
                // NiShadowTechniques.
                NIASSERT (usActiveSlot < 
                    NiShadowManager::MAX_ACTIVE_SHADOWTECHNIQUES );
                            
                SetLightInfo(uiWhichLight, true);

                // Only use the assigned shadow technique from the light if the
                // NiShadowTechnique has a higher priority than the one already
                // applied. Note: NiShadowTechnique priority is defined by the
                // slot the NiShadowTechnique is assigned to. 
                if ( usActiveSlot > GetSHADOWTECHNIQUE())
                    SetSHADOWTECHNIQUE(usActiveSlot);
            }
        }
        
        return bFound;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::GetLightType(unsigned int uiWhichLight, 
    NiStandardMaterial::LightType& eLightType)
{
    unsigned int uiPointLights = GetPOINTLIGHTCOUNT();
    unsigned int uiDirLights = GetDIRLIGHTCOUNT();
    unsigned int uiSpotLights = GetSPOTLIGHTCOUNT();

    if (uiWhichLight > uiPointLights + uiDirLights + uiSpotLights)
    {
        return false;
    }
    else if (uiWhichLight > uiPointLights + uiDirLights)
    {
        eLightType = NiStandardMaterial::LIGHT_SPOT;
        return true;
    }
    else if (uiWhichLight > uiPointLights)
    {
        eLightType = NiStandardMaterial::LIGHT_DIR;
        return true;
    }
    else
    {
        eLightType = NiStandardMaterial::LIGHT_POINT;
        return true;
    }
}
//---------------------------------------------------------------------------
bool NiStandardMaterialDescriptor::GetLightInfo(unsigned int uiWhichLight, 
    bool& bShadowed)
{
    unsigned int uiPointLights = GetPOINTLIGHTCOUNT();
    unsigned int uiDirLights = GetDIRLIGHTCOUNT();
    unsigned int uiSpotLights = GetSPOTLIGHTCOUNT();

    if (uiWhichLight > uiPointLights + uiDirLights + uiSpotLights)
    {
       return false;
    }

    unsigned int uiShadowBits = GetSHADOWMAPFORLIGHT();
    bShadowed = NiTGetBit< unsigned int >(uiShadowBits, 1 << uiWhichLight);

    return true;
}
//---------------------------------------------------------------------------
void NiStandardMaterialDescriptor::SetLightInfo(unsigned int uiWhichLight, 
    bool bShadowed)
{
    unsigned int uiShadowBits = GetSHADOWMAPFORLIGHT();
    NiTSetBit< unsigned int >(uiShadowBits, bShadowed, 
        1 << uiWhichLight);

    SetSHADOWMAPFORLIGHT(uiShadowBits);

}
//---------------------------------------------------------------------------
void NiStandardMaterialDescriptor::SetTextureUsage(
    unsigned int uiWhichTexture, unsigned int uiInputUV,
    NiStandardMaterial::TexGenOutput eTextureOutput)
{
    switch (uiWhichTexture)
    {
    default:
        NIASSERT(!"Should not get here!");
        break;
    case 0:
        SetMAP00(uiInputUV); 
        SetMAP00TEXOUTPUT(eTextureOutput);
        break;
    case 1:
        SetMAP01(uiInputUV);
        SetMAP01TEXOUTPUT(eTextureOutput);
        break;
    case 2:
        SetMAP02(uiInputUV); 
        SetMAP02TEXOUTPUT(eTextureOutput);
        break;
    case 3:
        SetMAP03(uiInputUV); 
        SetMAP03TEXOUTPUT(eTextureOutput);
        break;
    case 4:
        SetMAP04(uiInputUV); 
        SetMAP04TEXOUTPUT(eTextureOutput);
        break;
    case 5:
        SetMAP05(uiInputUV); 
        SetMAP05TEXOUTPUT(eTextureOutput);
        break;
    case 6:
        SetMAP06(uiInputUV); 
        SetMAP06TEXOUTPUT(eTextureOutput);
        break;
    case 7:
        SetMAP07(uiInputUV); 
        SetMAP07TEXOUTPUT(eTextureOutput);
        break;
    case 8:
        SetMAP08(uiInputUV); 
        SetMAP08TEXOUTPUT(eTextureOutput);
        break;
    case 9:
        SetMAP09(uiInputUV); 
        SetMAP09TEXOUTPUT(eTextureOutput);
        break;
    case 10:
        SetMAP10(uiInputUV); 
        SetMAP10TEXOUTPUT(eTextureOutput);
        break;
    case 11:
        SetMAP11(uiInputUV); 
        SetMAP11TEXOUTPUT(eTextureOutput);
        break;
    }
}
//---------------------------------------------------------------------------
void NiStandardMaterialDescriptor::GetTextureUsage(
    unsigned int uiWhichTexture, unsigned int& uiInputUV,
    NiStandardMaterial::TexGenOutput& eTextureOutput)
{
    switch (uiWhichTexture)
    {
    default:
        NIASSERT(!"Should not get here!");
        break;
    case 0:
        uiInputUV = GetMAP00();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP00TEXOUTPUT();
        break;
    case 1:
        uiInputUV = GetMAP01();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP01TEXOUTPUT();
        break;
    case 2:
        uiInputUV = GetMAP02();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP02TEXOUTPUT();
        break;
    case 3:
        uiInputUV = GetMAP03();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP03TEXOUTPUT();
        break;
    case 4:
        uiInputUV = GetMAP04();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP04TEXOUTPUT();
        break;
    case 5:
        uiInputUV = GetMAP05();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP05TEXOUTPUT();
        break;
    case 6:
        uiInputUV = GetMAP06();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP06TEXOUTPUT();
        break;
    case 7:
        uiInputUV = GetMAP07();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP07TEXOUTPUT();
        break;
    case 8:
        uiInputUV = GetMAP08();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP08TEXOUTPUT();
        break;
    case 9:
        uiInputUV = GetMAP09();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP09TEXOUTPUT();
        break;
    case 10:
        uiInputUV = GetMAP10();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP10TEXOUTPUT();
        break;
    case 11:
        uiInputUV = GetMAP11();
        eTextureOutput = (NiStandardMaterial::TexGenOutput)
            GetMAP11TEXOUTPUT();
        break;
    }
}
//---------------------------------------------------------------------------
unsigned int NiStandardMaterialDescriptor::GetStandardTextureCount()
{
    unsigned int uiCount = 
        GetPARALLAXMAPCOUNT() + 
        GetBASEMAPCOUNT() +        
        GetNORMALMAPCOUNT() +      
        GetDARKMAPCOUNT() +        
        GetDETAILMAPCOUNT() +      
        GetBUMPMAPCOUNT() +        
        GetGLOSSMAPCOUNT()+       
        GetGLOWMAPCOUNT() +
        GetCUSTOMMAP00COUNT() + 
        GetCUSTOMMAP01COUNT() + 
        GetCUSTOMMAP02COUNT() + 
        GetCUSTOMMAP03COUNT() + 
        GetCUSTOMMAP04COUNT() + 
        GetDECALMAPCOUNT();
    return uiCount;
}
//---------------------------------------------------------------------------
void NiStandardMaterialDescriptor::SetProjectedLight(unsigned int uiWhichLight,
    bool bClipped, NiStandardMaterial::TexEffectType eType)
{
    NIASSERT(eType == NiStandardMaterial::TEXEFFECT_WORLD_PARALLEL ||
        eType == NiStandardMaterial::TEXEFFECT_WORLD_PERSPECTIVE);
    NIASSERT(uiWhichLight < (1<<PROJLIGHTMAPCOUNT_SIZE));

    unsigned int uiField = GetPROJLIGHTMAPCLIPPED();
    NiTSetBit<unsigned int>(uiField, bClipped, 1 << uiWhichLight);
    SetPROJLIGHTMAPCLIPPED(uiField);

    bool bPersp = true;
    if (eType == NiStandardMaterial::TEXEFFECT_WORLD_PARALLEL)
        bPersp = false;

    uiField = GetPROJLIGHTMAPTYPES();
    NiTSetBit<unsigned int>(uiField, bPersp, 1 << uiWhichLight);
    SetPROJLIGHTMAPTYPES(uiField);

}
//---------------------------------------------------------------------------
void NiStandardMaterialDescriptor::GetProjectedLight(unsigned int uiWhichLight,
    bool& bClipped, NiStandardMaterial::TexEffectType& eType)
{
    NIASSERT(uiWhichLight < (1<<PROJLIGHTMAPCOUNT_SIZE));

    unsigned int uiField = GetPROJLIGHTMAPCLIPPED();
    bClipped = NiTGetBit<unsigned int>(uiField, 1 << uiWhichLight);

    bool bPersp = true;   
    uiField = GetPROJLIGHTMAPTYPES();
    bPersp = NiTGetBit<unsigned int>(uiField, 1 << uiWhichLight);

    if (!bPersp)
        eType = NiStandardMaterial::TEXEFFECT_WORLD_PARALLEL;
    else
        eType = NiStandardMaterial::TEXEFFECT_WORLD_PERSPECTIVE;
}
//---------------------------------------------------------------------------
void NiStandardMaterialDescriptor::SetProjectedShadow(
    unsigned int uiWhichShadow, bool bClipped,
    NiStandardMaterial::TexEffectType eType)
{
    NIASSERT(eType == NiStandardMaterial::TEXEFFECT_WORLD_PARALLEL ||
        eType == NiStandardMaterial::TEXEFFECT_WORLD_PERSPECTIVE);
    NIASSERT(uiWhichShadow < (1<<PROJSHADOWMAPCOUNT_SIZE));

    unsigned int uiField = GetPROJSHADOWMAPCLIPPED();
    NiTSetBit<unsigned int>(uiField, bClipped, 1 << uiWhichShadow);
    SetPROJSHADOWMAPCLIPPED(uiField);

    bool bPersp = true;
    if (eType == NiStandardMaterial::TEXEFFECT_WORLD_PARALLEL)
        bPersp = false;

    uiField = GetPROJSHADOWMAPTYPES();
    NiTSetBit<unsigned int>(uiField, bPersp, 1 << uiWhichShadow);
    SetPROJSHADOWMAPTYPES(uiField);

}
//---------------------------------------------------------------------------
void NiStandardMaterialDescriptor::GetProjectedShadow(
    unsigned int uiWhichShadow, bool& bClipped, 
    NiStandardMaterial::TexEffectType& eType)
{
    NIASSERT(uiWhichShadow < (1<<PROJSHADOWMAPCOUNT_SIZE));

    unsigned int uiField = GetPROJSHADOWMAPCLIPPED();
    bClipped = NiTGetBit<unsigned int>(uiField, 1 << uiWhichShadow);

    bool bPersp = true;   
    uiField = GetPROJSHADOWMAPTYPES();
    bPersp = NiTGetBit<unsigned int>(uiField, 1 << uiWhichShadow);

    if (!bPersp)
        eType = NiStandardMaterial::TEXEFFECT_WORLD_PARALLEL;
    else
        eType = NiStandardMaterial::TEXEFFECT_WORLD_PERSPECTIVE;
}
//---------------------------------------------------------------------------
NiString NiStandardMaterialDescriptor::ToString()
{
    NiString kString;
    ToStringCUSTOMMAP03COUNT(kString, true);
    ToStringPROJLIGHTMAPCLIPPED(kString, true);
    ToStringMAP05TEXOUTPUT(kString, true);
    ToStringALPHATEST(kString, true);
    return kString;
}
//---------------------------------------------------------------------------
