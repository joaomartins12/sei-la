
inline void CsGBCamera::_UpdateCamera()
{
	m_pTargetObj->Update( 0.0f, false );
}

inline bool CsGBCamera::WindowPointToRay( int nWindowX, int nWindowY, NiPoint3& ptOrigin, NiPoint3& ptDir )
{
	return m_pCameraObj->WindowPointToRay( nWindowX, nWindowY, ptOrigin, ptDir );
}

//===========================================================================================
//
//	Fog
//
//===========================================================================================

inline float CsGBCamera::FogStartDistToFogDepth( float fRealDist )
{
	return ( m_CameraInfo.s_fFarPlane - fRealDist )/( m_CameraInfo.s_fFarPlane - m_CameraInfo.s_fNearPlane );
}


inline float CsGBCamera::FogEndDistToMaxFogFactor( float fRealDist, float fDepth )
{
	return 1.0f/( ( fRealDist - m_CameraInfo.s_fFarPlane )/( ( m_CameraInfo.s_fFarPlane - m_CameraInfo.s_fNearPlane )*fDepth) + 1.0f );
}