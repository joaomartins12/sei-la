
#include "stdafx.h"
#include "ObjectModel.h"

cObjectModel::cObjectModel()
:m_bEditedLock(false)
//,m_Locking(1.0f, 0.0f,0.0f)
{
	m_pNiNode = NULL;

}

void cObjectModel::DeleteSelect()
{
	// 베이스 오브젝트라면 그쪽 반경 헤이트 필드 리셋
	CsGBObject* pObject = nsCsGBTerrain::g_ObjectResMgr.GetObject( m_ObjectInfo.s_dwUniqID );
	if( pObject == NULL )
		return;

	if( pObject->IsRefZero() == true )
		return;

	/*if( pObject->UD_IsTerrainBase() == true )
		nsCsGBTerrain::g_pCurRoot->ResetHeightField_TBObject( pObject );*/
	if( pObject->GetType() == CsGBObject::OT_PORTAL )
		g_ObjectMng.PushPortalTypeID( ( (CsGBObj_Portal*)pObject )->GetPortalID() );

	// 부모가 있다면 부모 리스트에서 제거
	if( pObject->GetParentID() != 0 )
	{
		CsGBObject* pParentObj = nsCsGBTerrain::g_ObjectResMgr.GetObject( pObject->GetParentID() );
		pParentObj->DeleteChild_FromUniqID( pObject->GetInfo()->s_dwUniqID );
	}	

	// 차일드
	std::map< DWORD, cObjectModel* >::iterator it = m_mapChild.begin();
	for( ; it!=m_mapChild.end(); ++it )
	{
		it->second->DeleteSelect();
	}

	pObject->DecreaseRefCount();

	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	nsCsMapTable::g_pActObjectMng->DeleteActorObject( dwMapID, m_ObjectInfo.s_dwUniqID );
}

void cObjectModel::CancelSelect()
{
	CsGBObject* pCsGBObject = nsCsGBTerrain::g_ObjectResMgr.GetObject( &m_ObjectInfo );
	assert_cs( pCsGBObject->IsRefZero() == false );	

	pCsGBObject->GetInfo()->s_bCheckDoAnimation = m_ObjectInfo.s_bCheckDoAnimation;
	pCsGBObject->GetInfo()->s_fAniSpeed = m_ObjectInfo.s_fAniSpeed;
	pCsGBObject->GetInfo()->s_fStopAniTime = m_ObjectInfo.s_fStopAniTime;

	pCsGBObject->SetTransform( m_pNiNode->GetWorldTransform() );

	if( pCsGBObject->UDF_IsAnimation() == false )
	{
		pCsGBObject->GetNiNode()->Update( 0.0f, false );
	}
	else
	{
		pCsGBObject->SetAniTime( m_fAniTime );
		pCsGBObject->GetNiNode()->Update( m_fAniTime );
	}


	pCsGBObject->GetInfo()->s_Bound.Reset();		
	NiBound aa = m_pNiNode->GetWorldBound();
	if( nsCSGBFUNC::CalMinMaxBound( m_pNiNode, m_pNiNode->GetWorldTransform(), &pCsGBObject->GetInfo()->s_Bound ) == false )
	{
		pCsGBObject->GetInfo()->s_Bound.s_ptMin = NiPoint2( m_pNiNode->GetWorldTransform().m_Translate.x, m_pNiNode->GetWorldTransform().m_Translate.y );
		pCsGBObject->GetInfo()->s_Bound.s_ptMax = pCsGBObject->GetInfo()->s_Bound.s_ptMin;
	}
	if( pCsGBObject->GetInfo()->s_Bound.s_ptMin.x > pCsGBObject->GetInfo()->s_Bound.s_ptMax.x )
	{
		pCsGBObject->GetInfo()->s_Bound.s_ptMin.x = pCsGBObject->GetInfo()->s_Bound.s_ptMax.x = m_pNiNode->GetWorldTransform().m_Translate.x;
		pCsGBObject->GetInfo()->s_Bound.s_ptMin.y = pCsGBObject->GetInfo()->s_Bound.s_ptMax.y = m_pNiNode->GetWorldTransform().m_Translate.y;
	}
	
	nsCsGBTerrain::g_pCurRoot->ResistObject_ChangeRotation( &m_ObjectInfo );
	pCsGBObject->DecreaseRefCount();

	// 차일드
	std::map< DWORD, cObjectModel* >::iterator it = m_mapChild.begin();
	for( ; it!=m_mapChild.end(); ++it )
	{
		switch( it->second->GetObjectState() )
		{
		case OS_DISABLE:
			{
				it->second->DeleteSelect();
			}
			break;
		case OS_READY:
			{
				CsGBObject* pChildResistObject = it->second->ResistObject();
				if( pChildResistObject == NULL )
					continue;

				CsGBObject::sCHILD_INFO ci;
				ci.s_dwChildUniqID = pChildResistObject->GetInfo()->s_dwUniqID;
				ci.s_dwSlotID = it->second->GetChildSlotID();
				pCsGBObject->InsertChild( ci );
				pChildResistObject->SetParentID( pCsGBObject->GetInfo()->s_dwUniqID );
			}
			break;
		case OS_RESIST:
			{
				it->second->CancelSelect();
			}
			break;
		default:
			assert_cs( false );
		}
	}
}

void cObjectModel::Delete()
{
	assert_cs( m_pNiNode != NULL );
	m_pNiNode = 0;

	// 차일드
	std::map< DWORD, cObjectModel* >::iterator it = m_mapChild.begin();
	for( ; it!=m_mapChild.end(); ++it )
	{
		it->second->Delete();
		g_ObjectMng.m_poolModel.Push( it->second );
	}
	m_mapChild.clear();
}

void cObjectModel::Create( CsGBObject::sINFO* pObjectInfo, bool bRender, float fAniTime, float fOpacity  )
{
	Create( fOpacity, pObjectInfo, bRender );
	m_fAniTime = fAniTime;
}

void cObjectModel::Create( float fOpacity, CsGBObject::sINFO* pObjectInfo, bool bRender )
{
	m_bRender = bRender;
	m_bEditedLock = false;

	nsCsGBTerrain::g_ObjectResMgr.IsObject( pObjectInfo->s_dwUniqID ) == true ? m_eObjectState = OS_RESIST : m_eObjectState = OS_READY;

	memcpy( &m_ObjectInfo, pObjectInfo, sizeof( CsGBObject::sINFO ) );
	m_ObjectInfo.s_PathInfo.Reset();

	// Transform
	m_trOrigin = m_ObjectInfo.s_trObject;
	m_ptGroupOffset = NiPoint3::ZERO;
	m_ptGroupPos = NiPoint3::ZERO;

	sUSER_DEFINE* pUserDefine = nsCsGBTerrain::g_ObjectResMgr.GetUserDefine( &m_ObjectInfo );
	if(pUserDefine == NULL)
		return;

	m_pNiNode = nsCsGBTerrain::g_ObjectResMgr.GetNiObject( m_ObjectInfo.s_cObjectPath, pUserDefine );
	CsGBObject::GlobalApplyUserDefine( m_pNiNode, &m_ObjectInfo, pUserDefine );

	// Animation
	if( bRender == false )
	{
		if( ( pUserDefine->s_dwPlag & sUSER_DEFINE::UDF_ANIMATION ) == 0 )
		{
			m_ObjectInfo.s_bCheckDoAnimation = false;
		}
		else
		{
			m_ObjectInfo.s_bCheckDoAnimation = true;
		}
	}	

	m_fAniTime = 0.0f;
	nsCSGBFUNC::SetMaterial_Alpha_Emittance( m_pNiNode, fOpacity );

	m_pNiNode->UpdateProperties();
	m_pNiNode->SetLocalTransform( m_ObjectInfo.s_trObject );
	m_pNiNode->Update( 0.0f );

	// 차일드
	if( ( pUserDefine->s_dwPlag & sUSER_DEFINE::UDF_HAVE_CHILD ) != 0 )
	{
		if( ( pUserDefine->s_dwPlag & sUSER_DEFINE::UD_MERGE ) != 0 )
		{					
			sUDD_Merge* pUDD = (sUDD_Merge*)pUserDefine->GetData( sUDD_Merge::Type() );

			// 등록 안된 오브젝트일 경우에
			if( nsCsGBTerrain::g_ObjectResMgr.IsObject( pObjectInfo->s_dwUniqID ) == false )
			{
				std::map< DWORD, sUDD_Merge::sINFO* >::iterator it = pUDD->s_mapInfo.begin();
				for( ;it!=pUDD->s_mapInfo.end(); ++it )
				{
					CsGBObject::sINFO childInfo;
					childInfo.Reset();
					childInfo.s_trObject.MakeIdentity();
					strcpy_s( childInfo.s_cObjectPath, OBJECT_PATH_LEN, it->second->s_cFileName );

					cObjectModel* pModel = g_ObjectMng.m_poolModel.Pop();
					pModel->Create( fOpacity, &childInfo, bRender );
					pModel->SetNiChildIndex( it->second->s_nNiNodeChildIndex );
					pModel->SetChildSlotID( it->second->s_nSlotID );

					assert_cs( m_mapChild.find( it->second->s_nSlotID ) == m_mapChild.end() );
					m_mapChild[ it->second->s_nSlotID ] = pModel;					
				}
			}
			// 등록이 되어 있는 오브젝트
			else
			{
				std::map< DWORD, sUDD_Merge::sINFO* >::iterator it = pUDD->s_mapInfo.begin();
				for( ;it!=pUDD->s_mapInfo.end(); ++it )
				{
					CsGBObject* pParentObject = nsCsGBTerrain::g_ObjectResMgr.GetObject( pObjectInfo->s_dwUniqID );
					CsGBObject::sCHILD_INFO* pChildInfo = pParentObject->GetChild_FormSlotID( it->second->s_nSlotID );
					// 슬롯이 존재 한다면
					if( pChildInfo != NULL )
					{
						CsGBObject* pChildObject = nsCsGBTerrain::g_ObjectResMgr.GetObject( pChildInfo->s_dwChildUniqID );
						assert_cs( pChildObject != NULL );
						cObjectModel* pModel = g_ObjectMng.m_poolModel.Pop();
						pModel->Create( fOpacity , pChildObject->GetInfo(), bRender );
						pModel->SetChildSlotID( it->second->s_nSlotID );
						pModel->SetNiChildIndex( it->second->s_nNiNodeChildIndex );
						assert_cs( m_mapChild.find( it->second->s_nSlotID ) == m_mapChild.end() );
						m_mapChild[ it->second->s_nSlotID ] = pModel;
					}
					// 슬롯 오브젝트가 꺼져 있다면
					else
					{
						CsGBObject::sINFO childInfo;
						childInfo.Reset();
						childInfo.s_trObject.MakeIdentity();
						strcpy_s( childInfo.s_cObjectPath, OBJECT_PATH_LEN, it->second->s_cFileName );

						cObjectModel* pModel = g_ObjectMng.m_poolModel.Pop();
						pModel->Create( fOpacity, &childInfo, bRender );
						pModel->SetNiChildIndex( it->second->s_nNiNodeChildIndex );
						pModel->SetChildSlotID( it->second->s_nSlotID );
						pModel->SetObjectState( OS_DISABLE );
						assert_cs( m_mapChild.find( it->second->s_nSlotID ) == m_mapChild.end() );
						m_mapChild[ it->second->s_nSlotID ] = pModel;
					}
				}
			}

			// 차일드
			std::map< DWORD, cObjectModel* >::iterator it = m_mapChild.begin();
			for( ; it!=m_mapChild.end(); ++it )
			{
				it->second->ResetChildPos( this );
			}			
		}
	}	
}

void cObjectModel::Update()
{
	if( ( m_bRender == true )&&( m_eObjectState != OS_DISABLE ) )
	{
		if( m_ObjectInfo.s_bCheckDoAnimation == true )
		{
			m_fAniTime += g_fElapsedTime*m_ObjectInfo.s_fAniSpeed;
		}
		else
		{
			m_fAniTime = m_ObjectInfo.s_fStopAniTime;
		}
		if( m_fAniTime < 0.0f )
			m_fAniTime = 0.0f;
		m_pNiNode->Update( m_fAniTime );

		// 차일드
		std::map< DWORD, cObjectModel* >::iterator it = m_mapChild.begin();
		for( ; it!=m_mapChild.end(); ++it )
		{
			it->second->Update();
		}
	}
}

void cObjectModel::Render()
{
	if( ( m_bRender == true )&&( m_eObjectState != OS_DISABLE ) )
	{
		NiDrawScene( CAMERA_ST.GetCameraObj(), m_pNiNode, *g_pCuller );

		// 차일드
		std::map< DWORD, cObjectModel* >::iterator it = m_mapChild.begin();
		for( ; it!=m_mapChild.end(); ++it )
		{
			it->second->Render();
		}
	}
}

void cObjectModel::SetRender( bool bRender )
{
	m_bRender = bRender;

	// 차일드
	std::map< DWORD, cObjectModel* >::iterator it = m_mapChild.begin();
	for( ; it!=m_mapChild.end(); ++it )
	{
		it->second->SetRender( bRender );
	}
}

CsGBObject::sINFO* cObjectModel::GetObjectInfo()
{
	m_ObjectInfo.s_trObject = m_pNiNode->GetWorldTransform();
	return &m_ObjectInfo;
}

void cObjectModel::SetTransform( NiTransform tr )
{	
	m_pNiNode->SetLocalTransform( tr );
	m_pNiNode->Update( 0.0f );

	// 차일드
	std::map< DWORD, cObjectModel* >::iterator it = m_mapChild.begin();
	for( ; it!=m_mapChild.end(); ++it )
	{
		it->second->ResetChildPos( this );
	}
}

void cObjectModel::SetGroupOffsetPos( NiPoint3 ptOffset )
{
	m_ptGroupPos = ptOffset;
	m_ptGroupOffset = m_pNiNode->GetWorldTranslate() - ptOffset;
	
	m_pNiNode->SetTranslate( m_ptGroupPos + m_ptGroupOffset );
	m_pNiNode->Update( 0.0f );

	// 차일드
	std::map< DWORD, cObjectModel* >::iterator it = m_mapChild.begin();
	for( ; it!=m_mapChild.end(); ++it )
	{
		it->second->ResetChildPos( this );
	}
}

void cObjectModel::SetGroupTranslate( NiPoint3 pos, bool bOffSnap )
{
	// 스냅 이동 관련	
	if( ( bOffSnap == false )&&( GET_MV_MAINTAB_DLG->_GetDlgObejct()->_IsUseSnap() == true ) )
	{
		float fGrid = (float)GET_MV_MAINTAB_DLG->_GetDlgObejct()->_GetSnapSize();
		NiPoint3 NewPos;
		NewPos.x = pos.x + fGrid*0.5f;
		NewPos.y = pos.y + fGrid*0.5f;
		int nIndex = (int)NewPos.x / (int)fGrid;
		pos.x = nIndex*fGrid;
		nIndex = (int)NewPos.y / (int)fGrid;
		pos.y = nIndex*fGrid;
		pos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( pos.x, pos.y );
	}

	//NiPoint3 ptDelta = pos - m_ptGroupPos;
	m_ptGroupPos = pos;
	m_pNiNode->SetTranslate( m_ptGroupPos + m_ptGroupOffset );
	m_pNiNode->Update( 0.0f );

	// 차일드
	std::map< DWORD, cObjectModel* >::iterator it = m_mapChild.begin();
	for( ; it!=m_mapChild.end(); ++it )
	{
		it->second->ResetChildPos( this );
	}
}

void cObjectModel::SetGroupRotation( NiMatrix3 mat )
{
	NiTransform tr;
	m_matRotation = mat;

	m_pNiNode->SetRotate(m_matRotation);
	m_pNiNode->Update( 0.0f );
	//tr.MakeIdentity();
	//tr.m_Rotate = mat;	

	//NiTransform trDest = m_pNiNode->GetWorldTransform();
	//trDest.m_Translate -= m_ptGroupPos;
	//NiTransform trResult = tr*trDest;
	//m_ptGroupOffset = trResult.m_Translate;
	//trResult.m_Translate += m_ptGroupPos;
	//m_pNiNode->SetLocalTransform( trResult );
	//m_pNiNode->Update( 0.0f );

	// 차일드
	std::map< DWORD, cObjectModel* >::iterator it = m_mapChild.begin();
	for( ; it!=m_mapChild.end(); ++it )
	{
		it->second->ResetChildPos( this );
	}
}

void cObjectModel::SetGroupRotationZ( float fAngle )
{
	NiTransform tr;
	tr.MakeIdentity();
	tr.m_Rotate.MakeZRotation( fAngle );

	NiTransform trDest = m_pNiNode->GetWorldTransform();
	trDest.m_Translate -= m_ptGroupPos;
	NiTransform trResult = tr*trDest;
	m_ptGroupOffset = trResult.m_Translate;
	trResult.m_Translate += m_ptGroupPos;
	m_pNiNode->SetLocalTransform( trResult );
	m_pNiNode->Update( 0.0f );

	// 차일드
	std::map< DWORD, cObjectModel* >::iterator it = m_mapChild.begin();
	for( ; it!=m_mapChild.end(); ++it )
	{
		it->second->ResetChildPos( this );
	}
}

void cObjectModel::ResetTransformSelectObject()
{	
	m_pNiNode->SetLocalTransform( m_trOrigin );
	m_pNiNode->Update( 0.0f );

	// 차일드
	std::map< DWORD, cObjectModel* >::iterator it = m_mapChild.begin();
	for( ; it!=m_mapChild.end(); ++it )
	{
		it->second->ResetChildPos( this );
	}
}

void cObjectModel::AttachTerrain()
{
	NiPoint3 vPos = m_pNiNode->GetWorldTranslate();
	vPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPos );
	m_pNiNode->SetTranslate( vPos );
	m_pNiNode->Update( 0.0f );

	// 차일드
	std::map< DWORD, cObjectModel* >::iterator it = m_mapChild.begin();
	for( ; it!=m_mapChild.end(); ++it )
	{
		it->second->ResetChildPos( this );
	}
}

CsGBObject*		cObjectModel::ResistBuffRegion(const NiPoint3& kPos, const float fScale)
{
	NiTransform kTr;
	kTr.m_Translate = kPos;
	kTr.m_fScale = fScale;
	kTr.m_Rotate = NiMatrix3::IDENTITY;

	m_pNiNode->Update( 0.0f );

	m_ObjectInfo.s_Bound.Reset();
	if( nsCSGBFUNC::CalMinMaxBound( m_pNiNode, kTr, &m_ObjectInfo.s_Bound ) == false )
	{
		m_ObjectInfo.s_Bound.s_ptMin = NiPoint2( kTr.m_Translate.x, kTr.m_Translate.y );
		m_ObjectInfo.s_Bound.s_ptMax = m_ObjectInfo.s_Bound.s_ptMin;
	}
	if( m_ObjectInfo.s_Bound.s_ptMin.x > m_ObjectInfo.s_Bound.s_ptMax.x )
	{
		m_ObjectInfo.s_Bound.s_ptMin.x = m_ObjectInfo.s_Bound.s_ptMax.x = kTr.m_Translate.x;
		m_ObjectInfo.s_Bound.s_ptMin.y = m_ObjectInfo.s_Bound.s_ptMax.y = kTr.m_Translate.y;
	}


	m_ObjectInfo.s_trObject = kTr;
	CsGBObject* pObject = nsCsGBTerrain::g_pCurRoot->ResistObject( &m_ObjectInfo );
	if( pObject->GetType() == CsGBObject::OT_PORTAL )
	{
		CsGBObj_Portal* pObjPortal = ( CsGBObj_Portal* )pObject;
		pObjPortal->SetPortalID( g_ObjectMng.PopPortalTypeID() );
	}

	// 차일드	
	std::map< DWORD, cObjectModel* >::iterator it = m_mapChild.begin();
	for( ; it!=m_mapChild.end(); ++it )
	{
		CsGBObject* pChildObject = it->second->ResistObject();
		if( pChildObject == NULL )
			continue;

		CsGBObject::sCHILD_INFO ci;
		ci.s_dwChildUniqID = pChildObject->GetInfo()->s_dwUniqID;
		ci.s_dwSlotID = it->second->GetChildSlotID();
		pObject->InsertChild( ci );
		pChildObject->SetParentID( pObject->GetInfo()->s_dwUniqID );
	}

	return pObject;
}


bool				cObjectModel::ResistObject(NiTransform kTr)
{
	if( ( m_bRender == false )||( m_eObjectState == OS_DISABLE ) )
		return NULL;

	m_pNiNode->Update( 0.0f );

	m_ObjectInfo.s_Bound.Reset();
	if( nsCSGBFUNC::CalMinMaxBound( m_pNiNode, kTr, &m_ObjectInfo.s_Bound ) == false )
	{
		m_ObjectInfo.s_Bound.s_ptMin = NiPoint2( kTr.m_Translate.x, kTr.m_Translate.y );
		m_ObjectInfo.s_Bound.s_ptMax = m_ObjectInfo.s_Bound.s_ptMin;
	}
	if( m_ObjectInfo.s_Bound.s_ptMin.x > m_ObjectInfo.s_Bound.s_ptMax.x )
	{
		m_ObjectInfo.s_Bound.s_ptMin.x = m_ObjectInfo.s_Bound.s_ptMax.x = kTr.m_Translate.x;
		m_ObjectInfo.s_Bound.s_ptMin.y = m_ObjectInfo.s_Bound.s_ptMax.y = kTr.m_Translate.y;
	}


	m_ObjectInfo.s_trObject = kTr;
	CsGBObject* pObject = nsCsGBTerrain::g_pCurRoot->ResistObject( &m_ObjectInfo );
	if( pObject->GetType() == CsGBObject::OT_PORTAL )
	{
		CsGBObj_Portal* pObjPortal = ( CsGBObj_Portal* )pObject;
		pObjPortal->SetPortalID( g_ObjectMng.PopPortalTypeID() );
	}

	// 차일드	
	std::map< DWORD, cObjectModel* >::iterator it = m_mapChild.begin();
	for( ; it!=m_mapChild.end(); ++it )
	{
		CsGBObject* pChildObject = it->second->ResistObject();
		if( pChildObject == NULL )
			continue;

		CsGBObject::sCHILD_INFO ci;
		ci.s_dwChildUniqID = pChildObject->GetInfo()->s_dwUniqID;
		ci.s_dwSlotID = it->second->GetChildSlotID();
		pObject->InsertChild( ci );
		pChildObject->SetParentID( pObject->GetInfo()->s_dwUniqID );
	}

	return (pObject != NULL);

}

CsGBObject* cObjectModel::ResistObject()
{
	if( ( m_bRender == false )||( m_eObjectState == OS_DISABLE ) )
		return NULL;
	
	m_pNiNode->Update( 0.0f );

	m_ObjectInfo.s_Bound.Reset();
	if( nsCSGBFUNC::CalMinMaxBound( m_pNiNode, m_pNiNode->GetWorldTransform(), &m_ObjectInfo.s_Bound ) == false )
	{
		m_ObjectInfo.s_Bound.s_ptMin = NiPoint2( m_pNiNode->GetWorldTransform().m_Translate.x, m_pNiNode->GetWorldTransform().m_Translate.y );
		m_ObjectInfo.s_Bound.s_ptMax = m_ObjectInfo.s_Bound.s_ptMin;
	}
	if( m_ObjectInfo.s_Bound.s_ptMin.x > m_ObjectInfo.s_Bound.s_ptMax.x )
	{
		m_ObjectInfo.s_Bound.s_ptMin.x = m_ObjectInfo.s_Bound.s_ptMax.x = m_pNiNode->GetWorldTransform().m_Translate.x;
		m_ObjectInfo.s_Bound.s_ptMin.y = m_ObjectInfo.s_Bound.s_ptMax.y = m_pNiNode->GetWorldTransform().m_Translate.y;
	}


	m_ObjectInfo.s_trObject = m_pNiNode->GetWorldTransform();
	CsGBObject* pObject = nsCsGBTerrain::g_pCurRoot->ResistObject( &m_ObjectInfo );
	if( pObject->GetType() == CsGBObject::OT_PORTAL )
	{
		CsGBObj_Portal* pObjPortal = ( CsGBObj_Portal* )pObject;
		pObjPortal->SetPortalID( g_ObjectMng.PopPortalTypeID() );
	}

	// 차일드	
	std::map< DWORD, cObjectModel* >::iterator it = m_mapChild.begin();
	for( ; it!=m_mapChild.end(); ++it )
	{
		CsGBObject* pChildObject = it->second->ResistObject();
		if( pChildObject == NULL )
			continue;

		CsGBObject::sCHILD_INFO ci;
		ci.s_dwChildUniqID = pChildObject->GetInfo()->s_dwUniqID;
		ci.s_dwSlotID = it->second->GetChildSlotID();
		pObject->InsertChild( ci );
		pChildObject->SetParentID( pObject->GetInfo()->s_dwUniqID );
	}

	return pObject;
}


//=====================================================================================
//
//		Child
//
//=========================================================f============================

bool cObjectModel::ResetChildPos( cObjectModel* pParent )
{
	NiAVObject* pAvObj = pParent->GetNiNode()->GetAt( m_nNiChildIndex );
	m_ObjectInfo.s_trObject = pAvObj->GetWorldTransform();

	m_pNiNode->SetLocalTransform( m_ObjectInfo.s_trObject );
	m_pNiNode->Update( 0.0f );

	if( ( pParent->GetObjectInfo()->s_dwUniqID == 0 )||( m_ObjectInfo.s_dwUniqID == 0 ) )
		return false;

	CsGBObject* pObject = nsCsGBTerrain::g_ObjectResMgr.GetObject( &m_ObjectInfo );
	return false;
}

cObjectModel* cObjectModel::GetChild( int nSlotID )
{
	assert_cs( m_mapChild.find( nSlotID ) != m_mapChild.end() );
	return m_mapChild[ nSlotID ];
}

void	cObjectModel::SetTranslateByGizmo(const NiPoint3& kPos)
{
	m_pNiNode->SetTranslate(m_pNiNode->GetTranslate()+kPos);
	m_pNiNode->Update(0.0f);
}
void	cObjectModel::SetSanpByGizmo(const NiPoint3& kSnapPos)
{
	m_pNiNode->SetTranslate(kSnapPos);
	m_pNiNode->Update(0.0f);
}
void	cObjectModel::SetRotateByGizmo(const NiPoint3& kCenter, const NiMatrix3& kRot)
{
	NiTransform tr, trDest, trResult;
	tr.MakeIdentity();
	tr.m_Rotate = kRot;

	trDest = m_pNiNode->GetWorldTransform();
	trDest.m_Translate -= kCenter;//m_pNiNode->GetTranslate();
	trResult = tr*trDest;
	trResult.m_Translate += kCenter;//m_pNiNode->GetTranslate();
	m_pNiNode->SetLocalTransform(trResult);
	m_pNiNode->Update(0.0f);
}
void	cObjectModel::SetRotateByGizmo(const NiPoint3& kCenterPos, int iCoord, NiFixedString strAxis, float fAngle)
{
	if(iCoord == GIZMO::MODEL)
	{
		NiCamera* pkCamera = CAMERA_ST.GetCameraObj();
		NiString kAxisName = strAxis;
		kAxisName.Replace("_pick", "");
		NiMatrix3 kCurrmat = m_pNiNode->GetWorldTransform().m_Rotate;
		NiPoint3 kCamDir = pkCamera->GetWorldDirection();
		NiPoint3 kModelDir = NiPoint3::ZERO;
		NiPoint3 kAxis;
		if(kAxisName == "X_axis")
		{
			kCurrmat.GetCol(1, kAxis);
			if(kAxis.Dot(kCamDir) < 0 )
			{
				fAngle *= -1.0f;
			}
		}
		else if(kAxisName == "Y_axis")
		{
			kCurrmat.GetCol(0, kAxis);
			if(kAxis.Dot(kCamDir) < 0 )
			{
				fAngle *= -1.0f;
			}
		}
		else if(kAxisName == "Z_axis")
		{
			kCurrmat.GetCol(2, kAxis);
			fAngle *= -1.0f;
		}
		else
			return;

		NiPoint3::UnitizeVector(kAxis);
		//BHPRT();
		kCurrmat.MakeRotation(fAngle,kAxis);

		NiTransform tr, trDest, trResult;
		tr.MakeIdentity();
		tr.m_Rotate = kCurrmat;
		//tr.m_fScale = m_pNiNode->GetScale();
 
		trDest = m_pNiNode->GetLocalTransform();
		trDest.m_Translate -=  kCenterPos;//m_pNiNode->GetWorldTransform().m_Translate;
		trResult = tr*trDest;
		trResult.m_Translate += kCenterPos;//m_pNiNode->GetWorldTransform().m_Translate;

		float frx, fry, frz;
		trResult.m_Rotate.ToEulerAnglesZXY( frz, frx, fry );
		trResult.m_Rotate.FromEulerAnglesZXY( frz, frx, fry );
		m_pNiNode->SetLocalTransform(trResult);
		m_pNiNode->Update(0.0f);

		CString kDot;
		kDot.Format(L"Axis : X[%f] Y[%f] Z[%f] Angle [%f] Scale [%f] \n ", 
			kAxis.x, kAxis.y, kAxis.z, fAngle, trResult.m_fScale);
		OutputDebugString(kDot);

	}
}

void	cObjectModel::SetScaleByGizmo(const float fScale)
{
	m_pNiNode->SetScale(m_pNiNode->GetScale() + fScale);
	m_pNiNode->Update(0.0f);
}
