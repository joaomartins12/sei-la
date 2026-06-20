
#include "stdafx.h"
#include "CsGBSubCamera.h"

CsGBSubCamera::CsGBSubCamera()
{
	m_pTargetObj = NiNew NiNode;
	m_pCameraObj = NiNew NiCamera;
	
	m_fElapsedTime = 0.0f;
	m_fTargetDeltaHeight = 0.0f;

	m_bDistRange = false;
}

CsGBSubCamera::~CsGBSubCamera()
{
	m_pTargetObj = 0;
	m_pCameraObj = 0;
}

void CsGBSubCamera::Create( NiDX9Renderer* pRenderer, sCAMERAINFO* ci /*=NULL*/ )
{
	NIASSERT( pRenderer != NULL );

	if( ci == NULL )
	{
		sCAMERAINFO cInfo;
		memcpy( &m_CameraInfo, &cInfo, sizeof( sCAMERAINFO ) );		
	}
	else
	{
		memcpy( &m_CameraInfo, ci, sizeof( sCAMERAINFO ) );		
	}

	// AspectRatio
	Ni2DBuffer* pkBackbuffer = pRenderer->GetDefaultBackBuffer();
	m_fAspectRatio = (float)pkBackbuffer->GetWidth() / (float)pkBackbuffer->GetHeight();

	// Setup the camera frustum and viewport
	float fViewPlaneHalfHeight = tanf( CsD2R( m_CameraInfo.s_fFOV ) * 0.5f);
	float fViewPlaneHalfWidth = fViewPlaneHalfHeight * m_fAspectRatio;

	// 원근 투영
	m_Frustum.m_fLeft = -fViewPlaneHalfWidth;
	m_Frustum.m_fRight = fViewPlaneHalfWidth;
	m_Frustum.m_fTop = fViewPlaneHalfHeight;
	m_Frustum.m_fBottom = -fViewPlaneHalfHeight;
	m_Frustum.m_fNear = m_CameraInfo.s_fNearPlane;
	m_Frustum.m_fFar = m_CameraInfo.s_fFarPlane;

	// 직교 투영
	m_OrthoFrustum.m_bOrtho = true;
	m_OrthoFrustum.m_fNear = m_CameraInfo.s_fNearPlane;
	m_OrthoFrustum.m_fFar = m_CameraInfo.s_fFarPlane;

	NiRect<float> kPort(0.0f, 1.0f, 1.0f, 0.0f);
	m_pCameraObj->SetViewFrustum(m_Frustum);
	m_pCameraObj->SetViewPort(kPort);

	m_pTargetObj->AttachChild( m_pCameraObj );

	Reset();	
}

void CsGBSubCamera::Reset( sCAMERAINFO* ci /*=NULL*/ )
{
	if( ci != NULL )
	{
		memcpy( &m_CameraInfo, ci, sizeof( sCAMERAINFO ) );
	}

	NiMatrix3 matX, matY;
	matX.MakeXRotation(-NI_HALF_PI);
	matY.MakeYRotation(-NI_HALF_PI);

	m_fCurDist = m_CameraInfo.s_fDist;
	m_fDestDist = m_fCurDist;

	m_pCameraObj->SetTranslate( NiPoint3( 0.0f, -m_fCurDist, 0.0f ) );
	m_pCameraObj->SetRotate( matX * matY );
	SetRotation( m_CameraInfo.s_fInitRoll, m_CameraInfo.s_fInitPitch );
	m_pTargetObj->SetTranslate( m_CameraInfo.s_ptInitPos );
	m_pTargetObj->Update( 0.0f );

	m_FrustumPlanes.Set( m_Frustum, m_pCameraObj->GetWorldTransform() );
}

void CsGBSubCamera::_UpdateCamera()
{
	m_pTargetObj->Update( 0.0f, false );
}

void CsGBSubCamera::SetRotation( NiMatrix3 matRotation )
{
	float fYaw;
	matRotation.ToEulerAnglesZXY( m_fCurRoll, m_fCurPitch, fYaw );
	m_fCurPitch = -m_fCurPitch;

	assert_cs( _finite( m_fCurRoll ) == TRUE );
	assert_cs( _finite( m_fCurPitch ) == TRUE );

	SetRotation( m_fCurRoll, m_fCurPitch );
}

void CsGBSubCamera::SetRotation( float fRoll, float fPich )
{
	m_fCurRoll = fRoll;
	m_fCurPitch = fPich;

	CsInFi( m_fCurRoll );
	CsInFi( m_fCurPitch );

	if( m_bRotationLimit == true )
	{
		if( -m_fCurPitch < m_fRotationLimitMin )		m_fCurPitch = -m_fRotationLimitMin;
		else if( -m_fCurPitch > m_fRotationLimitMax )	m_fCurPitch = -m_fRotationLimitMax;
	}

	NiMatrix3 matX, matZ;
	matX.MakeXRotation( -m_fCurPitch );
	matZ.MakeZRotation( m_fCurRoll );
	m_pTargetObj->SetRotate( matZ * matX );
} 

void CsGBSubCamera::SetRotation( NiPoint3 ptDir )
{
	ptDir.Unitize();
	NiPoint3 ptOrg( 0, 1, 0 );

	float fValue = ptDir.Dot( ptOrg );

	if( abs( fValue ) < 0.9998f )
	{
		NiQuaternion q;
		quatRotationArc( &q, ptOrg, ptDir );

		NiMatrix3 mat;
		q.ToRotation( mat );

		SetRotation( mat );		
	}
	else
	{
		NiMatrix3 mat;
		if( fValue < 0.0f )
		{
			mat.MakeZRotation( -NI_PI );
		}
		else
		{
			mat.MakeIdentity();
		}
		SetRotation( mat );
	}
}

void CsGBSubCamera::SetTranslate( NiPoint3 pos )
{
	pos.z += m_fTargetDeltaHeight;

	if( m_pTargetObj->GetTranslate() == pos )
		return;

	m_pTargetObj->SetTranslate( pos );
}

void CsGBSubCamera::SetDist( float fDist, bool bReleaseRemainDist /*=false*/ )
{
	if( m_bDistRange == true )
	{
		if( fDist < m_fDistMin )
			fDist = m_fDistMin;
		else if( fDist > m_fDistMax )
			fDist = m_fDistMax;
	}

	m_fCurDist = fDist;

	m_pCameraObj->SetTranslate( 0.0f, -fDist, 0.0f );
}

void CsGBSubCamera::SetDeltaDist( float fDeltaDist )
{
	if( fDeltaDist == 0.0f )
		return;

	m_fDestDist += fDeltaDist;

	if( m_bDistRange == true )
	{
		if( m_fDestDist < m_fDistMin )
			m_fDestDist = m_fDistMin;
		else if( m_fDestDist > m_fDistMax )
			m_fDestDist = m_fDistMax;
	}
}

void CsGBSubCamera::ChangeStandardFov( float fWidth, float fHeight )
{	
	m_fAspectRatio = fWidth / fHeight;	
	float fViewPlaneHalfHeight = tanf( CsD2R( m_CameraInfo.s_fFOV )  * 0.5f);
	float fViewPlaneHalfWidth = fViewPlaneHalfHeight * m_fAspectRatio;

	m_Frustum.m_fLeft = -fViewPlaneHalfWidth;
	m_Frustum.m_fRight = fViewPlaneHalfWidth;
	m_Frustum.m_fTop = fViewPlaneHalfHeight;
	m_Frustum.m_fBottom = -fViewPlaneHalfHeight;
	m_Frustum.m_fNear = m_CameraInfo.s_fNearPlane;
	m_Frustum.m_fFar = m_CameraInfo.s_fFarPlane;
	m_pCameraObj->SetViewFrustum( m_Frustum );
}

void CsGBSubCamera::quatRotationArc( NiQuaternion* p, NiPoint3 v0, NiPoint3 v1)
{
	NiPoint3 c = v0.Cross( v1 );
	float s = sqrtf( ( 1.0f+ v0.Dot( v1 ) )*2.0f);
	p->SetX( c.x/s );
	p->SetY( c.y/s );
	p->SetZ( c.z/s );
	p->SetW( s/2.0f );
}

void CsGBSubCamera::ApplyFrustumPlane( bool bOther )
{
	if( bOther == false )
	{
		m_FrustumPlanes.Set( m_Frustum, m_pCameraObj->GetWorldTransform() );
	}
	else
	{
		m_FrustumPlanes.Set( m_OrthoFrustum, m_pCameraObj->GetWorldTransform() );
	}
}

void CsGBSubCamera::SetOrthoFrustum( float fWidth, float fHeight )
{
	m_OrthoFrustum.m_fLeft = -fWidth;
	m_OrthoFrustum.m_fRight = fWidth;
	m_OrthoFrustum.m_fTop = fHeight;
	m_OrthoFrustum.m_fBottom = -fHeight;
	m_OrthoFrustum.m_fNear = m_CameraInfo.s_fNearPlane;
	m_OrthoFrustum.m_fFar = m_CameraInfo.s_fFarPlane;

	m_pCameraObj->SetViewFrustum( m_OrthoFrustum );
}

bool CsGBSubCamera::UserCulling( NiAVObject* pkObject )
{
	assert_cs( 0x3f == m_FrustumPlanes.GetActivePlaneState() );
	assert_cs( m_FrustumPlanes.IsAnyPlaneActive() == true );

	unsigned int i;
	for (i = 0; i < NiFrustumPlanes::MAX_PLANES; i++)
	{
		assert_cs( m_FrustumPlanes.IsPlaneActive(i) == true );		
		if (pkObject->GetWorldBound().WhichSide( m_FrustumPlanes.GetPlane(i)) == NiPlane::NEGATIVE_SIDE)
		{
			return false;
		}
	}
	return true;
}