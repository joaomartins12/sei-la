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


#include "NiOFConst.h"  // for degree-to-radian conversion
#include "NiOFLight.h"

#include <NiDirectionalLight.h>
#include <NiNode.h>
#include <NiPointLight.h>
#include <NiSpotLight.h>

#include <fltcode.h>
#include <mgapiattr.h>
#include <mgapilight.h>
#include <mgapistruc.h>

// caches for textures, materials, and lights
NiTStringMap<NiOFLight*> NiOFLight::ms_kLightCache;

bool NiOFLight::ms_bAddDefaultLights = true;

class NiOFLightInit  // pre-main initialization, post-main termination
{
public:
    NiOFLightInit ()
    {
    };

    ~NiOFLightInit ()
    {
        NiTMapIterator pos = NiOFLight::ms_kLightCache.GetFirstPos();
        while ( pos )
        {
            const char* pcName;
            NiOFLight* pkLight;
            NiOFLight::ms_kLightCache.GetNext(pos, pcName, pkLight);
            NiDelete pkLight;
        }
        NiOFLight::ms_kLightCache.RemoveAll();
    };
};

//---------------------------------------------------------------------------
NiOFLight::NiOFLight (mgrec* pkLtsrec) :
    m_kAmbi(0,0,0),
    m_kDiff(0,0,0),
    m_kSpec(0,0,0)
{
    m_iPaletteId = -1;
    m_eType = LR_INFINITE;
    m_fAttenConst = 1.0f;
    m_fAttenLinear = 0.0f;
    m_fAttenQuad = 0.0f;
    m_fSpotExponent = 1.0f;
    m_fSpotAngle = 0.0f;
    m_fSpotYaw = 0.0f;
    m_fSpotPitch = 0.0f;
    m_bModelingLight = false;

    // get palette index;
    mgGetAttList(pkLtsrec, fltLtspPaletteId, &m_iPaletteId, mgNULL);

    float fDummyAlpha;  // get light colors
    mgGetColorRGBA(pkLtsrec, fltLtspAmbient, &m_kAmbi.r, &m_kAmbi.g, 
        &m_kAmbi.b, &fDummyAlpha);
    mgGetColorRGBA(pkLtsrec, fltLtspDiffuse, &m_kDiff.r, &m_kDiff.g,
        &m_kDiff.b, &fDummyAlpha);
    mgGetColorRGBA(pkLtsrec, fltLtspSpecular, &m_kSpec.r, &m_kSpec.g,
        &m_kSpec.b, &fDummyAlpha);

    // determine if light illuminates everything in model
    int iIlluminateAll = 0;
    mgGetAttList(pkLtsrec, fltLtspModeling, &iIlluminateAll, mgNULL);
    m_bModelingLight = (iIlluminateAll != 0);

    // get light type
    int iLightType;
    mgGetAttList(pkLtsrec, fltLtspType, &iLightType, mgNULL);
    m_eType = (unsigned int)iLightType;

    switch (iLightType)
    {
        case LR_INFINITE:
        {
            mgGetAttList(pkLtsrec, fltLtspYaw, &m_fSpotYaw, fltLtspPitch, 
                &m_fSpotPitch, mgNULL);

            m_fSpotYaw = RADIANS_PER_DEGREE * m_fSpotYaw;
            m_fSpotPitch = RADIANS_PER_DEGREE * m_fSpotPitch;
            break;
        }
        case LR_LOCAL:
        {
            mgGetAttList(pkLtsrec, fltLtspConstAtten, &m_fAttenConst,
                fltLtspLinearAtten, &m_fAttenLinear, fltLtspQuadAtten,
                &m_fAttenQuad, mgNULL);
            break;
        }
        case LR_SPOT:
        {
            mgGetAttList(pkLtsrec, fltLtspSpotExp, &m_fSpotExponent,
                fltLtspSpotSpread, &m_fSpotAngle, fltLtspYaw, &m_fSpotYaw,
                fltLtspPitch, &m_fSpotPitch, fltLtspConstAtten, 
                &m_fAttenConst, fltLtspLinearAtten, &m_fAttenLinear,
                fltLtspQuadAtten, &m_fAttenQuad, mgNULL);

            m_fSpotAngle = RADIANS_PER_DEGREE * m_fSpotAngle;
            m_fSpotYaw = RADIANS_PER_DEGREE * m_fSpotYaw;
            m_fSpotPitch = RADIANS_PER_DEGREE * m_fSpotPitch;
            break;
        }
    }

    // add light record to cache
    char acStr[32];
    NiSprintf(acStr, 32, "Light:%d", m_iPaletteId);
    ms_kLightCache.SetAt(acStr, this);
}

//---------------------------------------------------------------------------
void NiOFLight::GetLightPalette (mgrec* pkDB)
{
    if (mgGetLightSourceCount(pkDB) > 0)
    {
        int iIndex;
        mgrec* pkLtsrec = mgGetFirstLightSource(pkDB, &iIndex);

        // pLightRecord's put in static cache
        NiOFLight* pkLightRecord = NiExternalNew NiOFLight(pkLtsrec);
        while (pkLtsrec = mgGetNextLightSource(pkLtsrec, &iIndex))
        {
            pkLightRecord = NiExternalNew NiOFLight(pkLtsrec);
        }
    }
}

//---------------------------------------------------------------------------
NiLight* NiOFLight::GetLight ()
{
    NiLight * pkLight = NULL;

    switch ( m_eType )
    {
        case LR_INFINITE:
            pkLight = NiNew NiDirectionalLight;
            break;
        case LR_LOCAL:
            pkLight = NiNew NiPointLight;
            break;
        case LR_SPOT:
        {
            NiSpotLight* pkSpotLight = NiNew NiSpotLight;
            NIASSERT(pkSpotLight);

            pkSpotLight->SetSpotAngle(m_fSpotAngle);
            pkSpotLight->SetSpotExponent(m_fSpotExponent);

            pkLight = pkSpotLight;
            break;
        }
    }

    NIASSERT(pkLight);

    pkLight->SetAmbientColor(m_kAmbi);  // light colors
    pkLight->SetDiffuseColor(m_kDiff);
    pkLight->SetSpecularColor(m_kSpec);

    pkLight->Update(0.0f);

    return pkLight;
}

//---ProcessLights-------------------------------------------------------------

void NiOFLight::ProcessLights (mgrec* pkDB, NiNode* pkRotNode, 
    NiNode* pkModel)
{
    NiLightPtr spLight = NULL;

    NiTObjectArray<NiLightPtr> kLights;

    GetLightPalette(pkDB);

    NiTMapIterator pos = ms_kLightCache.GetFirstPos();
    while ( pos )
    {
        const char* pcName;
        NiOFLight* pkLightRecord;
        ms_kLightCache.GetNext(pos, pcName, pkLightRecord);

        // modeling lights always enabled and illuminate entire scene graph
        if (pkLightRecord->m_bModelingLight)
        {
            spLight = pkLightRecord->GetLight();

            // Modeling lights are attached to camera; ignore Multigen's
            //    setting

            pkRotNode->AttachChild(spLight);
            kLights.Add(spLight);
        }
    }

    // OpenFlight does not have concept of light illuminating a subtree,
    //    so all lights must illuminate root of tree.
    unsigned int i;
    for (i = 0; i < kLights.GetSize(); i++)
    {
        NiLightPtr spLight = kLights.GetAt(i);

        spLight->AttachAffectedNode(pkModel);
    }

    if (ms_bAddDefaultLights)
    {
        // Add two reasonable "default" lights in case lights defined in the
        // OpenFlight file are not reasonable.
        NiDirectionalLight* pkLight = NiNew NiDirectionalLight;
        pkLight->SetAmbientColor(NiColor(1.0f, 1.0f, 1.0f)*0.25f);
        pkLight->SetDiffuseColor(NiColor(1.0f, 1.0f, 1.0f));
        pkLight->SetRotate(-NI_PI*0.3f, 0.0f, 1.0f, 0.0f);
        pkLight->AttachAffectedNode(pkModel);
        pkModel->AttachChild(pkLight);

        pkLight = NiNew NiDirectionalLight;
        pkLight->SetAmbientColor(NiColor(0.25f, 0.25f, 0.25f)*0.25f);
        pkLight->SetDiffuseColor(NiColor(0.25f, 0.25f, 0.25f));
        pkLight->SetRotate(NI_PI*0.7f, 0.0f, 1.0f, 0.0f);
        pkLight->AttachAffectedNode(pkModel);
        pkModel->AttachChild(pkLight);
    }

#ifdef TEST_LIGHTS
    NiDirectionalLight* pkTestLight = NiNew NiDirectionalLight;
    pkTestLight->SetAmbientColor(NiColor(1, 0, 0)*0.25);
    pkTestLight->SetDiffuseColor(NiColor(1, 0, 0));
    pkTestLight->SetRotate(NI_PI, 1, 0, 0);
    pkTestLight->AttachAffectedNode(pkModel);
    pkModel->AttachChild(pkTestLight);

    pkTestLight = NiNew NiDirectionalLight;
    pkTestLight->SetAmbientColor(NiColor(0, 1, 0)*0.25);
    pkTestLight->SetDiffuseColor(NiColor(0, 1, 0));
    pkTestLight->SetRotate(-NI_PI/2, 0, 0, 1);
    pkTestLight->AttachAffectedNode(pkModel);
    pkModel->AttachChild(pkTestLight);

    pkTestLight = NiNew NiDirectionalLight;
    pkTestLight->SetAmbientColor(NiColor(0, 0, 1)*0.25);
    pkTestLight->SetDiffuseColor(NiColor(0, 0, 1));
    pkTestLight->SetRotate(NI_PI/2, 0, 1, 0);
    pkTestLight->AttachAffectedNode(pkModel);
    pkModel->AttachChild(pkTestLight);
#endif

}

//----------------------------------------------------------------------------
NiNode* NiOFLight::CreateLight(mgrec* pkRec)
{
    NiMatrix3 kRot;
    int iIndex;
    char acStr[32];
    NiOFLight* pkLightRecord;
    int iIlluminateAll = 0;
    float fYaw = 0.0f, fPitch = 0.0f, fRoll = 0.0f;

    int iEnabled = 0;   // Initialize "mgbool (4)" to "false".  

    // get light record from cache
    mgGetAttList(pkRec, fltLtsIndex, &iIndex, mgNULL);
    NiSprintf(acStr, 32, "Light:%d", iIndex);
    pkLightRecord = NiOFLight::GetLightByName(acStr);

    NiLight* pkLight = pkLightRecord->GetLight();
    if (!pkLightRecord) // Sanity check on whether light is found.
        return NULL;

    // get attributes specific to this light (may override palette)
    mgGetAttList(pkRec, fltLtsEnabled, &iEnabled, mgNULL);

    // modeling lights illuminate entire scene graph; note we create 2
    //    modeling lights that are the 3DMAX defaults and ignore all Multigen
    //    modeling lights (for consistency with 3DMAX)

    // If the light source is a part of the scene graph, then the following
    //    call does not return that the light source illuminates all geometry
    //    in the scene, regardless of whether the light is set to a "modeling
    //    light" or not.  For this reason the code is commented out.
    // - 7/10/2000
    // determine if light illuminates everything in model
    //mgGetAttList(pkRec, fltLtspModeling, &illuminateAll, mgNULL);
    //if (illuminateAll != 0)
    //{
    //    return NULL;
    //}
    //else...

    pkLight->SetSwitch(true);
    pkLight->SetDimmer(1.0f);

    if ( pkLightRecord->GetType() == NiOFLight::LR_INFINITE 
    ||   pkLightRecord->GetType() == NiOFLight::LR_SPOT )
    {
        mgGetAttList(pkRec, fltLtsYaw, &fYaw, fltLtsPitch, &fPitch, mgNULL);
        fYaw = RADIANS_PER_DEGREE * (fYaw + 90.0f);  // offset 90 degrees due
                // to Multigen starting at (0, 1, 0), but Gamebryo starting
                // at (1, 0, 0)
        fPitch = RADIANS_PER_DEGREE * fPitch;

        // Multigen light: [Right | Dir | Up]
        NiMatrix3 kYawMat, kPitchMat, kRollMat;
        kYawMat.MakeZRotation(fYaw);
        kPitchMat.MakeXRotation(fPitch);
        kRollMat.MakeYRotation(fRoll);
        NiMatrix3 kMat = kYawMat * kPitchMat * kRollMat;

        // Gamebryo light:  [Dir | Up | Right]

        kRot.SetEntry(0, 0, kMat.GetEntry(0, 1));
        kRot.SetEntry(0, 1, kMat.GetEntry(0, 2));
        kRot.SetEntry(0, 2, kMat.GetEntry(0, 0));
        kRot.SetEntry(1, 0, kMat.GetEntry(1, 1));
        kRot.SetEntry(1, 1, kMat.GetEntry(1, 2));
        kRot.SetEntry(1, 2, kMat.GetEntry(1, 0));
        kRot.SetEntry(2, 0, kMat.GetEntry(2, 1));
        kRot.SetEntry(2, 1, kMat.GetEntry(2, 2));
        kRot.SetEntry(2, 2, kMat.GetEntry(2, 0));

        pkLight->SetRotate(kRot);
    }

    if ( pkLightRecord->GetType() == NiOFLight::LR_LOCAL 
    ||   pkLightRecord->GetType() == NiOFLight::LR_SPOT )
    {
        double x = 0.0, y = 0.0, z = 0.0;
        mgGetIcoord(pkRec, fltLtsPosition, &x, &y, &z);
        NiPoint3 kLoc((float)x,(float)y,(float)z);
        pkLight->SetTranslate(kLoc);
    }

    if (pkLight)
    {
        NiNode* pkNode = NiNew NiNode;
    
        // attach sub-tree to spLight for scope
        pkLight->AttachAffectedNode(pkNode);

        pkNode->AttachChild(pkLight);

        return pkNode;
    }
    else
    {
        return NULL;
    }
}