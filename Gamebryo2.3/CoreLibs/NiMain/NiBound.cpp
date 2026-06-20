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

#include "NiBound.h"
#include "NiMath.h"
#include "NiMatrix3.h"
#include "NiNode.h"
#include "NiPoint3.h"
#include <NiRTLib.h>

float NiBound::ms_fFuzzFactor = 1e-04f;
float NiBound::ms_fTolerance  = 1e-06f;

//---------------------------------------------------------------------------
void NiBound::ComputeMinimalBound(NiTPrimitiveArray<const NiBound*>& kBounds)
{
    // assert:  array contains only non-null pointers to NiBound
    NIASSERT(kBounds.GetSize() > 0);

    // get bounding sphere of first child
    const NiBound* pkBound = kBounds.GetAt(0);
    m_kCenter = pkBound->m_kCenter;
    m_fRadius = pkBound->m_fRadius;

    // process remaining bounding spheres
    for (unsigned int i = 1; i < kBounds.GetSize(); i++)
        Merge(kBounds.GetAt(i));
}
//---------------------------------------------------------------------------
bool NiBound::TestIntersect(float fTime, 
    const NiBound& kB0, const NiPoint3& kV0, 
    const NiBound& kB1, const NiPoint3& kV1)
{
    NiPoint3 kDeltaV = kV1 - kV0;
    float fA = kDeltaV.SqrLength();
    NiPoint3 kDeltaC = kB1.GetCenter() - kB0.GetCenter();
    float fC = kDeltaC.SqrLength();
    float fRadiusSum = kB0.GetRadius() + kB1.GetRadius();
    float fRadiusSumSqr = fRadiusSum * fRadiusSum;

    if (fA > 0.0f)
    {
        float fB = kDeltaC.Dot(kDeltaV);
        if (fB <= 0.0f)
        {
            if (-fTime * fA <= fB)
                return fA * fC - fB * fB <= fA * fRadiusSumSqr;
            else
                return fTime * (fTime * fA + 2.0f * fB) + fC <= fRadiusSumSqr;
        }
    }

    return fC <= fRadiusSumSqr;
}
//---------------------------------------------------------------------------
bool NiBound::FindIntersect(float fTime, 
    const NiBound& kB0, const NiPoint3& kV0, 
    const NiBound& kB1, const NiPoint3& kV1, float& fIntrTime, 
    NiPoint3& kIntrPt, bool bCalcNormals, NiPoint3& kNormal0, 
    NiPoint3& kNormal1)
{
    // compute the theoretical first time and point of contact

    NiPoint3 kDeltaV = kV1 - kV0;
    float fA = kDeltaV.SqrLength();
    NiPoint3 kDeltaC = kB1.GetCenter() - kB0.GetCenter();
    float fC = kDeltaC.SqrLength();
    float fRadiusSum = kB0.GetRadius() + kB1.GetRadius();
    float fRadiusSumSqr = fRadiusSum * fRadiusSum;

    if (fA > 0.0f)
    {
        float fB = kDeltaC.Dot(kDeltaV);
        if (fB <= 0.0f)
        {
            if (-fTime * fA <= fB
                || fTime * (fTime * fA + 2.0f * fB) + fC <= fRadiusSumSqr)
            {
                float fCDiff = fC - fRadiusSumSqr;
                float fDiscr = fB * fB - fA * fCDiff;
                if (fDiscr >= 0.0f)
                {
                    if (fCDiff <= 0.0f)
                    {
                        // spheres are already intersecting, use midpoint
                        // of segment connecting centers
                        fIntrTime = 0.0f;
                        kIntrPt = 0.5f*(kB0.GetCenter()+kB1.GetCenter());

                        if (bCalcNormals)
                        {
                            kNormal0 = kDeltaC;
                            kNormal0.Unitize();
                            kNormal1 = -kNormal0;
                        }
                    }
                    else
                    {
                        // first time of contact is in [0, tmin]
                        fIntrTime = -(fB+NiSqrt(fDiscr))/fA;

                        // clamp to handle floating point round-off errors
                        if (fIntrTime < 0.0f)
                            fIntrTime = 0.0f;
                        else if (fIntrTime > fTime)
                            fIntrTime = fTime;

                        NiPoint3 kCenDiff = kDeltaC + fIntrTime*kDeltaV;

                        kIntrPt = kB0.GetCenter() + fIntrTime*kV0 +
                            (kB0.GetRadius()/fRadiusSum)*kCenDiff;

                        if (bCalcNormals)
                        {
                            kNormal0 = kCenDiff;
                            kNormal0.Unitize();
                            kNormal1 = -kNormal0;
                        }
                    }
                    return true;
                }
            }
            return false;
        }
    }

    if (fC <= fRadiusSumSqr)
    {
        // spheres are already intersecting, use midpoint of segment
        // connecting centers
        fIntrTime = 0.0f;
        kIntrPt = 0.5f*(kB0.GetCenter()+kB1.GetCenter());

        if (bCalcNormals)
        {
            kNormal0 = kDeltaC;
            kNormal0.Unitize();
            kNormal1 = -kNormal0;
        }
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiBound::LoadBinary(NiStream& kStream)
{
    m_kCenter.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_fRadius);
}
//---------------------------------------------------------------------------
void NiBound::SaveBinary(NiStream& kStream)
{
    m_kCenter.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_fRadius);
}
//---------------------------------------------------------------------------
char* NiBound::GetViewerString(const char* pPrefix) const
{
    unsigned int uiLen = strlen(pPrefix) + 65;
    char* pString = NiAlloc(char, uiLen);
    NiSprintf(pString, uiLen, "%s = (%g, %g, %g) , %g", pPrefix, m_kCenter.x, 
        m_kCenter.y, m_kCenter.z, m_fRadius);
    return pString;
}
//---------------------------------------------------------------------------

