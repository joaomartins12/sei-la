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


#include "stdafx.h"
#include "PlanarOccluder.h"

// #include <NiCamera.h>
// #include <NiGeometry.h>
// #include <NiNode.h>

NiImplementRTTI(PlanarOccluder, Occluder);

//---------------------------------------------------------------------------
PlanarOccluder::PlanarOccluder()
{
}
//---------------------------------------------------------------------------
PlanarOccluder::PlanarOccluder(NiGeometry* pkPlane) 
{
    NIASSERT(pkPlane != 0);
    m_spGeometry = pkPlane;
    m_iCameraWhichSide = NiPlane::NO_SIDE;
    FillData();
}
//---------------------------------------------------------------------------
PlanarOccluder::~PlanarOccluder()
{
    m_spGeometry = 0;
}
//---------------------------------------------------------------------------
void PlanarOccluder::Update(NiCamera* pkCamera)
{
    // Determine which side of the occlusion planes the camera is on for
    // fast dismissal during occlusion testing.
    if (m_kLastTransform != m_spGeometry->GetWorldTransform())
        FillData();
    

	NiPoint3 kCameraLoc = pkCamera->GetWorldLocation();
	m_iCameraWhichSide = m_kOcclusionPlane.WhichSide(kCameraLoc);

	// Generate the four world-space points of the occluder plane.
	NiPoint3 kCenter = m_spGeometry->GetWorldBound().GetCenter();
	NiPoint3 kR = m_fRightMag * m_kWorldRight;
	NiPoint3 kUR = kR + m_fUpMag * m_kWorldUp;
	NiPoint3 kLR = kR - m_fUpMag * m_kWorldUp;
	NiPoint3 akSidePoints[4] =
	{
		kCenter + kUR,
		kCenter + kLR,
		kCenter - kUR,
		kCenter - kLR
	};

	// akSideVec[i] == (akSidePoints[(i+1)%4]-akSidePoints[i]).Unitize()
	NiPoint3 akSideVecs[4] =
	{
		-m_kWorldUp,
		-m_kWorldRight,
		m_kWorldUp,
		m_kWorldRight
	};

	NiPoint3 kCameraVec = pkCamera->GetWorldDirection();

	// If the plane is mostly perpendicular to the camera...
	if (m_iCameraWhichSide == NiPlane::NO_SIDE ||
		NiAbs(kCameraVec.Dot(m_kOcclusionPlane.GetNormal())) < 0.001f)
	{
		m_iCameraWhichSide = NiPlane::NO_SIDE;
		return;
	}

	// Generate frustum planes.  Each frustum plane contains one side
	// of the occluder plane and the camera dir.  These planes will all
	// face out if and only if the occlusion plane faces out (to the camera).
	bool bPerspective = !pkCamera->GetViewFrustum().m_bOrtho;
	for (unsigned int i = 0; i < 4; i++)
	{
		if (bPerspective)
		{
			kCameraVec = akSidePoints[i] - kCameraLoc;
		}

		NiPoint3 kNorm = kCameraVec.UnitCross(akSideVecs[i]);
		m_akSides[i].SetNormal(kNorm);
		m_akSides[i].SetConstant(kNorm.Dot(akSidePoints[i]));

		// Assert normals of the planes are all pointing the same way 
		// (in or out) as the occlusion plane. This will simplify 
		// calculations later.  This assertion may trigger if the logic
		// calculating the normal in FillData() has changed.
		NIASSERT(m_akSides[i].WhichSide(akSidePoints[(i+2)%4]) != 
			m_iCameraWhichSide);
	}
    return;
}
//---------------------------------------------------------------------------
bool PlanarOccluder::IsOccluded(NiCamera* pkCamera, const NiBound& kBound)
{
	NiPoint3 kTestLocation;

	if (m_iCameraWhichSide == NiPlane::NO_SIDE)
		return false;

	// If the plane is facing out (camera is on the positive side), then
	// bias the point along the normal and vice versa.
	float fRadius = NiSelect(m_iCameraWhichSide == NiPlane::POSITIVE_SIDE, 
		kBound.GetRadius(), -kBound.GetRadius()); 
	NiPoint3 kCenter = kBound.GetCenter();

	// Test if the bound crosses the occlusion plane
	kTestLocation = kCenter + fRadius * m_kOcclusionPlane.GetNormal();
	if (m_kOcclusionPlane.WhichSide(kTestLocation) == m_iCameraWhichSide)
		return false;

	bool bCulled = true;
	for (unsigned int i = 0; i < 4 && bCulled; i++)
	{
		NiPoint3 kTestPoint = kCenter + fRadius * m_akSides[i].GetNormal();

		// If the furthest extent of the sphere in the direction of this plane
		// is not on the same side as the camera (i.e. crosses the plane), 
		// then it is culled by the occluder.
		bCulled &= m_akSides[i].WhichSide(kTestPoint) != m_iCameraWhichSide;
	}

	// If we reach here and bCulled is true, then the all four frustum planes
	// and the infinite occlusion plane have occluded the geometry.
	return bCulled;
}
//---------------------------------------------------------------------------
void PlanarOccluder::ShowObject(bool bVisible)
{
	NiNode* pNiNode = nsCSGBFUNC::GetRootNiNode(m_spGeometry);
	if( pNiNode )
		pNiNode->SetAppCulled( !bVisible );
}
//---------------------------------------------------------------------------
// bool PlanarOccluder::CheckPlaneExtents(NiCamera* pkCamera, const NiBound& kBound)
// {
//     // The full bound falls on the opposite side of the plane from the
//     // camera. We just need to check and see if each point intersects the
//     // plane within it's extents.
//     NiPoint3 kCenter = kBound.GetCenter();
//     float fRadius = kBound.GetRadius();
// 
//     NiPoint3 kTestPoint;
// 
//     // Bound offset up.
//     kTestPoint = kCenter + fRadius * pkCamera->GetWorldUpVector();
//     if (!TestExtent(pkCamera, kTestPoint))
//         return false;
// 
//     // Bound offset down.
//     kTestPoint = kCenter - fRadius * pkCamera->GetWorldUpVector();
//     if (!TestExtent(pkCamera, kTestPoint))
//         return false;
// 
//     // Bound offset right.
//     kTestPoint = kCenter + fRadius * pkCamera->GetWorldRightVector();
//     if (!TestExtent(pkCamera, kTestPoint))
//         return false;
// 
//     // Bound offset left.
//     kTestPoint = kCenter - fRadius * pkCamera->GetWorldRightVector();
//     if (!TestExtent(pkCamera, kTestPoint))
//         return false;
// 
//     // If we reach here, then all the offset bound points are within the
//     // plane extents so the object is occluded.
//     return true;
// }
// //---------------------------------------------------------------------------
// bool PlanarOccluder::TestExtent(NiCamera* pkCamera,
//     const NiPoint3& kTestPoint) 
// {
//     // Find intersection with the geometric occlusion plane.
//     NiPoint3 kCameraLoc = pkCamera->GetWorldTranslate();
//     NiPoint3 kPlaneNormal = m_kOcclusionPlane.GetNormal();
//     float fPlaneConstant = m_kOcclusionPlane.GetConstant();
// 
//     NiPoint3 kTemp = (kTestPoint - kCameraLoc);
//     float fBottomTerm = kTemp.Dot(kPlaneNormal);
// 
//     if (fBottomTerm < 0.001f && fBottomTerm > -0.001f)
//         return true;
// 
//     float fTopTerm = fPlaneConstant - kPlaneNormal.Dot(kCameraLoc);
// 
//     float fU = fTopTerm / fBottomTerm;
// 
//     NiPoint3 kIntersection = kCameraLoc + fU * (kTestPoint - kCameraLoc);
// 
//     // Project a vector from the plane center to kIntersection onto right
//     // and up vectors to make sure that the projection length is not 
//     // greater than the stored magnitude. Since the world up and right 
//     // are normalized, we can do this with a dot product.
//     kTemp = kIntersection - m_spGeometry->GetWorldBound().GetCenter();
// 
//     float fRightMag = kTemp.Dot(m_kWorldRight);
//     // If this passes, then we're outside the geometric plane extents.
//     if (NiAbs(fRightMag) > m_fRightMag)
//         return false;
// 
//     float fUpMag = kTemp.Dot(m_kWorldUp);
//     // If this passes, then we're outside the geometric plane extents.
//     if (NiAbs(fUpMag) > m_fUpMag)
//         return false;
// 
//     return true;
// }
//---------------------------------------------------------------------------
void PlanarOccluder::FillData()
{
    NiGeometryData* pkData = m_spGeometry->GetModelData();
    NIASSERT(pkData->GetVertexCount() >= 3);

    NiPoint3 kPoint[3];
    kPoint[0] = *pkData->GetVertices();
    kPoint[1] = *(pkData->GetVertices() + 1);
    kPoint[2] = *(pkData->GetVertices() + 2);

    kPoint[0] = m_spGeometry->GetWorldTransform() * kPoint[0];
    kPoint[1] = m_spGeometry->GetWorldTransform() * kPoint[1];
    kPoint[2] = m_spGeometry->GetWorldTransform() * kPoint[2];

    m_kWorldUp = kPoint[1] - kPoint[0];
    m_kWorldRight = kPoint[2] - kPoint[0];
    m_fUpMag = m_kWorldUp.Unitize() * 0.5f;
    m_fRightMag = m_kWorldRight.Unitize() * 0.5f;

    NiPoint3 kNormal = m_kWorldUp.Cross(m_kWorldRight);

    m_kOcclusionPlane = NiPlane(kNormal, kPoint[0]);

    m_kLastTransform = m_spGeometry->GetWorldTransform();
}
//---------------------------------------------------------------------------
NiGeometry* PlanarOccluder::GetGeometry()
{
    return m_spGeometry;
}
//---------------------------------------------------------------------------
