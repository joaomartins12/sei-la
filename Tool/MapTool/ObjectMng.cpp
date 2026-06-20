
#include "stdafx.h"
#include "ObjectMng.h"
#include "ToolNiNode/UndoManager.h"
#include "ToolNiNode/UndoManager/UndoCommand.h"
cObjectMng g_ObjectMng;


cObjectMng::cObjectMng()
:m_bUseObjectSnap(false),
m_bUseTerrainSnap(false),
m_fSnapInterval(0.0f),
m_fSelectedOpacity(0.0f),
pkTestNode(NULL),
m_bShowCollisionBox(false),
m_bCopyObjectNomove(false)

{
}

cObjectMng::~cObjectMng()
{

}

void cObjectMng::Delete()
{
	DeleteModel();
	_DeleteType();
}

void cObjectMng::DeleteModel()
{
	_CancelAllSelectModel();
	_DeleteAllResistModel();
	_DeleteAllSaveModel();
}

void cObjectMng::Init()
{
	_InitType();
}

void cObjectMng::PosetViewReset()
{
	// 포탈 오브젝트 ID = 0 인것은 아이디 입력 하여주자
	nsCsGBTerrain::g_pCurRoot->ObjectResetCallReady();
	for( int i=0; i<nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount; ++i )
	{
		CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( i );
		if( pLeaf->IsExistTerrain() == true )
		{			
			MAP_GBTERRAIN_OBJECT* pPortalMap = pLeaf->GetPortalObjMap();
			MAP_GBTERRAIN_OBJECT_IT itPortal = pPortalMap->begin();
			for( ;itPortal != pPortalMap->end(); ++itPortal )
			{
				assert_cs( itPortal->second->GetType() == CsGBObject::OT_PORTAL );
				CsGBObj_Portal* pPortalObj = (CsGBObj_Portal*)itPortal->second;
				if( pPortalObj->IsCallReady() == true )
				{
					if( pPortalObj->GetPortalID() == 0 )
					{
						pPortalObj->SetPortalID( PopPortalTypeID() );						
					}
					else
					{
						DWORD dwReturn;
						if( ( dwReturn = UsePortalTypeID( pPortalObj->GetPortalID() ) ) != 0 )
							pPortalObj->SetPortalID( dwReturn );
					}
				}
			}
		}
	}

	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;

	// 포탈 엔피씨 ID
	CsNpc::MAP::iterator it = nsCsFileTable::g_pNpcMng->GetNpcMap()->begin();
	CsNpc::MAP::iterator itEnd = nsCsFileTable::g_pNpcMng->GetNpcMap()->end();
	for( ; it!=itEnd; ++it )
	{
		// 포탈형 엔피씨 인지
		if( it->second->GetInfo()->s_eType != nsCsNpcTable::NT_PORTAL )
			continue;

		// 배치되어 있는 넘인지
		if( nsCsMapTable::g_pMapNpcMng->IsNpc( it->second->GetInfo()->s_dwNpcID ) == false )
			continue;

		// 같은 맵이라면
		if( dwMapID == it->second->GetInfo()->s_dwMapID )
		{
			CsMapNpc* pMapNpc = nsCsMapTable::g_pMapNpcMng->GetNpc( it->second->GetInfo()->s_dwNpcID );
			sNPC_TYPE_PORTAL* pType = (sNPC_TYPE_PORTAL*)it->second->GetType();
			for( int i=0; i<pType->s_nPortalCount; ++i )
			{
				CsMapPortal* pPortalFT = nsCsMapTable::g_pMapPortalMng->GetNpcPortal_ByObjectID( dwMapID, it->second->GetInfo()->s_dwNpcID, i );
				if( pPortalFT == NULL )
				{
					CsMapPortal::sINFO info;
					info.Init();

					DWORD dwPortalID = g_ObjectMng.PopPortalTypeID();
					info.s_dwPortalID = PORTAL_TO_MAPPORTAL( dwMapID, dwPortalID );
					assert_cs( info.s_dwPortalID > PORTAL_MIN_LIMIT );
					info.s_dwSrcMapID = dwMapID;

					info.s_ePortalType = CsMapPortal::NPC;
					info.s_dwUniqObjectID = it->second->GetInfo()->s_dwNpcID;

					info.s_nPortalKindIndex = i;
					info.s_nSrcRadius = 2000;
					info.s_nSrcTargetX = pMapNpc->GetInfo()->s_nInitPosX;
					info.s_nSrcTargetY = pMapNpc->GetInfo()->s_nInitPosY;

					info.s_dwDestMapID = 0;

					nsCsMapTable::g_pMapPortalMng->AddPortal( &info, pType->GetPortal( i ) );
				}
				else
				{
					pPortalFT->SetLinkNpcType( pType->GetPortal( i ) );

					DWORD dwReturn;
					if( ( dwReturn = UsePortalTypeID( pPortalFT->GetInfo()->s_dwPortalID ) ) != 0 )
						pPortalFT->GetInfo()->s_dwPortalID = dwReturn;
				}
			}
		}
		// 다른 맵이라면
		else
		{
			sNPC_TYPE_PORTAL* pType = (sNPC_TYPE_PORTAL*)it->second->GetType();
			for( int i=0; i<pType->s_nPortalCount; ++i )
			{
				CsMapPortal* pPortalFT = nsCsMapTable::g_pMapPortalMng->GetNpcPortal_ByObjectID( it->second->GetInfo()->s_dwMapID, it->second->GetInfo()->s_dwNpcID, i );
				if( pPortalFT )
				{
					pPortalFT->SetLinkNpcType( pType->GetPortal( i ) );
				}
			}
		}
	}

	/*CsNpc::MAP* pMap = nsCsFileTable::g_pNpcMng->GetGroup( dwMapID )->GetNpcMap();
	CsNpc::MAP_IT it = pMap->begin();
	CsNpc::MAP_IT itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		if( it->second->GetInfo()->s_eType != nsCsNpcTable::NT_PORTAL )
			continue;

		if( nsCsMapTable::g_pMapNpcMng->IsNpc( it->second->GetInfo()->s_dwNpcID ) == false )
			continue;

		CsMapNpc* pMapNpc = nsCsMapTable::g_pMapNpcMng->GetNpc( it->second->GetInfo()->s_dwNpcID );
		sNPC_TYPE_PORTAL* pType = (sNPC_TYPE_PORTAL*)it->second->GetType();
		for( int i=0; i<pType->s_nPortalCount; ++i )
		{
			CsMapPortal* pPortalFT = nsCsMapTable::g_pMapPortalMng->GetNpcPortal_ByObjectID( dwMapID, it->second->GetInfo()->s_dwNpcID, i );
			if( pPortalFT == NULL )
			{
				CsMapPortal::sINFO info;
				info.Init();

				DWORD dwPortalID = g_ObjectMng.PopPortalTypeID();
				info.s_dwPortalID = PORTAL_TO_MAPPORTAL( dwMapID, dwPortalID );
				assert_cs( info.s_dwPortalID > PORTAL_MIN_LIMIT );
				info.s_dwSrcMapID = dwMapID;

				info.s_ePortalType = CsMapPortal::NPC;
				info.s_dwUniqObjectID = it->second->GetInfo()->s_dwNpcID;

				info.s_nPortalKindIndex = i;
				info.s_nSrcRadius = 2000;
				info.s_nSrcTargetX = pMapNpc->GetInfo()->s_nInitPosX;
				info.s_nSrcTargetY = pMapNpc->GetInfo()->s_nInitPosY;

				info.s_dwDestMapID = 0;

				nsCsMapTable::g_pMapPortalMng->AddPortal( &info, pType->GetPortal( i ) );
			}
			else
			{
				pPortalFT->SetLinkNpcType( pType->GetPortal( i ) );

				DWORD dwReturn;
				if( ( dwReturn = UsePortalTypeID( pPortalFT->GetInfo()->s_dwPortalID ) ) != 0 )
					pPortalFT->GetInfo()->s_dwPortalID = dwReturn;
			}
		}
	}*/
}

void cObjectMng::Update()
{
	if( IsClientRect() == true )
		_UpdateResistModel();
	_UpdateSelectModel();
}

void cObjectMng::Render()
{
	if( IsClientRect() == true )
		_RenderResistModel();
	_RenderSelectModel();

	int nLeafCount = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;
	CsGBTerrainLeaf* pLeafNode = NULL;
	MAP_GBTERRAIN_OBJECT* pMapObject;
	for( int i=0; i<nLeafCount; ++i )
	{
		pLeafNode = nsCsGBTerrain::g_pCurRoot->GetNode( i );

		pMapObject = pLeafNode->GetObjectMap();
		MAP_GBTERRAIN_OBJECT_IT itObject = pMapObject->begin();
		for( ; itObject!=pMapObject->end(); ++itObject )
		{
			NiCollisionData* pCollision = NiGetCollisionData( itObject->second->GetNiNode() );
			if( pCollision != NULL )
			{
				NiAVObjectPtr pkAVObj = itObject->second->GetNiNode()->GetObjectByName("CollisionBV");
				if( pkAVObj == NULL)
				{
					pkAVObj = NiDrawableBV::CreateWireframeBV( pCollision->GetModelSpaceABV(), NiColor( 0, 0, 1 ) );
					itObject->second->GetNiNode()->AttachChild( pkAVObj );
					pkAVObj->SetName("CollisionBV");
					pkAVObj->UpdateEffects();
					pkAVObj->UpdateProperties();
					pkAVObj->Update( 0.0f );
				}
				pkAVObj->SetAppCulled(!m_bShowCollisionBox);
				if(pkAVObj && 	pkAVObj->GetAppCulled() == false)
				{
					NiDrawScene( CAMERA_ST.GetCameraObj(), pkAVObj, *g_pCuller );
				}
			}
		}
	}
}

void cObjectMng::RotationModelOnKey( bool bPlus )
{
	if( m_listResistModel.size() != 0 )
	{
		LIST_OBJMODEL_IT itResist = m_listResistModel.begin();
		for( ; itResist!=m_listResistModel.end(); ++itResist )
		{
			if( bPlus )	(*itResist)->SetGroupRotationZ( NI_PI*0.25f );
			else		(*itResist)->SetGroupRotationZ( -NI_PI*0.25f );
		}
	}
	else if( m_listSelectModel.size() > 0 )
	{		
		LIST_OBJMODEL_IT itSelect = m_listSelectModel.begin();
		for( ; itSelect!=m_listSelectModel.end(); ++itSelect )
		{
			if( bPlus )	(*itSelect)->SetGroupRotationZ( NI_PI*0.25f );
			else		(*itSelect)->SetGroupRotationZ( -NI_PI*0.25f );
		}

		NiTransform tr;
		tr.MakeIdentity();
		tr.m_Rotate = (*m_listSelectModel.begin())->GetNiNode()->GetWorldTransform().m_Rotate;
		tr.m_Translate = g_ToolNiNodeMng.GetAxisRotation()->GetPosition();
		GET_MV_OBJECTCONTROL_DLG->_ResetValue( tr );
	}
}


//==========================================================================================
//
//		Type
//
//==========================================================================================

void cObjectMng::_DeleteType()
{
	m_mapPortalTypeID.clear();
}

void cObjectMng::_InitType()
{
	#define MAX_PORTAL_TYPE_ID		999
	assert_cs( m_mapPortalTypeID.empty() == true );
	for( DWORD dwID = MAX_PORTAL_TYPE_ID; dwID > 0; --dwID )
	{
		m_mapPortalTypeID[ dwID ] = 0;
	}
}

DWORD cObjectMng::PopPortalTypeID()
{	
	std::map< DWORD, DWORD >::iterator it = m_mapPortalTypeID.begin();
	DWORD dwReturn = it->first;
	m_mapPortalTypeID.erase( it );
	return dwReturn;
}

void cObjectMng::PushPortalTypeID( DWORD dwPushID )
{
	DWORD dwID = MAPPORTAL_TO_PORTAL( dwPushID );
	assert_cs( m_mapPortalTypeID.find( dwID ) == m_mapPortalTypeID.end() );
	m_mapPortalTypeID[ dwID ] = 0;
}

DWORD cObjectMng::UsePortalTypeID( DWORD dwUseID )
{
	DWORD dwID = MAPPORTAL_TO_PORTAL( dwUseID );	
	if( m_mapPortalTypeID.find( dwID ) != m_mapPortalTypeID.end() )
	{
		m_mapPortalTypeID.erase( dwID );
		return 0;
		
	}	
	return PopPortalTypeID();
}

//==========================================================================================
//
//		Mouse
//
//==========================================================================================

void cObjectMng::LButtonDown( CPoint pt )
{
	if( IsClientRect() == false )
		return;

	if( m_listResistModel.size() != 0 )
	{
		_Resist( pt );
	}
	else
	{
		_PickSelectModel( pt );
	}
}

void cObjectMng::LButtonDbClick( CPoint pt )
{
	if( IsClientRect() == false )
		return;

	if( m_listSelectModel.size() != 1 )
		return;

	LIST_OBJMODEL_IT itSelect = m_listSelectModel.begin();

	NiNode* pNode = (*itSelect)->GetNiNode();
	if( pNode == NULL )
		return;
	GET_MV_VIEW->ChangeCameraPos( pNode->GetWorldTransform() );
}

void cObjectMng::LButtonUp( CPoint pt )
{
}

void cObjectMng::MouseMove( CPoint pt )
{
	m_ptMouse = pt;

	if( m_listResistModel.size() != 0 )
		_PickResistModel( pt );
}

void cObjectMng::MouseWheel()
{
	MouseMove( m_ptMouse );
}

void cObjectMng::OnClickUseObjectSnap(bool bChecked, const float fInterval)
{
	m_bUseObjectSnap = bChecked;
	m_bUseTerrainSnap = !bChecked;
	m_fSnapInterval = fInterval;
}
void cObjectMng::OnClickUseTerrainSnap(bool bChecked, const float fInterval)
{
	m_bUseObjectSnap = !bChecked;
	m_bUseTerrainSnap = bChecked;
	m_fSnapInterval = fInterval;
}


void cObjectMng::_DeleteTerrainObject( DWORD dwUniqID )
{
	CsGBObject* pCsGBObject = nsCsGBTerrain::g_pCurRoot->FindCsGBObject( dwUniqID );
	if( pCsGBObject == NULL )
		return;

	// 자식들
	std::list< CsGBObject::sCHILD_INFO* >* pChildList = pCsGBObject->GetChildList();
	std::list< CsGBObject::sCHILD_INFO* >::iterator itChild = pChildList->begin();
	for( ; itChild!=pChildList->end(); ++itChild )
	{
		_DeleteTerrainObject( ( *itChild )->s_dwChildUniqID );
	}

	pCsGBObject->IncreaseRefCount();	
	nsCsGBTerrain::g_pCurRoot->DeleteCsGBObject( dwUniqID );
}

//====================================================================================================
//
// Resist Model
//
//====================================================================================================

void cObjectMng::_DeleteAllResistModel()
{
	LIST_OBJMODEL_IT it = m_listResistModel.begin();
	for( ; it!=m_listResistModel.end(); ++it )
	{
		(*it)->Delete();
		m_poolModel.Push( (*it) );
	}
	m_listResistModel.clear();
}

cObjectModel* cObjectMng::InsertModel( TCHAR* szPath, TCHAR* szName )
{
	assert_cs( szName != NULL );	

	TCHAR szFullPath[ MAX_PATH ] = {0, };
	if( szPath != NULL )
	{
		_stprintf_s( szFullPath, MAX_PATH, L"%s\\%s", szPath, szName );
	}
	else
	{
		_tcscpy_s( szFullPath, MAX_PATH, szName );
	}	
	assert_cs( _taccess( szFullPath, 0 ) == 0 );

	CsGBObject::sINFO ObjInfo;
	ObjInfo.Reset();
	W2M( ObjInfo.s_cObjectPath, szFullPath, OBJECT_PATH_LEN );
	ObjInfo.s_trObject.MakeIdentity();

	sUSER_DEFINE* pUserDefine = nsCsGBTerrain::g_ObjectResMgr.GetUserDefine( &ObjInfo );
	if(pUserDefine == NULL)
		return NULL;

	if( ( pUserDefine->s_dwPlag & sUSER_DEFINE::UDF_ANIMATION ) != 0 )
		ObjInfo.s_bCheckDoAnimation = true;

	cObjectModel* pModel = m_poolModel.Pop();
	pModel->Create( m_fSelectedOpacity, &ObjInfo, false );
	m_listResistModel.push_back( pModel );



	return pModel;
}

cObjectModel* cObjectMng::InsertModel( cObjectModel* pObjectModel )
{	
	cObjectModel* pModel = m_poolModel.Pop();
	pModel->Create( m_fSelectedOpacity, pObjectModel->GetObjectInfo(), false );
	m_listResistModel.push_back( pModel );

	return pModel;
}

DWORD cObjectMng::InsertModel( const std::string& kFullPath, const NiTransform& kLastPos )
{
	CsGBObject::sINFO ObjInfo;
	ObjInfo.Reset();
	wsprintfA(ObjInfo.s_cObjectPath, kFullPath.c_str());
	//W2M( ObjInfo.s_cObjectPath, kFullPath, OBJECT_PATH_LEN );
	ObjInfo.s_trObject.MakeIdentity();

	sUSER_DEFINE* pUserDefine = nsCsGBTerrain::g_ObjectResMgr.GetUserDefine( &ObjInfo );
	if(pUserDefine == NULL)
		return -1;

	if( ( pUserDefine->s_dwPlag & sUSER_DEFINE::UDF_ANIMATION ) != 0 )
		ObjInfo.s_bCheckDoAnimation = true;

	cObjectModel* pModel = m_poolModel.Pop();
	pModel->Create( m_fSelectedOpacity, &ObjInfo, true );
	m_listResistModel.push_back( pModel );

	pModel->ResistObject(kLastPos);
	return pModel->GetObjectInfo()->s_dwUniqID;
}
void cObjectMng::_UpdateResistModel()
{
	LIST_OBJMODEL_IT it = m_listResistModel.begin();
	for( ; it!=m_listResistModel.end(); ++it )
	{
		(*it)->Update();
	}	
}

void cObjectMng::_RenderResistModel()
{
	LIST_OBJMODEL_IT it = m_listResistModel.begin();
	for( ; it!=m_listResistModel.end(); ++it )
	{
		(*it)->Render();
	}
}

//Deploy때 Obj Translate
bool cObjectMng::_PickResistModel( CPoint pt )
{
	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
	if( pRecord == NULL )
	{
		LIST_OBJMODEL_IT it = m_listResistModel.begin();
		for( ; it!=m_listResistModel.end(); ++it )
		{
			(*it)->SetRender( false );
		}
		return false;
	}

	NiPoint3 pos = pRecord->GetIntersection();
	LIST_OBJMODEL_IT it = m_listResistModel.begin();
	for( ; it!=m_listResistModel.end(); ++it )
	{
		(*it)->SetRender( true );
		(*it)->SetGroupTranslate( pos, false );
	}
	return true;
}
CsGBObject* cObjectMng::CreateBuffRegion(NiPoint3 kPos, float fRadius)
{
	LIST_OBJMODEL_IT it = m_listResistModel.begin();
	for( ; it!=m_listResistModel.end(); ++it )
	{
		CsGBObject* pkObj = (*it)->ResistBuffRegion(kPos, fRadius);
		_DeleteAllResistModel();
		return pkObj;
	}
	return NULL;
}

void cObjectMng::_Resist( CPoint pt )
{
	if( _PickResistModel( pt ) == false )
		return;

	UNDO::Snapshot* pkSnapshot = new UNDO::Snapshot();
	UNDO::Snapshot* pkRedoSnapshot = new UNDO::Snapshot();

	LIST_OBJMODEL_IT it = m_listResistModel.begin();
	for( ; it!=m_listResistModel.end(); ++it )
	{
		CsGBObject* pkObj = (*it)->ResistObject();

		pkSnapshot->AddSnapshot(pkObj->GetInfo()->s_dwUniqID, 
		pkObj->GetNiNode()->GetWorldTransform());
		pkRedoSnapshot->AddSnapshot(pkObj->GetInfo()->s_cObjectPath, pkObj->GetNiNode()->GetWorldTransform());
	}
	int CUID =UNDO::Manager::GetSingletonPtr()->AddCommand(UNDO::OBJ_CREATE, pkSnapshot);
	UNDO::Manager::GetSingletonPtr()->AddRedoSnapshot(CUID, pkRedoSnapshot);

	if( GET_MV_VIEW->_IsShift() == false )
	{
		// 트리 해제
		GET_MV_MAINTAB_DLG->_GetDlgObejct()->_ReleaseObjectTree();

		// 셀렉트 오브젝트로
		LIST_OBJMODEL_IT it = m_listResistModel.begin();
		for( ; it!=m_listResistModel.end(); ++it )
		{
			TerrainObjectToSelectModel( (*it)->GetObjectInfo()->s_dwUniqID );
		}

		_DeleteAllResistModel();
	}
}

void cObjectMng::_SelectModelToResistModel()
{
	assert_cs( m_listResistModel.size() == 0 );
	assert_cs( m_listSelectModel.size() != 0 );

	// 중간 점을 구한다.
	NiPoint3 ptMin = NiPoint3( FLT_MAX, FLT_MAX, FLT_MAX );
	NiPoint3 ptMax = NiPoint3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	NiPoint3 pos;
	{
		LIST_OBJMODEL_IT itSelect = m_listSelectModel.begin();
		for( ; itSelect!=m_listSelectModel.end(); ++itSelect )
		{
			pos = (*itSelect)->GetNiNode()->GetWorldTranslate();
			if( ptMin.x > pos.x )	ptMin.x = pos.x;
			if( ptMin.y > pos.y )	ptMin.y = pos.y;
			if( ptMin.z > pos.z )	ptMin.z = pos.z;
			if( ptMax.x < pos.x )	ptMax.x = pos.x;
			if( ptMax.y < pos.y )	ptMax.y = pos.y;
			if( ptMax.z < pos.z )	ptMax.z = pos.z;
		}
		pos = ( ptMax + ptMin )*0.5f;
		pos.z = ptMin.z;
	}

	{
		LIST_OBJMODEL_IT itSelect = m_listSelectModel.begin();
		for( ; itSelect!=m_listSelectModel.end(); ++itSelect )
		{
			m_listResistModel.push_back( ( *itSelect ) );
			( *itSelect )->SetGroupOffsetPos( pos );
			( *itSelect )->DeleteSelect();
		}
		m_listSelectModel.clear();
	}

	g_ToolNiNodeMng.GetAxisRotation()->ReleaseVisible();
}

void cObjectMng::SelectModelCopyResistModel()
{
	if( m_listSelectModel.size() == 0 )
		return;
	assert_cs( m_listResistModel.size() == 0 );

	// 중간 점을 구한다.
	NiPoint3 ptMin = NiPoint3( FLT_MAX, FLT_MAX, FLT_MAX );
	NiPoint3 ptMax = NiPoint3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	NiPoint3 pos;
	{
		LIST_OBJMODEL_IT itSelect = m_listSelectModel.begin();
		for( ; itSelect!=m_listSelectModel.end(); ++itSelect )
		{
			pos = (*itSelect)->GetNiNode()->GetWorldTranslate();
			if( ptMin.x > pos.x )	ptMin.x = pos.x;
			if( ptMin.y > pos.y )	ptMin.y = pos.y;
			if( ptMin.z > pos.z )	ptMin.z = pos.z;
			if( ptMax.x < pos.x )	ptMax.x = pos.x;
			if( ptMax.y < pos.y )	ptMax.y = pos.y;
			if( ptMax.z < pos.z )	ptMax.z = pos.z;
		}
		pos = ( ptMax + ptMin )*0.5f;
		pos.z = ptMin.z;
	}	

	if(m_bCopyObjectNomove)
	{
		std::list<DWORD> OriginIDs;
		LIST_OBJMODEL_IT itSelect = m_listSelectModel.begin();
		for( ; itSelect!=m_listSelectModel.end(); ++itSelect )
		{
			cObjectModel* pModel = InsertModel( (*itSelect) );
			if(pModel != NULL)
			{
				pModel->SetGroupOffsetPos( pos );
				pModel->SetRender( true );
				OriginIDs.push_back(pModel->GetObjectInfo()->s_dwUniqID);
			}
		}
		_CancelAllSelectModel();

		LIST_OBJMODEL_IT it = m_listResistModel.begin();
		for( ; it!=m_listResistModel.end(); ++it )
		{
			CsGBObject* pkObj = (*it)->ResistObject();
			TerrainObjectToSelectModel( (*it)->GetObjectInfo()->s_dwUniqID );
		}
		_DeleteAllResistModel();
	}
	else
	{
		LIST_OBJMODEL_IT itSelect = m_listSelectModel.begin();
		for( ; itSelect!=m_listSelectModel.end(); ++itSelect )
		{
			cObjectModel* pModel = InsertModel( (*itSelect) );
			if(pModel != NULL)
			{
				pModel->SetGroupOffsetPos( pos );
				pModel->SetRender( true );
			}
		}
		_CancelAllSelectModel();
	}
}

//==========================================================================================
//
//	Select Model
//
//==========================================================================================

void cObjectMng::_DeleteAllSelectModel()
{
	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	LIST_OBJMODEL_IT it = m_listSelectModel.begin();
	UNDO::Snapshot* pkSnapshot = new UNDO::Snapshot();
	UNDO::Snapshot* pkRedoSnapshot = new UNDO::Snapshot();
	
	for( ; it!=m_listSelectModel.end(); ++it )
	{
		DWORD dwObjID = (*it)->GetObjectInfo()->s_dwUniqID;
		nsCsMapTable::g_pActObjectMng->DeleteActorObject( dwMapID, dwObjID );

		pkSnapshot->AddSnapshot((*it)->GetObjectInfo()->s_cObjectPath,
			(*it)->GetNiNode()->GetWorldTransform());

		pkRedoSnapshot->AddSnapshot(dwObjID, (*it)->GetNiNode()->GetWorldTransform());

		(*it)->DeleteSelect();
		(*it)->Delete();
		//LockingInfos.erase(dwObjID);
		m_poolModel.Push( (*it) );
	}
	int CUID = UNDO::Manager::GetSingletonPtr()->AddCommand(UNDO::OBJ_DELETE, pkSnapshot);
	UNDO::Manager::GetSingletonPtr()->AddRedoSnapshot(CUID,pkRedoSnapshot);

	m_listSelectModel.clear();

	g_ToolNiNodeMng.GetAxisRotation()->ReleaseVisible();

	if( IS_ENABLEFRM( nsMAPTOOL::MV_MAINTAB ) == true )
		GET_MV_MAINTAB_DLG->_GetDlgObejct()->_SetUniqObject( NULL );
}

bool cObjectMng::DeleteSelectModel( DWORD dwUniqID )
{
	NiNode* pDeleteNode = NULL;
	LIST_OBJMODEL_IT it = m_listSelectModel.begin();
	for( ; it!=m_listSelectModel.end(); ++it )
	{
		if( (*it)->GetObjectInfo()->s_dwUniqID == dwUniqID )
		{
			pDeleteNode = (*it)->GetNiNode();
			(*it)->DeleteSelect();
			(*it)->Delete();
			m_poolModel.Push( (*it) );
			m_listSelectModel.erase( it );
			break;
		}		
	}

	if( pDeleteNode == NULL )
		return false;

	NiTransform tr = g_ToolNiNodeMng.GetAxisRotation()->RemoveVisible( pDeleteNode );
	it = m_listSelectModel.begin();
	for( ; it!=m_listSelectModel.end(); ++it )
	{
		(*it)->SetGroupOffsetPos( tr.m_Translate );
	}

	if( IS_ENABLEFRM( nsMAPTOOL::MV_MAINTAB ) == true )
		GET_MV_MAINTAB_DLG->_GetDlgObejct()->_SetUniqObject( NULL );

	return true;
}

void cObjectMng::_CancelAllSelectModel()
{
	LIST_OBJMODEL_IT it = m_listSelectModel.begin();
	for( ; it!=m_listSelectModel.end(); ++it )
	{
		(*it)->CancelSelect();
		(*it)->Delete();
		m_poolModel.Push( (*it) );
	}
	m_listSelectModel.clear();

	g_ToolNiNodeMng.GetAxisRotation()->ReleaseVisible();
	g_ToolNiNodeMng.UpdateGizmoPos(this);
	if( IS_ENABLEFRM( nsMAPTOOL::MV_MAINTAB ) == true )
		GET_MV_MAINTAB_DLG->_GetDlgObejct()->_SetUniqObject( NULL );
}

void cObjectMng::_CancelUnSelectModel( NiNode* pSelNode )
{
	g_ToolNiNodeMng.GetAxisRotation()->ReleaseVisible();

	LIST_OBJMODEL_IT it = m_listSelectModel.begin();
	LIST_OBJMODEL_IT itDelete;
	while( it!=m_listSelectModel.end() )
	{
		itDelete = it;
		++it;
		if( (*itDelete)->GetNiNode() != pSelNode )
		{
			(*itDelete)->CancelSelect();
			(*itDelete)->Delete();
			m_poolModel.Push( (*itDelete) );
			m_listSelectModel.erase( itDelete );
		}
	}
	cObjectModel* pObjectModel;
	if( m_listSelectModel.size() /*- nDeleteCnt*/ == 1 )
	{
		LIST_OBJMODEL_IT itTmp = m_listSelectModel.begin();
		pObjectModel = (*itTmp);
	}
	else
		pObjectModel = NULL;

	NiTransform tr = g_ToolNiNodeMng.GetAxisRotation()->InsertVisible( pSelNode, cAxisRotation::AT_OBJECT, pObjectModel );
	it = m_listSelectModel.begin();
	for( ; it!=m_listSelectModel.end(); ++it )
	{
		(*it)->SetGroupOffsetPos( tr.m_Translate );
	}


	GET_MV_MAINTAB_DLG->_GetDlgObejct()->_SetUniqObject( m_listSelectModel.size() == 1 ?
		nsCsGBTerrain::g_ObjectResMgr.GetObject( ( *m_listSelectModel.begin() )->GetObjectInfo() ) : NULL );
}


void cObjectMng::_CancelSelectModel( NiNode* pSelNode )
{
	g_ToolNiNodeMng.GetAxisRotation()->ReleaseVisible();

	LIST_OBJMODEL_IT it = m_listSelectModel.begin();
	LIST_OBJMODEL_IT itDelete;
	while( it!=m_listSelectModel.end() )
	{
		itDelete = it;
		++it;
		if( (*itDelete)->GetNiNode() == pSelNode )
		{
			(*itDelete)->CancelSelect();
			(*itDelete)->Delete();
			m_poolModel.Push( (*itDelete) );
			m_listSelectModel.erase( itDelete );
		}
		else
		{			
			cObjectModel* pObjectModel;
			if( m_listSelectModel.size()/* - nDeleteCnt*/ == 1 )
			{
				LIST_OBJMODEL_IT itTmp = m_listSelectModel.begin();
				pObjectModel = (*itTmp);
			}
			else
				pObjectModel = NULL;
			NiTransform tr = g_ToolNiNodeMng.GetAxisRotation()->InsertVisible( (*itDelete)->GetNiNode(), cAxisRotation::AT_OBJECT, pObjectModel );
			LIST_OBJMODEL_IT itAdd = m_listSelectModel.begin();
			for( ; itAdd!=m_listSelectModel.end(); ++itAdd )
			{
				(*itAdd)->SetGroupOffsetPos( tr.m_Translate );
			}
		}
	}	
	g_ToolNiNodeMng.UpdateGizmoPos(this);
	GET_MV_MAINTAB_DLG->_GetDlgObejct()->_SetUniqObject( m_listSelectModel.size() == 1 ?
		nsCsGBTerrain::g_ObjectResMgr.GetObject( ( *m_listSelectModel.begin() )->GetObjectInfo() ) : NULL );
}

void cObjectMng::_UpdateSelectModel()
{
	LIST_OBJMODEL_IT it = m_listSelectModel.begin();
	for( ; it!=m_listSelectModel.end(); ++it )
	{
		(*it)->Update();
	}
}

void cObjectMng::_RenderSelectModel()
{
	LIST_OBJMODEL_IT it = m_listSelectModel.begin();
	for( ; it!=m_listSelectModel.end(); ++it )
	{
		(*it)->Render();

	}
}

void cObjectMng::_PickSelectModel( CPoint pt )
{
	NiPoint3 origin, dir;
	CAMERA_ST.WindowPointToRay( pt.x, pt.y, origin, dir );

	CsGBPick pick( false );

	// 셀렉트 오브젝트 선별
	LIST_OBJMODEL_IT it = m_listSelectModel.begin();
	for( ; it!=m_listSelectModel.end(); ++it )
	{
		pick.Pick( (*it)->GetNiNode(), origin, dir, true );
	}

	// 터레인 오브젝트 선별
	{
		MAP_GBTERRAIN_OBJECT* pMap = nsCsGBTerrain::g_pCurRoot->GetMap_No_Pick();
		MAP_GBTERRAIN_OBJECT_IT it = pMap->begin();
		MAP_GBTERRAIN_OBJECT_IT itEnd = pMap->end();
		for( ; it!=itEnd; ++it )
		{
			pick.Pick( it->second->GetNiNode(), origin, dir, true );
		}
	}

	int nLeafCount = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;
	CsGBTerrainLeaf* pLeafNode = NULL;
	MAP_GBTERRAIN_OBJECT* pMapObject;
	for( int i=0; i<nLeafCount; ++i )
	{
		pLeafNode = nsCsGBTerrain::g_pCurRoot->GetNode( i );

		pMapObject = pLeafNode->GetObjectMap();
		MAP_GBTERRAIN_OBJECT_IT itObject = pMapObject->begin();
		for( ; itObject!=pMapObject->end(); ++itObject )
		{
			pick.Pick( itObject->second->GetNiNode(), origin, dir, true );
		}
	}

	NiPick::Record* pRecord = pick.GetRecord();
	if( pRecord == NULL )
		return;

	NiNode* pNode = nsCSGBFUNC::GetRootNiNode( pRecord->GetAVObject() );

	// 추가 오브젝트의 근원이 셀렉트 오브젝트 인가	
	it = m_listSelectModel.begin();
	for( ; it!=m_listSelectModel.end(); ++it )
	{
		if( (*it)->GetNiNode() == pNode )
		{
			if( GET_MV_VIEW->_IsShift() == false )
				_CancelUnSelectModel( pNode );
			else									
				_CancelSelectModel( pNode );
			return;
		}
	}

	// 아니면 터레인 오브젝트
	if( GET_MV_VIEW->_IsShift() == false )
		_CancelAllSelectModel();

	TerrainObjectToSelectModel( pNode );
}
void cObjectMng::ShowCollisionBox(bool TOF)
{
	m_bShowCollisionBox = TOF;
		// 셀렉트 오브젝트 선별
		LIST_OBJMODEL_IT it = m_listSelectModel.begin();
		for( ; it!=m_listSelectModel.end(); ++it )
		{
			UpdateCollisionBox((*it)->GetNiNode(), m_bShowCollisionBox);
		}

		// 터레인 오브젝트 선별
		{
			MAP_GBTERRAIN_OBJECT* pMap = nsCsGBTerrain::g_pCurRoot->GetMap_No_Pick();
			MAP_GBTERRAIN_OBJECT_IT it = pMap->begin();
			MAP_GBTERRAIN_OBJECT_IT itEnd = pMap->end();
			for( ; it!=itEnd; ++it )
			{
				UpdateCollisionBox( it->second->GetNiNode(), m_bShowCollisionBox);
			}
		}

		int nLeafCount = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;
		CsGBTerrainLeaf* pLeafNode = NULL;
		MAP_GBTERRAIN_OBJECT* pMapObject;
		for( int i=0; i<nLeafCount; ++i )
		{
			pLeafNode = nsCsGBTerrain::g_pCurRoot->GetNode( i );

			pMapObject = pLeafNode->GetObjectMap();
			MAP_GBTERRAIN_OBJECT_IT itObject = pMapObject->begin();
			for( ; itObject!=pMapObject->end(); ++itObject )
			{
				UpdateCollisionBox( itObject->second->GetNiNode(), m_bShowCollisionBox);
			}
		}
}


void cObjectMng::UpdateCollisionBox(NiNode* pkNode, bool bShow)
{

	NiCollisionData* pCollision = NiGetCollisionData( pkNode );
	if( pCollision != NULL )
	{
		NiAVObjectPtr pkAVObj = pkNode->GetObjectByName("CollisionBV");
		if( pkAVObj == NULL)
		{
			pkAVObj = NiDrawableBV::CreateWireframeBV( pCollision->GetModelSpaceABV(), NiColor( 0, 0, 1 ) );
			pkNode->AttachChild( pkAVObj );
			pkAVObj->SetName("CollisionBV");
		}
		pkAVObj->SetAppCulled(!bShow);
		pkAVObj->UpdateEffects();
		pkAVObj->UpdateProperties();
		pkAVObj->Update( 0.0f );

	}
}
cObjectModel* cObjectMng::TerrainObjectToSelectModel( NiNode* pTRObject )
{
	// 찾아서 지우자 ( 그냥 지우면 리소스 다 날라가니 레퍼런스 1 증가시키고 지우자
	CsGBObject* pCsGBObject = nsCsGBTerrain::g_pCurRoot->FindCsGBObject( pTRObject );
	assert_cs( pCsGBObject != NULL );
	if( pCsGBObject->GetParentID() != 0 )
	{
		pCsGBObject = nsCsGBTerrain::g_pCurRoot->FindCsGBObject( pCsGBObject->GetParentID() );
		assert_cs( pCsGBObject != NULL );
	}
	_DeleteTerrainObject( pCsGBObject->GetInfo()->s_dwUniqID );

	cObjectModel* pModel = m_poolModel.Pop();
	pModel->Create( pCsGBObject->GetInfo(), true, pCsGBObject->GetAniTime(), m_fSelectedOpacity );

	//LockIter kLockIter = LockingInfos.find(pCsGBObject->GetInfo()->s_dwUniqID);
	//if(kLockIter != LockingInfos.end())
	//{
	//	pModel->SetLockObject(kLockIter->second);
	//}

	m_listSelectModel.push_back( pModel);

	UpdateCollisionBox(pModel->GetNiNode(),	m_bShowCollisionBox );

	GET_MV_MAINTAB_DLG->_GetDlgObejct()->_SetUniqObject( m_listSelectModel.size() == 1 ? pCsGBObject : NULL );

	NiTransform tr = g_ToolNiNodeMng.GetAxisRotation()->InsertVisible( pModel->GetNiNode(), cAxisRotation::AT_OBJECT, pModel );
	g_ToolNiNodeMng.OnObjectSelected(GetCenterSelectedObj(), this);
	LIST_OBJMODEL_IT it = m_listSelectModel.begin();
	for( ; it!=m_listSelectModel.end(); ++it )
	{
		(*it)->SetGroupOffsetPos( tr.m_Translate );
	}	

	return pModel;
}

cObjectModel* cObjectMng::TerrainObjectToSelectModel( DWORD dwObjectUniqID )
{
	// 찾아서 지우자 ( 그냥 지우면 리소스 다 날라가니 레퍼런스 1 증가시키고 지우자
	CsGBObject* pCsGBObject = nsCsGBTerrain::g_pCurRoot->FindCsGBObject( dwObjectUniqID );
	if( pCsGBObject == NULL )
		return NULL;
	
	assert_cs( pCsGBObject->GetInfo()->s_dwUniqID == dwObjectUniqID );
	if( pCsGBObject->GetParentID() != 0 )
	{
		pCsGBObject = nsCsGBTerrain::g_pCurRoot->FindCsGBObject( pCsGBObject->GetParentID() );
		assert_cs( pCsGBObject != NULL );
	}
	_DeleteTerrainObject( dwObjectUniqID );
	
	cObjectModel* pModel = m_poolModel.Pop();
	pModel->Create( pCsGBObject->GetInfo(), true, pCsGBObject->GetAniTime(), m_fSelectedOpacity );
	//LockIter kLockIter= LockingInfos.find(dwObjectUniqID);
	//if(kLockIter != LockingInfos.end()){
	//	pModel->SetLockObject(kLockIter->second);
	//}
	m_listSelectModel.push_back( pModel );
	UpdateCollisionBox(pModel->GetNiNode(), m_bShowCollisionBox );

	GET_MV_MAINTAB_DLG->_GetDlgObejct()->_SetUniqObject( m_listSelectModel.size() == 1 ? pCsGBObject : NULL );	
	NiTransform tr = g_ToolNiNodeMng.GetAxisRotation()->InsertVisible( pModel->GetNiNode(), cAxisRotation::AT_OBJECT, pModel );
	g_ToolNiNodeMng.OnObjectSelected(GetCenterSelectedObj(), this);
	LIST_OBJMODEL_IT it = m_listSelectModel.begin();
	for( ; it!=m_listSelectModel.end(); ++it )
	{
		(*it)->SetGroupOffsetPos( tr.m_Translate );
	}

	return pModel;
}

void cObjectMng::SelectObject_SelectAll()
{
	if( m_listSelectModel.size() != 1 )
		return;

	cObjectModel* pSel = *m_listSelectModel.begin();
	char cSelPath[ MAX_PATH ];
	strcpy_s( cSelPath, OBJECT_PATH_LEN, pSel->GetObjectInfo()->s_cObjectPath );

	_CancelAllSelectModel();
	std::list< DWORD > listUniqID;

	{
		MAP_GBTERRAIN_OBJECT* pMap = nsCsGBTerrain::g_pCurRoot->GetMap_No_Pick();
		MAP_GBTERRAIN_OBJECT_IT it = pMap->begin();
		MAP_GBTERRAIN_OBJECT_IT itEnd = pMap->end();
		for( ; it!=itEnd; ++it )
		{
			if( strcmp( it->second->GetInfo()->s_cObjectPath, cSelPath ) == 0 )
			{
				listUniqID.push_back( it->second->GetInfo()->s_dwUniqID );
			}		
		}
	}	

	nsCsGBTerrain::g_pCurRoot->ObjectResetCallReady();
	for( int i=0; i<nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount; ++i )
	{
		CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( i );
		if( pLeaf->IsVisible() == false )
			continue;

		MAP_GBTERRAIN_OBJECT* pMap = pLeaf->GetObjectMap();
		MAP_GBTERRAIN_OBJECT_IT it = pMap->begin();
		for( ; it!=pMap->end(); ++it )
		{
			if( it->second->IsCallReady() == false )
				continue;

			if( strcmp( it->second->GetInfo()->s_cObjectPath, cSelPath ) == 0 )
			{
				listUniqID.push_back( it->second->GetInfo()->s_dwUniqID );
			}
		}		
	}

	std::list< DWORD >::iterator it = listUniqID.begin();
	for( ; it!=listUniqID.end(); ++it )
	{
		TerrainObjectToSelectModel( *it );
	}
	listUniqID.clear();
}

void cObjectMng::SelectObject_AttachTerrain()
{
	if( m_listSelectModel.size() == 0 )
		return;

	LIST_OBJMODEL_IT it = m_listSelectModel.begin();
	for( ; it!=m_listSelectModel.end(); ++it )
	{
		(*it)->AttachTerrain();
	}
}

void cObjectMng::DeleteSelectObject()
{
	if( m_listSelectModel.size() == 0 )
		return;

	_DeleteAllSelectModel();	
}

void cObjectMng::DetachSelectObject()
{
	if( m_listSelectModel.size() == 0 )
		return;

	_SelectModelToResistModel();	
}

void cObjectMng::ResetTransformSelectObject()
{
	if( m_listSelectModel.size() == 0 )
		return;

	LIST_OBJMODEL_IT it = m_listSelectModel.begin();
	for( ; it!=m_listSelectModel.end(); ++it )
	{
		(*it)->ResetTransformSelectObject();
	}
}
//Dlg (Object Controller)에서 Object 이동할때
void cObjectMng::SelectModel_ApplyTransform( NiTransform tr, bool resetViewer )
{
	NiPoint3 kGap = tr.m_Translate - GetCenterSelectedObj();
	if(kGap != NiPoint3::ZERO)
	{
		SetTranslateByGizmo(kGap, resetViewer);
	}
	else
	{
		if( m_listSelectModel.size() == 1 )
		{
			LIST_OBJMODEL_IT it = m_listSelectModel.begin();
			tr.m_fScale = (*it)->GetNiNode()->GetScale();
			(*it)->SetTransform( tr );
		}
	}
}


//==========================================================================================
//
//	Save Model
//
//==========================================================================================

void cObjectMng::_DeleteAllSaveModel()
{
	for( int i=0; i<10; ++i )
	{
		LIST_OBJMODEL_IT itSave = m_listSaveModel[ i ].begin();
		for( ; itSave != m_listSaveModel[ i ].end(); ++itSave )
		{
			( *itSave )->Delete();
			m_poolModel.Push( *itSave );
		}
		m_listSaveModel[ i ].clear();
	}		
}

void cObjectMng::_DeleteSaveModel( int nIndex )
{
	LIST_OBJMODEL_IT itSave = m_listSaveModel[ nIndex ].begin();
	for( ; itSave != m_listSaveModel[ nIndex ].end(); ++itSave )
	{
		( *itSave )->Delete();
		m_poolModel.Push( *itSave );
	}
	m_listSaveModel[ nIndex ].clear();
}


void cObjectMng::SelectModelCopySaveModel( int nIndex )
{
	if( GET_MV_MAINTAB_DLG->_GetSelTabIndex() != _DlgMainTab::TT_OBJECT )
		return;

	if( m_listSelectModel.size() == 0 )
		return;

	_DeleteSaveModel( nIndex );

	LIST_OBJMODEL_IT itSelect = m_listSelectModel.begin();
	for( ; itSelect != m_listSelectModel.end(); ++itSelect )
	{
		cObjectModel* pModel = m_poolModel.Pop();
		CsGBObject* pCsGBObject = nsCsGBTerrain::g_ObjectResMgr.GetObject( ( *itSelect )->GetObjectInfo()->s_dwUniqID );
		pModel->Create( pCsGBObject->GetInfo(), false, pCsGBObject->GetAniTime(), m_fSelectedOpacity );
		m_listSaveModel[ nIndex ].push_back( pModel );
	}

	assert_cs( m_listSelectModel.size() == m_listSaveModel[ nIndex ].size() );
}

void cObjectMng::SaveModelCopySelectModel( int nIndex )
{
	if( GET_MV_MAINTAB_DLG->_GetSelTabIndex() != _DlgMainTab::TT_OBJECT )
		return;

	if( m_listSaveModel[ nIndex ].size() == 0 )
		return;

	_DeleteAllResistModel();
	_CancelAllSelectModel();

	LIST_OBJMODEL_IT itSave = m_listSaveModel[ nIndex ].begin();
	LIST_OBJMODEL_IT itDelete;
	for( ; itSave != m_listSaveModel[ nIndex ].end(); )
	{
		itDelete = itSave;
		++itSave;
		if( nsCsGBTerrain::g_ObjectResMgr.IsObject( (*itDelete)->GetObjectInfo()->s_dwUniqID ) == true )
		{
			TerrainObjectToSelectModel( (*itDelete)->GetObjectInfo()->s_dwUniqID );
		}
		else
		{
			m_listSaveModel[ nIndex ].erase( itDelete );
		}
	}
	assert_cs( m_listSelectModel.size() == m_listSaveModel[ nIndex ].size() );		
}






//==========================================================================================
//	Gizmo Editing
//==========================================================================================
int	cObjectMng::TakeUndoSnapshot(int eEditType)
{
	UNDO::Snapshot* pkSnapshot = new UNDO::Snapshot();
	LIST_OBJMODEL_IT kIter = m_listSelectModel.begin();
	for(;kIter != m_listSelectModel.end(); ++kIter)
	{
		pkSnapshot->AddSnapshot((*kIter)->GetObjectInfo()->s_dwUniqID, (*kIter)->GetNiNode()->GetWorldTransform());
	}
	return UNDO::Manager::GetSingletonPtr()->AddCommand((UNDO::EDIT_TYPE)eEditType, pkSnapshot);
}
void	cObjectMng::TakeRedoSnapshot(int CUID)
{
	UNDO::Snapshot* pkSnapshot = new UNDO::Snapshot();
	LIST_OBJMODEL_IT kIter = m_listSelectModel.begin();
	for(;kIter != m_listSelectModel.end(); ++kIter)
	{
		pkSnapshot->AddSnapshot((*kIter)->GetObjectInfo()->s_dwUniqID, (*kIter)->GetNiNode()->GetWorldTransform());
	}
	UNDO::Manager::GetSingletonPtr()->AddRedoSnapshot(CUID, pkSnapshot);
}
void	cObjectMng::SetTranslateByGizmo(const NiPoint3& kPos, bool UpdateObsever, bool isTwoSideTranslate)
{
	LIST_OBJMODEL_IT kIter = m_listSelectModel.begin();
	for(;kIter != m_listSelectModel.end(); ++kIter)
	{
		(*kIter)->SetTranslateByGizmo(kPos);
	}
	if(UpdateObsever)	
		UpdateViewer();
}
void	cObjectMng::SetSnapByGizmo(const NiPoint3& kSnapPos)
{
	LIST_OBJMODEL_IT kIter = m_listSelectModel.begin();
	for(;kIter != m_listSelectModel.end(); ++kIter)
	{
		(*kIter)->SetSanpByGizmo(kSnapPos);
	}
	UpdateViewer();
}
void	cObjectMng::SetRotateByGizmo(const NiMatrix3& kRotate, bool UpdateObsever)
{
	NiPoint3 kSelectedCenter = GetCenterSelectedObj();
	LIST_OBJMODEL_IT kIter = m_listSelectModel.begin();
	for(;kIter != m_listSelectModel.end(); ++kIter)
	{
		(*kIter)->SetRotateByGizmo(kSelectedCenter, kRotate);
	}
	UpdateViewer();
}
void	cObjectMng::SetRotateByGizmo(const NiPoint3& kCenterPos, int iCoord, NiFixedString kAxis, float fAngle)
{
	NiPoint3 kSelectedCenter = GetCenterSelectedObj();
	LIST_OBJMODEL_IT kIter = m_listSelectModel.begin();
	for(;kIter != m_listSelectModel.end(); ++kIter)
	{
		(*kIter)->SetRotateByGizmo(kCenterPos, iCoord, kAxis, fAngle);
	}
	UpdateViewer();
}
void	cObjectMng::SetScaleByGizmo(const NiPoint3& vDir, const float& fScale)
{
	const float fSpeed = 0.16f;
	LIST_OBJMODEL_IT kIter = m_listSelectModel.begin();
	for(;kIter != m_listSelectModel.end(); ++kIter)
	{
		(*kIter)->SetScaleByGizmo(fScale * fSpeed);
	}
	UpdateViewer();
}
void	cObjectMng::SetTransformByOUID(const DWORD& OUID, const NiTransform& kTr)
{
	CsGBObject* pCsGBObject = nsCsGBTerrain::g_pCurRoot->FindCsGBObject( OUID );
	if( pCsGBObject != NULL )
	{
		if( pCsGBObject->GetParentID() != 0 )
		{
			pCsGBObject = nsCsGBTerrain::g_pCurRoot->FindCsGBObject( pCsGBObject->GetParentID() );
			assert_cs( pCsGBObject != NULL );
		}

		pCsGBObject->SetTransform(kTr);
		return;
	}
	LIST_OBJMODEL_IT kIter = m_listResistModel.begin();
	for(;kIter != m_listResistModel.end(); ++kIter)
	{
		if( (*kIter)->GetObjectInfo()->s_dwUniqID == OUID)
		{
			(*kIter)->SetTransform(kTr);
			return;
		}
	}
	kIter = m_listSelectModel.begin();
	for(;kIter != m_listSelectModel.end(); ++kIter)
	{
		if( (*kIter)->GetObjectInfo()->s_dwUniqID == OUID)
		{
			(*kIter)->SetTransform(kTr);
			return;
		}
	}

}
const NiPoint3	cObjectMng::GetCenterSelectedObj(void)
{
	NiPoint3 kCenter = NiPoint3::ZERO;
	LIST_OBJMODEL_IT kIter = m_listSelectModel.begin();
	for(;kIter != m_listSelectModel.end(); ++kIter)
	{
		kCenter += (*kIter)->GetNiNode()->GetWorldTranslate();
	}
	if(kCenter != NiPoint3::ZERO)
		return kCenter / m_listSelectModel.size();
	return NiPoint3::ZERO;
}
const int			cObjectMng::GetSelectModelCnt(void)
{
	if(m_listSelectModel.empty() == false)
	{
		return m_listSelectModel.size();
	}
	else return 0;
}


void cObjectMng::UpdateViewer()
{
	if(m_listSelectModel.empty() == false)
	{
		cObjectModel* pkFront = m_listSelectModel.front();
		if(pkFront != NULL)
		{
			NiNode* pkNode= pkFront->GetNiNode();
			if(pkNode != NULL)
			{	
				NiTransform kSelectedFirstTr = pkNode->GetWorldTransform();
				kSelectedFirstTr.m_Translate = GetCenterSelectedObj();

				GET_MV_OBJECTCONTROL_DLG->_ResetValue( kSelectedFirstTr );
				GET_MV_MAINTAB_DLG->_GetDlgObejct()->UpdateViewer(kSelectedFirstTr);
			}
		}
		g_ToolNiNodeMng.UpdateGizmoPos(this);
	}
}

NiPick::Record*	cObjectMng::SnapPickingObj(const CPoint& kMousePt)
{
	NiPoint3 origin, dir;
	CAMERA_ST.WindowPointToRay( kMousePt.x, kMousePt.y, origin, dir );

	CsGBPick pick( false );

	// 셀렉트 오브젝트 선별
	//LIST_OBJMODEL_IT it = m_listSelectModel.begin();
	//for( ; it!=m_listSelectModel.end(); ++it )
	//{
	//	pick.Pick( (*it)->GetNiNode(), origin, dir, true );
	//}
	{
		MAP_GBTERRAIN_OBJECT* pMap = nsCsGBTerrain::g_pCurRoot->GetMap_No_Pick();
		MAP_GBTERRAIN_OBJECT_IT it = pMap->begin();
		MAP_GBTERRAIN_OBJECT_IT itEnd = pMap->end();
		for( ; it!=itEnd; ++it )
		{
			pick.Pick( it->second->GetNiNode(), origin, dir, true );
		}

	}
	int nLeafCount = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;
	CsGBTerrainLeaf* pLeafNode = NULL;
	MAP_GBTERRAIN_OBJECT* pMapObject;
	for( int i=0; i<nLeafCount; ++i )
	{
		pLeafNode = nsCsGBTerrain::g_pCurRoot->GetNode( i );

		pMapObject = pLeafNode->GetObjectMap();
		MAP_GBTERRAIN_OBJECT_IT itObject = pMapObject->begin();
		for( ; itObject!=pMapObject->end(); ++itObject )
		{
			pick.Pick( itObject->second->GetNiNode(), origin, dir, true );
		}
	}

	return pick.GetRecord();
}


int cObjectMng::GetSelectedCnt(void)
{
	return GetSelectModelCnt();

}
const NiPoint3 cObjectMng::GetSelectedCenter(void)
{
	return GetCenterSelectedObj();
}
const NiPoint3 cObjectMng::GetSelectedFront(void)
{
	NiPoint3 kFront = NiPoint3::ZERO;
	if(m_listSelectModel.empty() == false)
	{
		LIST_OBJMODEL_IT kIter = m_listSelectModel.begin();
		NiPoint3 kFront = GetWorldDirection((*kIter)->GetNiNode());
		NiPoint3::UnitizeVector(kFront);
		return kFront;
	}
	return NiPoint3::ZERO;
}
const NiMatrix3 cObjectMng::GetSelectedRot(void)
{
	NiPoint3 kFront = NiPoint3::ZERO;
	if(m_listSelectModel.empty() == false)
	{
		LIST_OBJMODEL_IT kIter = m_listSelectModel.begin();
		return (*kIter)->GetNiNode()->GetWorldTransform().m_Rotate;
	}
	return NiMatrix3::IDENTITY;
}
const NiPoint3 cObjectMng::GetWorldDirection(NiAVObject* pkAVObject)
{
	NiPoint3 kDVector;
	pkAVObject->GetWorldTransform().m_Rotate.GetCol(0, kDVector);
	return kDVector;
}
void	cObjectMng::UpdateOpacity(float fValue)
{
	m_fSelectedOpacity = fValue;
	LIST_OBJMODEL_IT kiter = m_listSelectModel.begin();
	for( ; kiter!=m_listSelectModel.end(); ++kiter )
	{
		nsCSGBFUNC::SetMaterial_Alpha_Emittance( (*kiter)->GetNiNode(), fValue );
	}
}

void	cObjectMng::SelectByDrag(const NiPoint3& kLT, const NiPoint3& kRB)
{
	DeleteModel();
	int nLeafCount = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;
	CsGBTerrainLeaf* pLeafNode = NULL;
	std::vector<int> kSeletedIndex;
	MAP_GBTERRAIN_OBJECT* pMapObject;
	for( int i=0; i<nLeafCount; ++i )
	{
		pLeafNode = nsCsGBTerrain::g_pCurRoot->GetNode( i );

		pMapObject = pLeafNode->GetObjectMap();
		MAP_GBTERRAIN_OBJECT_IT itObject = pMapObject->begin();
		for( ; itObject!=pMapObject->end(); ++itObject )
		{
			NiPoint3 kWorld = itObject->second->GetNiNode()->GetTranslate();
			{
				if(  kLT.x < kWorld.x && kWorld.x < kRB.x)
				{
					if(kRB.y < kWorld.y && kWorld.y < kLT.y)
					{
						kSeletedIndex.push_back(itObject->first);
					}
				}
			}
		}
	}
	
	for(int i=0; i < kSeletedIndex.size(); ++i)
	{
		TerrainObjectToSelectModel(kSeletedIndex.at(i));
	}
}
void	cObjectMng::LockSelectedObj()
{
	//LIST_OBJMODEL_IT kiter = m_listSelectModel.begin();
	//for( ; kiter!=m_listSelectModel.end(); ++kiter )
	//{
	//	DWORD OriginID = (*kiter)->GetObjectInfo()->s_dwUniqID;
	//	LockIter kLockIter = LockingInfos.find(OriginID);
	//	if(kLockIter != LockingInfos.end())
	//	{
	//		bool CurrLockingInfo = kLockIter->second;
	//		if(CurrLockingInfo == true){
	//			kLockIter->second = false;
	//		}
	//		else{
	//			kLockIter->second = true;
	//		}
	//	}
	//	else
	//	{
	//		LockingInfos.insert(LOCKINFO::value_type(OriginID, true));
	//		kLockIter = LockingInfos.find(OriginID);
	//	}
	//	(*kiter)->SetLockObject(kLockIter->second);
	//}	
}
void	cObjectMng::DeployOnTerrain(void)
{
	int CUID = TakeUndoSnapshot(0);

	NiPoint3 kNormal = NiPoint3::ZERO;
	LIST_OBJMODEL_IT kIter = m_listSelectModel.begin();
	for(;kIter != m_listSelectModel.end(); ++kIter)
	{
		NiPoint3 kTranslate =  (*kIter)->GetNiNode()->GetTranslate();
		nsCsGBTerrain::g_pCurRoot->GetHeight_Normal(kTranslate.x, kTranslate.y, kNormal );
		if(kNormal != NiPoint3::ZERO)
		{
			LookAt((*kIter)->GetNiNode(), kNormal);

		}
	}
	TakeRedoSnapshot(CUID);
}
void cObjectMng::LookAt(NiAVObject* pNode, const NiPoint3& kNormal)
{
	NiPoint3 TargetUp, TargetRight, TargetDir;
	
	TargetUp = kNormal;
	NiPoint3 ObjDir = GetWorldDirection(pNode);
	NiPoint3::UnitizeVector(ObjDir);
	TargetRight = TargetUp.Cross(ObjDir) * -1.0f;
	NiPoint3::UnitizeVector(TargetRight);
	TargetDir = TargetUp.Cross(TargetRight);
	NiPoint3::UnitizeVector(TargetDir);
	TargetRight = TargetRight * -1.0f;
	//TargetUp = kNormal;
	//NiPoint3::UnitizeVector(TargetUp);
	//NiPoint3 TargetProjZ = TargetUp;
	//TargetProjZ.x = 0.0f;
	//NiPoint3::UnitizeVector(TargetProjZ);
	//NiMatrix3 kZRot;
	//kZRot.MakeXRotation(NI_PI * 0.5f);
	//TargetRight = kZRot * TargetProjZ;
	//NiPoint3::UnitizeVector(TargetRight);
	//NiPoint3 TargetDir =TargetUp.Cross(TargetRight) * -1.0f;
	//NiPoint3::UnitizeVector(TargetDir);
	//TargetRight = TargetUp.Cross(TargetDir);
	//NiPoint3::UnitizeVector(TargetRight);
	// get parent world transforms
	NiNode* pkParent = pNode->GetParent();
	NiCamera* pkCamera = CAMERA_ST.GetCameraObj();

	NiMatrix3 kFaceMat = NiMatrix3::IDENTITY;
	kFaceMat.SetCol(0, TargetDir);
	kFaceMat.SetCol(1, TargetRight);
	kFaceMat.SetCol(2, TargetUp);
/////////////////////////////////
	NiTransform tr, trDest, trResult;
	tr.MakeIdentity();
	tr.m_Rotate = kFaceMat;
	
	trDest = pNode->GetWorldTransform();
	trDest.m_Rotate = kFaceMat;//pkCamera->GetWorldTransform().m_Rotate;
	trDest.m_Translate -= pNode->GetTranslate();
	trResult = tr;//*trDest;
	trResult.m_Translate += pNode->GetTranslate();
	trResult.m_fScale = trDest.m_fScale;
	pNode->SetLocalTransform(trResult);
	pNode->Update(0.0f);
}

void	cObjectMng::CopyNomove(bool TOF)
{
	m_bCopyObjectNomove = TOF;
}