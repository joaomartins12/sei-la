
#include "stdafx.h"
#include "CsGBCamera.h"

CsGBCamera::CsGBCamera():m_bUsingTerrainCollision(true)
{
	m_pTargetObj = NULL;
	m_pCameraObj = NULL;
	m_pCameraObjBackup = NULL;
	m_pAttachCamera = NULL;
	m_fElapsedTime = 0.0f;

	m_bAttachActor_ByTool = false;
	m_bStaticCamera = false;

	m_CallBackKeyAutoMapping = NULL;

 	m_CurAniRoll.s_eSetting = sANI_ROTAION::NONE;	//2015-12-01-nova 입장시 카메라 자동 회전 방지

	m_spCuller = NiNew NiOcclusionCuller;
	m_bOcclusionCullingUse = true;

	for(int i = 0; i < CAMERA_CNT; ++i)
		mSubCamera[i] = NULL;
}

CsGBCamera::~CsGBCamera()
{
	for(int i = 0; i < CAMERA_CNT; ++i)
		SAFE_NIDELETE(mSubCamera[i]);

	SAFE_NIDELETE( m_spCuller );
}

void CsGBCamera::Delete()
{
	RestOcclusionGeometry();
	_DeleteCollision();

	m_pCameraObj = 0;
	m_pTargetObj = 0;

	DeleteDetachCamera();
}

void CsGBCamera::SetUseOcclusionCulling( bool bValue )
{
	m_bOcclusionCullingUse = bValue;
}

void CsGBCamera::SetShowOccluder( bool bValue )
{
	if( m_spCuller )
		m_spCuller->ShowOccluder( bValue );
}

void CsGBCamera::RestOcclusionGeometry()
{
	if( m_spCuller )
		m_spCuller->RemoveAllOcclusionGeometry();
}

void CsGBCamera::AddOcclusionGeometry(NiNode* pSceneNode)
{
	if( NULL == pSceneNode )
		return;

	if( m_spCuller )
		m_spCuller->AddOcclusionGeometry(pSceneNode);
}


void CsGBCamera::FindOcclusionGeometry( NiNode* pSceneNode )
{
	if( NULL == pSceneNode )
		return;

	if( m_spCuller )
		m_spCuller->FindOcclusionGeometry(pSceneNode);
}

void CsGBCamera::Create( NiDX9Renderer* pRenderer, sCAMERAINFO* ci /*=NULL*/ )
{
	NIASSERT( pRenderer != NULL );

	for(int i = 0; i < CAMERA_CNT; ++i)
	{
		mSubCamera[i] = NiNew CsGBSubCamera();
		
		if(i != 0)
		{
			mSubCamera[i]->Create(pRenderer);
		}
	}

	if( ci == NULL )
	{
		sCAMERAINFO cInfo;
		memcpy( &m_CameraInfo, &cInfo, sizeof( sCAMERAINFO ) );		
	}
	else
	{
		memcpy( &m_CameraInfo, ci, sizeof( sCAMERAINFO ) );		
	}
	assert_cs( m_pTargetObj == NULL );
	assert_cs( m_pCameraObj == NULL );
	m_pTargetObj = NiNew NiNode;
	m_pCameraObj = NiNew NiCamera;

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

	_CreateCollision();

	Reset();	
}

void CsGBCamera::Reset( sCAMERAINFO* ci /*=NULL*/ )
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

	_ResetControl();
}

void CsGBCamera::_ResetControl( bool bAccelReset /*=true*/ )
{
	if( bAccelReset == true )
	{
		m_fDAAConstant = CAMERA_DIST_ANI_ACCEL_CONSTANT;
		SetDistAccel( 1.5f );
		SetTranslateAccel( 0.0f );
		SetRotationAccel( 0.0035f );
	}

	m_fTargetDeltaHeight = 0.0f;
	m_bLBtnDown = false;
	m_bMBtnDown = false;
	m_bRBtnDown = false;

	m_fRemainDist = 0.0f;
	m_fDestDist = 0.0f;
	m_bDestDistInit = true;

	m_bDistRange = false;
	m_bRotationLimit = false;

	m_bTerrainCollision = false;
	SetUsingTerrainCollition(true);

	ReleaseDistRange();
}

void CsGBCamera::SetCameraDist( float fDistRange )
{
	m_fDestDist = fDistRange;

	if( m_fDestDist < m_fDistMin )
		m_fDestDist = m_fDistMin;
	else if( m_fDestDist > m_fDistMax )
		m_fDestDist = m_fDistMax;
}


void CsGBCamera::SetUsingTerrainCollition( bool bValue )
{
	m_bUsingTerrainCollision = bValue;
}

void CsGBCamera::Update( float fElapsedTime )
{
	m_fElapsedTime = fElapsedTime;	

	if( IsAttachCamera() == false )
	{
		 _CalCollision();

		//_UpdateTerrainCollision();
		_UpdateDistAni( fElapsedTime );
		//_UpdateStaticCamera();

		_UpdateAutoDirection( fElapsedTime );	

		_UpdateCamera();
		_UpdateObjectPick( fElapsedTime );	
	}
	else
	{
		if( m_pAttachCamera->GetCurApplyCamera()->s_bOwner == false )
		{
			_CalCollision();

			//_UpdateTerrainCollision();
			_UpdateDistAni( fElapsedTime );
			//_UpdateStaticCamera();

			_UpdateAutoDirection( fElapsedTime );	

			_UpdateCamera();
			_UpdateObjectPick( fElapsedTime );

			m_pAttachCamera->Update( fElapsedTime, m_pCameraObj );
		}
		else
		{
			m_pAttachCamera->Update( fElapsedTime, NULL );
		}
	}	

	if( m_spCuller )
		m_spCuller->Update( m_pCameraObj );
}

void CsGBCamera::Update_ByTool( float fElapsedTime, bool bUpdateTargetDist_ByTool /*=true*/ )
{
	if( m_bAttachActor_ByTool == true )
	{
		_UpdateTerrainCollision_ByTool();		
		_UpdateDistAni( fElapsedTime );
		_UpdateAutoDirection( fElapsedTime );			
		_UpdateCamera();
		_UpdateObjectPick( fElapsedTime );
	}
	else
	{
		_UpdateDistAni_ByTool( fElapsedTime );		
		_UpdateCamera();
// 		{
// 			if( bUpdateTargetDist_ByTool == true )
// 				_UpdateTargetDist_ByTool();
// 
// 			_UpdateCamera();			
// 		}
	}

	if( m_spCuller )
		m_spCuller->Update( m_pCameraObj );
}

void CsGBCamera::SetAttachActor_ByTool( bool bAttach )
{
	m_bAttachActor_ByTool = bAttach;
	if( m_bAttachActor_ByTool == false )
	{
		m_bTerrainCollision = false;

		CsGBObjProp* pProp = NULL;
		// 투명되어 있는거 전부 캔슬
		NiPoint3 ptCamera = GetWorldTranslate();
		NiPoint3 ptTarget = m_pTargetObj->GetWorldTranslate();
		CsGBTerrainLeaf* pCameraLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( ptCamera );
		CsGBTerrainLeaf* pTargetLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( ptTarget );
		MAP_GBTERRAIN_OBJECT* pMapObject = NULL;
		MAP_GBTERRAIN_OBJECT_IT itObject;

		pMapObject = pCameraLeaf->GetObjectMap();
		itObject = pMapObject->begin();
		for( ; itObject != pMapObject->end(); ++itObject )
		{
			pProp = itObject->second->GetProp( CsGBObjProp::PROP_ALPHA );
			if( pProp != NULL )
				( ( CsGBObjProp_Alpha* )pProp )->SetAlphaBlend( false );
		}
		if( pCameraLeaf != pTargetLeaf )
		{
			pMapObject = pTargetLeaf->GetObjectMap();
			itObject = pMapObject->begin();
			for( ; itObject != pMapObject->end(); ++itObject )
			{
				pProp = itObject->second->GetProp( CsGBObjProp::PROP_ALPHA );
				if( pProp != NULL )
					( ( CsGBObjProp_Alpha* )pProp )->SetAlphaBlend( false );
			}
		}
	}
	m_fRemainDist = 0.0f;
}

void CsGBCamera::_CalCollision()
{
	m_bTerrainCollision = false;
	m_fTerrainCollitionDist = 0.0f;

	if( !m_bUsingTerrainCollision )
		return;

	#define CAMERA_TERRAIN_HEIGHT			30.0f
	NiPoint3 ptCamera = m_pCameraObj->GetWorldTranslate();
	NiPoint3 ptTarget = m_pTargetObj->GetTranslate();
	ptTarget.z += CAMERA_TERRAIN_HEIGHT;
	NiPoint3 dir = ptCamera - ptTarget;
	dir.Unitize();
	ptTarget.z -= CAMERA_TERRAIN_HEIGHT;


	float fDoorNearDist = 0.0f;
	bool bDoorPick = false;
	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_ActorObj( "door", ptTarget, dir, m_fDestDist );
	if( pRecord )
	{
		fDoorNearDist = pRecord->GetDistance();
		bDoorPick = true;
	}

	float fObjNearDist = 0.0f;
	bool bObjPick = false;
	pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_CAP( ptTarget, dir );
	if( pRecord )
	{
		if( pRecord->GetDistance() <= m_fDestDist )
		{
			fObjNearDist = pRecord->GetDistance();
			bObjPick = true;
		}
	}
	
	if( bDoorPick && bObjPick )
	{
		m_fTerrainCollitionDist = __min( fDoorNearDist, fObjNearDist );
		m_bTerrainCollision = true;
	}
	else if( bDoorPick )
	{
		m_fTerrainCollitionDist = fDoorNearDist;
		m_bTerrainCollision = true;
	}
	else if( bObjPick )
	{
		m_fTerrainCollitionDist = fObjNearDist;
		m_bTerrainCollision = true;
	}
}

// void CsGBCamera::_UpdateTerrainCollision()
// {
// 	if( !m_bTerrainCollision )
// 		return;
// 
// 	SetDist( m_fTerrainCollitionDist );
//}

void CsGBCamera::_UpdateDistAni( float fElapsedTime )
{
	if( m_bTerrainCollision )
	{
		if( m_fCurDist == m_fTerrainCollitionDist )
			return;

		if( m_fCurDist > m_fTerrainCollitionDist )
			SetDist( m_fTerrainCollitionDist );
		else
		{
			float fDeltaDist = (fElapsedTime*m_fDistAniAccel);
			m_fCurDist -= fDeltaDist;
			if( m_fCurDist < m_fTerrainCollitionDist )
				m_fCurDist = m_fTerrainCollitionDist;
			m_pCameraObj->SetTranslate( 0.0f, -m_fCurDist, 0.0f );
		}		
		return;	
	}

	if( m_fCurDist == m_fDestDist )
		return;

	float fDeltaDist = (fElapsedTime*m_fDistAniAccel);
	if( m_fCurDist - m_fDestDist > 0 )
	{
		m_fCurDist -= fDeltaDist;
		if( m_fCurDist < m_fDestDist )
			m_fCurDist = m_fDestDist;
	}
	else
	{
		m_fCurDist += fDeltaDist;
		if( m_fCurDist > m_fDestDist )
			m_fCurDist = m_fDestDist;
	}

	if( m_fCurDist < m_fDistMin )
		m_fCurDist = m_fDistMin;
	else if( m_fCurDist > m_fDistMax )
		m_fCurDist = m_fDistMax;

	m_pCameraObj->SetTranslate( 0.0f, -m_fCurDist, 0.0f );
}

void CsGBCamera::_UpdateDistAni_ByTool( float fElapsedTime )
{

	if( m_fCurDist == m_fDestDist )
		return;

	float fDeltaDist = (fElapsedTime*m_fDistAniAccel);
	if( m_fCurDist - m_fDestDist > 0 )
	{
		m_fCurDist -= fDeltaDist;
		if( m_fCurDist < m_fDestDist )
			m_fCurDist = m_fDestDist;
	}
	else
	{
		m_fCurDist += fDeltaDist;
		if( m_fCurDist > m_fDestDist )
			m_fCurDist = m_fDestDist;
	}

	if( m_fCurDist < m_fDistMin )
		m_fCurDist = m_fDistMin;
	else if( m_fCurDist > m_fDistMax )
		m_fCurDist = m_fDistMax;

	m_pCameraObj->SetTranslate( 0.0f, -m_fCurDist, 0.0f );
}

//void CsGBCamera::_UpdateStaticCamera()
//{
//	if( m_bStaticCamera == false )
//		return;
//
//	// 거리에 따른 비율값 산출
//	float fTotalDist = m_fDistMax - m_fDistMin;
//	float fCurDist = m_fCurDist - m_fDistMin;
//	float fRate = fCurDist/fTotalDist;
//
//	// 피치 맥스값
//	float fPitchMax = CsD2R( -35 );
//	// 피치 최하값
//	float fPitchMin = CsD2R( 5 );
//
//	SetRotation( GetCurRoll(), fRate*( fPitchMax - fPitchMin ) + fPitchMin );
//}


void CsGBCamera::_UpdateTerrainCollision_ByTool()
{
#define CAMERA_TERRAIN_HEIGHT			30.0f
	NiPoint3 ptCamera = m_pCameraObj->GetWorldTranslate();
	NiPoint3 ptTarget = m_pTargetObj->GetTranslate();
	ptTarget.z += CAMERA_TERRAIN_HEIGHT;
	NiPoint3 dir = ptCamera - ptTarget;
	dir.Unitize();
	ptTarget.z -= CAMERA_TERRAIN_HEIGHT;

	m_bTerrainCollision = false;
	m_fTerrainCollitionDist = 0.0f;

	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_CAP_ByTool( ptTarget, dir );
	if( pRecord == NULL )
		return;

	m_fTerrainCollitionDist = pRecord->GetDistance();
	if( m_fTerrainCollitionDist <= m_fDestDist )
		m_bTerrainCollision = true;
// 
// 	NiPoint3 ptCamera = GetWorldTranslate();
// 	if( nsCsGBTerrain::g_pCurRoot->IsInTerrain( ptCamera.x, ptCamera.y ) == false )
// 		return;
// 
// 	float fTHeight = nsCsGBTerrain::g_pCurRoot->GetHeight( ptCamera.x, ptCamera.y );
// 
// 	m_bTerrainCollision = false;
// 	if( ptCamera.z < fTHeight + CAMERA_TERRAIN_HEIGHT )
// 	{
// 		NiPoint3 ptTarget = m_pTargetObj->GetWorldTranslate();
// 		ptTarget.z += CAMERA_TERRAIN_HEIGHT;
// 		NiPoint3 dir = ptCamera - ptTarget;
// 		dir.Unitize();
// 		ptTarget.z -= CAMERA_TERRAIN_HEIGHT;
// 
// 		NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_CAP_ByTool( ptTarget, dir );
// 		if( pRecord == NULL )
// 			return;
// 
// 		m_fTerrainCollitionDist = pRecord->GetDistance();
// 		if( m_fTerrainCollitionDist <= m_fDestDist )
// 			m_bTerrainCollision = true;
// 	}
// 	else
// 	{
// 		NiPoint3 ptTarget = m_pTargetObj->GetWorldTranslate();
// 		NiPoint3 dir = ptCamera - ptTarget;
// 		dir.Unitize();
// 
// 		NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_CAP_ByTool( ptTarget, dir );
// 		if( pRecord == NULL )
// 			return;		
// 
// 		m_fTerrainCollitionDist = pRecord->GetDistance();
// 		if( m_fTerrainCollitionDist <= m_fDestDist )
// 			m_bTerrainCollision = true;
// 	}
}

void CsGBCamera::_UpdateObjectPick( float fElapsedTime )
{
#define BV_TEST_INTERSECT	( (CsGBObjProp_ABV*)pObject->GetProp( CsGBObjProp::PROP_ABV ) )->TestIntersect( fElapsedTime, pABV )

	NiPoint3 ptCamera = GetWorldTranslate();
	NiPoint3 ptTarget = m_pTargetObj->GetWorldTranslate();
	NiPoint3 dir = ptTarget - ptCamera;
	dir.Unitize();
	NiPoint3 ptCenter = ( ptCamera + ptTarget )*0.5f;
	float fHlafDist = m_fCurDist*0.5f;

	m_pBoundCapsule->SetExtent( fHlafDist );
	m_pBoundCapsule->SetAxis( dir );
	m_pBoundNode->SetTranslate( ptCenter );
	m_pBoundNode->Update( 0.0f );

	CsGBTerrainLeaf* pCameraLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( ptCamera );
	CsGBTerrainLeaf* pTargetLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( ptTarget );
	MAP_GBTERRAIN_OBJECT* pCAMapObject = NULL;
	MAP_GBTERRAIN_OBJECT_IT itObject;
	CsGBObject* pObject;
	CsGBObject::eCOLLISION_TYPE eCollisionType;
	CsGBPick pick( false, NiPick::NO_SORT, false );
	NiPoint3 originPick = GetWorldTranslate();
	NiPoint3 dirPick = GetWorldDirection();
	NiPick::Record* pRecord = NULL;

	NiBoundingVolume* pABV = NiGetCollisionData( m_pBoundNode )->GetWorldSpaceABV();

	pCAMapObject = pCameraLeaf->GetCameraAlphaObjMap();	
	itObject = pCAMapObject->begin();
	for( ; itObject != pCAMapObject->end(); ++itObject )
	{
		pObject = itObject->second;
		assert_cs( pObject->UDF_IsCameraAlpha() == true );
		eCollisionType = pObject->InBound( ptCenter, fHlafDist );
		if( eCollisionType == CsGBObject::CT_IN_BOUND )
		{
			if( pObject->UD_IsCameraAlpha() == true )
			{
				( (CsGBObjProp_Alpha*)pObject->GetProp( CsGBObjProp::PROP_ALPHA ) )->SetAlphaBlend( BV_TEST_INTERSECT );
			}
			else
			{
				assert_cs( pObject->UD_IsCameraAlphaPick() == true );
				pObject->PropAvCap_Pick( &pick, originPick, dirPick, false );				
				pRecord = pick.GetRecord();
				( (CsGBObjProp_Alpha*)pObject->GetProp( CsGBObjProp::PROP_ALPHA ) )->SetAlphaBlend( 
					( pRecord != NULL )&&( pRecord->GetDistance() < m_fCurDist ) );
			}
		}
		else if( eCollisionType == CsGBObject::CT_OUT_BOUND )
		{
			( (CsGBObjProp_Alpha*)pObject->GetProp( CsGBObjProp::PROP_ALPHA ) )->SetAlphaBlend( false );
		}
	}
	if( pCameraLeaf != pTargetLeaf )
	{
		pCAMapObject = pTargetLeaf->GetCameraAlphaObjMap();
		itObject = pCAMapObject->begin();		
		for( ; itObject != pCAMapObject->end(); ++itObject )
		{
			pObject = itObject->second;
			assert_cs( pObject->UDF_IsCameraAlpha() == true );
			eCollisionType = pObject->InBound( ptCenter, fHlafDist );
			if( eCollisionType == CsGBObject::CT_IN_BOUND )
			{
				if( pObject->UD_IsCameraAlpha() == true )
				{
					( (CsGBObjProp_Alpha*)pObject->GetProp( CsGBObjProp::PROP_ALPHA ) )->SetAlphaBlend( BV_TEST_INTERSECT );
				}
				else
				{
					assert_cs( pObject->UD_IsCameraAlphaPick() == true );
					pObject->PropAvCap_Pick( &pick, originPick, dirPick, false );
					pRecord = pick.GetRecord();
					( (CsGBObjProp_Alpha*)pObject->GetProp( CsGBObjProp::PROP_ALPHA ) )->SetAlphaBlend( 
						( pRecord != NULL )&&( pRecord->GetDistance() < m_fCurDist ) );					
				}
			}
			else if( eCollisionType == CsGBObject::CT_OUT_BOUND )
			{
				( (CsGBObjProp_Alpha*)pObject->GetProp( CsGBObjProp::PROP_ALPHA ) )->SetAlphaBlend( false );
			}
		}
	}
}

void CsGBCamera::_UpdateTargetDist_ByTool()
{
#define TARGET_MIN_DIST		2500.0f

	if( m_fCurDist < TARGET_MIN_DIST )
	{
		_UpdateCamera();
		NiPoint3 ptTarget = m_pTargetObj->GetWorldTranslate();
		ptTarget += GetWorldDirection()*( TARGET_MIN_DIST - m_fCurDist );
		m_pTargetObj->SetTranslate( ptTarget );

		m_fCurDist = TARGET_MIN_DIST;
		m_pCameraObj->SetTranslate( 0, -m_fCurDist, 0 );
	}
}

void CsGBCamera::_UpdateAutoDirection( float fElapsedTime )
{
	if( m_CurAniRoll.GetSetting() == sANI_ROTAION::NONE )
		return;

	if( (m_CurAniRoll.s_fActiveTime -= fElapsedTime) < 0.0f )
	{
		m_CurAniRoll.s_eSetting = sANI_ROTAION::ACTIVE;			
	}

	if( m_CurAniRoll.GetSetting() != sANI_ROTAION::ACTIVE )
		return;

	float fSrc = m_fCurRoll;
	float fDest = m_CurAniRoll.s_fAniRotation;
	float fSpeed = m_fAutoRotSpeed*fElapsedTime;
	float fDelta = ( fDest - fSrc );
	CsInFi( fDelta );

	if( fDelta >= 0.0f )
	{				
		float fRot = fSpeed;
		if( fRot > fDelta )
		{
			fRot = fDelta;
			m_CurAniRoll.s_eSetting = sANI_ROTAION::NONE;
		}

		SetDeltaRotation( fRot, 0.0f );
	}
	else
	{
		float fRot = -fSpeed;
		if( fRot < fDelta )
		{
			fRot = fDelta;
			m_CurAniRoll.s_eSetting = sANI_ROTAION::NONE;
		}

		SetDeltaRotation( fRot, 0.0f );
	}

	if( m_CallBackKeyAutoMapping )
		m_CallBackKeyAutoMapping();
}

//===========================================================================================
//
//	Info
//
//===========================================================================================
void CsGBCamera::SaveCameraState()
{
	_UpdateCamera();

	m_BackupCameraInfo.s_fDist = -m_pCameraObj->GetTranslate().y;
	m_BackupCameraInfo.s_fInitRoll = m_fCurRoll;
	m_BackupCameraInfo.s_fInitPitch = m_fCurPitch;
	m_BackupCameraInfo.s_ptInitPos = m_pTargetObj->GetWorldTranslate();
	m_BackupCameraInfo.s_fFOV = m_CameraInfo.s_fFOV;
	
	m_fBackupTargetDeltaHeight = m_fTargetDeltaHeight;
	m_bBackupRotationLimit = m_bRotationLimit;
	m_bBackupDistRange = m_bDistRange;
	m_fBackupAspectRatio = m_fAspectRatio;
}

void CsGBCamera::LoadCameraState()
{
	m_fTargetDeltaHeight = m_fBackupTargetDeltaHeight;
	m_bRotationLimit = m_bBackupRotationLimit;
	m_bDistRange = m_bBackupDistRange;

	m_fCurDist = m_BackupCameraInfo.s_fDist;
	m_fDestDist = m_fCurDist;
	m_pCameraObj->SetTranslate( 0.0f, -m_fCurDist, 0.0f );
	SetRotation( m_BackupCameraInfo.s_fInitRoll, m_BackupCameraInfo.s_fInitPitch );
	m_pTargetObj->SetTranslate( m_BackupCameraInfo.s_ptInitPos );

	if( m_fAspectRatio != m_fBackupAspectRatio )
	{
		m_fAspectRatio = m_fBackupAspectRatio;
		ChangeStandardFov( m_BackupCameraInfo.s_fFOV );
	}
	else if( m_CameraInfo.s_fFOV != m_BackupCameraInfo.s_fFOV )
	{
		ChangeStandardFov( m_BackupCameraInfo.s_fFOV );
	}

	_UpdateCamera();
}

//===========================================================================================
//
//	Frustum
//
//===========================================================================================
void CsGBCamera::SetStandardFrustum()
{
	m_Frustum.m_fFar = m_CameraInfo.s_fFarPlane;
	m_pCameraObj->SetViewFrustum( m_Frustum );
}

void CsGBCamera::SetFarFrustumPlane( float fFar )
{
	m_Frustum.m_fFar = fFar;
	m_FrustumPlanes.Set( m_Frustum, m_pCameraObj->GetWorldTransform() );
}

void CsGBCamera::SetDefaultFarFrustumPlane()
{
	m_Frustum.m_fFar = m_CameraInfo.s_fFarPlane;	
	m_FrustumPlanes.Set( m_Frustum, m_pCameraObj->GetWorldTransform() );
}

bool CsGBCamera::SetFarFrustum( float fFar )
{
	if( m_pCameraObj->GetViewFrustum().m_fFar != fFar )
	{
		( (NiFrustum&)( m_pCameraObj->GetViewFrustum()) ).m_fFar = fFar;
		return true;
	}
	return false;
}

bool CsGBCamera::SetDefaultFarFrustum()
{
	if( m_pCameraObj->GetViewFrustum().m_fFar != m_CameraInfo.s_fFarPlane )
	{
		( (NiFrustum&)( m_pCameraObj->GetViewFrustum()) ).m_fFar = m_CameraInfo.s_fFarPlane;
		return true;
	}
	return false;
}

void CsGBCamera::ChangeStandardFov( float fWidth, float fHeight, float fFOV )
{
	m_CameraInfo.s_fFOV = fFOV;
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


void CsGBCamera::ChangeStandardFov( float fWidth, float fHeight )
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

	for(int i = 0; i < CAMERA_CNT; ++i)
	{
		mSubCamera[i]->ChangeStandardFov(fWidth, fHeight);
	}
}

void CsGBCamera::ChangeStandardFov( float fFOV )
{
	m_CameraInfo.s_fFOV = fFOV;

	float fAspectRatio = GetAspectRatio();
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

void CsGBCamera::SetOrthoFrustum( float fHeightSize )
{
	float fWidth = fHeightSize*m_fAspectRatio;
	m_OrthoFrustum.m_fLeft = -fWidth;
	m_OrthoFrustum.m_fRight = fWidth;
	m_OrthoFrustum.m_fTop = fHeightSize;
	m_OrthoFrustum.m_fBottom = -fHeightSize;
	m_OrthoFrustum.m_fNear = m_CameraInfo.s_fNearPlane;
	m_OrthoFrustum.m_fFar = m_CameraInfo.s_fFarPlane;

	m_pCameraObj->SetViewFrustum( m_OrthoFrustum );
}

void CsGBCamera::SetOrthoFrustum( float fWidth, float fHeight )
{
	m_OrthoFrustum.m_fLeft = -fWidth;
	m_OrthoFrustum.m_fRight = fWidth;
	m_OrthoFrustum.m_fTop = fHeight;
	m_OrthoFrustum.m_fBottom = -fHeight;
	m_OrthoFrustum.m_fNear = m_CameraInfo.s_fNearPlane;
	m_OrthoFrustum.m_fFar = m_CameraInfo.s_fFarPlane;

	m_pCameraObj->SetViewFrustum( m_OrthoFrustum );
}

void CsGBCamera::ApplyFrustumPlane( bool bOther )
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

bool CsGBCamera::UserCulling( NiAVObject* pkObject )
{
	assert_cs( 0x3f == m_FrustumPlanes.GetActivePlaneState() );
	assert_cs( m_FrustumPlanes.IsAnyPlaneActive() == true );

	if( NULL == pkObject || pkObject->GetAppCulled() )
		return false;

	unsigned int i;
	for (i = 0; i < NiFrustumPlanes::MAX_PLANES; i++)
	{
		assert_cs( m_FrustumPlanes.IsPlaneActive(i) == true );		
		if (pkObject->GetWorldBound().WhichSide( m_FrustumPlanes.GetPlane(i)) == NiPlane::NEGATIVE_SIDE)
		{
			return false;
		}
	}

	if( m_spCuller && m_bOcclusionCullingUse )
	{
		if( m_spCuller->IsCulling( m_pCameraObj, pkObject ) )
			return false;
	}

	return true;
}

//===========================================================================================
//
//	Collision
//
//===========================================================================================
void CsGBCamera::_DeleteCollision()
{
	m_pBoundNode = 0;
}

void CsGBCamera::_CreateCollision()
{
	m_pBoundNode = NiNew NiNode;

	m_pBoundCapsule = NiNew NiCapsuleBV;
	m_pBoundCapsule->SetRadius( 1.0f );
	m_pBoundCapsule->SetCenter( NiPoint3::ZERO );

	NiCollisionData* pCollision = NiNew NiCollisionData( m_pBoundNode );
	pCollision->SetPropagationMode( NiCollisionData::PROPAGATE_ON_SUCCESS );
	pCollision->SetCollisionMode( NiCollisionData::USE_ABV );
	pCollision->SetModelSpaceABV( m_pBoundCapsule );

	m_pBoundNode->SetCollisionObject( pCollision );

	NiTransform tr;
	tr.MakeIdentity();
	m_pBoundNode->SetLocalTransform( tr );
	m_pBoundNode->UpdateProperties();
	m_pBoundNode->Update( 0.0f );
}

void CsGBCamera::SetDistRange( float fMin, float fMax )
{
	m_bDistRange = true;

	assert_cs( fMin < fMax );
	m_fDistMin = fMin;
	m_fDistMax = fMax;
	SetDist( m_fCurDist );	//2015-12-29-nova 문제 없으면 삭제
//	SetDist( m_fDistMax );
	m_fDestDist = m_fCurDist;
}

void CsGBCamera::SetDist( float fDist, bool bReleaseRemainDist /*=false*/ )
{
	if( m_bDistRange == true )
	{
		if( fDist < m_fDistMin )
			fDist = m_fDistMin;
		else if( fDist > m_fDistMax )
			fDist = m_fDistMax;
	}

	if( bReleaseRemainDist == true )
		m_fRemainDist = 0.0f;

	m_fCurDist = fDist;

	m_pCameraObj->SetTranslate( 0.0f, -fDist, 0.0f );

//	return m_fCurDist - fDist;
}

void CsGBCamera::SetDeltaDist( float fDeltaDist )
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

float CsGBCamera::GetDistLevel( int nLevel )
{
	float fMaxDistRange = 1500.0f;
	switch( nLevel )
	{
	case 0:		fMaxDistRange = 600.0f;			break;
	case 1:		fMaxDistRange = 1000.0f;		break;
	case 2:		fMaxDistRange = 1250.0f;		break;
	case 3:		fMaxDistRange = 1500.0f;		break;
	case 4:		fMaxDistRange = 2000.0f;		break;
	case 5:		fMaxDistRange = 2500.0f;		break;
	case 6:		fMaxDistRange = 4000.0f;		break;
	case 7:		fMaxDistRange = 5000.0f;		break;
	case 8:		fMaxDistRange = 6000.0f;		break;
	case 9:		fMaxDistRange = 7000.0f;		break;
	case 10:	fMaxDistRange = 8000.0f;		break;
	case 11:	fMaxDistRange = 9000.0f;		break;
	case 12:	fMaxDistRange = 10000.0f;		break;
	default:	assert_cs( false );
	}

	return fMaxDistRange;
}

void CsGBCamera::SetDistLevel( int nLevel, bool bSetDist )
{
	m_nDistLevel = nLevel;

	float fMaxDist = CAMERA_ST.GetDistLevel( nLevel );
	CAMERA_ST.SetDistRange( 250.0f, fMaxDist );
	if( bSetDist )
	{
		CAMERA_ST.SetDist( fMaxDist, true );
	}
}

void CsGBCamera::_SetDeltaTranslate_OnCamera( float fCross, float fUp )
{
	NiPoint3 Pos = m_pTargetObj->GetTranslate();
	Pos.z -= m_fTargetDeltaHeight;
	NiTransform tr = m_pTargetObj->GetWorldTransform();	
	if( fCross != 0.0f )
	{
		NiPoint3 vRes = tr.m_Rotate*NiPoint3( 1.0f, 0.0f, 0.0f );
		Pos += vRes*fCross;
	}
	if( fUp != 0.0f )
	{
		NiPoint3 vRes = tr.m_Rotate*NiPoint3( 0.0f, 0.0f, 1.0f );
		Pos += vRes*fUp;
	}
	Pos.z += m_fTargetDeltaHeight;
	m_pTargetObj->SetTranslate( Pos );
}

void CsGBCamera::SetTranslate( NiPoint3 pos )
{
	pos.z += m_fTargetDeltaHeight;
	if( m_pTargetObj->GetTranslate() == pos )
		return;
	
	m_pTargetObj->SetTranslate( pos );
}

void CsGBCamera::SetDeltaTranslate( NiPoint3 pos )
{
	pos = m_pTargetObj->GetTranslate() + pos;
	m_pTargetObj->SetTranslate( pos );
}

void CsGBCamera::SetRotationLimit( float fMin, float fMax )
{
	m_bRotationLimit = true;
	m_fRotationLimitMin = fMin;
	m_fRotationLimitMax = fMax;
}

void CsGBCamera::SetRotation( NiMatrix3 matRotation )
{
	float fYaw;
	matRotation.ToEulerAnglesZXY( m_fCurRoll, m_fCurPitch, fYaw );
	m_fCurPitch = -m_fCurPitch;

	assert_cs( _finite( m_fCurRoll ) == TRUE );
	assert_cs( _finite( m_fCurPitch ) == TRUE );

	SetRotation( m_fCurRoll, m_fCurPitch );
}

void CsGBCamera::SetRotation( float fRoll, float fPich )
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

/// 벡터 v0에서 v1까지 호를 그리는 회전 사원수
/// 주의: v0, v1는 단위 벡터
void quatRotationArc( NiQuaternion* p, NiPoint3 v0, NiPoint3 v1)
{
	NiPoint3 c = v0.Cross( v1 );
	float s = sqrtf( ( 1.0f+ v0.Dot( v1 ) )*2.0f);
	p->SetX( c.x/s );
	p->SetY( c.y/s );
	p->SetZ( c.z/s );
	p->SetW( s/2.0f );
}

void CsGBCamera::SetRotation( NiPoint3 ptDir )
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

void CsGBCamera::SetDeltaRotation( float fRoll, float fPich )
{	
	m_fCurRoll += fRoll;
	m_fCurPitch += fPich;
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

void CsGBCamera::SetDirection_FormDirectionLight( NiDirectionalLight* pDirectionLight )
{
	//// 카메라 기준과 라이트 기준을 맞춰준다.
	//NiMatrix3 matZRot;
	//matZRot.MakeZRotation( NI_HALF_PI );
	//// 회전
	//SetRotation( nsCsGBTerrain::g_pCurRoot->GetLight_Main()->GetRotate()*matZRot );
}

//=======================================================================================================================
//
//	Ani Rotation
//
//=======================================================================================================================

void CsGBCamera::SetAniRotation_Roll( float fActiveTime, float fRot, float fSpeed )
{
	assert_cs( fRot <= NI_TWO_PI );
	assert_cs( fRot >= -NI_TWO_PI );

	m_fAutoRotSpeed = fSpeed;
	if( abs( GetCurRoll() - fRot ) > 0.01f )
	{
		m_CurAniRoll.Set( fActiveTime, fRot );
	}
}

//=======================================================================================================================
//
//	Attach Camera
//
//=======================================================================================================================

void CsGBCamera::AttachCamera( char* cName, bool bOwner )
{
	if( m_pAttachCamera == NULL )
	{
		m_pAttachCamera = NiNew CsGBAttachCamera;
		m_pAttachCamera->Init();
	}
	m_pAttachCamera->AttachCamera( cName, bOwner );
}

void CsGBCamera::DeleteDetachCamera()
{
	SAFE_NIDELETE( m_pAttachCamera );
}


void CsGBCamera::_SetRotationOnMouse( POINT dest )
{
	if( ( dest.x == m_ptRBtnDown.x )&&( dest.y == m_ptRBtnDown.y ) )
		return;

	NiPoint2 v		= NiPoint2( (dest.x - m_ptRBtnDown.x)*m_fRotationAccel, (m_ptRBtnDown.y - dest.y)*m_fRotationAccel );
	m_ptRBtnDown	= dest;

	SetDeltaRotation( v.x, v.y );
}

void CsGBCamera::_SetTranslateOnMouse( POINT dest )
{
	if( ( dest.x == m_ptMBtnDown.x )&&( dest.y == m_ptMBtnDown.y ) )
		return;

	NiPoint2 v		= NiPoint2( (m_ptMBtnDown.x-dest.x)*m_fTranslateAccel, (dest.y - m_ptMBtnDown.y)*m_fTranslateAccel );
	m_ptMBtnDown	= dest;

	_SetDeltaTranslate_OnCamera( v.x, v.y );
}

void CsGBCamera::_SetTranslateOnMouse( POINT dest, float fAccel )
{
	if( ( dest.x == m_ptMBtnDown.x )&&( dest.y == m_ptMBtnDown.y ) )
		return;

	NiPoint2 v		= NiPoint2( (m_ptMBtnDown.x-dest.x)*fAccel, (dest.y - m_ptMBtnDown.y)*fAccel );
	m_ptMBtnDown	= dest;

	_SetDeltaTranslate_OnCamera( v.x, v.y );
}

//=======================================================================================================
//
//		Input
//
//=======================================================================================================
void CsGBCamera::OnLButtonDown( POINT pt )
{
	m_bLBtnDown = true;
	m_ptLBtnDown = pt;
}

void CsGBCamera::OnLButtonUp( POINT pt )
{
	m_bLBtnDown = false;
}

void CsGBCamera::OnMButtonDown( POINT pt )
{
	m_bMBtnDown = true;
	m_ptMBtnDown = pt;
}

void CsGBCamera::OnMButtonUp( POINT pt )
{
	m_bMBtnDown = false;
}

void CsGBCamera::OnRButtonDown( POINT pt )
{
	m_bRBtnDown		=	true;
	m_ptRBtnDown	=	pt;	
}

void CsGBCamera::OnRButtonUp( POINT pt )
{
	m_bRBtnDown		=	false;
}

void CsGBCamera::OnMouseMove( POINT pt )
{
	if( m_bRBtnDown == true )
	{
		_SetRotationOnMouse( pt );
	}
	else if( m_bMBtnDown == true )
	{
		_SetTranslateOnMouse( pt );
	}
}

void CsGBCamera::OnMouseMove( POINT pt, float fAccel )
{
	if( m_bRBtnDown == true )
	{
		_SetRotationOnMouse( pt );
	}
	else if( m_bMBtnDown == true )
	{
		_SetTranslateOnMouse( pt, fAccel );
	}
}


void CsGBCamera::OnMouseWheel( short zDelta )
{
	SetDirectDist( -zDelta* m_fDistAccel );
}

void CsGBCamera::SetDirectDist( float zDelta )
{
	if( zDelta == 0.0f )
		return;

	m_fCurDist += zDelta;

	if( m_fCurDist < m_fDistMin )
		m_fCurDist = m_fDistMin;
	else if( m_fCurDist > m_fDistMax )
		m_fCurDist = m_fDistMax;

	SetDist( m_fCurDist, true );

	m_fDestDist = m_fCurDist;
}

void CsGBCamera::OnRButtonPressed( int x, int y )
{
	ReleaseAniRotation_Roll();

	assert_cs( ( x != 0 )||( y != 0 ) );
	SetDeltaRotation( x*m_fRotationAccel, -y*m_fRotationAccel );

	CAMERA_ST._UpdateCamera();
}

void CsGBCamera::ChangeCameraObj( NiCamera* pObj )
{ 
	m_pCameraObjBackup = m_pCameraObj; 
	m_pCameraObj = pObj; 
}

void CsGBCamera::RollBackCameraObj()
{ 
	if(NULL == m_pCameraObjBackup )
		return;

	m_pCameraObj = m_pCameraObjBackup; 
	m_pCameraObjBackup = NULL; 
}

CsGBSubCamera* CsGBCamera::getSubCamera(int num)
{ 
	return mSubCamera[num]; 
}	//2015-11-26-nova

NiCamera* CsGBCamera::GetCameraObj()
{ 
	if( IsAttachCamera() )
	{ 
		return m_pAttachCamera->GetCameraObj(); 
	} 
	else
	{
		return m_pCameraObj; 
	}
}