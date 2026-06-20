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


#include "NiOpenFlight.h"
#include "NiOFPolygon.h"

#include <NiAlphaProperty.h>
#include <NiImageConverter.h>
#include <NiPixelData.h>
#include <NiPoint2.h>
#include <NiPoint3.h>
#include <NiTriShape.h>
#include <NiTStringMap.h>
#include <NiVertexColorProperty.h>
#include <NiWireframeProperty.h>


#include <fltcode.h>
#include <mgapiattr.h>
#include <mgapicolor.h>
#include <mgapimaterial.h>
#include <mgapistruc.h>
#include <mgapitexture.h>
#include <mgapiutil.h>


// shared properties
NiAlphaPropertyPtr NiOFPolygon::ms_spAlpha;
NiSpecularPropertyPtr NiOFPolygon::ms_spSpec;
NiVertexColorPropertyPtr NiOFPolygon::ms_spVCVertex;
NiVertexColorPropertyPtr NiOFPolygon::ms_spVCLit;
NiVertexColorPropertyPtr NiOFPolygon::ms_spVCLitTimesVertex;
NiWireframePropertyPtr NiOFPolygon::ms_spWire;
NiStencilPropertyPtr NiOFPolygon::ms_spStencilTwoSidedProperty;

// caches for textures and materials
NiTStringMap<NiTexturingPropertyPtr> NiOFPolygon::ms_kTextureCache;
NiTStringMap<NiMaterialPropertyPtr> NiOFPolygon::ms_kMaterialCache;

NiSearchPath NiOFPolygon::ms_kPath;
bool NiOFPolygon::ms_bInternalTextures = true;
bool NiOFPolygon::ms_bUseTrueDoubleSided = true;

// pre-main initialization, post-main termination
class NiOFPolygonInit
{
public:
    NiOFPolygonInit ()
    {
        // If not duplicating vertices, then need stencil property.
        NiOFPolygon::ms_spStencilTwoSidedProperty = NiNew NiStencilProperty;
        NiOFPolygon::ms_spStencilTwoSidedProperty->SetDrawMode(
            NiStencilProperty::DRAW_BOTH);
    
        // alpha blending
        NiOFPolygon::ms_spAlpha = NiNew NiAlphaProperty;
        NiOFPolygon::ms_spAlpha->SetSrcBlendMode(
            NiAlphaProperty::ALPHA_SRCALPHA);
        NiOFPolygon::ms_spAlpha->SetDestBlendMode(
            NiAlphaProperty::ALPHA_INVSRCALPHA);
        NiOFPolygon::ms_spAlpha->SetAlphaBlending(true);
        NiOFPolygon::ms_spAlpha->SetAlphaTesting(true);
        NiOFPolygon::ms_spAlpha->SetTestMode(
            NiAlphaProperty::TEST_GREATER);
        NiOFPolygon::ms_spAlpha->SetTestRef(127);
        
        NiOFPolygon::ms_spSpec = NiNew NiSpecularProperty;
        NiOFPolygon::ms_spSpec->SetSpecular(true);

        NiOFPolygon::ms_spVCVertex = NiNew NiVertexColorProperty;
        NiOFPolygon::ms_spVCVertex->SetLightingMode(
                NiVertexColorProperty::LIGHTING_E);
        NiOFPolygon::ms_spVCVertex->SetSourceMode(
                NiVertexColorProperty::SOURCE_EMISSIVE);

        NiOFPolygon::ms_spVCLit = NiNew NiVertexColorProperty;
        NiOFPolygon::ms_spVCLit->SetLightingMode(
                NiVertexColorProperty::LIGHTING_E_A_D);
        NiOFPolygon::ms_spVCLit->SetSourceMode(
                NiVertexColorProperty::SOURCE_IGNORE);

        NiOFPolygon::ms_spVCLitTimesVertex = NiNew NiVertexColorProperty;
        NiOFPolygon::ms_spVCLitTimesVertex->SetLightingMode(
                NiVertexColorProperty::LIGHTING_E_A_D);
        NiOFPolygon::ms_spVCLitTimesVertex->SetSourceMode(
                NiVertexColorProperty::SOURCE_AMB_DIFF);
        
        NiOFPolygon::ms_spWire = NiNew NiWireframeProperty;
        NiOFPolygon::ms_spWire->SetWireframe(true);
    };

    ~NiOFPolygonInit ()
    {
        NiOFPolygon::ms_spAlpha = 0;
        NiOFPolygon::ms_spSpec = 0;
        NiOFPolygon::ms_spVCVertex = 0;
        NiOFPolygon::ms_spVCLit = 0;
        NiOFPolygon::ms_spVCLitTimesVertex = 0;
        NiOFPolygon::ms_spWire = 0;
    }
};

static NiOFPolygonInit _init;

//---------------------------------------------------------------------------
static void Bilerp(unsigned int uiComponents, unsigned char* pucDest, 
    NiPixelData& kData, float fX, float fY)
{
    // compute the bounding texels of the current real-valued texel coord
    int iX0 = (int)floor(fX);
    int iY0 = (int)floor(fY);

    int iX1 = iX0 + 1;
    int iY1 = iY0 + 1;

    iX1 = (iX1 < (int)kData.GetWidth()) ? iX1 : iX0;
    iY1 = (iY1 < (int)kData.GetHeight()) ? iY1 : iY0;

    const unsigned char* pucP00 = kData(iX0, iY0);
    const unsigned char* pucP10 = kData(iX1, iY0);
    const unsigned char* pucP01 = kData(iX0, iY1);
    const unsigned char* pucP11 = kData(iX1, iY1);

    // compute the fractional component
    fX -= iX0;
    fY -= iY0;

    unsigned int i;
    for (i = 0; i < uiComponents; i++)
    {
        float fColor = (((float)(*pucP00)) * (1.0f - fX) 
            + ((float)(*pucP10)) * fX) * (1.0f - fY) + 
            (((float)(*pucP01)) * (1.0f - fX) 
            + ((float)(*pucP11)) * fX) * fY;

        // Safety code - this should not be required, but floating-point
        // may generate a wrap
        unsigned char ucColor;
        if (fColor >= 255.0f)
            ucColor = 255;
        else if (fColor <= 0.0f)
            ucColor = 0;
        else
            ucColor = (unsigned char)fColor;

        *(pucDest++) = ucColor;

        pucP00++;
        pucP10++;
        pucP01++;
        pucP11++;
    }
}
//---------------------------------------------------------------------------
static NiPixelData* ResizeTexture(NiPixelData& kData)
{
    const unsigned int uiSrcW = kData.GetWidth();
    const unsigned int uiSrcH = kData.GetHeight();

    // compute destination sizes
    unsigned int uiDestW = 1;
    while ((uiDestW << 1) <= uiSrcW)
        uiDestW <<= 1;

    unsigned int uiDestH = 1;
    while ((uiDestH << 1) <= uiSrcH)
        uiDestH <<= 1;

    if ((uiDestW == uiSrcW) && (uiDestH == uiSrcH))
        return &kData;

    // Can only convert RGB24 and RGBA32 data
    const NiPixelFormat& kFmt = kData.GetPixelFormat();
    if ((kFmt != NiPixelFormat::RGB24) && (kFmt != NiPixelFormat::RGBA32))
        return &kData;

    // create the new destination data
    NiPixelData* pkDest = NiNew NiPixelData(uiDestW, uiDestH, kFmt);

    // Bilerp the original image into the new image's pixels
    // This is a backward-mapping process, to ensure that every destination
    // pixel is lit
    const unsigned int uiComponents = (kFmt == NiPixelFormat::RGB24) ? 3 : 4;

    unsigned int i, j;
    for (j = 0; j < uiDestH; j++)
    {
        float fY = (j * (uiSrcH - 1)) / (float)(uiDestH - 1);
        for (i = 0; i < uiDestW; i++)
        {
            float fX = (i * (uiSrcW - 1)) / (float)(uiDestW - 1);
            Bilerp(uiComponents, (*pkDest)(i,j), kData, fX, fY);
        }
    }

    return pkDest;
}
//---------------------------------------------------------------------------
NiOFPolygon::NiOFPolygon ()
{
    /* */
}
//---------------------------------------------------------------------------
NiOFPolygon::~NiOFPolygon ()
{
    /* */
}
//---------------------------------------------------------------------------
void NiOFPolygon::GetDrawType (mgrec* pkPoly, bool& bDoubleSided,
    bool& bWireFrame)
{
    char cDrawType = 0;
    mgGetAttList(pkPoly, fltPolyDrawType, &cDrawType, mgNULL);

    bDoubleSided = (cDrawType == 1);

    bWireFrame = ( cDrawType == 2 || cDrawType == 3 ) ? true : false;
}
//---------------------------------------------------------------------------
void NiOFPolygon::GetAlphaAndBillboard (mgrec* pkPoly, bool &bBillboard,
    enum NiBillboardNode::FaceMode& eBillboardType, bool& bAlpha)
{
    char cAlphaType = 0;
    mgGetAttList(pkPoly, fltPolyMgTemplate, &cAlphaType, mgNULL);

    switch ( cAlphaType )
    {
        case 0:  // none
            bBillboard = false;
            break;
        case 1:  // alpha, static object
            bBillboard = false;
            bAlpha = true;
            break;
        case 2:  // alpha, billboard "rotate about axis"
            bBillboard = true;
            eBillboardType = NiBillboardNode::ROTATE_ABOUT_UP;
            bAlpha = true;
            break;
        case 3:  // alpha, billboard "rotate about point"
            bBillboard = true;
            eBillboardType = NiBillboardNode::RIGID_FACE_CAMERA;
            bAlpha = true;
            break;
    }
}
//---------------------------------------------------------------------------
void NiOFPolygon::GetFaceColor (mgrec* pkPoly, NiColorA& kColor, 
    NiVertexColorPropertyPtr& spVC, bool& bGouraud, bool& bLit)
{
    unsigned int uiColorIndex = 0;
    float fIntensity = 0.0f;
    unsigned char ucLightMode;

    short sR = 0, sG = 0, sB = 0;
    if (!mgGetAttList(pkPoly, fltPolyPrimeColor, &uiColorIndex, 
        fltPolyPrimeIntensity, &fIntensity, fltGcLightMode, &ucLightMode, 
        mgNULL))
    {
        // Force to 75% white if can't translate.
        sR = sG = sB = 0xbf;
    }
    else
    {
        if ((uiColorIndex > 0x1ffff) ||
            (!mgIndex2RGB(NiOpenFlight::GetActiveDB(), uiColorIndex,
            fIntensity, &sR, &sG, &sB)))
        {
            // Force to 75% white if can't translate.
            sR = sG = sB = 0xbf;
        }
    }
    kColor.r = (float)sR / 255.0f;
    kColor.g = (float)sG / 255.0f;
    kColor.b = (float)sB / 255.0f;
    kColor.a = 1.0f;

    switch (ucLightMode)    // Polygon lighting mode
    {
        case 0: // None ("Flat")
            bGouraud = false;
            bLit = false;
            spVC = ms_spVCVertex;
            break;
        case 1: // Gouraud
            bGouraud = true;
            bLit = false;
            spVC = ms_spVCVertex;
            break;
        case 2: // Dynamic ("Lit")
            bGouraud = false;
            bLit = true;
            spVC = ms_spVCLitTimesVertex;
            break;
        case 3: // Dynamic Gouraud ("Lit Gouraud")
            bGouraud = true;
            bLit = true;
            spVC = ms_spVCLitTimesVertex;
            break;
    };
}
//---------------------------------------------------------------------------
NiTexturingPropertyPtr NiOFPolygon::GetTexture (mgrec* pkPoly)
{
    NiTexturingPropertyPtr spTexture;

    const int TX_WRAP = 0;
    const int TX_CLAMP = 1;
    const int TX_NONE = 3;

    short sTxtIndex = -1;
    mgGetAttList(pkPoly, fltPolyTexture, &sTxtIndex, mgNULL);
    if (sTxtIndex == -1)
    {
        return NULL;
    }

    mgrec* pkTexrec = mgGetTextureAttributes(NiOpenFlight::GetActiveDB(),
            sTxtIndex);
    if (pkTexrec)
    {
        // It is possible for txtIndex != -1, yet texrec = NULL.  This
        //    situation happened for N36_45_0W121_30_0.flt, polygon p23228.
        //    The artist set txtIndex = 18 when it should have been 16.

        char* pcTxtName = mgGetTextureName(NiOpenFlight::GetActiveDB(),
            sTxtIndex);

        // Grab the search path object and prepare to search for the file
        ms_kPath.SetFilePath(pcTxtName);
        ms_kPath.Reset();

        // First, look for the file in the texture cache without even
        // touching the disk
        char acFile[NI_MAX_PATH];
        while (!spTexture && ms_kPath.GetNextSearchPath(acFile, NI_MAX_PATH))
        {
            NiPath::Standardize(acFile);
            ms_kTextureCache.GetAt(acFile, spTexture);
        }

        if ( spTexture == 0 )
        {
            // Image not found in texture palette under any of the possible
            // paths - search for the image on disk at all of the same paths
            ms_kPath.Reset();
            while (ms_kPath.GetNextSearchPath(acFile, NI_MAX_PATH))
            {
                if (NiFile::Access(acFile, NiFile::READ_ONLY))
                {
                    NiPath::Standardize(acFile);
                    break;
                }

                acFile[0] = 0;
            }

            if (ms_bInternalTextures)
            {
     
                NiPixelDataPtr spFileData 
                    = NiImageConverter::GetImageConverter()->ReadImageFile(
                    acFile, NULL);

                if (spFileData)
                {
                    // Resize texture (if needed) - this function will return
                    // the source if resizing is not needed or not possible
                    spFileData = ResizeTexture(*spFileData);

                    unsigned int uiWidth =  spFileData->GetWidth();
                    unsigned int uiHeight =  spFileData->GetHeight();

                    if ((uiWidth & (uiWidth-1)) || (uiHeight & (uiHeight-1)))
                    {
                        NiOutputDebugString("Texture not power of two!\n");
                    }
    
                    spTexture = NiNew NiTexturingProperty(spFileData);
                }
            }   
            else
            {
                spTexture = NiNew NiTexturingProperty(acFile);
            }

            if (spTexture)
            {
                spTexture->SetApplyMode(
                    NiTexturingProperty::APPLY_MODULATE);
                NiTexturingProperty::Map* pkBase = spTexture->GetBaseMap();
                pkBase->SetFilterMode(NiTexturingProperty::FILTER_TRILERP);

                // The following lines were added to attach clamp mode 
                //    attributes, so that the rendering pipeline will handle
                //    clamping later.

                // Note that the wrap method u,v specifies defaults for 
                //    wrapping textures when either wrap methods u or v is 
                //    set to None.
                // int (4) fltImgWrap Repetition type 
                //    0 = Repeat    1 = Clamp  
                // int (4) fltImgWrapU Repetition type in u direction 
                //    0 = Repeat    1 = Clamp   3 = None  
                // int (4) fltImgWrapV Repetition type in v direction 
                //    0 = Repeat    1 = Clamp   3 = None  

                int iImgWrap, iImgWrapU, iImgWrapV;
                mgGetAttList(pkTexrec, fltImgWrap, &iImgWrap,
                    fltImgWrapU, &iImgWrapU, fltImgWrapV, &iImgWrapV, 
                    mgNULL);
        
                // Initialize to default (wrap u,v).
                bool bClampU = ((iImgWrap == TX_WRAP) ? (false) : (true));
                bool bClampV = bClampU;

                if (iImgWrapU != TX_NONE)
                {
                    bClampU = ((iImgWrapU == TX_WRAP) ? (false) : (true));
                }

                if (iImgWrapV != TX_NONE)
                {
                    bClampV = ((iImgWrapV == TX_WRAP) ? (false) : (true));
                }

                if (bClampU)
                {
                    if (bClampV)
                    {
                        pkBase->SetClampMode(
                            NiTexturingProperty::CLAMP_S_CLAMP_T);
                    }
                    else
                    {
                        pkBase->SetClampMode(
                            NiTexturingProperty::CLAMP_S_WRAP_T);
                    }
                }
                else
                {
                    if (bClampV)
                    {
                        pkBase->SetClampMode(
                            NiTexturingProperty::WRAP_S_CLAMP_T);
                    }
                    else
                    {
                        pkBase->SetClampMode(
                            NiTexturingProperty::WRAP_S_WRAP_T);
                    }
                }

                ms_kTextureCache.SetAt(acFile, spTexture);
            }
        }
    }
    else
    {
        spTexture = 0;
    }

    return spTexture;
}
//---------------------------------------------------------------------------
NiMaterialProperty* NiOFPolygon::GetMaterial (mgrec* pkPoly, bool& bAlpha, 
    bool& bSpec)
{
    NiMaterialPropertyPtr spMaterial;

    short sMatIndex = -1;
    mgGetAttList(pkPoly, fltPolyMaterial, &sMatIndex, mgNULL);
    if ( sMatIndex == -1 )
        return NULL;

    char acStr[32];
    NiSprintf(acStr, 32, "Material:%d", (int)sMatIndex);
    ms_kMaterialCache.GetAt(acStr, spMaterial);

    if ( !spMaterial )
    {
        mgrec* pkMatrec = mgGetMaterial(NiOpenFlight::GetActiveDB(),
            sMatIndex);
        
        spMaterial = NiNew NiMaterialProperty;
        ms_kMaterialCache.SetAt(acStr, spMaterial);
        
        NiColor kAmb(0, 0, 0);
        mgGetNormColor(pkMatrec, fltAmbient, &kAmb.r, &kAmb.g, &kAmb.b);
        spMaterial->SetAmbientColor(kAmb);

        NiColor kDiff(0, 0, 0);
        mgGetNormColor(pkMatrec, fltDiffuse, &kDiff.r, &kDiff.g, &kDiff.b);
        spMaterial->SetDiffuseColor(kDiff);

        NiColor kSpec(0, 0, 0);
        mgGetNormColor(pkMatrec, fltSpecular, &kSpec.r, &kSpec.g, &kSpec.b);
        spMaterial->SetSpecularColor(kSpec);

        NiColor kEmit(0, 0, 0);
        mgGetNormColor(pkMatrec, fltEmissive, &kEmit.r, &kEmit.g, &kEmit.b);
        spMaterial->SetEmittance(kEmit);

        float fShine = 0.0f, fAlpha = 0.0f;
        mgGetAttList(pkMatrec, fltShininess, &fShine, fltMatAlpha, &fAlpha,
            mgNULL);
        spMaterial->SetShineness(fShine);

        spMaterial->SetAlpha(fAlpha);
    }

    // Avoid adding alpha to worthless materials - alpha values of 0.998 are
    // treated as translucent.
    float fAlpha = spMaterial->GetAlpha();
    if (fAlpha < (1.0f - (1.0f/256.0f)))
        bAlpha = true;

    NiColor kSpec = spMaterial->GetSpecularColor();
    if ((kSpec.r > (1.0f/256.0f)) || (kSpec.g > (1.0f/256.0f)) 
        || (kSpec.b > (1.0f/256.0f)))
    {
        bSpec = true;
    }

    return spMaterial;
}
//---------------------------------------------------------------------------
NiPoint3 NiOFPolygon::GetFaceNormal (mgrec* pkPoly)
{
    double x = 0.0, y = 0.0, z = 0.0;
    mgGetPolyNormal(pkPoly, &x, &y, &z);

    return NiPoint3((float)x, (float)y, (float)z);
}
//---------------------------------------------------------------------------
NiPoint3 NiOFPolygon::GetVertexCoord (mgrec* pkVtx)
{
    double x = 0.0, y = 0.0, z = 0.0;

    mgGetIcoord(pkVtx, fltCoord3d, &x, &y, &z);

    return NiPoint3((float)x, (float)y, (float)z);
}
//---------------------------------------------------------------------------
NiPoint3 NiOFPolygon::GetVertexNormal (mgrec* pkVtx, 
    const NiPoint3& kFaceNormal)
{
    NiPoint3 kNormal = NiPoint3::ZERO;
    if ( !mgGetVtxNormal(pkVtx, &kNormal.x, &kNormal.y, &kNormal.z) )
        kNormal = kFaceNormal;

    return kNormal;
}
//---------------------------------------------------------------------------
NiPoint2 NiOFPolygon::GetVertexTexture (mgrec* pkVtx)
{
    float fU = 0.0f, fV = 0.0f;
    if (mgGetAttList(pkVtx, fltVU, &fU, fltVV, &fV, mgNULL))
        return NiPoint2(fU, 1.0f - fV);
    else
        return NiPoint2::ZERO;
}
//---------------------------------------------------------------------------
NiColorA NiOFPolygon::GetVertexColor (mgrec* pkVtx, 
    const NiColorA& kFaceColor, const NiColorA& kDiffuseColor, bool bGouraud)
{
    if (bGouraud)
    {
        float fIntensity = 1.0f;
        int iColorIndex = -1;
        mgGetAttList(pkVtx, fltVColor, &iColorIndex, fltVIntensity, 
            &fIntensity, mgNULL);

        if ( iColorIndex != -1 )
        {
            short usR, usG, usB;
            if (!mgIndex2RGB(NiOpenFlight::GetActiveDB(), iColorIndex, 
                fIntensity, &usR, &usG, &usB))
            {
                // Force to 75% white if can't translate.
                usR = usG = usB = 0xbf;
            }

            return NiColorA((float)usR / 255.0f, (float)usG / 255.0f,
                (float)usB / 255.0f, 1.0f) * kDiffuseColor;
        }
    }

    return kFaceColor * kDiffuseColor;
}
//-----------------------------------------------------------------------------
NiAVObject* NiOFPolygon::CreateShape (mgrec* pkPoly, bool bFreezeBillboards)
{
    bool bDoubleSided, bWireFrame;
    GetDrawType(pkPoly, bDoubleSided, bWireFrame);

    NiColorA kFaceColor;
    NiVertexColorPropertyPtr spVC;
    bool bGouraud, bLit;
    GetFaceColor(pkPoly, kFaceColor, spVC, bGouraud, bLit);
    
    NiTexturingPropertyPtr spTexture = GetTexture(pkPoly);

    bool bAlpha = false;
    bool bSpec = false;
    NiMaterialPropertyPtr spMaterial = GetMaterial(pkPoly, bAlpha, bSpec);

    NiPoint3 kFaceNormal = GetFaceNormal(pkPoly);

    bool bBillboard;
    NiBillboardNode::FaceMode eBillboardType;
    GetAlphaAndBillboard(pkPoly, bBillboard, eBillboardType, bAlpha);

    unsigned short usVertexCount = 0;
    mgrec* pkVtx;
    for (pkVtx = mgGetChild(pkPoly); pkVtx; pkVtx = mgGetNext(pkVtx))
        usVertexCount++;

    if (usVertexCount < 3)  // For polygons that have zero vertices.
        return 0;

    unsigned short usArraySize;
    if (ms_bUseTrueDoubleSided) // If duplicating vertices for double-sided...
    {
        usArraySize = bDoubleSided ? 2 * usVertexCount : usVertexCount;
    }
    else
    {
        usArraySize = usVertexCount;
    }

    NiPoint3* pkCoords = NiNew NiPoint3[usArraySize];
    NiPoint3* pkNorms = NiNew NiPoint3[usArraySize];
    NiPoint2* pkUVs = NiNew NiPoint2[usArraySize];
    NiColorA* pkColors = NiNew NiColorA[usArraySize];

    NiColorA kDiffuse;

    if (bLit && spMaterial)
    {
        kDiffuse = NiColorA(spMaterial->GetDiffuseColor().r,
            spMaterial->GetDiffuseColor().g, spMaterial->GetDiffuseColor().b,
            spMaterial->GetAlpha());
    }
    else
    {
        kDiffuse = NiColorA(1,1,1,1);
    }

    int i = 0;
    NiPoint3 kAvgCoord = NiPoint3(0.0f, 0.0f, 0.0f);
    for (pkVtx = mgGetChild(pkPoly); pkVtx; pkVtx = mgGetNext(pkVtx))
    {
        NIASSERT(i < usVertexCount);

        pkCoords[i] = GetVertexCoord(pkVtx);
        pkNorms[i] = GetVertexNormal(pkVtx, kFaceNormal);
        pkUVs[i] = GetVertexTexture(pkVtx);
        pkColors[i] = GetVertexColor(pkVtx, kFaceColor, kDiffuse,
            bGouraud);

        kAvgCoord = kAvgCoord + pkCoords[i];

        i++;
    }

    if (bBillboard && !bFreezeBillboards)
    {
        kAvgCoord = kAvgCoord / (float)(i);

        // Offset by average coordinate (which will become local translation).
        for (int k=0; k < i; k++)
            pkCoords[k] = pkCoords[k] - kAvgCoord;
    }


    // Duplicate vertices, if needed.
    if (ms_bUseTrueDoubleSided && bDoubleSided)
    {
        for (int i = 0; i < usVertexCount; i++)
        {
            const int iDest = i + usVertexCount;

            pkCoords[iDest] = pkCoords[i];
            pkUVs[iDest] = pkUVs[i];
            pkColors[iDest] = pkColors[i];

            // Negate normal for back face.
            pkNorms[iDest] = -pkNorms[i];
        }

        usVertexCount *= 2;
    }
    
    // Build triangle fan(s) from the polygon.
    unsigned short usTriangles;
    unsigned short* pusTriList;
    unsigned int j;

    // Create triangles, duplicating if needed.
    if (ms_bUseTrueDoubleSided && bDoubleSided)
    {
        usTriangles = (usVertexCount / 2) - 2;
        pusTriList = NiAlloc(unsigned short, 3 * 2 * usTriangles);
        unsigned short* pusTris = pusTriList;

        for (j = 0; j < usTriangles; j++)
        {
            *(pusTris++) = 0;
            *(pusTris++) = j + 1;
            *(pusTris++) = j + 2;
        }

        const unsigned short usBase = usVertexCount / 2;
        for (j = 0; j < usTriangles; j++)
        {
            *(pusTris++) = usBase;
            *(pusTris++) = usBase + j + 2;
            *(pusTris++) = usBase + j + 1;
        }

        usTriangles *= 2;
    }
    else
    {
        usTriangles = usVertexCount - 2;
        pusTriList = NiAlloc(unsigned short, 3 * usTriangles);
        unsigned short* pusTris = pusTriList;

        for (j = 0; j < usTriangles; j++)
        {
            *(pusTris++) = 0;
            *(pusTris++) = j + 1;
            *(pusTris++) = j + 2;
        }
    }

    NiTriShape* pkShape = NiNew NiTriShape(usVertexCount, pkCoords, pkNorms, 
        pkColors, pkUVs, 1, NiGeometryData::NBT_METHOD_NONE, usTriangles, 
        pusTriList);

    // If not duplicating vertices, attach stencil property for double-sided.
    if (!ms_bUseTrueDoubleSided && bDoubleSided)
        pkShape->AttachProperty(ms_spStencilTwoSidedProperty);
    if ( bAlpha )
        pkShape->AttachProperty(ms_spAlpha);
    if ( spMaterial )
        pkShape->AttachProperty(spMaterial);

    if ( spTexture )
        pkShape->AttachProperty(spTexture);
    if ( spVC )
        pkShape->AttachProperty(spVC);
    if ( bWireFrame )
        pkShape->AttachProperty(ms_spWire);

    if (bSpec)
        pkShape->AttachProperty(ms_spSpec);

    if (bBillboard && !bFreezeBillboards)
    {
        NiBillboardNode* pkBillboard = NiNew NiBillboardNode;
        pkBillboard->SetMode(eBillboardType);
        pkBillboard->AttachChild(pkShape);

        pkShape->SetRotate(NI_HALF_PI, 1.0f, 0.0f, 0.0f);
        pkBillboard->SetRotate(-NI_HALF_PI, 1.0f, 0.0f, 0.0f);
        pkBillboard->SetTranslate(kAvgCoord);
        
        return pkBillboard;
    }

    //pkShape->SetTranslate(kAvgCoord);

    return pkShape;
}
//---------------------------------------------------------------------------
