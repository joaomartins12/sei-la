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


#include <NiFilename.h>
#include <NiOptimize.h>

#include "NiOFConst.h"  // for degree-to-radian conversion
#include "NiOFEyepoint.h"
#include "NiOFLight.h"
#include "NiOFPolygon.h"
#include "NiOFTransform.h"
#include "NiOpenFlight.h"

#include <NiBSPNode.h>
#include <NiLODNode.h>
#include <NiRangeLODData.h>
#include <NiStringExtraData.h>
#include <NiTriShape.h>

#include <fltcode.h>
#include <mgapiattr.h>
#include <mgapiio.h>
#include <mgapipref.h>
#include <mgapistruc.h>

#ifdef _DEBUG
#include <mgapiinfo.h>
#endif

mgrec* NiOpenFlight::ms_pkActiveDB = 0;

bool NiOpenFlight::ms_bForceMergeSiblingSwitchNodes = false;
bool NiOpenFlight::ms_bForceMergeSiblingLODNodes = true;

#ifdef _DEBUG
static char* g_pcName = 0;
#endif

class NiOFTransformExtraData : public NiExtraData
{
    NiDeclareRTTI;
public:
    NiOFTransformExtraData (float* pfGenMat) { m_pfGenMat = pfGenMat; }
    void SetTransform (float* pfGenMat) { m_pfGenMat = pfGenMat; }
    float* GetTransform () { return m_pfGenMat; }
protected:
    float* m_pfGenMat;
};

NiImplementRTTI(NiOFTransformExtraData,NiExtraData);

//-----------------------------------------------------------------------------
void NiOpenFlight::Init ()
{
    int iCount = 0;
    mgInit (&iCount, NULL);
}
//-----------------------------------------------------------------------------
void NiOpenFlight::Shutdown ()
{
    mgExit();   // Always call mgExit() after all API calls.
}
//-----------------------------------------------------------------------------
NiOpenFlight::NiOpenFlight ()
{
    // Always call mgInit before any other API calls.
    mgSetReadExtFlag(MG_TRUE);  

    m_bFreezeBillboards = false;

    m_uiXrefDepth = 0;
}

//-----------------------------------------------------------------------------
NiOpenFlight::~NiOpenFlight ()
{
    NiOFPolygon::ClearTextureCache();
    NiOFPolygon::ClearMaterialCache();
    m_kInstanceCache.RemoveAll();
}

//-----------------------------------------------------------------------------
void NiOpenFlight::AttachTransformMatrix (mgrec* pkMgNode, 
    NiAVObject* pkObject)
{
    const float fDelta = 1e-5f;

    if ( mgHasXform(pkMgNode) != mgTRUE )
        return;

    mgmatrix m;
    mgGetMatrix(pkMgNode, fltMatrix, &m);

    NiMatrix3 kCompMat;
    kCompMat.SetEntry(0, 0, (float)m[0]);
    kCompMat.SetEntry(1, 0, (float)m[1]);
    kCompMat.SetEntry(2, 0, (float)m[2]);
    kCompMat.SetEntry(0, 1, (float)m[4]);
    kCompMat.SetEntry(1, 1, (float)m[5]);
    kCompMat.SetEntry(2, 1, (float)m[6]);
    kCompMat.SetEntry(0, 2, (float)m[8]);
    kCompMat.SetEntry(1, 2, (float)m[9]);
    kCompMat.SetEntry(2, 2, (float)m[10]);

    NiPoint3 kR0;
    kCompMat.GetRow(0, kR0);

    float fScale = kR0.Length();

    NiMatrix3 kScaledMat = kCompMat * (1.0f / fScale);

    NiPoint3 kScaledR0, kScaledR1, kScaledR2;
    kScaledMat.GetRow(0, kScaledR0);
    kScaledMat.GetRow(1, kScaledR1);
    kScaledMat.GetRow(2, kScaledR2);

    {
        // scale 0 will be 1.0, as we just scaled the matrix by its inverse
        float fTestScale = kScaledR1.Length();
        if ((fTestScale > (1.0f + fDelta)) 
            || (fTestScale < (1.0f - fDelta)))
        {
            // Non-uniform scale
            goto general_matrix;
        }

        fTestScale = kScaledR2.Length();
        if ((fTestScale > (1.0f + fDelta)) 
            || (fTestScale < (1.0f - fDelta)))
        {
            // Non-uniform scale
            goto general_matrix;
        }
    }

    // Test for Shear
    {
        float fTestShear = kScaledR0 * kScaledR1;
        if ((fTestShear > fDelta) || (fTestShear < -fDelta))
            goto general_matrix;

        fTestShear = kScaledR1 * kScaledR2;
        if ((fTestShear > fDelta) || (fTestShear < -fDelta))
            goto general_matrix;

        fTestShear = kScaledR2 * kScaledR0;
        if ((fTestShear > fDelta) || (fTestShear < -fDelta))
            goto general_matrix;
    }

    // 3x3 component is ortho-normal - safe to make it an sR+T matrix

    // Must multiply by the existing scale, to avoid overwriting any
    // existing unit conversions
    pkObject->SetScale(pkObject->GetScale() * fScale);

    pkObject->SetRotate(kScaledMat);

    // translation
    pkObject->SetTranslate((float)m[12], (float)m[13], (float)m[14]);

    return;

general_matrix:
    NiOutputDebugString("General Matrix encountered - must push to leaves\n");
    float* pfGenMat = NiAlloc(float, 12);
    pfGenMat[0] = (float)m[0];
    pfGenMat[1] = (float)m[4];
    pfGenMat[2] = (float)m[8];
    pfGenMat[3] = (float)m[1];
    pfGenMat[4] = (float)m[5];
    pfGenMat[5] = (float)m[9];
    pfGenMat[6] = (float)m[2];
    pfGenMat[7] = (float)m[6];
    pfGenMat[8] = (float)m[10];

    pfGenMat[9] = (float)m[12];
    pfGenMat[10] = (float)m[13];
    pfGenMat[11] = (float)m[14];

    NiOFTransformExtraData* pkExtra = NiNew NiOFTransformExtraData(pfGenMat);
    NIASSERT(pkExtra);
    pkObject->AddExtraData("FLTXFORM", pkExtra);
}
//-----------------------------------------------------------------------------
void NiOpenFlight::AttachDOFTransformMatrix (mgrec* pkMgNode, NiNode* pkNode)
{
    if ( mgHasXform(pkMgNode) == mgTRUE )
    {
        // composite local transforms
        NiMatrix3 kCompMat = NiMatrix3::IDENTITY;
        NiPoint3 kCompTrn = NiPoint3::ZERO;
        NiMatrix3 kPutMatA = NiMatrix3::IDENTITY;
        NiPoint3 kPutTrnA = NiPoint3::ZERO;
        NiMatrix3 kPutMatB = NiMatrix3::IDENTITY;
        NiPoint3 kPutTrnB = NiPoint3::ZERO;
        bool bDeferApply = false;
        
        mgrec* pkXform;
        int iMatNum = 0;
        for (pkXform = mgGetXform(pkMgNode); pkXform; pkXform = 
            mgGetNext(pkXform))
        {
            iMatNum++;   // DOF nodes always have 11 transformations in the
                        //    order:  Put, Translate, Translate, Translate,
                        //    Rotate, Rotate, Rotate, Scale, Scale, Scale,
                        //    Inverse of original Put
            // get local transforms
            NiOFTransform kNiform(pkXform);
            if ( kNiform.DeferApply() )
            {
                bDeferApply = true;
                break;
            }
            NiPoint3& kTrn = kNiform.GetTranslate();
            NiMatrix3& kMat = kNiform.GetMatrix();

            // Capture "put" matrix and its inverse separately; these are
            //    needed by the runtime to manipulate articulated characters.
            if (iMatNum == 1)
            {
                kPutTrnA += kPutMatA * kTrn;
                kPutMatA = kMat * kPutMatA;
            }
            else if (iMatNum == 11)
            {
                kPutTrnB += kPutMatB * kTrn;
                kPutMatB = kMat * kPutMatB;
            }
            else    // Remaining transformations for DOF are 3 translates, 3
                    //    rotates, and 3 scales.
            {
                // Update composite transform (do not change order of
                //    statements).
                kCompTrn += kCompMat * kTrn;
                kCompMat = kMat * kCompMat;
            }
        }

        if ( bDeferApply )
        {
            mgmatrix gmat;
            mgGetMatrix(pkMgNode, fltMatrix, &gmat);

            float* pfGenMat = NiAlloc(float, 12);
            pfGenMat[0] = (float)gmat[0];
            pfGenMat[1] = (float)gmat[4];
            pfGenMat[2] = (float)gmat[8];
            pfGenMat[3] = (float)gmat[1];
            pfGenMat[4] = (float)gmat[5];
            pfGenMat[5] = (float)gmat[9];
            pfGenMat[6] = (float)gmat[2];
            pfGenMat[7] = (float)gmat[6];
            pfGenMat[8] = (float)gmat[10];
            pfGenMat[9] = (float)gmat[12];
            pfGenMat[10] = (float)gmat[13];
            pfGenMat[11] = (float)gmat[14];

            NiOFTransformExtraData* pkExtra =
                NiNew NiOFTransformExtraData(pfGenMat);
            NIASSERT( pkExtra );
            pkNode->AddExtraData("FLTXFORM", pkExtra);
        }
        else
        {
            NiStringExtraData* pkStringExtraData
                    = NiNew NiStringExtraData("sgoKeep");

            NiNode * pkPutNodeA = pkNode;
            pkPutNodeA->SetTranslate(kPutTrnA);
            pkPutNodeA->SetRotate(kPutMatA);
            pkPutNodeA->AddExtraData(pkStringExtraData);

            NiNode * pkCompNode = NiNew NiNode;
            pkPutNodeA->AttachChild(pkCompNode);
            pkCompNode->SetTranslate(kCompTrn);
            pkCompNode->SetRotate(kCompMat);
            pkCompNode->AddExtraData(pkStringExtraData);

            NiNode * pkPutNodeB = NiNew NiNode;
            pkCompNode->AttachChild(pkPutNodeB);
            pkPutNodeB->SetTranslate(kPutTrnB);
            pkPutNodeB->SetRotate(kPutMatB);
            pkPutNodeB->AddExtraData(pkStringExtraData);
        }
    }
}

//-----------------------------------------------------------------------------
NiAVObject* NiOpenFlight::ProcessPolygons (mgrec* pkPoly, NiNode*& pkNode)
{
    // Get current polygon and its vertices.
    NiOFPolygon kPolygon;

    // Create corresponding Gamebryo shape (may be 0 for degenerate poly).
    NiAVObject* pkShape = kPolygon.CreateShape(pkPoly, m_bFreezeBillboards);

    if (!pkShape)
        return 0;

    mgrec* pkRec = mgGetNestedChild(pkPoly);
    if (!pkRec)
    {
        if (pkNode) // Polygon does not have nested children.
        {
            // Polygon itself is nested; attach to appropriate parent
            if (pkShape)
                pkNode->AttachChild(pkShape);

            return 0;
        }
        else
        {
            return pkShape;
        }
    }

    // Process nested polygons.
    NiNode* pkNestedNode = NiNew NiNode;

    if (pkShape)
        pkNestedNode->AttachChild(pkShape);

    for ( /**/ ; pkRec; pkRec = mgGetNext(pkRec))
        ProcessPolygons(pkRec, pkNestedNode);

    if (pkNode)
        pkNode->AttachChild(pkNestedNode);
    else
        pkNode = pkNestedNode;

    return 0;
}

//-----------------------------------------------------------------------------
NiNode* NiOpenFlight::HeaderHandler (mgrec* pkMgNode, float& fUnitScale)
{
    NiNode* pkRoot = NiNew NiNode;

    unsigned char ucUnits;
    mgGetAttList ( pkMgNode, fltHdrUnits, &ucUnits, MG_NULL );

    float fNewScale = 1.0f;
    switch (ucUnits)
    {
    case 0: // Meters
        fNewScale = 1.0f;
        break;
    case 1: // Kilometers 
        fNewScale = 1000.0f;
        break;
    case 4: // Feet 
        fNewScale = 0.3048f;
        break;
    case 5: // Inches 
        fNewScale = 0.3048f/12.0f;
        break;
    case 8: // Nautical miles 
        fNewScale = 1.152f * 1609.0f;
        break;
    };

    fUnitScale = fNewScale / fUnitScale;
    pkRoot->SetScale(fUnitScale);

    return pkRoot;
}

//-----------------------------------------------------------------------------
NiNode* NiOpenFlight::GroupHandler (mgrec* pkRec, bool& bFlipbook)
{
    NiNode * pkNode = NiNew NiNode;

    mgbool eAnimated = mgFALSE;
    mgbool eAnimatedBounce = mgFALSE;
    mgGetAttList(
        pkRec,
        fltGrpFlagAnimation, &eAnimated,
        fltGrpFlagAnimationFB, &eAnimatedBounce,
        mgNULL);

    NiStringExtraData* pkStringExtraData = NULL;
    if (eAnimated == mgTRUE)
    {
        pkStringExtraData = NiNew NiStringExtraData(
            "type: OpenFlight Flipbook");
        pkNode->AddExtraData(pkStringExtraData);
        bFlipbook = true;
    }
    else
    {
        bFlipbook = false;
    }

    AttachTransformMatrix(pkRec, pkNode);

    return pkNode;
}

//-----------------------------------------------------------------------------
NiNode* NiOpenFlight::SwitchHandler (mgrec* pkRec)
{
    NiNode * pkNode = NiNew NiNode;

    AttachTransformMatrix(pkRec, pkNode);

    return pkNode;
}

//-----------------------------------------------------------------------------
NiNode* NiOpenFlight::LodHandler (mgrec* pkRec)
{
    NiNode* pkLodSwitchNode = NiNew NiNode;
    NiLODNode* pkLOD = NiNew NiLODNode;
    
    double dnear, dfar;
    mgbool freezeCenter, useLast;
    mgGetAttList(pkRec, fltLodSwitchIn, &dfar, fltLodSwitchOut, &dnear,
        fltLodFlagFreezeCenter, &freezeCenter, fltLodFlagAdditive, &useLast,
        mgNULL);
    
    static NiPoint3 kLastCenter;
    if (!useLast)   // Get LOD center if it's to be used.
    {
        double x, y, z;
        mgGetIcoord(pkRec,fltLodCenterPoint,&x,&y,&z);
        kLastCenter.x = float(x);
        kLastCenter.y = float(y);
        kLastCenter.z = float(z);
    }    

    NiRangeLODData* pkLODRange = NiNew NiRangeLODData;
    pkLODRange->SetNumRanges(1);
    pkLODRange->SetRange(0, (float)dnear, (float)dfar);
    pkLODRange->SetCenter(kLastCenter);
    
    pkLOD->SetLODData(pkLODRange);

    pkLOD->UpdateOnlyActiveChild(false);

    pkLOD->AttachChild(pkLodSwitchNode);
    AttachTransformMatrix(pkRec,pkLOD);

    return pkLOD;
}

//-----------------------------------------------------------------------------
NiNode* NiOpenFlight::BspHandler (mgrec* pkRec)
{
    double dCoefa, dCoefb, dCoefc, dCoefd;
    mgGetAttList(
        pkRec,
        fltDPlaneA, &dCoefa,
        fltDPlaneB, &dCoefb,
        fltDPlaneC, &dCoefc,
        fltDPlaneD, &dCoefd,
        mgNULL
        );

    NiPoint3 kNormal((float)dCoefa, (float)dCoefb, (float)dCoefc);
    NiBSPNode* pkBsp = NiNew NiBSPNode;
    NiPlane kPlane(kNormal,(float)dCoefd);
    pkBsp->SetModelPlane(kPlane);
    pkBsp->AttachLeft(0);
    pkBsp->AttachRight(0);
    AttachTransformMatrix(pkRec,pkBsp);

    return pkBsp;
}

//-----------------------------------------------------------------------------
NiNode* NiOpenFlight::DofHandler (mgrec* pkRec)
{
    NiNode* pNode = NiNew NiNode;

    AttachDOFTransformMatrix(pkRec, pNode);

    return pNode;
}

//-----------------------------------------------------------------------------
NiNode* NiOpenFlight::LightSourceHandler (mgrec* pkRec)
{
    // Default behavior is to trivially reject processing of lights for
    // external references, partly due to the 8 light limit of most graphics
    // cards.
    if (m_uiXrefDepth > 0)
        return NULL;

     NiNode* pkNode = NiOFLight::CreateLight(pkRec);

     if (pkNode)
         AttachTransformMatrix(pkRec, pkNode);

     return pkNode;
}

//-----------------------------------------------------------------------------
NiNode* NiOpenFlight::SoundHandler (mgrec* pkRec)
{
    NiNode* pkNode = NiNew NiNode();
    AttachTransformMatrix(pkRec, pkNode);

    return pkNode;
}

//-----------------------------------------------------------------------------
NiNode* NiOpenFlight::ObjectHandler (mgrec* pkRec)
{
    NiNode* pkNode = NiNew NiNode;
    AttachTransformMatrix(pkRec, pkNode);
    return pkNode;
}

//-----------------------------------------------------------------------------
NiAVObject* NiOpenFlight::PolygonHandler (mgrec* pkRec, NiNode* pkNode)
{
    NiAVObject* pkShape = ProcessPolygons(pkRec, pkNode);

    if (pkShape)    // Singleton polygon found.
    {
        return pkShape;
    }
    else    // Nested polygons found, return as single node with polygon
    {       // children ordered by depth-first search.
        return pkNode;
    }
}

//-----------------------------------------------------------------------------
NiNode* NiOpenFlight::XrefHandler (mgrec* pkRec, float fUnitScale)
{
    NiNode* pkNode;

#ifdef _DEBUG
    char acMsg[NI_MAX_PATH];
    char* pcName = mgGetName(pkRec);
#endif

    if ( mgIsInstance(pkRec) )
    {

#ifdef _DEBUG
        NiSprintf(acMsg, NI_MAX_PATH, "pushing %s\n", pcName);
        OutputDebugString(acMsg);
#endif

        m_uiXrefDepth++;    // Descending to external reference.

        NiNode* pkHeaderNode = NULL;
        mgrec* pkOldDB = ms_pkActiveDB;
        ms_pkActiveDB = mgGetReference(pkRec);
        pkNode = (NiNode*) Traverse(ms_pkActiveDB, fUnitScale, pkHeaderNode);
        AttachTransformMatrix(pkRec, pkNode);
        ms_pkActiveDB = pkOldDB;

        m_uiXrefDepth--;    // Ascending from external reference.

#ifdef _DEBUG
        NiSprintf(acMsg, NI_MAX_PATH, "popping %s\n", pcName);
        OutputDebugString(acMsg);
#endif

    }
    else
    {

#ifdef _DEBUG
        NiSprintf(acMsg, NI_MAX_PATH, 
            "Unable to get external instance for %s\n", pcName);
        OutputDebugString(acMsg);
#endif

        pkNode = NiNew NiNode;
    }

#ifdef _DEBUG
    mgFree(pcName);
#endif

    return pkNode;
}

//-----------------------------------------------------------------------------
void NiOpenFlight::DefaultHandler (int iCode)
{

#ifdef _DEBUG
    char acMsg[NI_MAX_PATH];
    NiSprintf(acMsg, NI_MAX_PATH, "%s not supported (code %d)\n", 
        ddGetLabel(iCode), iCode);
    OutputDebugString(acMsg);
#endif

}

//-----------------------------------------------------------------------------
NiAVObject* NiOpenFlight::Traverse (mgrec* pkMgNode, float fUnitScale,
    NiNode*& pkHeaderNode)
{
    NiNode* pkNode = 0;
    int iBspNum = 0;
    mgbool onFlag;
    int iBitNum = 0;
    int iMaskNum = 0;
    NiStringExtraData* pkStringExtraData = NULL;
    bool bFlipbook = false;


    int iCode = mgGetCode(pkMgNode);
    char* pcMgName = mgGetName(pkMgNode);

#ifdef _DEBUG
    g_pcName = pcMgName;
#endif

    switch ( iCode )
    {
        case eFltHeader:    // code 310
        {
            pkNode = HeaderHandler(pkMgNode, fUnitScale);
            pkHeaderNode = pkNode;
            break;
        }
        case eFltGroup: // code 309
        {
            bFlipbook = false;
            pkNode = GroupHandler(pkMgNode, bFlipbook);
            if ((*pcMgName != 'g') ||    // keep non-default-named object nodes
               ((*(pcMgName+1) < '1') || (*(pcMgName+1) > '9')) ||
               (bFlipbook))
            {
                pkStringExtraData = NiNew NiStringExtraData("sgoKeep");
                pkNode->AddExtraData(pkStringExtraData);
            }
            break;
        }
        case eFltSwitch:
        {
            pkNode = SwitchHandler(pkMgNode);
            pkStringExtraData = NiNew NiStringExtraData("sgoKeep"); // keep all
            pkNode->AddExtraData(pkStringExtraData);          // switch nodes
            break;
        }
        case eFltLod:
        {
            pkNode = LodHandler(pkMgNode);
            break;
        }
        case eFltBsp:
        {
            pkNode = BspHandler(pkMgNode);
            break;
        }
        case eFltDof:
        {
            pkNode = DofHandler(pkMgNode);    // Always keep DOF nodes.
            pkStringExtraData = NiNew NiStringExtraData(
                "type: OpenFlight DOF");
            pkNode->AddExtraData(pkStringExtraData);
            break;
        }
        case eFltLightSource:
        {
            pkNode = LightSourceHandler(pkMgNode);
            if (!pkNode)    // Sanity check on whether light source is found.
            {
                if (pcMgName)
                {
                    mgFree(pcMgName);
                }
                return NULL;
            }
            break;
        }
        case eFltSound:
        {
            pkNode = SoundHandler(pkMgNode);
            break;
        }
        case eFltObject:    // code 341
        {
            pkNode = ObjectHandler(pkMgNode);
            if ((*pcMgName != 'o') ||   // keep non-default-named object nodes
               ((*(pcMgName+1) < '1') || (*(pcMgName+1) > '9')))
            {
                pkStringExtraData = NiNew NiStringExtraData("sgoKeep");
                pkNode->AddExtraData(pkStringExtraData);
            }
            break;
        }
        case eFltPolygon:   // code 346
        {
            NiAVObject* pkPolyShape = PolygonHandler(pkMgNode, pkNode);
            if (pkPolyShape)
                pkPolyShape->SetName(pcMgName);
            mgFree(pcMgName);
            return pkPolyShape;
        }
        case eFltVertex:
        {
            NIASSERT(0);  // Should not get here.
            break;
        }
        case eFltXref:
        {
            pkNode = XrefHandler(pkMgNode, fUnitScale);
            break;
        }
        default:
        {
            DefaultHandler(iCode);
            if (pcMgName)
            {
                mgFree(pcMgName);
            }
            return NULL;
            break;
        }
    }

    unsigned int uiChildCount = 0;
    for (mgrec* pkRec = mgGetChild(pkMgNode); pkRec; pkRec = mgGetNext(pkRec))
    {
        NiAVObjectPtr spChild = Traverse(pkRec, fUnitScale, pkHeaderNode);
        if ( spChild )
        {
            if (iCode == eFltBsp)
            {
                switch(iBspNum)
                {
                case 0:
                    ((NiBSPNode*)pkNode)->AttachLeft(spChild);
                    iBspNum++;
                    break;
                case 1:
                    iBspNum++;
                    spChild = 0;
                    break;
                case 2:
                    ((NiBSPNode*)pkNode)->AttachRight(spChild);
                    iBspNum++;
                    break;
                default:
                    // GameGen allows you to attach an arbitrary number of
                    //    polys to a BSP record; bad news for us.
                    spChild = 0;
                    break;
                }
            }
            else if (iCode == eFltDof)
            {
                NiNode* pkCompNode = (NiNode*) pkNode->GetAt(0);
                NIASSERT( pkCompNode );
                NiNode* pkPutNodeB = (NiNode*) pkCompNode->GetAt(0);
                NIASSERT( pkPutNodeB );
                pkPutNodeB->AttachChild(spChild);
            }
            else if (iCode == eFltLod)
            {
                NiNode* pkLodSwitchNode = (NiNode*) pkNode->GetAt(0);
                NIASSERT( pkLodSwitchNode );
                pkLodSwitchNode->AttachChild(spChild);
            }
            else if (iCode == eFltSwitch)
            {
                NiSwitchNode * pkSwitchNode = NiNew NiSwitchNode;

                pkNode->AttachChild(pkSwitchNode);
                pkSwitchNode->AttachChild(spChild);

                mgGetSwitchBit(pkMgNode, iMaskNum, iBitNum, &onFlag);

                if (spChild)
                {
                    if (onFlag)
                        pkSwitchNode->SetIndex(0);
                    else
                        pkSwitchNode->SetIndex(-1);  // Turn "off".
                }

                iBitNum++;
                if (iBitNum == 32)
                {
                    iBitNum = 0;
                    iMaskNum++;
                }
            }
            else if ((iCode == eFltGroup) && (bFlipbook))
            {
                // Convert "flipbook animation" group to NiSwitchNode.
                NiSwitchNode * pkSwitchNode = NiNew NiSwitchNode;

                pkNode->AttachChild(pkSwitchNode);
                pkSwitchNode->AttachChild(spChild);

                if (spChild)
                {
                    if (uiChildCount == 0)  // Force 1st child on.
                        pkSwitchNode->SetIndex(0);
                    else            // Turn others off.
                        pkSwitchNode->SetIndex(-1);
                }

                iBitNum++;
                if (iBitNum == 32)
                {
                    iBitNum = 0;
                    iMaskNum++;
                }
            }
            else
            {
                pkNode->AttachChild(spChild);
            }
        }
        else
        {
            if ( iCode == eFltBsp )
            {
                // Just in case modelers forget to have a BSP with 2 children
                //    or if they leave off the polygon visualization of the
                //    BSP.
                switch(iBspNum)
                {

                case 0:
                    ((NiBSPNode*)pkNode)->AttachLeft(0);
                    iBspNum++;
                    break;

                case 1:
                    iBspNum++;
                    break;

                case 2:
                    ((NiBSPNode*)pkNode)->AttachRight(0);
                    iBspNum++;
                    break;

                }
            }
        }

        uiChildCount++;
    }

    // Handle Multigen instances.
    if ( iCode != eFltXref && mgIsInstance(pkMgNode) )
    {
        mgrec* pkReference = mgGetReference(pkMgNode);

        NiAVObject* pkChild = 0;
        if ( m_kInstanceCache.GetAt(pkReference, pkChild) )
        {
            // Instanced object was already created; just attach to it.
            if ( pkChild )
            {
                if ( NiIsKindOf(NiNode, pkChild) )
                {
                    NiObject* pkClone = pkChild->Clone();
                    pkNode->AttachChild((NiAVObject*) pkClone);
                }
            }
        }
        else
        {
            // First occurrence of the instance; create it.

            // Ignore instance headers
            NiNode* pkNewHeader;
            pkChild = Traverse(pkReference, fUnitScale, pkNewHeader);
            if ( pkChild )
            {
                pkNode->AttachChild(pkChild);
            }

            // Cache instanced subtree for later lookup.
            m_kInstanceCache.SetAt(pkReference, pkChild);
        }
    }

    pkNode->SetName(pcMgName);
    mgFree(pcMgName);

    return pkNode;
}

//-----------------------------------------------------------------------------
void NiOpenFlight::ApplyDeferredTransforms (NiAVObject* pkObject)
{
    if ( NiIsKindOf(NiNode, pkObject) )
    {
        NiNode* pkNode = (NiNode*) pkObject;
        for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if ( pkChild )
                ApplyDeferredTransforms(pkChild);
        }
    }

    NiExtraData* pkExtra = pkObject->GetExtraData("FLTXFORM");

    NiOFTransformExtraData* pkTransformExtra = NiDynamicCast(
        NiOFTransformExtraData, pkExtra);
        
    if ( pkTransformExtra )
    {
        float* pfGenMat = pkTransformExtra->GetTransform();

        // Must transpose for converting to new constructor
        const NiPoint3 kCol0(pfGenMat[0], pfGenMat[3], pfGenMat[6]);
        const NiPoint3 kCol1(pfGenMat[1], pfGenMat[4], pfGenMat[7]);
        const NiPoint3 kCol2(pfGenMat[2], pfGenMat[5], pfGenMat[8]);

        NiMatrix3 kMat(kCol0, kCol1, kCol2);

        NiPoint3 kTrn(pfGenMat[9], pfGenMat[10], pfGenMat[11]);

        pkObject->ApplyTransform(kMat, kTrn, true);

        NiFree(pfGenMat);
    
        pkObject->RemoveExtraData("FLTXFORM");
    }
}
//-----------------------------------------------------------------------------
void CheckForZeroVerts(NiAVObject* pkModel)
{
    // Detach any NiTriShape objects that contain no vertices or contain no
    // triangles - Optimize can generate these, GB can save them to NIF, but
    // cannot read them back in.
    if (NiIsKindOf(NiTriShape, pkModel))
    {
        if ((((NiTriShape*)pkModel)->GetVertexCount() == 0) ||
            (((NiTriShape*)pkModel)->GetTriangleCount() == 0))
        {
            if (pkModel->GetParent())
                pkModel->GetParent()->DetachChild(pkModel);
        }
    }
    else if (NiIsKindOf(NiNode, pkModel))
    {
        NiNode* pkNode = (NiNode*)pkModel;

        unsigned int i;
        for (i = 0; i < pkNode->GetArrayCount(); i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);

            if (pkChild)
                CheckForZeroVerts(pkChild);
        }
    }
}
//-----------------------------------------------------------------------------
// If all children of a node are switch nodes, merge them into a single switch
// node.
void NiOpenFlight::MergeSiblingSwitchNodes(NiAVObject* pkModel)
{
    unsigned int i;

    if (NiIsKindOf(NiNode, pkModel))
    {
        NiNode* pkNode = (NiNode*)pkModel;
        NiAVObject* pkChild = NULL;

        // Determine if the NiSwitchNodes have been created from flipbook
        // animation nodes (and therefore are necessarily mutually exclusive
        // siblings).
        bool bFlipbookAnimation = false;
        for (i=0; i < pkNode->GetExtraDataSize(); i++)
        {
            NiExtraData* pkED = pkModel->GetExtraDataAt(i);
            NiStringExtraData* pkStrED
                = NiDynamicCast(NiStringExtraData, pkED);
            if (pkStrED)
            {
                const char* pcString = pkStrED->GetValue();
                if (pcString)
                {
                    if (!strcmp(pcString, "type: OpenFlight Flipbook"))
                        bFlipbookAnimation = true;
                }
            }
        }

        // Sanity check - all siblings must exist and must be NiSwitchNodes.
        bool bAllSiblingsAreSwitchNodes = false;
        if (pkNode->GetArrayCount() > 0)
            bAllSiblingsAreSwitchNodes = true;  // Could be true.
        for (i = 0; i < pkNode->GetArrayCount(); i++)
        {
            pkChild = pkNode->GetAt(i);

            if (pkChild)
            {
                if (!NiIsExactKindOf(NiSwitchNode, pkChild))
                    bAllSiblingsAreSwitchNodes = false;
            }
            else
            {
                bAllSiblingsAreSwitchNodes = false;
            }
            if (!bAllSiblingsAreSwitchNodes)
                break;
        }

        if ((bAllSiblingsAreSwitchNodes)
            && ((bFlipbookAnimation) || (ms_bForceMergeSiblingSwitchNodes)))
        {
            // Merge sibling NiSwitchNodes.
            NiSwitchNode* pkFirstChildNode
                = NiDynamicCast(NiSwitchNode, pkNode->GetAt(0));
            if (pkFirstChildNode)
            {
                for (i = 1; i < pkNode->GetArrayCount(); i++)
                {
                    NiSwitchNode* pkChildNode
                        = NiDynamicCast(NiSwitchNode, pkNode->GetAt(i));
                    if (pkChildNode)
                    {
                        unsigned int j;
                        for (j = 0; j < pkChildNode->GetArrayCount(); j++)
                        {
                            NiAVObject* pkGrandChild = pkChildNode->GetAt(j);

                            if (pkGrandChild)
                            {
                                NiAVObjectPtr spProxy = 
                                    pkChildNode->DetachChild(pkGrandChild);
                                pkFirstChildNode->AttachChild(spProxy);
                            }
                        }
                    }

                    pkNode->DetachChildAt(i);
                }
            }
        }

        for (i = 0; i < pkNode->GetArrayCount(); i++)   // Recurse.
        {
            pkChild = pkNode->GetAt(i);

            if (pkChild)
                MergeSiblingSwitchNodes(pkChild);
        }
    }
}
//-----------------------------------------------------------------------------

// If some children of a node are LOD nodes, assume their ranges do not
// overlap and merge them into a single LOD node.
// However, as a simple sanity check, verify only one range has
// a near value < 0.5.  This simple check resolves a significant number of
// issues in which sibling LOD nodes represent different objects.

void NiOpenFlight::MergeSiblingLODNodes(NiAVObject* pkModel)
{
    if (!ms_bForceMergeSiblingLODNodes)
        return;

    unsigned int i;

    if (NiIsKindOf(NiNode, pkModel))
    {
        NiNode* pkNode = (NiNode*)pkModel;
        NiAVObject* pkChild = NULL;
        NiLODNode* pkFirstChildNode = NULL;
        unsigned int uiCountLODNodeSiblings = 0;
        NiPoint3 kAvgCenter;
        NiRangeLODData* pkFirstChildLODRange = NULL;

        // Sanity check.  (Verify only one range has a near value < 0.5)
        bool bFoundStartRange = false;
        for (i = 0; i < pkNode->GetArrayCount(); i++)
        {
            pkChild = pkNode->GetAt(i);

            if ((pkChild) && (NiIsExactKindOf(NiLODNode, pkChild)))
            {
                NiLODNode* pkChildNode
                    = NiDynamicCast(NiLODNode, pkChild);
                if (pkChildNode)
                {
                    NiRangeLODData* pkLODRange = NiDynamicCast(
                        NiRangeLODData, pkChildNode->GetLODData());

                    float fNear, fFar;
                    pkLODRange->GetRange(0, fNear, fFar);

                    if (fNear < 0.5f)
                    {
                        if (bFoundStartRange)
                            goto RECURSE;
                        else
                            bFoundStartRange = true;
                    }
                }
            }
        }

        // Merge sibling NiLODNodes.
        for (i = 0; i < pkNode->GetArrayCount(); i++)
        {
            pkChild = pkNode->GetAt(i);

            if ((pkChild) && (NiIsExactKindOf(NiLODNode, pkChild)))
            {
                uiCountLODNodeSiblings++;

                if (uiCountLODNodeSiblings == 1)
                {
                    pkFirstChildNode = NiDynamicCast(NiLODNode, pkChild);
                    if (pkFirstChildNode)
                    {
                        pkFirstChildLODRange = NiDynamicCast(
                            NiRangeLODData, pkFirstChildNode->GetLODData());

                        kAvgCenter = pkFirstChildLODRange->GetCenter();
                    }
                }
                else
                {
                    NiLODNode* pkChildNode
                        = NiDynamicCast(NiLODNode, pkChild);
                    if (pkChildNode)
                    {
                        NiRangeLODData* pkLODRange = NiDynamicCast(
                            NiRangeLODData, pkChildNode->GetLODData());

                        kAvgCenter += pkLODRange->GetCenter();

                        float fNear, fFar;
                        pkLODRange->GetRange(0, fNear, fFar);

                        NIASSERT(pkChildNode->GetArrayCount() == 1);

                        unsigned int j;
                        for (j = 0; j < pkChildNode->GetArrayCount(); j++)
                        {
                            NiAVObject* pkGrandChild
                                = pkChildNode->GetAt(j);

                            if (pkGrandChild)
                            {
                                NiAVObjectPtr spProxy = 
                                    pkChildNode->DetachChild(pkGrandChild);
                                pkFirstChildNode->AttachChild(spProxy);

                                // Transfer LOD ranges, etc. to first child.
                                pkFirstChildLODRange->SetNumRanges(
                                    uiCountLODNodeSiblings);
                                pkFirstChildLODRange->SetRange(
                                    uiCountLODNodeSiblings - 1, fNear, fFar);
                            }
                        }
                    }

                    pkNode->DetachChildAt(i);
                }
            }
        }

        // Adjust shared center to be average of every child's center.
        if ((pkFirstChildLODRange) && (uiCountLODNodeSiblings > 0))
        {
            pkFirstChildLODRange->SetCenter(
                kAvgCenter / (float)(uiCountLODNodeSiblings));
        }

RECURSE:
        // Recurse.
        for (i = 0; i < pkNode->GetArrayCount(); i++)
        {
            pkChild = pkNode->GetAt(i);

            if (pkChild)
                MergeSiblingLODNodes(pkChild);
        }
    }
}
//-----------------------------------------------------------------------------
NiNode* NiOpenFlight::CreateScene (const char* pcFLTName)
{
    // Multigen _should_ have used const char*
    ms_pkActiveDB = mgOpenDb((char*)pcFLTName);

    if (!ms_pkActiveDB)
        return NULL;

    NiFilename kNIFName(pcFLTName);
    kNIFName.SetExt("");
    kNIFName.SetFilename("");

    char acFLTPath[MAX_PATH];
    kNIFName.GetFullPath(acFLTPath, MAX_PATH);

    NiOFPolygon::SetFilePath(acFLTPath);

    // Get light palette only for the master file.
    NiOFLight::GetLightPalette(ms_pkActiveDB);

    NiNode* pkHeaderNode = NULL;
    NiNode* pkModel = (NiNode*)Traverse(ms_pkActiveDB, 1.0f, pkHeaderNode);

    NiOFLight::ProcessLights (ms_pkActiveDB, pkModel, pkModel);

    NiTObjectArray<NiCameraPtr> kCameras;
    NiOFGetEyepoints(ms_pkActiveDB, kCameras);

    unsigned int i;
    for (i = 0; i < kCameras.GetSize(); i++)
        pkHeaderNode->AttachChild(kCameras.GetAt(i));

    ApplyDeferredTransforms(pkModel);

    // close the database
    mgCloseDb(ms_pkActiveDB);
    ms_pkActiveDB = NULL;


    MergeSiblingSwitchNodes(pkModel);
    MergeSiblingLODNodes(pkModel);


    // Force sharing of property pointers for shared properties.
    NiOptimize::RemoveDupProperties(pkModel);
    NiOptimize::RemoveChildlessNodes(pkModel);

    NiOptimize::RemoveSingleChildNodes(pkModel);

    NiOptimize::MergeSiblingTriShapes(pkModel, true);
    NiOptimize::RemoveSingleChildNodes(pkModel);

 
    CheckForZeroVerts(pkModel);


    bool bSelectiveUpdate, bRigid;
    pkModel->SetSelectiveUpdateFlags(bSelectiveUpdate, false, bRigid);


    return pkModel;
}
//-----------------------------------------------------------------------------
void NiOpenFlight::SetUseInternalTextures(bool bInt)
{
    NiOFPolygon::SetUseInternalTextures(bInt);
}
//-----------------------------------------------------------------------------
void NiOpenFlight::SetAddDefaultLights(bool bAddDefaultLights)
{
    NiOFLight::SetAddDefaultLights(bAddDefaultLights);
}
//-----------------------------------------------------------------------------
void NiOpenFlight::SetUseTrueDoubleSided(bool bUseTrueDoubleSided)
{
    NiOFPolygon::SetUseTrueDoubleSided(bUseTrueDoubleSided);
}
//-----------------------------------------------------------------------------
