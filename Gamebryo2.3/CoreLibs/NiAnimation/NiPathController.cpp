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
#include "NiAnimationPCH.h"

#include "NiPathController.h"

NiImplementRTTI(NiPathController,NiTimeController);

//---------------------------------------------------------------------------
NiPathController::NiPathController(NiPosData* pkPathData,
    NiFloatData* pkPctData) :
    m_spPathData(pkPathData), m_spPctData(pkPctData)
{
    m_uFlags = 0;
    m_uiLastPathIdx = 0;
    m_uiLastPctIdx = 0;

    SetAllowFlip(false);
    SetConstVelocity(false);
    SetFollow(false);
    m_fMaxBankAngle = 0.0f;
    m_fSmoothing = 0.0f;
    m_eBankDir = POSITIVE;
    m_sFollowAxis = 0;
    SetFlip(false);
    SetCurveTypeOpen(true);
    m_fMaxCurvature = 0.0f;

    // constant velocity data
    m_pfPartialLength = 0;
    m_fTotalLength = -1.0f;
    SetCVDataNeedsUpdate(true);
}
//---------------------------------------------------------------------------
NiPathController::~NiPathController()
{
    m_spPathData = 0;
    m_spPctData = 0;
    NiFree(m_pfPartialLength);
}
//---------------------------------------------------------------------------
void NiPathController::ReplacePathData(NiPosKey* pkKeys,
    unsigned int uiNumKeys, NiPosKey::KeyType eType)
{
    if (!m_spPathData)
        m_spPathData = NiNew NiPosData;

    m_spPathData->ReplaceAnim(pkKeys,uiNumKeys,eType);

    m_uiLastPathIdx = 0;
    unsigned char ucSize;
    GetPathData(pkKeys, uiNumKeys, eType, ucSize);

    // determine if curve is open or closed
    SetCurveTypeOpen(pkKeys->GetKeyAt(0, ucSize)->GetPos() !=
        pkKeys->GetKeyAt(uiNumKeys-1, ucSize)->GetPos());

    // preserve constant velocity status
    SetCVDataNeedsUpdate(true);
    if (GetConstVelocity())
        SetConstantVelocityData();

    // approximate the maximum curvature of path
    NiPosKey::CurvatureFunction pfnCF;
    pfnCF = NiPosKey::GetCurvatureFunction(eType);
    m_fMaxCurvature = pfnCF((NiAnimationKey*) pkKeys, uiNumKeys);
}
//---------------------------------------------------------------------------
void NiPathController::ReplacePctData(NiFloatKey* pkKeys,
    unsigned int uiNumKeys, NiFloatKey::KeyType eType)
{
    if (!m_spPctData)
        m_spPctData = NiNew NiFloatData;

    m_spPctData->ReplaceAnim(pkKeys,uiNumKeys,eType);

    m_uiLastPctIdx = 0;
    unsigned char ucSize;
    GetPctData(pkKeys, uiNumKeys, eType, ucSize);

    m_fLoKeyTime = pkKeys->GetKeyAt(0, ucSize)->GetTime();
    m_fHiKeyTime = pkKeys->GetKeyAt(uiNumKeys - 1, ucSize)->GetTime();
}
//---------------------------------------------------------------------------
float NiPathController::GetTotalPathLength() const
{
    if (m_fTotalLength < 0.0f)
    {
        unsigned int uiNumPathKeys;
        NiPosKey::KeyType ePathType;
        NiPosKey* pkPathKeys;
        unsigned char ucSize;

        GetPathData(pkPathKeys, uiNumPathKeys, ePathType, ucSize);
        NIASSERT(uiNumPathKeys >= 1);
        NiFree(m_pfPartialLength);
        m_pfPartialLength = NiAlloc(float, uiNumPathKeys);
        NIASSERT(m_pfPartialLength);

        m_pfPartialLength[0] = 0.0f;
        for (unsigned int ui0 = 0, ui1 = 1; ui0 < uiNumPathKeys-1; ui0++,
            ui1++)
        {
            // arc length of curve between knots i0 and i1
            float fLength = PathLength(ui0,ui1,1.0f);
    
            // length of curve between knots 0 and i1
            m_pfPartialLength[ui1] = m_pfPartialLength[ui0] + fLength;
        }
        m_fTotalLength = m_pfPartialLength[uiNumPathKeys-1];
    }
    return m_fTotalLength;
}
//---------------------------------------------------------------------------
void NiPathController::SetConstantVelocityData()
{
    if (!GetCVDataNeedsUpdate())
        return;

    m_fTotalLength = GetTotalPathLength();

    SetCVDataNeedsUpdate(false);
}
//---------------------------------------------------------------------------
void NiPathController::Update(float fTime) 
{
    unsigned int uiNumPctKeys;
    NiFloatKey::KeyType ePctType;
    NiFloatKey* pkPctKeys;
    unsigned char ucPctSize;
    
    GetPctData(pkPctKeys, uiNumPctKeys, ePctType, ucPctSize);
    if (!pkPctKeys)
        return;

    unsigned int uiNumPathKeys;
    NiPosKey::KeyType ePathType;
    NiPosKey* pkPathKeys;
    unsigned char ucPathSize;

    GetPathData(pkPathKeys, uiNumPathKeys, ePathType, ucPathSize);
    if (!pkPathKeys)
        return;

    if (DontDoUpdate(fTime))
        return;
    
    float fPct = NiFloatKey::GenInterp(m_fScaledTime, pkPctKeys, ePctType,
        uiNumPctKeys, m_uiLastPctIdx, ucPathSize);

    unsigned int ui1, ui2;
    float fNormTime;
    GetLoc(fPct, ui1, ui2, fNormTime);

    if (GetFollow())
    {
        NiMatrix3 kRefFrame;
        CalcRefFrame(ui1, ui2, fNormTime, kRefFrame);
        ((NiAVObject*)m_pkTarget)->SetRotate(kRefFrame);
    }

    NiPoint3 kPos = GenPathInterp(ui1, ui2, fNormTime);
    ((NiAVObject*)m_pkTarget)->SetTranslate(kPos);
}
//---------------------------------------------------------------------------
void NiPathController::GetPercentLoc(float fPercent, unsigned int& uiI1,
    unsigned int& uiI2, float& fNormTime)
{
    unsigned int uiNumPathKeys;
    NiPosKey::KeyType ePathType;
    NiPosKey* pkPathKeys;
    unsigned char ucSize;

    GetPathData(pkPathKeys, uiNumPathKeys, ePathType, ucSize);
    NIASSERT(uiNumPathKeys >= 2);

    if (fPercent == 0.0f)
    {
        uiI1 = 0;
        uiI2 = 1;
        fNormTime = 0.0f;
        return;
    }
    else if (fPercent < 0.0f)
    {
        fPercent = NiFmod(fPercent, 1.0f) + 1.0f;
    }

    if (fPercent == 1.0f)
    {
        uiI1 = uiNumPathKeys - 2;
        uiI2 = uiNumPathKeys - 1;
        fNormTime = 1.0f;
        return;
    }
    else if (fPercent > 1.0f)
    {
        fPercent = NiFmod(fPercent, 1.0f);
    }

    // determine which polynomial segment corresponds to fPercent
    float fLength = fPercent*m_fTotalLength;
    float fDist = 0.0f;
    float fTime = 0.0f;
    unsigned int ui0, ui1;
    for (ui0 = 0, ui1 = 1; ui0 < uiNumPathKeys-1; ui0++, ui1++)
    {
        if (fLength <= m_pfPartialLength[ui1])
        {
            // distance along segment
            fDist = fLength-m_pfPartialLength[ui0];
            
            // initial guess for integral upper limit
            fTime = fDist/(m_pfPartialLength[ui1]-m_pfPartialLength[ui0]);
            break;
        }
    }

    // use Newton's method to invert the path length integral
    const int iMaxIterations = 32;
    for (int i = 0; i < iMaxIterations; i++)
    {
        float fDiff = PathLength(ui0, ui1, fTime) - fDist;
        const float fTolerance = 1e-04f;
        if (NiAbs(fDiff) <= fTolerance)
            break;

        // assert: PathSpeed(i0,i1,fTime) != 0
        fTime -= fDiff/PathSpeed(ui0, ui1, fTime);
    }

    uiI1 = ui0;
    uiI2 = ui1;
    fNormTime = fTime;
}
//---------------------------------------------------------------------------
void NiPathController::GetLoc(float fTime, unsigned int& uiI1,
    unsigned int& uiI2, float& fNormTime)
{
    if (GetConstVelocity())
    {
        GetPercentLoc(fTime, uiI1, uiI2, fNormTime);
        m_uiLastPathIdx = uiI1;
        return;
    }

    unsigned int uiNumPathKeys;
    NiPosKey::KeyType ePathType;
    NiPosKey* pkPathKeys;
    unsigned char ucSize;

    GetPathData(pkPathKeys, uiNumPathKeys, ePathType, ucSize);
    NIASSERT(uiNumPathKeys >= 1);

    // clamp to time interval of key sequence
    if (fTime <= pkPathKeys->GetKeyAt(0, ucSize)->GetTime())
    {
        uiI1 = 0;
        uiI2 = 1;
        fNormTime = 0.0f;
        return;
    }

    NIASSERT(uiNumPathKeys >= 2);
    if (fTime >= pkPathKeys->GetKeyAt(uiNumPathKeys - 1, ucSize)->GetTime())
    {
        uiI1 = uiNumPathKeys - 2;
        uiI2 = uiNumPathKeys - 1;
        fNormTime = 1.0f;
        return;
    }

    bool bFound = false;
    bool bFirstLoop = true;
    unsigned int ui1;
    unsigned int ui2 = 0;
    for (ui1 = m_uiLastPathIdx; ui1 < uiNumPathKeys - 1;)
    {
        //
        // Check for direct hit
        //
        if (fTime == pkPathKeys->GetKeyAt(ui1, ucSize)->GetTime())
        {
            m_uiLastPathIdx = ui1;
            ui2 = ui1 + 1;
            bFound = true;
            break;
        }
        else if (fTime > pkPathKeys->GetKeyAt(ui1, ucSize)->GetTime() &&
            fTime < pkPathKeys->GetKeyAt(ui1 + 1, ucSize)->GetTime())
        {
            m_uiLastPathIdx = ui1;
            ui2 = ui1 + 1;
            bFound = true;
            break;
        }
        if (ui1 == uiNumPathKeys - 2 && bFirstLoop)
        {
            ui1 = 0;
            bFirstLoop = false;
            continue;
        }

        ui1++;
    }
        
    NIASSERT(bFound);
        
    // Normalize the time between 0 and 1 for the time values represented
    // in the two keys that will be used as interpolants.
    fNormTime = (fTime - pkPathKeys->GetKeyAt(ui1, ucSize)->GetTime()) / 
        (pkPathKeys->GetKeyAt(ui2, ucSize)->GetTime() -
        pkPathKeys->GetKeyAt(ui1, ucSize)->GetTime());
    uiI1 = ui1;
    uiI2 = ui2;
}
//---------------------------------------------------------------------------
NiPoint3 NiPathController::GenPathInterp(int iI1, int iI2, float fNormTime)
{
    unsigned int uiNumPathKeys;
    NiPosKey::KeyType ePathType;
    NiPosKey* pkPathKeys;
    unsigned char ucSize;
    
    GetPathData(pkPathKeys, uiNumPathKeys, ePathType, ucSize);
    NiPosKey::InterpFunction pfnInterp = 
        NiPosKey::GetInterpFunction(ePathType);
    NIASSERT(pfnInterp);
    NiPoint3 kResult;
    pfnInterp(fNormTime, pkPathKeys->GetKeyAt(iI1, ucSize), 
        pkPathKeys->GetKeyAt(iI2, ucSize), &kResult);
    return kResult;
}
//---------------------------------------------------------------------------
void NiPathController::CalcRefFrame(unsigned int ui1, unsigned int ui2,
    float fNormTime, NiMatrix3& kRefFrame)
{
    NiMatrix3 kTm = NiMatrix3::IDENTITY;

    unsigned int uiNumPathKeys;
    NiPosKey::KeyType ePathType;
    NiPosKey* pkPathKeys;
    unsigned char ucSize;
    
    GetPathData(pkPathKeys,uiNumPathKeys,ePathType,ucSize);

    if (uiNumPathKeys < 2)
    {
        kRefFrame = kTm;
        return;
    }
    
    NiPoint3 kTangent, kNormal, kBinormal;
    float fCurvature;

    if (GetAllowFlip())
    {
        // calculate coordinate frame as kNormal
        NiPosKey::CoordinateFrame(fNormTime,
            pkPathKeys->GetKeyAt(ui1, ucSize),
            pkPathKeys->GetKeyAt(ui2, ucSize), ePathType, kTangent, kNormal,
            kBinormal, fCurvature);

        // This code checks to see if time is within abs(tracking value).
        // If it is, the kNormal is calculated by finding two normals, 
        // one on each side of the control point in question, and weighting
        // their combination to give a single kNormal.  The purpose is to
        // give a result that will smooth any discontinuities in the
        // second derivative that might cause jumping/jerking as a 
        // controller passes over a control point.
        NiPoint3 kTmpTan, kNormal1;
        float fA0, fA1, fCurvature1;

        if (m_fSmoothing)
        {
            if (fNormTime >= 1.0f - m_fSmoothing)
            {
                unsigned int ui2a =
                  (!GetCurveTypeOpen() && ui2 == uiNumPathKeys-1 ? 0 : ui2);
                if (ui2a < uiNumPathKeys-1)
                {
                    fA0 = (1.0f - fNormTime + m_fSmoothing) /
                        (2.0f*m_fSmoothing);
                    fA1 = 1.0f - fA0;

                    NiPosKey::CoordinateFrame(1.0f - m_fSmoothing,
                        pkPathKeys->GetKeyAt(ui1, ucSize), 
                        pkPathKeys->GetKeyAt(ui2, ucSize),
                        ePathType, kTmpTan, kNormal, kBinormal, fCurvature);

                    NiPosKey::CoordinateFrame(m_fSmoothing,
                        pkPathKeys->GetKeyAt(ui2a, ucSize),
                        pkPathKeys->GetKeyAt(ui2a + 1, ucSize),
                        ePathType, kTmpTan, kNormal1, kBinormal, fCurvature1);

                    kNormal = fA0*kNormal + fA1*kNormal1;
                    kNormal.Unitize();
                    fCurvature = fA0*fCurvature + fA1*fCurvature1;
                    kBinormal = kTangent.Cross(kNormal);
                }
            }
            else if (fNormTime <= m_fSmoothing)
            {
                unsigned int ui1a =
                  (!GetCurveTypeOpen() && ui1 == 0 ? uiNumPathKeys-1 : ui1);
                if (ui1a >= 1)
                {
                    fA1 = (m_fSmoothing - fNormTime)/(2.0f*m_fSmoothing);
                    fA0 = 1.0f - fA1;

                    NiPosKey::CoordinateFrame(m_fSmoothing,
                        pkPathKeys->GetKeyAt(ui1, ucSize), 
                        pkPathKeys->GetKeyAt(ui2, ucSize),
                        ePathType, kTmpTan, kNormal, kBinormal, fCurvature);

                    NiPosKey::CoordinateFrame(1.0f - m_fSmoothing,
                        pkPathKeys->GetKeyAt(ui1a - 1, ucSize),
                        pkPathKeys->GetKeyAt(ui1a, ucSize),ePathType,
                        kTmpTan, kNormal1, kBinormal, fCurvature1);
                    kNormal = fA0*kNormal + fA1*kNormal1;
                    kNormal.Unitize();
                    fCurvature = fA0*fCurvature + fA1*fCurvature1;
                    kBinormal = kTangent.Cross(kNormal);
                }
            }
        }
        
        // direction
        kTm.SetCol(0, kTangent);
    
        // up
        kTm.SetCol(1, -kNormal);
        
        // right
        kTm.SetCol(2, -kBinormal);
    }
    else
    {
        // calculate the coordinate frame
        NiPosKey::CoordinateFrame(fNormTime,
            pkPathKeys->GetKeyAt(ui1, ucSize),
            pkPathKeys->GetKeyAt(ui2, ucSize), ePathType, kTangent, kNormal,
            kBinormal, fCurvature);

        // direction
        kTm.SetCol(0, kTangent);

        // up
        NiPoint3 up = NiPoint3::UNIT_Z.UnitCross(kTangent);
        kTm.SetCol(1, up);

        // right
        NiPoint3 right = kTangent.Cross(up);
        kTm.SetCol(2, right);
    }

    // swap axes if necessary
    if (GetFlip())
    {
        NiPoint3 kTemp;
        // at
        kTm.GetCol(0, kTemp);

        kTm.SetCol(0, -kTemp);

        // up
        kTm.GetCol(1, kTemp);

        kTm.SetCol(1, -kTemp);
    }

    NiPoint3 kTmp, kTmp2;

    switch (m_sFollowAxis)
    {
        case 1:
            kTm.GetCol(0, kTmp);
            kTm.GetCol(1, kTmp2);
            kTm.SetCol(0, -kTmp2);
            kTm.SetCol(1, kTmp);
            break;
        case 2:
            kTm.GetCol(0, kTmp);
            kTm.GetCol(2, kTmp2);
            kTm.SetCol(0, -kTmp2);
            kTm.SetCol(2, kTmp);
            break;
    } 

    if (GetBank())
    {
        static float s_fFourOverPi = 4.0f/NI_PI;
        float fBankAmt = float(m_eBankDir)*m_fMaxBankAngle;
        if (fCurvature < m_fMaxCurvature)
            fBankAmt *= s_fFourOverPi*NiATan(fCurvature/m_fMaxCurvature);

        NiMatrix3 kBankMat;
        
        kBankMat.MakeRotation(fBankAmt, kTangent);
        kTm = kBankMat*kTm;
    }

    kRefFrame = kTm;
}
//---------------------------------------------------------------------------
float NiPathController::PathSpeed(int iI0, int iI1, float fTime) const
{
    unsigned int uiNumPathKeys;
    NiPosKey::KeyType ePathType;
    NiPosKey* pkPathKeys;
    unsigned char ucSize;
    
    GetPathData(pkPathKeys, uiNumPathKeys, ePathType, ucSize);

    NiPosKey::InterpFunction interpD1 =
        NiPosKey::GetInterpD1Function(ePathType);
    NIASSERT(interpD1);

    NiPoint3 deriv;
    interpD1(fTime, pkPathKeys->GetKeyAt(iI0, ucSize), 
        pkPathKeys->GetKeyAt(iI1, ucSize), &deriv);
    return deriv.Length();
}
//---------------------------------------------------------------------------
float NiPathController::PathLength(int iI0, int iI1, float fTime) const
{
    // Legendre polynomial information for Gaussian quadrature of speed
    // on domain [0,fTime], 0 <= fTime <= 1.
    const int iDegree = 5;
    static float s_afModRoot[iDegree] =
    {
        // Legendre roots mapped to (root+1)/2
        0.046910077f,
        0.230765345f,
        0.5f,
        0.769234655f,
        0.953089922f
    };
    static float s_afModCoeff[iDegree] =
    {
        // original coefficients divided by 2
        0.118463442f,
        0.239314335f,
        0.284444444f,
        0.239314335f,
        0.118463442f
    };

    // Need to transform domain [0,fTime] to [-1,1].  If 0 <= x <= fTime
    // and -1 <= t <= 1, then x = fTime*(t+1)/2.
    float fResult = 0.0f;
    for (int i = 0; i < iDegree; i++)
        fResult += s_afModCoeff[i]*PathSpeed(iI0,iI1,fTime*s_afModRoot[i]);
    fResult *= fTime;

    return fResult;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPathController);
//---------------------------------------------------------------------------
void NiPathController::CopyMembers(NiPathController* pkDest,
    NiCloningProcess& kCloning)
{
    NiTimeController::CopyMembers(pkDest, kCloning);
    pkDest->m_uFlags = m_uFlags;

    // NOTE.  By calling CreateClone() rather than passing m_spPathData &
    // m_spPctData directly, this allows any change in what "clone" means to
    // be encapsulated by the CreateClone() call and does not require 
    // changing the code here.
    if (m_spPathData)
    {
        pkDest->SetPathData((NiPosData*)m_spPathData->CreateClone(
            kCloning));
    }

    if (m_spPctData)
    {
        pkDest->SetPctData((NiFloatData*)m_spPctData->CreateClone(
            kCloning));
    }

    pkDest->SetAllowFlip(GetAllowFlip());
    pkDest->SetBank(GetBank());
    pkDest->SetBankDir(GetBankDir());
    pkDest->SetConstVelocity(GetConstVelocity());
    pkDest->SetFollow(GetFollow());
    pkDest->SetMaxBankAngle(GetMaxBankAngle());
    pkDest->SetSmoothing(GetSmoothing());
    pkDest->SetFollowAxis(GetFollowAxis());
    pkDest->SetFlip(GetFlip());
    pkDest->SetCurveTypeOpen(GetCurveTypeOpen());

    if (m_pfPartialLength != NULL)
    {
        unsigned int uiNumPathKeys;
        NiPosKey::KeyType ePathType;
        NiPosKey* pkPathKeys;
        unsigned char ucSize;

        GetPathData(pkPathKeys, uiNumPathKeys, ePathType, ucSize);
        NIASSERT(uiNumPathKeys > 0);
        pkDest->m_pfPartialLength = NiAlloc(float,uiNumPathKeys);
        NIASSERT(pkDest->m_pfPartialLength != NULL);
        unsigned int uiDestSize = uiNumPathKeys * sizeof(float);
        NiMemcpy(pkDest->m_pfPartialLength, uiDestSize, m_pfPartialLength, 
            sizeof(m_pfPartialLength[0]) * uiNumPathKeys);
    }

    pkDest->m_fTotalLength = m_fTotalLength;
}
//---------------------------------------------------------------------------
void NiPathController::ProcessClone(NiCloningProcess& kCloning)
{
    NiTimeController::ProcessClone(kCloning);

    if (m_spPathData)
    {
        m_spPathData->ProcessClone(kCloning);
    }

    if (m_spPctData)
    {
        m_spPctData->ProcessClone(kCloning);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPathController);
//---------------------------------------------------------------------------
void NiPathController::LoadBinary(NiStream& kStream)
{
    NiTimeController::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);

    NiStreamLoadEnum(kStream, m_eBankDir);
    NiStreamLoadBinary(kStream, m_fMaxBankAngle);
    NiStreamLoadBinary(kStream, m_fSmoothing);
    NiStreamLoadBinary(kStream, m_sFollowAxis);

    kStream.ReadLinkID();   // m_spPathData
    kStream.ReadLinkID();   // m_spPctData
}
//---------------------------------------------------------------------------
void NiPathController::LinkObject(NiStream& kStream)
{
    NiTimeController::LinkObject(kStream);

    m_spPathData = (NiPosData*)kStream.GetObjectFromLinkID();

    m_spPctData = (NiFloatData*)kStream.GetObjectFromLinkID();

    // Update constant velocity data.
    SetCVDataNeedsUpdate(true);
    if (GetConstVelocity())
        SetConstantVelocityData();

    // approximate the maximum curvature of path
    unsigned int uiNumPathKeys;
    NiPosKey::KeyType ePathType;
    NiPosKey* pkPathKeys;
    unsigned char ucSize;
    GetPathData(pkPathKeys, uiNumPathKeys, ePathType, ucSize);
    if (uiNumPathKeys > 0)
    {
        NiPosKey::CurvatureFunction pfnCF;
        pfnCF = NiPosKey::GetCurvatureFunction(ePathType);
        m_fMaxCurvature = pfnCF((NiAnimationKey*) pkPathKeys, uiNumPathKeys);
    }
}
//---------------------------------------------------------------------------
bool NiPathController::RegisterStreamables(NiStream& kStream)
{
    if (!NiTimeController::RegisterStreamables(kStream))
        return false;
    
    if (m_spPathData)
        m_spPathData->RegisterStreamables(kStream);

    if (m_spPctData)
        m_spPctData->RegisterStreamables(kStream);
    return true;
}
//---------------------------------------------------------------------------
void NiPathController::SaveBinary(NiStream& kStream)
{
    NiTimeController::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uFlags);

    NiStreamSaveEnum(kStream, m_eBankDir);
    NiStreamSaveBinary(kStream, m_fMaxBankAngle);
    NiStreamSaveBinary(kStream, m_fSmoothing);
    NiStreamSaveBinary(kStream, m_sFollowAxis);

    kStream.SaveLinkID(m_spPathData);
    kStream.SaveLinkID(m_spPctData);
}
//---------------------------------------------------------------------------
bool NiPathController::IsEqual(NiObject* pkObject)
{
    if (!NiTimeController::IsEqual(pkObject))
        return false;

    NiPathController* pkCtrl = (NiPathController*) pkObject;

    if (!m_spPathData->IsEqual(pkCtrl->m_spPathData))
        return false;

    if (!m_spPctData->IsEqual(pkCtrl->m_spPctData))
        return false;

    if ((m_pfPartialLength && !pkCtrl->m_pfPartialLength)
    ||   (!m_pfPartialLength && pkCtrl->m_pfPartialLength))
    {
        return false;
    }

    if (m_pfPartialLength)
    {
        unsigned int uiNumPathKeys;
        NiPosKey::KeyType ePathType;
        NiPosKey* pkPathKeys;
        unsigned char ucSize;
        
        GetPathData(pkPathKeys,uiNumPathKeys,ePathType,ucSize);
        
        unsigned int uiCount = uiNumPathKeys*sizeof(float);
        if (memcmp(m_pfPartialLength,pkCtrl->m_pfPartialLength,uiCount) != 0)
            return false;
    }

    if (m_fTotalLength != pkCtrl->m_fTotalLength ||
        GetCVDataNeedsUpdate() != pkCtrl->GetCVDataNeedsUpdate() ||
        GetCurveTypeOpen() != pkCtrl->GetCurveTypeOpen() ||
        m_eBankDir != pkCtrl->m_eBankDir ||
        GetAllowFlip() != pkCtrl->GetAllowFlip() ||
        GetBank() != pkCtrl->GetBank() ||
        GetConstVelocity() != pkCtrl->GetConstVelocity() ||
        GetFollow() != pkCtrl->GetFollow() ||
        m_fMaxBankAngle != pkCtrl->m_fMaxBankAngle ||
        m_fSmoothing != pkCtrl->m_fSmoothing ||
        m_sFollowAxis != pkCtrl->m_sFollowAxis ||
        GetFlip() != pkCtrl->GetFlip())
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPathController::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiTimeController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPathController::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
