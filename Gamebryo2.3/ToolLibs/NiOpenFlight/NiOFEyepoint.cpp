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

#include "NiOFConst.h"
#include "NiOFEyepoint.h"

#include <fltcode.h>
#include <mgapiattr.h>
#include <mgapieyepoint1.h>

bool NiOFGetEyepoints(mgrec* pkDB, NiTObjectArray<NiCameraPtr>& kEyepoints)
{
    int i;
    for (i = 1; i <= 9; i++)
    {
        mgrec* pkEye = mgGetEyePoint (pkDB, i);
        if (pkEye)
        {
            int iActive = 0;
            mgGetAttList (pkEye, fltEyeActive,
                &iActive, MG_NULL);

            if (iActive)
            {
                NiCamera* pkCam = NiNew NiCamera;

                char acName[256];
                NiSprintf(acName, 256, "Eye %u", i);
                pkCam->SetName(acName);

                float fFOV, fScale, fNear, fFar;
                float fYaw, fPitch, fRoll;
                int iOrtho;
                int status = mgGetAttList (pkEye, 
                    fltEyeFov, &fFOV,
                    fltEyeScale, &fScale,
                    fltEyeNearClip, &fNear,
                    fltEyeFarClip, &fFar,
                    fltEyeOrthoFlag, &iOrtho, 
                    fltEyeYaw, &fYaw, 
                    fltEyePitch, &fPitch,
                    fltEyeRoll, &fRoll,
                    MG_NULL);

                // Sanity check, since openflightapi is reporting the
                //    following values, by default:
                //          Near = 0.01      Far = 160000000.0
                float fMaxFarNearRatio = pkCam->GetMaxFarNearRatio();
                if ((fFar/fNear) > 20000.0f)    // Sanity check.
                {
                    fFar = 10000.0f;
                    fNear = 0.5f;
                }
                pkCam->SetMaxFarNearRatio(fFar/fNear);
                pkCam->SetMinNearPlaneDist(fNear);

                fFOV *= RADIANS_PER_DEGREE;

                NiFrustum kFrustum;
                const float fFourThirds = 4.0f / 3.0f;
                float fCone = (float)tan(0.5f * fFOV);
                kFrustum.m_fLeft = -fCone * fFourThirds;
                kFrustum.m_fRight = fCone * fFourThirds;
                kFrustum.m_fTop = fCone;
                kFrustum.m_fBottom = -fCone;
                kFrustum.m_fNear = fNear;
                kFrustum.m_fFar = fFar;
                pkCam->SetViewFrustum(kFrustum);

                NiMatrix3 kYawMat, kPitchMat, kRollMat;
                kYawMat.MakeZRotation(RADIANS_PER_DEGREE * fYaw);
                kPitchMat.MakeXRotation(RADIANS_PER_DEGREE * fPitch);
                kRollMat.MakeYRotation(RADIANS_PER_DEGREE * fRoll);
                NiMatrix3 kMat = kYawMat * kPitchMat * kRollMat;

                NiMatrix3 kCompMat;
                kCompMat.SetEntry(0, 0, kMat.GetEntry(0, 1));
                kCompMat.SetEntry(0, 1, kMat.GetEntry(0, 2));
                kCompMat.SetEntry(0, 2, kMat.GetEntry(0, 0));
                kCompMat.SetEntry(1, 0, kMat.GetEntry(1, 1));
                kCompMat.SetEntry(1, 1, kMat.GetEntry(1, 2));
                kCompMat.SetEntry(1, 2, kMat.GetEntry(1, 0));
                kCompMat.SetEntry(2, 0, kMat.GetEntry(2, 1));
                kCompMat.SetEntry(2, 1, kMat.GetEntry(2, 2));
                kCompMat.SetEntry(2, 2, kMat.GetEntry(2, 0));

                pkCam->SetRotate(kCompMat);

                double lfX, lfY, lfZ;
                mgGetCoord3d(pkEye, fltEyeLookFrom, &lfX, &lfY, &lfZ);
                NiPoint3 kLookFrom((float)lfX, (float)lfY, (float)lfZ);

                // current eyepoint rotation center
                lfX = lfY = lfZ = 0.0f;
                mgGetCoord3d(pkEye, fltEyeRotCenter, &lfX, &lfY, &lfZ);
                NiPoint3 kEyeRotCen((float)lfX, (float)lfY, (float)lfZ);

                // direction from eye point to rotation center
                NiPoint3 kDiff = kLookFrom - kEyeRotCen;

                // The eyepoint is rotC-dist*cameraDir where rotC is rotation
                // center, dist is distance from rotation center to eyepoint,
                // and cameraDir is the camera direction vector.
                float fDist = kDiff.Length();

                NiPoint3 kCamDir(kCompMat.GetEntry(0, 0), 
                    kCompMat.GetEntry(1, 0), kCompMat.GetEntry(2, 0));

                pkCam->SetTranslate(kEyeRotCen - fDist * kCamDir);

                kEyepoints.Add(pkCam);
            }
        }
    }

    return true;
}


