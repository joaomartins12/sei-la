
#include "stdafx.h"
#include "CsGBObjectResMng.h"

#define TEXTURE_APPLY_TYPE		NiTexturingProperty::APPLY_REPLACE
//#define TEXTURE_APPLY_TYPE		NiTexturingProperty::APPLY_MODULATE

CsGBObjectResMng::CsGBObjectResMng()
{
	m_pmapNiObject = NULL;
	m_pmapUserDefine = NULL;
	m_pmapTexture = NULL;
	//m_pmapNiObjectInfo = NULL;
}

CsGBObjectResMng::~CsGBObjectResMng()
{
}

void CsGBObjectResMng::Delete()
{
	_DeleteObject();
	_DeleteNiObject();
	//_DeleteNiObjectInfo();
	_DeleteUserDefine();
	_DeleteTexture();

	while( m_queueGetID.empty() == false )
	{
		m_queueGetID.pop();
	}
}

void CsGBObjectResMng::Init()
{
	_InitObject();
	_InitNiObject();
	//_InitNiObjectInfo();
	_InitUserDefine();
	_InitTexture();
}

//===============================================================================================
//
// Object
//
//===============================================================================================
void CsGBObjectResMng::_DeleteObject()
{
	assert_cs( m_mapObject.empty() == true );
}

void CsGBObjectResMng::_InitObject()
{
	assert_cs( m_mapObject.size() == 0 );
}

CsGBObject* CsGBObjectResMng::GetObject( CsGBObject::sINFO* pObjectInfo )
{
	assert_cs( pObjectInfo->s_dwUniqID > OBJECT_UNIQ_ID_START );

	MAP_GBTERRAIN_OBJECT_IT it = m_mapObject.find( pObjectInfo->s_dwUniqID );
	if( it == m_mapObject.end() )
	{
		sUSER_DEFINE* pUserDefine = GetUserDefine( pObjectInfo );
		if( pUserDefine == NULL )
			return NULL;

		sNINODE_INFO NiNodeInfo;
		GetNiObject( &NiNodeInfo, pObjectInfo->s_cObjectPath, pUserDefine );

		if( g_pCsPathEngine && ( g_pCsPathEngine->IsLoad() == true ) )
		{
			SetObjPath( pObjectInfo );
		}
		// 패스 엔진 등록
		if( g_pCsPathEngine && ( g_pCsPathEngine->IsLoad() == true )&&( pObjectInfo->s_PathInfo.s_uiPEGroupCount != 0 ) )
		{
			int nOffset = 0;
			for( DWORD i =0; i<pObjectInfo->s_PathInfo.s_uiPEGroupCount; ++i )
			{
				if( g_pCsPathEngine->SetShape( pObjectInfo->s_PathInfo.s_pptTranse[ i ].x,
					pObjectInfo->s_PathInfo.s_pptTranse[ i ].y,
					pObjectInfo->s_PathInfo.s_pPECount[ i ],
					&pObjectInfo->s_PathInfo.s_pData[ nOffset ] ) == false )
				{
					char cMsg[ 512 ];
					sprintf_s( cMsg, 512, "%s\n\n오브젝트의 PathEngine 선이 잘못 되었습니다.", pObjectInfo->s_cObjectPath );
					MessageBoxA( NULL, cMsg, NULL, MB_OK );
				}
				nOffset += pObjectInfo->s_PathInfo.s_pPECount[ i ]*2;
			}
		}

		//====================================================================
		CsGBObject* pObject = NULL;
		if( ( pUserDefine->s_dwPlag & sUSER_DEFINE::UD_PORTAL ) != 0 )
		{
			pObject = CsGBObj_Portal::NewInstance();
		}
		else if( ( pUserDefine->s_dwPlag & sUSER_DEFINE::UDF_ROTATION ) != 0 )
		{
			pObject = CsGBObj_Rot::NewInstance();
		}
		else if( ( pUserDefine->s_dwPlag & sUSER_DEFINE::UD_EFFECTRENDER ) != 0 )
		{
			pObject = CsGBObj_Eff::NewInstance();
		}		
		else
		{
			pObject = CsGBObject::NewInstance();			;
		}
		//====================================================================

		pObject->Init( NiNodeInfo.s_pNiNode, NiNodeInfo.s_nGetID, pObjectInfo, pUserDefine );
		m_mapObject.insert( std::make_pair( pObjectInfo->s_dwUniqID, pObject ) );
		return pObject;
		//m_mapObject[ pObjectInfo->s_dwUniqID ] = pObject;
	}
	return it->second;
}

CsGBObject* CsGBObjectResMng::GetObject( DWORD dwObjectUniqID )
{
	MAP_GBTERRAIN_OBJECT_IT it = m_mapObject.find( dwObjectUniqID );
	if( it == m_mapObject.end() )
		return NULL;

	return it->second;
}

void CsGBObjectResMng::DeleteObject( DWORD dwUniqID )
{
	MAP_GBTERRAIN_OBJECT_IT it = m_mapObject.find( dwUniqID );
	if( it == m_mapObject.end() )
		return;

	CsGBObject* pObject = it->second;
	SAFE_POINTER_RET( pObject );
	
	pObject->Delete();

	switch( pObject->GetType() )
	{
	case CsGBObject::OT_NORMAL:
		CsGBObject::DeleteInstance( pObject );
		break;	
	case CsGBObject::OT_PORTAL:
		CsGBObj_Portal::DeleteInstance( (CsGBObj_Portal*)pObject );
		break;
	case CsGBObject::OT_ROTATION:
		CsGBObj_Rot::DeleteInstance( (CsGBObj_Rot*)pObject );
		break;		
	case CsGBObject::OT_EFFECT:
		CsGBObj_Eff::DeleteInstance( (CsGBObj_Eff*)pObject );
		break;
	/*case CsGBObject::OT_F1:
		g_mpGBObj_F1->Push( (CsGBObj_F1*)pObject );
		break;*/
	default:
		assert_cs( false );
	}		
	
	m_mapObject.erase( dwUniqID );
}

//===============================================================================================
//
// Ni Object
//
//===============================================================================================
void CsGBObjectResMng::_DeleteNiObject()
{
	if( m_pmapNiObject == NULL )
		return;

	sNINODE_INFO* pNode = NULL;
	NiTMapIterator itNiObj = m_pmapNiObject->GetFirstPos();
	while( itNiObj != NULL )
	{
		const char* pcName;		
		m_pmapNiObject->GetNext( itNiObj, pcName, pNode );
		assert_cs( pNode->s_nGetID != 0 );
		_PushGetID( pNode->s_nGetID );
		delete pNode;
	}
	m_pmapNiObject->RemoveAll();
	NiDelete m_pmapNiObject;
	m_pmapNiObject = NULL;
}

void CsGBObjectResMng::_InitNiObject()
{
	assert_cs( m_pmapNiObject == NULL );
	m_pmapNiObject = NiNew NiTStringPointerMap< sNINODE_INFO* >;
}

void CsGBObjectResMng::GetNiObject( sNINODE_INFO* pDestNode, const char* cObjectPath, sUSER_DEFINE* pUserDefine )
{
	// User Define 에서 만들었으므로 무조건 존재해야한다.
	sNINODE_INFO* pOrgNode = NULL;
	bool bFind = m_pmapNiObject->GetAt( cObjectPath, pOrgNode );
	assert_cs( bFind == true );
	assert_cs( pOrgNode != NULL );

	// 다크맵 오브젝트가 아니라면
	if( ( pUserDefine->s_dwPlag & sUSER_DEFINE::UD_DARKMAP ) == 0 )
	{
		NiCloningProcess cp;
		cp.m_eCopyType = NiObjectNET::COPY_EXACT;
		pDestNode->s_pNiNode = NiDynamicCast( NiNode, pOrgNode->s_pNiNode->Clone( cp ) );
		pDestNode->s_nGetID = pOrgNode->s_nGetID;
		return;
	}
	// 다크맵 오브젝트라면 버퍼 생성 카피
	pDestNode->s_pNiNode = NiDynamicCast( NiNode, pOrgNode->s_pNiNode->CreateDeepCopy() );
	pDestNode->s_nGetID = pOrgNode->s_nGetID;	
	_CloneTexturingProp( pDestNode->s_pNiNode, pOrgNode->s_pNiNode );
}

NiNodePtr CsGBObjectResMng::GetNiObject( const char* cObjectPath, sUSER_DEFINE* pUserDefine )
{
	// User Define 에서 만들었으므로 무조건 존재해야한다.
	sNINODE_INFO* pOrgNode = NULL;
	bool bFind = m_pmapNiObject->GetAt( cObjectPath, pOrgNode );
	assert_cs( bFind == true );
	assert_cs( pOrgNode != NULL );

	// 다크맵 오브젝트가 아니라면
	if( ( pUserDefine->s_dwPlag & sUSER_DEFINE::UD_DARKMAP ) == 0 )
	{
		NiCloningProcess cp;
		cp.m_eCopyType = NiObjectNET::COPY_EXACT;
		return NiDynamicCast( NiNode, pOrgNode->s_pNiNode->Clone( cp ) );		
	}
	// 다크맵 오브젝트라면 버퍼 생성 카피
	NiNodePtr pReturnNode = NiDynamicCast( NiNode, pOrgNode->s_pNiNode->CreateDeepCopy() );
	_CloneTexturingProp( pReturnNode, pOrgNode->s_pNiNode );
	return pReturnNode;
}

int CsGBObjectResMng::_PopGetID()
{
	static int nGetID = 0;
	if( m_queueGetID.empty() == false )
	{
		int nReturn = m_queueGetID.front();
		m_queueGetID.pop();
		return nReturn;
	}
	return ++nGetID;
}

void CsGBObjectResMng::_PushGetID( int nID )
{
	m_queueGetID.push( nID );
}

bool CsGBObjectResMng::_CheckDMObject( const char* cObjectPath, NiNodePtr pNiNode )
{
	// 툴상에서 - TB Object 라면
	sUSER_DEFINE* pUserDefine = NULL;
	m_pmapUserDefine->GetAt( cObjectPath, pUserDefine );
	if( ( pUserDefine->s_dwPlag & sUSER_DEFINE::UD_DARKMAP ) != 0 )
	{
		// 텍스쳐 셋 검사
		assert_cs( pNiNode->GetArrayCount() != 0 );
		NiAVObject* pObject = nsCSGBFUNC::GetEnableFirstNode( pNiNode );		

		while( NiIsKindOf( NiGeometry, pObject ) == false )
		{
			assert_cs( NiIsKindOf( NiNode, pObject ) == true );
			assert_csm1( ( (NiNode*)pObject )->GetArrayCount() != 0, "_CheckDMObject = %s", cObjectPath );
			pObject = ( (NiNode*)pObject )->GetAt( 0 );
		}
		assert_cs( NiIsKindOf( NiGeometry, pObject ) == true );
		// 하나라면 하나 더 늘려주고 재 세이브
		if( ( (NiGeometry*)pObject )->GetModelData()->GetTextureSets() == 1 )
		{
			if( _DMObjectAppendUV( pNiNode ) == false )
				return false;
			pNiNode->Update( 0.0f );
			NiStream ReSaveStream;
			ReSaveStream.InsertObject( pNiNode );
			ReSaveStream.Save( cObjectPath );
		}
	}
	return true;
}

bool CsGBObjectResMng::_DMObjectAppendUV( NiNode* pNiNode )
{
	assert_cs( NiIsKindOf( NiNode, pNiNode ) == true );
	int nChildCount = pNiNode->GetArrayCount();
	for( int i=0; i<nChildCount; ++i )
	{
		NiAVObject* pChild = pNiNode->GetAt( i );
		if( NiIsKindOf( NiNode, pChild ) == true )
		{
			if( _DMObjectAppendUV( (NiNode*)pChild ) == false )
				return false;
		}
		else if( NiIsKindOf( NiGeometry, pChild ) == true )
		{
			if( pChild->GetAppCulled() == false )
			{
				NiGeometry* pGeom = ( NiGeometry* )pChild;
				if( pGeom->GetProperty( NiTexturingProperty::GetType() ) == NULL )
				{
					MessageBox( NULL, _T( "DarkMap 오브젝트에 텍스쳐가 없는 메쉬가 있습니다.\n이 메쉬는 DarkMap 오브젝트로 동작 하지 않습니다." ), NULL, MB_OK );
					return false;
				}
				NiPoint2* pAppendUV = NiNew NiPoint2[ pGeom->GetVertexCount() ];
				pGeom->GetModelData()->SetConsistency( NiGeometryData::MUTABLE );
				pGeom->GetModelData()->AppendTextureSet( pAppendUV );
				pGeom->GetModelData()->MarkAsChanged( NiGeometryData::TEXTURE_MASK );
				NiDelete[] pAppendUV;
			}			
		}
	}
	return true;
}

void CsGBObjectResMng::_CloneTexturingProp( NiNode* pDestNiNode, NiNode* pSrcNiNode )
{
	assert_cs( NiIsKindOf( NiNode, pSrcNiNode ) == true );
	assert_cs( NiIsKindOf( NiNode, pDestNiNode ) == true );
	int nChildCount = pSrcNiNode->GetArrayCount();
	for( int i=0; i<nChildCount; ++i )
	{
		NiAVObject* pSrcChild = pSrcNiNode->GetAt( i );
		if( NiIsKindOf( NiNode, pSrcChild ) == true )
		{
			_CloneTexturingProp( (NiNode*)pDestNiNode->GetAt( i ),(NiNode*)pSrcChild );
		}
		else if( NiIsKindOf( NiGeometry, pSrcChild ) == true )
		{
			if( pSrcChild->GetAppCulled() == false )
			{
				NiGeometry* pSrcGeom = ( NiGeometry* )pSrcChild;
				NiGeometry* pDestGeom = ( NiGeometry* )pDestNiNode->GetAt( i );

				NiTexturingProperty* pSrcTexProp = (NiTexturingProperty*)pSrcGeom->GetProperty( NiTexturingProperty::GetType() );
				pDestGeom->RemoveProperty( NiTexturingProperty::GetType() );
				pDestGeom->AttachProperty( (NiTexturingProperty*)pSrcTexProp->Clone() );
			}
		}
	}
}


//===============================================================================================
//
// UserDefine
//
//===============================================================================================

void CsGBObjectResMng::_DeleteUserDefine()
{
	if( m_pmapUserDefine == NULL )
		return;

	sUSER_DEFINE* pObjUD = NULL;
	NiTMapIterator itUD = m_pmapUserDefine->GetFirstPos();
	while( itUD != NULL )
	{
		const char* pcName;
		m_pmapUserDefine->GetNext( itUD, pcName, pObjUD );
		pObjUD->Delete();
		// 데이터 제거
		sUSER_DEFINE::DeleteInstance( pObjUD );
	}
	m_pmapUserDefine->RemoveAll();

	NiDelete m_pmapUserDefine;
	m_pmapUserDefine = NULL;
}

void CsGBObjectResMng::_InitUserDefine()
{
	assert_cs( m_pmapUserDefine == NULL );
	m_pmapUserDefine = NiNew NiTStringPointerMap< sUSER_DEFINE* >;
}

sUSER_DEFINE* CsGBObjectResMng::GetUserDefine( CsGBObject::sINFO* pObjectInfo )
{
	sUSER_DEFINE* pReturnUserDefine;
	bool bFind = m_pmapUserDefine->GetAt( pObjectInfo->s_cObjectPath, pReturnUserDefine );
	if( bFind == false )
	{
		sNINODE_INFO* pNiNodeInfo = NULL;
		assert_cs( m_pmapNiObject->GetAt( pObjectInfo->s_cObjectPath, pNiNodeInfo ) == false );
		// NiNide 로딩
		NiStream stream;
		if( stream.Load( pObjectInfo->s_cObjectPath ) == false )
		{
			TCHAR szMsg[ 1024 ];
			char cMsg[ 1024 ];
			sprintf_s( cMsg, 1024, "\n오브젝트 로드 실패\n\n\nObject Path = %s\n", pObjectInfo->s_cObjectPath );
			M2W( szMsg, cMsg, 1024 );
			MessageBox( NULL, szMsg, NULL, MB_OK );
			return NULL;
		}

		pNiNodeInfo = csnew sNINODE_INFO;
		// Some pack-shipped NIF streams contain >1 top-level object; tolerate it,
		// just take the first. Only fail if there's literally nothing to load.
		if( stream.GetObjectCount() == 0 )
		{
			SAFE_DELETE( pNiNodeInfo );
			return NULL;
		}
		pNiNodeInfo->s_pNiNode = ( NiNode* )stream.GetObjectAt( 0 );
		if( !NiIsKindOf( NiNode, pNiNodeInfo->s_pNiNode ) )
		{
			SAFE_DELETE( pNiNodeInfo );
			return NULL;
		}
		pNiNodeInfo->s_nGetID = _PopGetID();
		m_pmapNiObject->SetAt( pObjectInfo->s_cObjectPath, pNiNodeInfo );

		// 모든 텍스쳐에 TEXTURE_FILTER_TYPE 설정
		nsCSGBFUNC::SetTextureFilterMode( pNiNodeInfo->s_pNiNode, nsCsGBTerrain::g_eTexFilter );

		// UserDefine 설정
		pReturnUserDefine = _ParcingUserDefined( pObjectInfo, pNiNodeInfo->s_pNiNode );
		m_pmapUserDefine->SetAt( pObjectInfo->s_cObjectPath, pReturnUserDefine );

		// 비지블 플래그를 얻어 온다
		CsGBVisible::GetPlag( pReturnUserDefine->s_dwVisiblePlag, pNiNodeInfo->s_pNiNode );

		// 툴상에서만 Base Object 체크
		if( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD )
		{
			if( _CheckDMObject( pObjectInfo->s_cObjectPath, pNiNodeInfo->s_pNiNode ) == false )
			{
				pReturnUserDefine->s_dwPlag &= ~ sUSER_DEFINE::UD_DARKMAP;
			}
		}
		// 디버그상에서만 조건검사
		#ifdef _DEBUG
			if( ( pReturnUserDefine->s_dwPlag & sUSER_DEFINE::UD_DARKMAP ) != 0 )
			{
				assert_cs( pNiNodeInfo->s_pNiNode->GetArrayCount() != 0 );

				NiAVObject* pObject = nsCSGBFUNC::GetEnableFirstNode( pNiNodeInfo->s_pNiNode );

				while( NiIsKindOf( NiGeometry, pObject ) == false )
				{
					assert_cs( NiIsKindOf( NiNode, pObject ) == true );
					assert_cs( ( (NiNode*)pObject )->GetArrayCount() != 0 );
					pObject = ( (NiNode*)pObject )->GetAt( 0 );
				}
				assert_cs( NiIsKindOf( NiGeometry, pObject ) == true );
				assert_cs( ( (NiGeometry*)pObject )->GetModelData()->GetTextureSets() >= 2 );
			}			
		#endif /*_DEBUG*/
	}
	return pReturnUserDefine;
}

sUSER_DEFINE* CsGBObjectResMng::_ParcingUserDefined( CsGBObject::sINFO* pObjectInfo, NiNode* pNiNode )
{
	static char cToken[ MAX_FILENAME ];

	NiAVObject* pChild = nsCSGBFUNC::GetEnableFirstNode( pNiNode );	

	sUSER_DEFINE* pUserDefine = sUSER_DEFINE::NewInstance();
	pUserDefine->Reset();

	NiStringExtraData* pExtraData = (NiStringExtraData*)pChild->GetExtraData( "UserPropBuffer" );
	if( NULL == pExtraData )
	{
		assert_csm1( pExtraData != NULL, "Max Property 에서 UserDefine을 정의하지 않았습니다.\n\n%s", pObjectInfo->s_cObjectPath );
		return pUserDefine;
	}

	const char* cUserDefine = (const char*)pExtraData->GetValue();
	
	nsCSFILE::sTOKEN_c token( cUserDefine );
	while( token.Next( cToken, MAX_FILENAME ) != NULL )
	{
		if( strcmp( cToken, "no" ) == 0 )
		{
			assert_cs( pUserDefine->s_dwPlag == 0 );
			return pUserDefine;
		}
		else if( strcmp( cToken, "ns" ) == 0 )
		{
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_NOSHADOW;
		}				
		else if( strcmp( cToken, "ca" ) == 0 )
		{
			assert_csm1( NiGetCollisionData( pNiNode ) != NULL, "%s\n\n오브젝트에 바운딩볼륨이 설치되지 않았습니다.", pObjectInfo->s_cObjectPath );
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_CAMERA_ALPHA;
		}
		else if( strcmp( cToken, "cap" ) == 0 )
		{
			assert_csm1( pNiNode->GetArrayCount() > 1, ( "%s\n\n오브젝트에 충돌박스 관력 박스가 존재하지 않습니다. (cap 이름의 오브젝트)" ), pObjectInfo->s_cObjectPath );
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_CAMERA_ALPHA_PICK;
		}
		else if( strcmp( cToken, "cc" ) == 0 )
		{
			assert_csm1( pNiNode->GetArrayCount() > 1, ( "%s\n\n오브젝트에 충돌박스 관력 박스가 존재하지 않습니다. (cap 이름의 오브젝트)" ), pObjectInfo->s_cObjectPath );
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_CAMERA_COLLISION;
		}
		else if( strcmp( cToken, "tbp" ) == 0 )
		{
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_TERRAIN_BASE_PICK;
		}
		else if( strcmp( cToken, "dm" ) == 0 )
		{
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_DARKMAP | sUSER_DEFINE::UD_CHAR_SHADOW;
			// 쉐도우맵 텍스쳐 사이즈
			sUDD_DARKMAP* pUDD = csnew sUDD_DARKMAP;
			pUDD->s_dwTerrainBaseShadowMapSize = atoi( token.Next( cToken, MAX_FILENAME ) );
			assert_csm1( pUDD->s_dwTerrainBaseShadowMapSize != 0, "%s\n\n dm 다음에 텍스쳐 사이즈가 와야 합니다.", pObjectInfo->s_cObjectPath );
			pUserDefine->InsertData( pUDD, sUDD_DARKMAP::Type() );
		}
		else if( strcmp( cToken, "shadow") == 0 )
		{
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_CHAR_SHADOW;
		}
		else if( strcmp( cToken, "nl" ) == 0 )
		{
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_NO_LIGHT;
		}
		else if( strcmp( cToken, "nf" ) == 0 )
		{
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_NO_FOG;
		}
		else if( strcmp( cToken, "ef" ) == 0 )
		{
			assert_csm1( ( pUserDefine->s_dwPlag & sUSER_DEFINE::UD_NO_CLIPPING ) == 0, "%s\n이펙트 랜더링과 노 클리핑은 같이 쓸수 없습니다.", pObjectInfo->s_cObjectPath );
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_EFFECTRENDER;
		}		
		else if( strcmp( cToken, "ro" ) == 0 )
		{
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_ROTATION;
			sUDD_Rotation* pUDD = csnew sUDD_Rotation;
			pUDD->s_fRotSpeed = CsD2R( atoi( token.Next( cToken, MAX_FILENAME ) ) )/60.0f;
			assert_csm( pUDD->s_fRotSpeed != 0.0f, "ro, 회전속도 가 나와야 합니다." );
			pUserDefine->InsertData( pUDD, sUDD_Rotation::Type() );
		}
		else if( strcmp( cToken, "rol" ) == 0 )
		{
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_ROTATION_LIMIT;
			sUDD_Rotation* pUDD = csnew sUDD_Rotation;
			pUDD->s_fRotSpeed = CsD2R( atoi( token.Next( cToken, MAX_FILENAME ) ) )/60.0f;
			assert_csm( pUDD->s_fRotSpeed != 0.0f, "rol, 회전속도, 제한최소각, 제한 최대각 이 나와야 합니다." );
			pUDD->s_fRotLimit_Min = CsD2R( atoi( token.Next( cToken, MAX_FILENAME ) ) );
			assert_csm( pUDD->s_fRotLimit_Min != 0.0f, "rol, 회전속도, 제한최소각, 제한 최대각 이 나와야 합니다." );
			pUDD->s_fRotLimit_Max = CsD2R( atoi( token.Next( cToken, MAX_FILENAME ) ) );
			assert_csm( pUDD->s_fRotLimit_Max != 0.0f, "rol, 회전속도, 제한최소각, 제한 최대각 이 나와야 합니다." );
			pUserDefine->InsertData( pUDD, sUDD_Rotation::Type() );			
		}
// 		else if( strcmp( cToken, "nc" ) == 0 )
// 		{			
// 			assert_csm1( ( pUserDefine->s_dwPlag & sUSER_DEFINE::UD_EFFECTRENDER ) == 0, "%s\n이펙트 랜더링과 노 클리핑은 같이 쓸수 없습니다.", pObjectInfo->s_cObjectPath );
// 			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_NO_CLIPPING;
// 		}
		else if( strcmp( cToken, "nopick" ) == 0 )
		{
			assert_csm1( pUserDefine->s_dwPlag == 0, "%s\nnopick 는 오직 단독으로 쓰여야 합니다.", pObjectInfo->s_cObjectPath );
			pUserDefine->s_dwPlag = sUSER_DEFINE::UD_NO_PICK | sUSER_DEFINE::UD_NO_FOG | sUSER_DEFINE::UD_NO_LIGHT;
		}
		else if( strcmp( cToken, "table" ) == 0 )
		{
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_TABLE_OBJ;
		}
		else if( strcmp( cToken, "queston" ) == 0 )
		{
			assert_csm( ( pUserDefine->s_dwPlag & sUSER_DEFINE::UD_QUEST_OFF ) == 0, "queston 과 questoff는 둘중에 하나만 쓸수 있습니다." );

			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_QUEST_ON;

			sUDD_QuestOn* pUDD = NULL;
			if( pUserDefine->IsData( sUSER_DEFINE::UD_QUEST_ON ) == true )
			{
				pUDD = (sUDD_QuestOn*)pUserDefine->GetData( sUSER_DEFINE::UD_QUEST_ON );
			}
			else
			{
				pUDD = csnew sUDD_QuestOn;
				pUserDefine->InsertData( pUDD, sUDD_QuestOn::Type() );
			}
			assert_cs( pUDD != NULL );

			sUDD_QuestOn::sINFO* pQuestInfo = csnew sUDD_QuestOn::sINFO;
			pUDD->s_listInfo.push_back( pQuestInfo );
			
			int nCnt = atoi( token.Next( cToken, MAX_FILENAME ) );
			assert_cs( nCnt > 0 );
			for( int i=0; i<nCnt; ++i )
			{
				int nQuestID = atoi( token.Next( cToken, MAX_FILENAME ) );
				assert_cs( nQuestID > 0 );				
				pQuestInfo->s_listQuest.push_back( nQuestID );
			}
		}
		else if( strcmp( cToken, "questoff" ) == 0 )
		{
			assert_csm( ( pUserDefine->s_dwPlag & sUSER_DEFINE::UD_QUEST_ON ) == 0, "queston 과 questoff는 둘중에 하나만 쓸수 있습니다." );

			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_QUEST_OFF;

			sUDD_QuestOff* pUDD = NULL;
			if( pUserDefine->IsData( sUSER_DEFINE::UD_QUEST_OFF ) == true )
			{
				pUDD = (sUDD_QuestOff*)pUserDefine->GetData( sUSER_DEFINE::UD_QUEST_OFF );
			}
			else
			{
				pUDD = csnew sUDD_QuestOff;
				pUserDefine->InsertData( pUDD, sUDD_QuestOff::Type() );
			}
			assert_cs( pUDD != NULL );

			sUDD_QuestOff::sINFO* pQuestInfo = csnew sUDD_QuestOff::sINFO;
			pUDD->s_listInfo.push_back( pQuestInfo );
			
			int nCnt = atoi( token.Next( cToken, MAX_FILENAME ) );
			assert_cs( nCnt > 0 );
			for( int i=0; i<nCnt; ++i )
			{
				int nQuestID = atoi( token.Next( cToken, MAX_FILENAME ) );
				assert_cs( nQuestID > 0 );
				pQuestInfo->s_listQuest.push_back( nQuestID );
			}
		}


		//else if( strcmp( cToken, "tt" ) == 0 )
		//{
		//	pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_USE_TEXTYPE;

		//	// 맵 인덱스
		//	sUDD_TexType* pUDD = NULL;
		//	
		//	if( pUserDefine->IsData( sUDD_TexType::Type() ) == false )
		//	{
		//		pUDD = csnew sUDD_TexType;
		//		pUserDefine->InsertData( pUDD, sUDD_TexType::Type() );
		//	}
		//	else
		//	{
		//		pUDD = (sUDD_TexType*)pUserDefine->GetData( sUDD_TexType::Type() );
		//	}

		//	int nTexGroupIndex = (int)pUDD->s_mapTextureGroup.size();
		//	assert_cs( nTexGroupIndex < 8 );
		//	assert_cs( pUDD->s_mapTextureGroup.find( nTexGroupIndex ) == pUDD->s_mapTextureGroup.end() );

		//	// Material ID - max는 1부터 시작이기 때문에 1감소
		//	int nMatID = atoi( token.Next( cToken, MAX_FILENAME ) ) - 1;
		//	assert_cs( nMatID >= 0 );

		//	// Texture 갯수
		//	int nTexCount = atoi( token.Next( cToken, MAX_FILENAME ) );
		//	assert_cs( nTexCount < 16 );
		//	assert_cs( nTexCount > 0 );
		//	// Texture 이름
		//	sUDD_TexType::sTEXTURE_GROUP* pUDTexGroup = NiNew sUDD_TexType::sTEXTURE_GROUP;
		//	pUDTexGroup->s_btMaterialID = nMatID;
		//	for( int i=0; i<nTexCount; ++ i )
		//	{
		//		nsCSFILE::sFileName_c* pTexFileName = csnew nsCSFILE::sFileName_c;
		//		strcpy_s( pTexFileName->s_cFileName, pTexFileName->GetLength(), token.Next( cToken, MAX_FILENAME ) );
		//		assert_cs( pUDTexGroup->s_mapFileName.find( i ) == pUDTexGroup->s_mapFileName.end() );
		//		pUDTexGroup->s_mapFileName[ i ] = pTexFileName;
		//	}
		//	pUDD->s_mapTextureGroup[ nTexGroupIndex ] = pUDTexGroup;

		//	//=======================================================================================
		//	// 초기 텍스쳐로 원본 셋팅
		//	char cCurPath[ MAX_PATH ] = {0, };
		//	char cTexPath[ MAX_PATH ] = {0, };
		//	nsCSFILE::GetFilePathOnly( cCurPath, OBJECT_PATH_LEN, pObjectInfo->s_cObjectPath );

		//	NiAVObject* pChild = nsCSGBFUNC::GetEnableFirstNode( pNiNode );			

		//	NiTriBasedGeom* pTriObject = NULL;			
		//	if( NiIsKindOf( NiNode, pChild ) == true )
		//	{
		//		pTriObject = (NiTriBasedGeom*)( (NiNode*)pChild )->GetAt( pUDTexGroup->s_btMaterialID );
		//		assert_csm1( NiIsKindOf( NiTriBasedGeom, pTriObject ) == true, "올바른 형식의 오브젝트가 아닙니다.\n오브젝트는 단일 메쉬여야 합니다.\n\n%s", pObjectInfo->s_cObjectPath );
		//		NiTexturingProperty* pOrgProp = NiDynamicCast( NiTexturingProperty, pTriObject->GetProperty( NiTexturingProperty::GetType() ) );
		//		assert_csm1( pOrgProp != NULL, "오브젝트에 텍스쳐가 없는상태로 익스포트 하면 안됩니다.\n폴더안에 해당 텍스쳐가 있을필요는 없습니다.\n\n%s", pObjectInfo->s_cObjectPath );
		//		// 텍스쳐 경로
		//		sprintf_s( cTexPath, MAX_PATH, "%s\\%s", cCurPath, pUDTexGroup->s_mapFileName[ 0 ]->s_cFileName );
		//		//assert_csm1( _access( cTexPath, 0 ) == 0, "로드할 텍스쳐가 존재하지 않습니다.\n\n%s", cTexPath );
		//		NiTexturingPropertyPtr pTexProp = nsCsGBTerrain::g_ObjectResMgr.GetTexture( cTexPath, pOrgProp->GetApplyMode() );

		//		NiTexturingProperty::Map* pMap = NiNew NiTexturingProperty::Map( *pTexProp->GetBaseMap() );				
		//		pOrgProp->SetBaseMap( pMap );
		//		pTriObject->UpdateProperties();

		//		pTexProp = 0;
		//	}
		//	else
		//	{
		//		assert_csm1( nTexGroupIndex == 0, "멀티앤 서브 오브젝트가 아닌것은 TextureType 형식이 하나여야 합니다.\n\n%s", pObjectInfo->s_cObjectPath );
		//		assert_csm1( NiIsKindOf( NiTriBasedGeom, pChild ) == true, "올바른 형식의 오브젝트가 아닙니다.\n오브젝트는 단일 메쉬여야 합니다.\n\n%s", pObjectInfo->s_cObjectPath );
		//		assert_csm1( pUDTexGroup->s_btMaterialID == 0 , "멀티앤 서브 오브젝트가 아닌것은 Material ID 가 1 이여야만 합니다.\n\n%s", pObjectInfo->s_cObjectPath );
		//		pTriObject = (NiTriBasedGeom*)pChild;
		//		NiTexturingProperty* pOrgProp = NiDynamicCast( NiTexturingProperty, pTriObject->GetProperty( NiTexturingProperty::GetType() ) );
		//		assert_csm1( pOrgProp != NULL, "오브젝트에 텍스쳐가 없는상태로 익스포트 하면 안됩니다.\n폴더안에 해당 텍스쳐가 있을필요는 없습니다.\n\n%s", pObjectInfo->s_cObjectPath );			
		//		// 텍스쳐 경로
		//		sprintf_s( cTexPath, MAX_PATH, "%s\\%s", cCurPath, pUDTexGroup->s_mapFileName[ 0 ]->s_cFileName );
		//		//assert_csm1( _access( cTexPath, 0 ) == 0, "로드할 텍스쳐가 존재하지 않습니다.\n\n%s", cTexPath );			
		//		NiTexturingPropertyPtr pTexProp = nsCsGBTerrain::g_ObjectResMgr.GetTexture( cTexPath, pOrgProp->GetApplyMode() );

		//		NiTexturingProperty::Map* pMap = NiNew NiTexturingProperty::Map( *pTexProp->GetBaseMap() );
		//		pOrgProp->SetBaseMap( pMap );
		//		pTriObject->UpdateProperties();

		//		pTexProp = 0;
		//	}
		//}
		else if( strcmp( cToken, "f1" ) == 0 )
		{
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_F1;
		}
		else if( strcmp( cToken, "ani" ) == 0 )
		{
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_ANIMATION;
			nsCSGBFUNC::InitAnimation( pNiNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
		}
		else if( strcmp( cToken, "awani" ) == 0 )
		{
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_ALWAYS_ANIMATION;
			nsCSGBFUNC::InitAnimation( pNiNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
		}
		else if( strcmp( cToken, "anir" ) == 0 )
		{
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_ANIMATION_RANDOM;
			nsCSGBFUNC::InitAnimation( pNiNode, NiTimeController::APP_TIME, NiTimeController::LOOP );

			// 쉐도우맵 텍스쳐 사이즈
			sUDD_ANI_RANDOM* pUDD = csnew sUDD_ANI_RANDOM;
			pUDD->s_nStartFrame = atoi( token.Next( cToken, MAX_FILENAME ) );
			pUDD->s_nEndFrame = atoi( token.Next( cToken, MAX_FILENAME ) );
			pUserDefine->InsertData( pUDD, sUDD_ANI_RANDOM::Type() );
		}
		else if( strcmp( cToken, "pt" ) == 0 )
		{
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_PORTAL;
		}
		else if( strcmp( cToken, "thp" ) == 0 )
		{
			assert_csm1( pNiNode->GetArrayCount() > 1, _T( "%s\n\nSelected object does not exist" ), pObjectInfo->s_cObjectPath );
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_TERRAIN_HIDE_PICK;
		}
		else if( strcmp( cToken, "sd" ) == 0 )
		{			
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_SOUND;

			// 사운드
			sUDD_Sound* pUDD = csnew sUDD_Sound;
			pUDD->s_fLoopTime = (float)atof( token.Next( cToken, MAX_FILENAME ) );
			pUDD->s_fMinDist = (float)atof( token.Next( cToken, MAX_FILENAME ) )*100.0f;
			pUDD->s_fMaxDist = (float)atof( token.Next( cToken, MAX_FILENAME ) )*100.0f;
			sprintf_s( pUDD->s_cFileName, MAX_FILENAME, "Map\\%s", token.Next( cToken, MAX_FILENAME ) );
			pUserDefine->InsertData( pUDD, sUDD_Sound::Type() );
		}
		else if( strcmp( cToken, "skin" ) == 0 )
		{
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_SKIN;
		}
		else if( strcmp( cToken, "actor" ) == 0 )
		{
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_ACTORMANAGER;

			sUDD_ACTOR* pUDD = csnew sUDD_ACTOR;
			strcpy_s( pUDD->s_cKey, 32, token.Next( cToken, MAX_FILENAME ) );
			pUserDefine->InsertData( pUDD, sUDD_ACTOR::Type() );
		}
		else if( strcmp( cToken, "act" ) == 0 )
		{
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_ACT;

			sUDD_ACT* pUDD = csnew sUDD_ACT;
			pUDD->s_nHashCode = (DWORD)CsFPS::GetHashCode( token.Next( cToken, MAX_FILENAME ) );
			pUserDefine->InsertData( pUDD, sUDD_ACT::Type() );
		}
		else if( strcmp( cToken, "path" ) == 0 )
		{
			pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_PATH;

			sUDD_PATH* pUDD = csnew sUDD_PATH;
			pUDD->s_nHashCode = (DWORD)CsFPS::GetHashCode( token.Next( cToken, MAX_FILENAME ) );
			pUDD->s_nPos[ 0 ] = atoi( token.Next( cToken, MAX_FILENAME ) );
			pUDD->s_nPos[ 1 ] = atoi( token.Next( cToken, MAX_FILENAME ) );
			pUDD->s_nData[ 0 ] = atoi( token.Next( cToken, MAX_FILENAME ) );
			pUDD->s_nData[ 1 ] = atoi( token.Next( cToken, MAX_FILENAME ) );
			pUserDefine->InsertData( pUDD, sUDD_PATH::Type() );
		}

		//else if( strcmp( cToken, "mg" ) == 0 )
		//{
		//	// 툴에서만 사용
		//	if( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD )
		//	{
		//		pUserDefine->s_dwPlag |= sUSER_DEFINE::UD_MERGE;				

		//		sUDD_Merge* pUDD = NULL;
		//		if( pUserDefine->IsData( sUDD_Merge::Type() ) == false )
		//		{
		//			pUDD = csnew sUDD_Merge;
		//			pUserDefine->InsertData( pUDD, sUDD_Merge::Type() );
		//		}
		//		else
		//		{
		//			pUDD = (sUDD_Merge*)pUserDefine->GetData( sUDD_Merge::Type() );
		//		}
		//		// ID
		//		DWORD dwMergeID = atoi( token.Next( cToken, MAX_FILENAME ) );
		//		sUDD_Merge::sINFO* pInfo = csnew sUDD_Merge::sINFO;

		//		pInfo->s_nSlotID = (int)dwMergeID;

		//		// 파일 네임
		//		sprintf_s( pInfo->s_cFileName, MAX_PATH, "Data\\Map\\%s", token.Next( cToken, MAX_FILENAME ) );
		//		assert_cs( pUDD->s_mapInfo.find( dwMergeID ) == pUDD->s_mapInfo.end() );
		//		pUDD->s_mapInfo[ dwMergeID ] = pInfo;
		//	}			
		//}		

		assert_csm1( strcmp( cToken, "tt" ) != 0, "tt 프로퍼티를 빼주세요! \n%s", pObjectInfo->s_cObjectPath );
		assert_csm1( strcmp( cToken, "mg" ) != 0, "mg 프로퍼티를 빼주세요! \n%s", pObjectInfo->s_cObjectPath );
		assert_csm1( strcmp( cToken, "sp" ) != 0, "sp 프로퍼티를 빼주세요! \n%s", pObjectInfo->s_cObjectPath );
	}

	assert_csm1( ( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_FILELOAD )||( pUserDefine->s_dwPlag != 0 ),
				"Max Property 에서 UserDefine이 제대로 정의되지 않았습니다.\n마지막은 콤마로 끝내주세욧!!\n\n%s", pObjectInfo->s_cObjectPath );

	//======================================================================================
	//
	//	파싱 종료 처리
	//
	//======================================================================================
	// 머지 라면 mg 풀어서 보관
	/*if( ( pUserDefine->s_dwPlag & sUSER_DEFINE::UD_MERGE ) != 0 )
	{
		sUDD_Merge* pNewUDD = csnew sUDD_Merge;
		sUDD_Merge* pUDD = (sUDD_Merge*)pUserDefine->GetData( sUDD_Merge::Type() );

		int nSlotID = 0;
		int nChildCount = pNiNode->GetArrayCount();
		for( int i=0; i<nChildCount; ++i )
		{
			const char* cName = pNiNode->GetAt( i )->GetName();
			if( strncmp( cName, "Dummy_mg", 8 ) == 0 )
			{
				DWORD dwMergeID = atoi( &cName[ 8 ] );
				sUDD_Merge::sINFO* pInfo = csnew sUDD_Merge::sINFO;
				pInfo->s_nSlotID = nSlotID;
				pInfo->s_nNiNodeChildIndex = i;
				strcpy_s( pInfo->s_cFileName, MAX_PATH, pUDD->s_mapInfo[ dwMergeID ]->s_cFileName );
				assert_cs( pNewUDD->s_mapInfo.find( nSlotID ) == pNewUDD->s_mapInfo.end() );
				pNewUDD->s_mapInfo[ nSlotID ] = pInfo;

				++nSlotID;
			}
		}
		pUserDefine->DeleteData( sUDD_Merge::Type() );

		assert_csm1( pNewUDD->s_mapInfo.size() > 0, "%s\nDummy_mg 더미가 뽑혀져 나오지 않았습니다. 체크해보세요", pObjectInfo->s_cObjectPath );
		pUserDefine->InsertData( pNewUDD, sUDD_Merge::Type() );
	}*/

	return pUserDefine;
}

//===============================================================================================
//
// Texture
//
//===============================================================================================
void CsGBObjectResMng::_DeleteTexture()
{
	if( m_pmapTexture == NULL )
		return;

	NiTexturingPropertyPtr pTexProp = NULL;
	NiTMapIterator it = m_pmapTexture->GetFirstPos();
	while( it != NULL )
	{
		const char* pcName;		
		m_pmapTexture->GetNext( it, pcName, pTexProp );
		pTexProp = 0;
	}
	m_pmapTexture->RemoveAll();

	NiDelete m_pmapTexture;
	m_pmapTexture = NULL;
}

void CsGBObjectResMng::_InitTexture()
{
	assert_cs( m_pmapTexture == NULL );
	m_pmapTexture = NiNew NiTStringPointerMap< NiTexturingPropertyPtr >;
}

NiTexturingProperty* CsGBObjectResMng::GetTexture( const char* cTexturePath, NiTexturingProperty::ApplyMode eApply )
{
	NiTexturingPropertyPtr pReturnTexProp = NULL;
	bool bFind = m_pmapTexture->GetAt( cTexturePath, pReturnTexProp );
	if( bFind == false )
	{
		pReturnTexProp = NiNew NiTexturingProperty( cTexturePath );
		
		assert_cs( pReturnTexProp != NULL );
		pReturnTexProp->SetApplyMode( eApply );
		pReturnTexProp->SetBaseClampMode( NiTexturingProperty::WRAP_S_WRAP_T );
		pReturnTexProp->SetBaseFilterMode( nsCsGBTerrain::g_eTexFilter );
		m_pmapTexture->SetAt( cTexturePath, pReturnTexProp );
	}
	assert_cs( pReturnTexProp != NULL );

	return NiDynamicCast( NiTexturingProperty, pReturnTexProp->Clone() );
}

NiTexturingProperty* CsGBObjectResMng::GetTexture( NiPixelData* pPData )
{
	return NiNew NiTexturingProperty( pPData );	
}

NiTexturingProperty* CsGBObjectResMng::GetTexture()
{
	return NiNew NiTexturingProperty;
}

NiTexturingProperty::ShaderMap* CsGBObjectResMng::GetShaderMap( const char* cTexturePath )
{
	NiTexturingPropertyPtr pReturnTexProp = NULL;
	bool bFind = m_pmapTexture->GetAt( cTexturePath, pReturnTexProp );
	if( bFind == false )
	{
		pReturnTexProp = NiNew NiTexturingProperty( cTexturePath );
		assert_cs( pReturnTexProp != NULL );
		pReturnTexProp->SetApplyMode(TEXTURE_APPLY_TYPE);		
		pReturnTexProp->SetBaseClampMode( NiTexturingProperty::WRAP_S_WRAP_T );
		pReturnTexProp->SetBaseFilterMode( nsCsGBTerrain::g_eTexFilter );		
		m_pmapTexture->SetAt( cTexturePath, pReturnTexProp );
	}

	NiTexturingProperty::ShaderMap* pShaderMap = NiNew NiTexturingProperty::ShaderMap(
			pReturnTexProp->GetBaseTexture(),
			0,
			NiTexturingProperty::WRAP_S_WRAP_T,
			nsCsGBTerrain::g_eTexFilter,
			0 );
	assert_cs( pShaderMap != NULL );
	
	return pShaderMap;
}

NiTexturingProperty::ShaderMap* CsGBObjectResMng::GetShaderMap( int nWidth, int nHeight, NiPixelFormat pf, DWORD dwInitColor )
{
	assert_cs( ( pf == NiPixelFormat::RGB24 )||( pf == NiPixelFormat::RGBA32 ) );

	NiPixelData* pPData = NiNew NiPixelData( nWidth, nHeight, pf );
	BYTE* pPixel = (BYTE*)pPData->GetPixels();
	DWORD count = pPData->GetHeight()*pPData->GetWidth();
	for( DWORD j=0; j<count; ++j )
	{
		if( pPData->GetPixelStride() == 3 )
		{
			pPixel[ j*pPData->GetPixelStride() + 0 ] = CS_GetRValue( dwInitColor );
			pPixel[ j*pPData->GetPixelStride() + 1 ] = CS_GetGValue( dwInitColor );
			pPixel[ j*pPData->GetPixelStride() + 2 ] = CS_GetBValue( dwInitColor );
		}
		else if( pPData->GetPixelStride() == 4 )
		{
			pPixel[ j*pPData->GetPixelStride() + 0 ] = CS_GetRValue( dwInitColor );
			pPixel[ j*pPData->GetPixelStride() + 1 ] = CS_GetGValue( dwInitColor );
			pPixel[ j*pPData->GetPixelStride() + 2 ] = CS_GetBValue( dwInitColor );
			pPixel[ j*pPData->GetPixelStride() + 3 ] = CS_GetAValue( dwInitColor );
		}
	}

	NiSourceTexture* pSourceTex = NiSourceTexture::Create( pPData );
	if( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD )
		pSourceTex->SetStatic( false );
	NiTexturingProperty::ShaderMap* pShaderMap = NiNew NiTexturingProperty::ShaderMap(
		pSourceTex, 0, NiTexturingProperty::WRAP_S_WRAP_T, nsCsGBTerrain::g_eTexFilter, 0 );
	return pShaderMap;
}

NiTexturingProperty::ShaderMap* CsGBObjectResMng::GetShaderMap( int nWidth, int nHeight, NiPixelFormat pf, BYTE btInitColor )
{
	assert_cs( pf == NiPixelFormat::I8 );

	NiPixelData* pPData = NiNew NiPixelData( nWidth, nHeight, pf );
	BYTE* pPixel = (BYTE*)pPData->GetPixels();
	DWORD count = pPData->GetHeight()*pPData->GetWidth();
	for( DWORD j=0; j<count; ++j )
	{
		pPixel[ j*pPData->GetPixelStride() ] = btInitColor;
	}

	NiSourceTexture* pSourceTex = NiSourceTexture::Create( pPData );
	if( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD )
		pSourceTex->SetStatic( false );
	NiTexturingProperty::ShaderMap* pShaderMap = NiNew NiTexturingProperty::ShaderMap(
		pSourceTex, 0, NiTexturingProperty::WRAP_S_WRAP_T, nsCsGBTerrain::g_eTexFilter, 0 );
	return pShaderMap;
}

NiTexturingProperty::ShaderMap* CsGBObjectResMng::GetShaderMap( int nWidth, int nHeight, NiPixelFormat pf, NiTexturingProperty::ShaderMap* pSrcShader,
															   DWORD dwInitColor )
{
	if( pSrcShader == NULL )
	{
		if( pf == NiPixelFormat::I8 )
			return GetShaderMap( nWidth, nHeight, pf, (BYTE)dwInitColor );
		else
			return GetShaderMap( nWidth, nHeight, pf, (DWORD)dwInitColor );
	}

	NiPixelData* pPData = NiNew NiPixelData( *( (NiSourceTexture*)pSrcShader->GetTexture() )->GetSourcePixelData() );
	NiSourceTexture* pSourceTex = NiSourceTexture::Create( pPData );
	if( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD )
		pSourceTex->SetStatic( false );
	NiTexturingProperty::ShaderMap* pShaderMap = NiNew NiTexturingProperty::ShaderMap(
		pSourceTex, 0, NiTexturingProperty::WRAP_S_WRAP_T, nsCsGBTerrain::g_eTexFilter, 0 );
	return pShaderMap;
}

NiTexturingProperty::ShaderMap* CsGBObjectResMng::GetShaderMap( NiPixelData* pPData )
{
	assert_cs( pPData != NULL );

	NiSourceTexture* pSourceTex = NiSourceTexture::Create( pPData );

	if( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD )
		pSourceTex->SetStatic( false );

	NiTexturingProperty::ShaderMap* pShaderMap = NiNew NiTexturingProperty::ShaderMap(
		pSourceTex, 0, NiTexturingProperty::WRAP_S_WRAP_T, nsCsGBTerrain::g_eTexFilter, 0 );
	return pShaderMap;
}

void CsGBObjectResMng::GetColor( NiPixelData* pPixelData, NiPoint2 fUV, BYTE& r, BYTE& g, BYTE& b )
{
	if( pPixelData == NULL )
		return;

	BYTE* pPixel = (BYTE*)pPixelData->GetPixels();

	int nWidth = pPixelData->GetWidth();
	int nHeight = pPixelData->GetHeight();
	int nBP = pPixelData->GetPixelStride();
	fUV.x *= nWidth;
	fUV.y *= nHeight;
	fUV.y = (nHeight - 1) - fUV.y;
	float du, dv;
	int x, y;

	// 입력 좌표로 변경
	x = (int)fUV.x;
	y = (int)fUV.y;
	du = fUV.x - x;
	dv = fUV.y - y;

	if( ( x < nWidth - 1 )&&( y < nHeight - 1 ) )
	{
		r = (BYTE)(
			( (1.0f-du)*(1.0f-dv)*pPixel[ ( y*nWidth+x )*nBP + 0 ] ) +
			( du*(1.0f-dv)*pPixel[ ( y*nWidth+(x+1) )*nBP + 0 ] ) +
			( (1.0f-du)*dv*pPixel[ ( (y+1)*nWidth+x )*nBP + 0 ] ) +
			( du*dv*pPixel[ ( (y+1)*nWidth+(x+1) )*nBP + 0 ] ) );

		g = (BYTE)(
			( (1.0f-du)*(1.0f-dv)*pPixel[ ( y*nWidth+x )*nBP + 1 ] ) +
			( du*(1.0f-dv)*pPixel[ ( y*nWidth+(x+1) )*nBP + 1 ] ) +
			( (1.0f-du)*dv*pPixel[ ( (y+1)*nWidth+x )*nBP + 1 ] ) +
			( du*dv*pPixel[ ( (y+1)*nWidth+(x+1) )*nBP + 1 ] ) );

		b = (BYTE)(
			( (1.0f-du)*(1.0f-dv)*pPixel[ ( y*nWidth+x )*nBP + 2 ] ) +
			( du*(1.0f-dv)*pPixel[ ( y*nWidth+(x+1) )*nBP + 2 ] ) +
			( (1.0f-du)*dv*pPixel[ ( (y+1)*nWidth+x )*nBP + 2 ] ) +
			( du*dv*pPixel[ ( (y+1)*nWidth+(x+1) )*nBP + 2 ] ) );
	}
	else if( x < nWidth - 1 )
	{
		r = (BYTE)(
			( (1.0f-du)*pPixel[ ( y*nWidth+x )*nBP + 0 ] ) +
			( du*pPixel[ ( y*nWidth+(x+1) )*nBP + 0 ] ) );

		g = (BYTE)(
			( (1.0f-du)*pPixel[ ( y*nWidth+x )*nBP + 1 ] ) +
			( du*pPixel[ ( y*nWidth+(x+1) )*nBP + 1 ] ) );

		b = (BYTE)(
			( (1.0f-du)*pPixel[ ( y*nWidth+x )*nBP + 2 ] ) +
			( du*pPixel[ ( y*nWidth+(x+1) )*nBP + 2 ] ) );
	}
	else if( y < nHeight - 1 )
	{
		r = (BYTE)(
			( (1.0f-dv)*pPixel[ ( y*nWidth+x )*nBP + 0 ] ) +
			( dv*pPixel[ ( (y+1)*nWidth+x )*nBP + 0 ] ) );

		g = (BYTE)(
			( (1.0f-dv)*pPixel[ ( y*nWidth+x )*nBP + 1 ] ) +
			( dv*pPixel[ ( (y+1)*nWidth+x )*nBP + 1 ] ) );

		b = (BYTE)(
			( (1.0f-dv)*pPixel[ ( y*nWidth+x )*nBP + 2 ] ) +
			( dv*pPixel[ ( (y+1)*nWidth+x )*nBP + 2 ] ) );
	}
	else
	{
		r =	(BYTE)( (1.0f-du)*(1.0f-dv)*pPixel[ ( y*nWidth+x )*nBP + 0 ] );
		g =	(BYTE)( (1.0f-du)*(1.0f-dv)*pPixel[ ( y*nWidth+x )*nBP + 1 ] );
		b =	(BYTE)( (1.0f-du)*(1.0f-dv)*pPixel[ ( y*nWidth+x )*nBP + 2 ] );
	}
}

void CsGBObjectResMng::GetColorAverage( const char* cTexturePath, BYTE& r, BYTE& g, BYTE& b )
{
	NiTexturingProperty::ShaderMap* pShaderMap = nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( cTexturePath );
	NiSourceTexture* pSourceTex = (NiSourceTexture*)pShaderMap->GetTexture();
	pSourceTex->LoadPixelDataFromFile();
	NiPixelData* pPData = pSourceTex->GetSourcePixelData();

	NiPixelDataPtr pDestPixelData = NiNew NiPixelData( pPData->GetWidth(), pPData->GetHeight(), NiPixelFormat::RGB24 );
	NiImageConverter::GetImageConverter()->ConvertPixelData( *pPData, NiPixelFormat::RGB24, pDestPixelData, false );

	BYTE* pPixel = (BYTE*)pDestPixelData->GetPixels();
	DWORD dr = 0;
	DWORD dg = 0;
	DWORD db = 0;
	for( unsigned int h=0; h<pPData->GetHeight(); ++h )
	{
		for( unsigned int w=0; w<pPData->GetWidth(); ++w )
		{
			dr += pPixel[ ( h*pPData->GetWidth() + w )*3 + 0 ];
			dg += pPixel[ ( h*pPData->GetWidth() + w )*3 + 1 ];
			db += pPixel[ ( h*pPData->GetWidth() + w )*3 + 2 ];
		}
	}
	DWORD dCount = pPData->GetHeight()*pPData->GetWidth();
	r = (BYTE)( dr/dCount );
	g = (BYTE)( dg/dCount );
	b = (BYTE)( db/dCount );

	NiDelete pShaderMap;
	pDestPixelData = 0;
}

void CsGBObjectResMng::SetObjPath( CsGBObject::sINFO* pObjectInfo )
{
	//Csp 파일이 없을때만 설정. 기존에 있는 오브젝트 패쓰 안건들기
	char cCsp[ MAX_PATH ];
	size_t nLen;

	strcpy_s( cCsp, OBJECT_PATH_LEN, pObjectInfo->s_cObjectPath );
	nLen = strlen( cCsp );
	strcpy_s( &cCsp[ nLen - 3 ], MAX_PATH-(nLen-3), "csp" );
	if( _access_s( cCsp, 0 ) != 0 )
	{
		//Property 확인, 없으면 안됨
		sUSER_DEFINE* pUserDefine = GetUserDefine( pObjectInfo );
		if( pUserDefine == NULL )
			return;

		// 오브젝트에 저장되있는 충돌 데이터 얻어오기
		sNINODE_INFO NiNodeInfo;
		GetNiObject( &NiNodeInfo, pObjectInfo->s_cObjectPath, pUserDefine );

		NiCollisionObject* pCollisionData = NiNodeInfo.s_pNiNode->GetCollisionObject();
		// 	NiCollisionData::PropagationMode eCollMode = pData->GetPropagationMode();

		// 기존의 포탈 오브젝트에도 충돌데이터가 들어가있어서 포탈도 걸러줌
		if( pCollisionData != NULL
			&& ( pUserDefine->s_dwPlag & sUSER_DEFINE::UD_PORTAL ) == 0 )
		{
			NiCollisionData* pData = (NiCollisionData*)pCollisionData;
			if( pData->GetPropagationMode() != NiCollisionData::PROPAGATE_NEVER )
				return;

			// 패스 정보 초기화
			pObjectInfo->s_PathInfo.Delete();
			pObjectInfo->s_PathInfo.Reset();

			// 충돌박스
			NiBoxBV* pBox = pData->GetModelABVBox();
			// 충돌박스가 여러개 일 경우 Union타입으로 저장되있을테니 타입 검사
			int nType = pBox->Type();
			NiUnionBV* pUnion;

			UINT nCnt = 1; // 충돌박스 갯수
			if( nType == NiBoundingVolume::UNION_BV )// 2개 이상의 충돌박스일 때 Union 사용
			{
				pUnion = (NiUnionBV*)pData->GetModelABVBox();
				nCnt = pUnion->GetSize();	

				//이때, PropagationMode가 PROPAGATE_ON_SUCCESS인 경우 패쓰는 만들면 안되는데
				//Union의 PropagationMode가 이미 PROPAGATE_NEVER이므로 각 바운딩볼륨으로는 확인 안됨
				//그래서 박스가 아닌 경우만 체크. 이래서 Csp 파일로 만든건가봄...충돌박스 마음대로 만들려고..
				int nNoBoxCnt = 0;//박스가 아닌 바운딩 볼륨 수
				for( UINT i = 0 ; i < nCnt ; ++i )
				{
					const NiBoundingVolume* pBoundingVolume = pUnion->GetBoundingVolume(i);
					int nTypeChild = ( (NiBoxBV*)pBoundingVolume )->Type();
					if( nTypeChild != NiBoundingVolume::BOX_BV )
						nNoBoxCnt++;
				}
				nCnt -= nNoBoxCnt;
			}

			// 박스 수만큼 패스 생성
			pObjectInfo->s_PathInfo.s_uiPEGroupCount = nCnt;
			pObjectInfo->s_PathInfo.s_pPECount = csnew USHORT[ pObjectInfo->s_PathInfo.s_uiPEGroupCount ];
			pObjectInfo->s_PathInfo.s_pptTranse = NiNew NiPoint2[ pObjectInfo->s_PathInfo.s_uiPEGroupCount ];

			pObjectInfo->s_PathInfo.s_uiTotalPECount = nCnt*4;	//박스 당 점 4개
			pObjectInfo->s_PathInfo.s_pData = csnew long[ 2*pObjectInfo->s_PathInfo.s_uiTotalPECount ];	// 점 좌표 데이터

			int nOffset = 0;	// 데이터 저장에 쓸 인덱스 오프셋

			for( UINT i = 0 ; i < nCnt ; ++i )
			{
				if( nType == NiBoundingVolume::UNION_BV )
				{
					//Union일 경우 BoundingVolume으로 각 충돌박스 데이터 얻을 수 있음
					const NiBoundingVolume* pBoundingVolume = pUnion->GetBoundingVolume(i);
					pBox = (NiBoxBV*)pBoundingVolume;
				}

				// 충돌박스의 위치값으로 패쓰 위치 셋팅
				NiPoint3 ptOffset =  pBox->GetCenter();

				ptOffset.z = 0.0f;
				ptOffset = pObjectInfo->s_trObject.m_Rotate*ptOffset*pObjectInfo->s_trObject.m_fScale;

				pObjectInfo->s_PathInfo.s_pptTranse[ i ].x = pObjectInfo->s_trObject.m_Translate.x + ptOffset.x;
				pObjectInfo->s_PathInfo.s_pptTranse[ i ].y = pObjectInfo->s_trObject.m_Translate.y + ptOffset.y;

				// 패쓰도 점 4개임
				pObjectInfo->s_PathInfo.s_pPECount[ i ] = 4;

				// 패스 설정
				NiPoint3 ptPath;
				ptPath.z = 0.0f;

				for( UINT uiPECount=0; uiPECount<pObjectInfo->s_PathInfo.s_pPECount[ i ]; ++uiPECount )
				{
					//NiBox 구하기
					ptPath.x = pBox->GetAxis(0).x*pBox->GetBox().m_afExtent[ 0 ];
					ptPath.y = pBox->GetAxis(1).y*pBox->GetBox().m_afExtent[ 0 ];

					if( ( uiPECount > 0 ) && (uiPECount < 3 ) )
						ptPath.x = -ptPath.x;
					if( (uiPECount < 2 ) )
						ptPath.y = -ptPath.y;

					NiTransform trVertPos;
					trVertPos = pObjectInfo->s_trObject;
					trVertPos.m_Translate = NiPoint3::ZERO;

					ptPath = trVertPos*ptPath;

					pObjectInfo->s_PathInfo.s_pData[ nOffset + uiPECount*2 + 0 ] = (long)( ptPath.x + 0.5f );
					pObjectInfo->s_PathInfo.s_pData[ nOffset + uiPECount*2 + 1 ] = (long)( ptPath.y + 0.5f );
				}

				nOffset += pObjectInfo->s_PathInfo.s_pPECount[ i ]*2;
			}
		}
	}
}

