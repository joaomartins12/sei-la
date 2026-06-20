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
#include "NiOFTransform.h"

#include <fltcode.h>
#include <mgapiattr.h>
#include <mgapistruc.h>
#include <mgapixform.h>

//---------------------------------------------------------------------------
NiOFTransform::NiOFTransform (mgrec* pkXform) :
    m_kTrn(NiPoint3::ZERO),
    m_kMat(NiMatrix3::IDENTITY)
{
    mgxfllcode eXtype = mgGetXformType(pkXform);
    switch ( eXtype )
    {
        case MXLL_TRANSLATE:
        {
            ConvertXllTranslate(pkXform);
            m_bDeferApply = false;
            break;
        }

        case MXLL_ROTEDGE:
        {
            ConvertXllRotEdge(pkXform);
            m_bDeferApply = false;
            break;
        }

        case MXLL_ROTPT:
        {
            ConvertXllRotPt(pkXform);
            m_bDeferApply = false;
            break;
        }

        case MXLL_PUT:
        {
            ConvertXllPut(pkXform);
            m_bDeferApply = false;
            break;
        }

        case MXLL_SCALE:
        {
            float fSx = 1.0f, fSy = 1.0f, fSz = 1.0f;
            mgGetAttList(pkXform, fltXmScaleX, &fSx,
                                fltXmScaleY, &fSy,
                                fltXmScaleZ, &fSz, mgNULL);

            // exempt the case of uniform unit scale from the non-uniform scale
            //    scenario  - 12/4/2000
            if ((fabs(double(fSx) - 1.0) < 0.005)
                    && (fabs(double(fSy) - 1.0) < 0.005)
                    && (fabs(double(fSz) - 1.0) < 0.005))
            {
                m_bDeferApply = false;
            }
            else
            {
                m_bDeferApply = true;
            }

            //  // ConvertXllScale(pkXform);
            //  m_bDeferApply = true;
            break;
        }

        case MXLL_TOPOINT:
        {
            // ConvertXllToPoint(pkXform);
            m_bDeferApply = true;
            break;
        }

        case MXLL_GENERAL:
        {
            // ConvertXllGeneral(pkXform);
            m_bDeferApply = true;
            break;
        }

        default:
        {
            m_bDeferApply = true;
            break;
        }
    }
}

//---------------------------------------------------------------------------
void NiOFTransform::ConvertXllTranslate (mgrec* pkXform)
{
    double dx, dy, dz;
    mgGetIcoord(pkXform, fltXmTranslateDelta, &dx, &dy, &dz);

    m_kTrn.x = float(dx);
    m_kTrn.y = float(dy);
    m_kTrn.z = float(dz);
}

//---------------------------------------------------------------------------
void NiOFTransform::ConvertXllRotEdge (mgrec* pkXform)
{
    // get first reference point
    double x1, y1, z1;
    mgGetIcoord(pkXform, fltXmRotRefPoint1, &x1, &y1, &z1);
    
    // get second reference point
    double x2, y2, z2;
    mgGetIcoord(pkXform, fltXmRotRefPoint2, &x2, &y2, &z2);

    // get angle of rotation (Multigen uses degrees)
    float fDegrees;
    mgGetAttList(pkXform, fltXmRotEdgeAngle, &fDegrees, mgNULL);

    // compute axis of rotation
    NiPoint3 kRef1((float)x1, (float)y1, (float)z1);
    NiPoint3 kRef2((float)x2, (float)y2, (float)z2);
    NiPoint3 kAxis = kRef2 - kRef1;
    kAxis.Unitize();

    // Use -degrees to get transpose of MG rotation since they do matrix
    //    multiplies on the right of vectors whereas NI does matrix
    //    multiplies on the left of vectors.
    float fRadians = -RADIANS_PER_DEGREE * fDegrees;

    m_kMat.MakeRotation(fRadians, kAxis);
    m_kTrn = kRef1 - m_kMat * kRef1;
}

//---------------------------------------------------------------------------
void NiOFTransform::ConvertXllRotPt (mgrec* pkXform)
{
    // get center of rotation
    double x, y, z;
    mgGetCoord3d(pkXform, fltXmRotateCenter, &x, &y, &z);
    NiPoint3 kCenter((float)x,(float)y,(float)z);

    // get axis of rotation
    NiPoint3 kAxis;
    mgGetVector(pkXform, fltXmRotateAxis, &kAxis.x, &kAxis.y, &kAxis.z);

    // get angle of rotation (Multigen uses degrees)
    float fDegrees;
    mgGetAttList(pkXform, fltXmRotateAngle, &fDegrees, mgNULL);

    // Use -degrees to get transpose of MG rotation since they do matrix
    //    multiplies on the right of vectors whereas NI does matrix
    //    multiplies on the left of vectors.
    float fRadians = -(RADIANS_PER_DEGREE * fDegrees);

    m_kMat.MakeRotation(fRadians, kAxis);
    m_kTrn = kCenter - (m_kMat * kCenter);
}

//---------------------------------------------------------------------------
void NiOFTransform::ConvertXllPut (mgrec* pkXform)
{
    mgmatrix m;
    mgGetMatrix(pkXform, fltMatrix, &m);

    m_kMat.SetEntry(0, 0, (float)m[0]);
    m_kMat.SetEntry(1, 0, (float)m[1]);
    m_kMat.SetEntry(2, 0, (float)m[2]);
    m_kMat.SetEntry(0, 1, (float)m[4]);
    m_kMat.SetEntry(1, 1, (float)m[5]);
    m_kMat.SetEntry(2, 1, (float)m[6]);
    m_kMat.SetEntry(0, 2, (float)m[8]);
    m_kMat.SetEntry(1, 2, (float)m[9]);
    m_kMat.SetEntry(2, 2, (float)m[10]);

    m_kTrn[0] = (float)m[12];
    m_kTrn[1] = (float)m[13];
    m_kTrn[2] = (float)m[14];
}

//---------------------------------------------------------------------------
