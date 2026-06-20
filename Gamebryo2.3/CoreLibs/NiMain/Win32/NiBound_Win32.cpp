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
#include "NiPlane.h"
#include "NiPoint3.h"
#include <NiRTLib.h>

//---------------------------------------------------------------------------
bool NiBound::operator==(const NiBound& bound)
{
    return (m_fRadius == bound.m_fRadius && m_kCenter == bound.m_kCenter);
}
//---------------------------------------------------------------------------
bool NiBound::operator<=(const NiBound& bound)
{
    // returns true iff 'this' is contained in 'bound'
    NiPoint3 diff = bound.m_kCenter - m_kCenter;
    return diff.Length()+m_fRadius <= bound.m_fRadius + ms_fFuzzFactor;
}
//---------------------------------------------------------------------------
bool NiBound::operator>=(const NiBound& bound)
{
    // returns true iff 'this' contains 'bound'
    NiPoint3 diff = bound.m_kCenter - m_kCenter;
    return diff.Length()+bound.m_fRadius <= m_fRadius + ms_fFuzzFactor;
}
//---------------------------------------------------------------------------
void NiBound::ComputeFromData(int iQuantity, const NiPoint3* pkData)
{
    if (iQuantity <= 0)
    {
        m_kCenter = NiPoint3::ZERO;
        m_fRadius = 0.0f;
        return;
    }

    // compute the axis-aligned box containing the data
    NiPoint3 kMin = pkData[0];
    NiPoint3 kMax = kMin;
    int i;
    for (i = 1; i < iQuantity; i++) 
    {
        if (kMin.x > pkData[i].x)
            kMin.x = pkData[i].x;
        if (kMin.y > pkData[i].y)
            kMin.y = pkData[i].y;
        if (kMin.z > pkData[i].z)
            kMin.z = pkData[i].z;
        if (kMax.x < pkData[i].x)
            kMax.x = pkData[i].x;
        if (kMax.y < pkData[i].y)
            kMax.y = pkData[i].y;
        if (kMax.z < pkData[i].z)
            kMax.z = pkData[i].z;
    }

    // sphere center is the axis-aligned box center
    m_kCenter = 0.5f * (kMin + kMax);

    // compute the radius
    float fRadiusSqr = 0.0f;
    for(i = 0; i < iQuantity; i++) 
    {
        NiPoint3 diff = pkData[i] - m_kCenter;
        float fLengthSqr = diff * diff;
        if (fLengthSqr > fRadiusSqr)
            fRadiusSqr = fLengthSqr;
    }
    m_fRadius = NiSqrt(fRadiusSqr);
}
//---------------------------------------------------------------------------
void NiBound::Merge(const NiBound *pBound)
{
    // difference of this' and pBound's sphere center
    NiPoint3 diff = m_kCenter-pBound->m_kCenter;
    
    float fLengthSqr = diff*diff;
    float fDeltaRad = pBound->m_fRadius - m_fRadius;
    float fDeltaRadSqr = fDeltaRad*fDeltaRad;
    float fLength, fAlpha;
    
    if (fDeltaRad >= 0.0f)
    {
        if (fDeltaRadSqr >= fLengthSqr)
        {
            // pBound's sphere encloses this's sphere
            m_kCenter = pBound->m_kCenter;
            m_fRadius = pBound->m_fRadius;
        }
        else
        {
            // this' sphere does not enclose pBound's sphere
            fLength = NiSqrt(fLengthSqr);
            if (fLength > ms_fTolerance)
            {
                fAlpha = (fLength - fDeltaRad)/(2.0f*fLength);
                m_kCenter = pBound->m_kCenter + fAlpha*diff;
            }
            m_fRadius = 0.5f*(pBound->m_fRadius+fLength+m_fRadius);
        }
    }
    else if (fDeltaRadSqr < fLengthSqr)
    {
        // this' sphere does not enclose pBound's sphere
        fLength = NiSqrt(fLengthSqr);
        if (fLength > ms_fTolerance)
        {
            fAlpha = (fLength - fDeltaRad)/(2.0f*fLength);
            m_kCenter = pBound->m_kCenter + fAlpha*diff;
        }
        m_fRadius = 0.5f*(pBound->m_fRadius+fLength+m_fRadius);
    }
    // else this's sphere encloses pBound's sphere
}
//---------------------------------------------------------------------------
void NiBound::Update(const NiBound& bound, const NiTransform& xform)
{
    // rigid motion does not change the bounding sphere radius
    m_kCenter = xform.m_fScale * (xform.m_Rotate * bound.m_kCenter) + 
        xform.m_Translate;
    m_fRadius = xform.m_fScale * bound.m_fRadius;
}
//---------------------------------------------------------------------------
