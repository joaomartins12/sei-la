
#include "stdafx.h"
#include "CsGBVisible.h"


void CsGBVisible::OnVisible( CsSortObj* pSortObj, NiAVObject* pObject, DWORD plagVP, float fAniTime )
{
	if( plagVP == 0 )
		return;

	if( IsBillboard( plagVP ) == true )
	{
		Billboard_Process( pObject, fAniTime );
	}

	if( IsPaticleContainer( plagVP ) == true )
	{
		pSortObj->Delete_IsFlag( CGeometry::GP_PARTICLE );
		PaticleContainer_Process( pSortObj, pObject, false );		
	}
}

void CsGBVisible::GetPlag( DWORD& dwReturn, NiAVObject* pObject )
{
	dwReturn = 0;
	_GetPlag( dwReturn, pObject );
}

void CsGBVisible::_GetPlag( DWORD& dwReturn, NiAVObject* pObject )
{
	if( ( pObject == NULL )||( pObject->GetAppCulled() == true ) )
		return;

	if( NiIsKindOf( NiBillboardNode, pObject ) == true )
	{
		dwReturn |= VP_BILLBOARD;
	}

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiNode* pNode = (NiNode*)pObject;		
		UINT uiChildCount = pNode->GetArrayCount();
		for( UINT i=0; i<uiChildCount; ++i )
		{
			_GetPlag( dwReturn, pNode->GetAt( i ) );
		}
	}
	else if (NiIsKindOf( NiGeometry, pObject ) == true )
	{
		NiGeometry* pGeom = (NiGeometry*)pObject;
		if( NiIsKindOf( NiMeshPSysData, pGeom->GetModelData() ) == true )
		{
			NiMeshPSysData* pPaticle = (NiMeshPSysData*)pGeom->GetModelData();
			if( pPaticle->GetContainerNode() != NULL )
			{
				dwReturn |= VP_PATICLE_CONTAINER;
			}
			_GetPlag( dwReturn, pPaticle->GetContainerNode() );
		}
	}
}

//=======================================================================================================
//
//		Billboard
//
//=======================================================================================================
void CsGBVisible::Billboard_Process( NiAVObject* pObject, float fAniTime )
{
	if( ( pObject == NULL )||( pObject->GetAppCulled() == true ) )
		return;

	if( NiIsKindOf( NiBillboardNode, pObject ) == true )
	{
		_Billboard( (NiBillboardNode*)pObject, fAniTime );
	}

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiNode* pNode = (NiNode*)pObject;

		UINT uiChildCount = pNode->GetArrayCount();
		for( UINT i=0; i<uiChildCount; ++i )
		{
			Billboard_Process( pNode->GetAt( i ), fAniTime );
		}
	}
}

void CsGBVisible::_Billboard( NiBillboardNode* pNode, float fAniTime )
{
	// get parent world transforms
	NiNode* pkParent = pNode->GetParent();
	NiCamera* pkCamera = CAMERA_ST.GetCameraObj();

	// compute billboard's world transforms
	NiTransform* m_pkWorld = (NiTransform*)&pNode->GetWorldTransform();

	if (pkParent)
	{
		NiTransform kParentXform;
		kParentXform = pkParent->GetWorldTransform();
		*m_pkWorld = kParentXform * pNode->GetLocalTransform();
	}
	else
	{		
		*m_pkWorld = pNode->GetLocalTransform();
	}	


	// additional orienting of billboard based on selected mode
	NiMatrix3 kFaceMat = NiMatrix3::IDENTITY;
	NiBillboardNode::FaceMode eFaceMode = pNode->GetMode();

	switch (eFaceMode)
	{
	case NiBillboardNode::ALWAYS_FACE_CENTER:
	case NiBillboardNode::ALWAYS_FACE_CAMERA:
		{
			NiPoint3 kCamD, kCamU, kCamR;

			if (eFaceMode == NiBillboardNode::ALWAYS_FACE_CENTER)
			{
				if(!_Billboard_RotateToCenter(pkCamera, m_pkWorld->m_Translate, kCamD, 
					kCamU, kCamR))
				{
					break;
				}
			}
			else
			{
				// billboard coordinates for world axes of camera
				kCamD = -pkCamera->GetWorldDirection(); 
				kCamU = pkCamera->GetWorldUpVector();
				kCamR = pkCamera->GetWorldRightVector();
			}
			kCamD = kCamD * m_pkWorld->m_Rotate;
			kCamU = kCamU * m_pkWorld->m_Rotate;
			kCamR = kCamR * m_pkWorld->m_Rotate;

			// Rotated model up vector is that vector in the plane
			// orthogonal to the camera direction which minimizes the angle
			// between it and the original model up (0,1,0).
			float fRoot = NiSqrt(kCamR.y*kCamR.y+kCamU.y*kCamU.y);
			if (fRoot > 1e-06f)
			{
				float fInvRoot = 1.0f/fRoot;
				float fCos = kCamU.y*fInvRoot;
				float fSin = -kCamR.y*fInvRoot;
				kFaceMat.SetCol(0, 
					fCos*kCamR.x+fSin*kCamU.x,
					fCos*kCamR.y+fSin*kCamU.y,
					fCos*kCamR.z+fSin*kCamU.z);
				kFaceMat.SetCol(1, 
					-fSin*kCamR.x+fCos*kCamU.x,
					-fSin*kCamR.y+fCos*kCamU.y,
					-fSin*kCamR.z+fCos*kCamU.z);
				kFaceMat.SetCol(2, kCamD);
			}
			else
			{
				kFaceMat.SetCol(0, -kCamR);
				kFaceMat.SetCol(1, -kCamU);
				kFaceMat.SetCol(2, kCamD);
			}
			break;
		}
	case NiBillboardNode::ROTATE_ABOUT_UP:
		{
			// transform the camera location to model space
			NiPoint3 kCamLoc = ((pkCamera->GetWorldLocation() - 
				m_pkWorld->m_Translate) * m_pkWorld->m_Rotate) / m_pkWorld->m_fScale;
			float fLength = NiSqrt(kCamLoc.x*kCamLoc.x+kCamLoc.z*kCamLoc.z);
			if (fLength < 1e-12f) 
				break;

			// unitize NDz
			fLength = 1.0f/fLength;
			kCamLoc.x *= fLength;
			kCamLoc.z *= fLength;

			kFaceMat.SetCol(0, kCamLoc.z, 0.0f, -kCamLoc.x);
			kFaceMat.SetCol(1, 0.0f, 1.0f, 0.0f);
			kFaceMat.SetCol(2, kCamLoc.x, 0.0f, kCamLoc.z);
			break;
		}

	case NiBillboardNode::RIGID_FACE_CAMERA:
	case NiBillboardNode::RIGID_FACE_CENTER:
		{
			NiPoint3 kCamD, kCamU, kCamR;

			if (eFaceMode == NiBillboardNode::RIGID_FACE_CENTER)
			{
				if(!_Billboard_RotateToCenter(pkCamera, m_pkWorld->m_Translate, kCamD, 
					kCamU, kCamR))
				{
					break;
				}
			}
			else
			{
				// billboard coordinates for world axes of camera
				kCamD = -pkCamera->GetWorldDirection();
				kCamU = pkCamera->GetWorldUpVector();
				kCamR = pkCamera->GetWorldRightVector();
			}
			kCamD = kCamD * m_pkWorld->m_Rotate;
			kCamU = kCamU * m_pkWorld->m_Rotate;
			kCamR = kCamR * m_pkWorld->m_Rotate;

			kFaceMat.SetCol(0, kCamR);
			kFaceMat.SetCol(1, kCamU);
			kFaceMat.SetCol(2, kCamD);
			break;
		}

	default:
		{
			NIASSERT(0);
		}
	}

	// adjust the billboard before applying its world transform
	m_pkWorld->m_Rotate = m_pkWorld->m_Rotate * kFaceMat;

	UINT uiChildCount = pNode->GetArrayCount();
	for( UINT i=0; i<uiChildCount; ++i )
	{
		pNode->GetAt( i )->UpdateDownwardPass( fAniTime, true );
	}
}

bool CsGBVisible::_Billboard_RotateToCenter(	const NiCamera* pkCamera, 
												const NiPoint3& kWorldTranslate, NiPoint3& kCamD, NiPoint3& kCamU, 
												NiPoint3& kCamR )
{
	// set kCamD, kCamU, and kCamR to look directly at kWorldTranslate

	NiPoint3 kAt = pkCamera->GetWorldLocation() - kWorldTranslate;
	if (kAt.SqrLength() < 0.001f)
		return false;

	kAt.Unitize();
	kCamD = -pkCamera->GetWorldDirection();
	kCamU = pkCamera->GetWorldUpVector();
	kCamR = pkCamera->GetWorldRightVector();
	float fDot = kAt.Dot(kCamD);
	if (fDot < 0.999999f) 
	{
		float fAngle = NiACos(fDot);
		NiPoint3 kAxis = kAt.UnitCross(kCamD);
		NiMatrix3 kRot;
		kRot.MakeRotation(fAngle, kAxis);
		kCamD = kAt; // equivalent to kRot * kCamD;
		kCamU = kRot * kCamU;
		kCamR = kRot * kCamR;
	}
	return true;
}


//=======================================================================================================
//
//		Particle Container
//
//=======================================================================================================

void CsGBVisible::PaticleContainer_Process( CsSortObj* pSortObj, NiAVObject* pObject, bool bParticle )
{
	if( ( pObject == NULL )||( pObject->GetAppCulled() == true ) )
		return;

	if( NiIsKindOf( NiNode, pObject ) == true )
	{
		NiNode* pNode = (NiNode*)pObject;
		UINT uiChildCount = pNode->GetArrayCount();
		for( UINT i=0; i<uiChildCount; ++i )
		{
			PaticleContainer_Process( pSortObj, pNode->GetAt( i ), bParticle );
		}
	}
	else if (NiIsKindOf( NiGeometry, pObject ) == true )
	{
		NiGeometry* pGeom = (NiGeometry*)pObject;

		if( bParticle == true )
		{
			CGeometry* pGeomClass = CGeometry::GetInstance( (CsNiGeometry*)pGeom, CGeometry::Normal );
			pGeomClass->SetGeometry( (CsNiGeometry*)pGeom );
			pGeomClass->m_nPlag |= CGeometry::GP_PARTICLE;
			pSortObj->m_vpGeom.PushBack( pGeomClass );
		}		
		
		if( NiIsKindOf( NiMeshPSysData, pGeom->GetModelData() ) == true )
		{
			NiMeshPSysData* pPaticle = (NiMeshPSysData*)pGeom->GetModelData();
			PaticleContainer_Process( pSortObj, pPaticle->GetContainerNode(), true );			
		}
	}
}