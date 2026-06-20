#include "stdafx.h"
#include "CsGBSkyBox.h"

CSGBMEMPOOL_CPP( CsGBSkyBox::sGEOM_INFO );

CsGBSkyBox::CsGBSkyBox()
{
	m_pNiNode = NULL;
}

CsGBSkyBox::~CsGBSkyBox()
{
	m_pNiNode = NULL;
}

void CsGBSkyBox::Delete()
{
	std::list< sGEOM_INFO* >::iterator it = m_listGeomInfo.begin();
	std::list< sGEOM_INFO* >::iterator itEnd = m_listGeomInfo.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->s_pNiGeom->Delete();
		CGeometry::DelInstance( (*it)->s_pNiGeom );
		CsGBSkyBox::sGEOM_INFO::DeleteInstance( *it );
	}
	m_listGeomInfo.clear();

	m_SkyBox.Delete();
	m_pNiNode = 0;
}

void CsGBSkyBox::Create()
{
	assert_cs( m_SkyBox.m_pNiNode == NULL );

	if( !CsFPS::CsFPSystem::IsExist( 0, m_Info.s_cSkyBox ) )
		return;

	NiStream stream;
	bool bSuccess = stream.Load( m_Info.s_cSkyBox );
	if( bSuccess == false )
	{
		assert_csm1( bSuccess == true, "스카이 박스 오브젝트가 존재하지 않습니다.\n\n%s", m_Info.s_cSkyBox );
		return;
	}	
	m_pNiNode = (NiNode*)stream.GetObjectAt( 0 );
	m_pNiNode->Update( 0.0f );
	
	std::map< int, sGEOM_INFO* >	mapSort;
	UINT uiChildCount = m_pNiNode->GetArrayCount();
	NiGeometryPtr pNiGeom = NULL;
	for( UINT i=0; i<uiChildCount; ++i )
	{		
		sGEOM_INFO* pGeomInfo = CsGBSkyBox::sGEOM_INFO::NewInstance();
		pGeomInfo->Reset();

		pGeomInfo->s_pNiGeom = (CGeomTR*)CGeometry::GetInstance( NULL, CGeometry::UseTR );
		pGeomInfo->s_pNiGeom->SetGeometry( (CsNiGeometry*)m_pNiNode->GetAt( i ) );		

		_ParcingUserDefine( pGeomInfo, &mapSort );
		assert_cs( ( pGeomInfo->s_dwType & GTMASK_POS ) != 0 );
		m_listGeomInfo.push_back( pGeomInfo );
	}
	
	m_SkyBox.SetNiObject( m_pNiNode, CGeometry::UseTR );

	if( mapSort.empty() == false )
	{
		std::list< sGEOM_INFO* >::iterator it = m_listGeomInfo.begin();
		std::list< sGEOM_INFO* >::iterator itEnd = m_listGeomInfo.end();
		while( it != itEnd )
		{
			bool bFind = false;
			std::map< int, sGEOM_INFO* >::iterator itSort = mapSort.begin();
			std::map< int, sGEOM_INFO* >::iterator itEndSort = mapSort.end();
			for( ; itSort!=itEndSort; ++itSort )
			{
				if( (*it) == itSort->second )
				{
					bFind = true;
					it = m_listGeomInfo.erase( it );
					break;
				}
			}
			if( bFind == false )
				++it;
		}
			

		std::map< int, sGEOM_INFO* >::iterator itSort = mapSort.begin();
		std::map< int, sGEOM_INFO* >::iterator itEndSort = mapSort.end();
		for( ; itSort!=itEndSort; ++itSort )
		{
			m_listGeomInfo.push_back( itSort->second );
		}
	}
}

void CsGBSkyBox::Update( float fElapsedTime )
{
	if( m_SkyBox.m_pNiNode == NULL )
		return;

	std::list< sGEOM_INFO* >::iterator it = m_listGeomInfo.begin();
	std::list< sGEOM_INFO* >::iterator itEnd = m_listGeomInfo.end();
	for( ; it!=itEnd; ++it )
	{
		if( ( (*it)->s_dwType & GTD_ROTATION ) != 0 )
			_Update_Rotation( fElapsedTime, (*it) );

		// 가장 마지막 업데이트 체크
		if( ( (*it)->s_dwType & GT_ANIMATION ) != 0 )
			_Update_Animation( fElapsedTime, (*it) );
	}
}

//void CsGBSkyBox::ResistVisibleArray( NiCamera* pNiCamera, NiCullingProcess* pCuller )
//{
//	if( m_pNiNode == NULL )
//		return;
//
//	pCuller->Process( pNiCamera, m_pNiNode, NULL );
//}

void CsGBSkyBox::Render()
{
	if( m_SkyBox.m_pNiNode != NULL )
	{
		std::list< sGEOM_INFO* >::iterator it = m_listGeomInfo.begin();
		std::list< sGEOM_INFO* >::iterator itEnd = m_listGeomInfo.end();
		for( ; it!=itEnd; ++it )
		{
			if( ( (*it)->s_dwType & GT_USER_SORTING ) == 0 )
				g_Sorting.Set( (*it)->s_pNiGeom );
			else
				(*it)->s_pNiGeom->m_pGeometry->RenderImmediate( NiRenderer::GetRenderer() );
		}
	}
}

void CsGBSkyBox::Render_Shader( char* cSkinShader, char* cObjectShader )
{
	if( m_SkyBox.m_pNiNode != NULL )
	{
		std::list< sGEOM_INFO* >::iterator it = m_listGeomInfo.begin();
		std::list< sGEOM_INFO* >::iterator itEnd = m_listGeomInfo.end();
		NiMaterial* pOldMat = NULL;
		NiGeometry* pNiGeom = NULL;
		NiSkinInstance* pNiSkin = NULL;
		for( ; it!=itEnd; ++it )
		{
			pNiGeom = (*it)->s_pNiGeom->m_pGeometry;
			pOldMat = (NiMaterial*)pNiGeom->GetActiveMaterial();

			pNiSkin = pNiGeom->GetSkinInstance();
			if( ( pNiSkin == NULL )||( pNiSkin->GetSkinPartition() == NULL ) )
			{
				(*it)->s_pNiGeom->m_pGeometry->ApplyAndSetActiveMaterial( cObjectShader );
			}
			else
			{
				(*it)->s_pNiGeom->m_pGeometry->ApplyAndSetActiveMaterial( cSkinShader );
			}			
			pNiGeom->RenderImmediate( NiRenderer::GetRenderer() );
			pNiGeom->SetActiveMaterial( pOldMat );
		}
	}
}


void CsGBSkyBox::SetViewerPos( NiPoint3 pos )
{
	pos.z += m_Info.s_fHeight;	 

	std::list< sGEOM_INFO* >::iterator it = m_listGeomInfo.begin();
	std::list< sGEOM_INFO* >::iterator itEnd = m_listGeomInfo.end();
	for( ; it!=itEnd; ++it )
	{
		if( ( (*it)->s_dwType & GT_POS_CHAR_FOLLOW ) != 0 )
		{
			NiPoint3 posGeom = pos + (*it)->s_vOrgPos;
			(*it)->s_pNiGeom->m_pGeometry->SetTranslate( posGeom );
			(*it)->s_pNiGeom->m_pGeometry->Update( 0.0f, false );
		}
	}
}

void CsGBSkyBox::SetHeight( float fHeight )
{
	m_Info.s_fHeight = fHeight;

	std::list< sGEOM_INFO* >::iterator it = m_listGeomInfo.begin();
	std::list< sGEOM_INFO* >::iterator itEnd = m_listGeomInfo.end();
	for( ; it!=itEnd; ++it )
	{
		if( ( (*it)->s_dwType & GT_POS_STATIC ) != 0 )
		{
			NiPoint3 pos = NiPoint3( nsCsGBTerrain::g_pCurRoot->GetMapSizeWidth()*0.5f, nsCsGBTerrain::g_pCurRoot->GetMapSizeHeight()*0.5f, m_Info.s_fHeight );
			pos += (*it)->s_vOrgPos;
			(*it)->s_pNiGeom->m_pGeometry->SetTranslate( pos );
			(*it)->s_pNiGeom->m_pGeometry->Update( (*it)->s_fAniTime );
		}
	}
}

void CsGBSkyBox::_ParcingUserDefine( sGEOM_INFO* pGeomInfo, std::map< int, sGEOM_INFO* >* pmapSort )
{
	char cToken[ MAX_FILENAME ];

	NiStringExtraData* pExtraData = (NiStringExtraData*)pGeomInfo->s_pNiGeom->m_pGeometry->GetExtraData( "UserPropBuffer" );
	assert_csm( pExtraData != NULL, "Sky Box의 UserDefine을 정의하지 않았습니다." );
	const char* cUserDefine = (const char*)pExtraData->GetValue();

	pGeomInfo->s_vOrgPos = pGeomInfo->s_pNiGeom->m_pGeometry->GetTranslate();	

	nsCSFILE::sTOKEN_c token( cUserDefine );
	while( token.Next( cToken, MAX_FILENAME ) != NULL )
	{
		if( strcmp( cToken, "st" ) == 0 )
		{
			assert_cs( ( pGeomInfo->s_dwType & GTMASK_POS ) == 0 );
			pGeomInfo->s_dwType |= GT_POS_STATIC;
			NiPoint3 pos = NiPoint3( nsCsGBTerrain::g_pCurRoot->GetMapSizeWidth()*0.5f, nsCsGBTerrain::g_pCurRoot->GetMapSizeHeight()*0.5f, m_Info.s_fHeight );
			pos += pGeomInfo->s_vOrgPos;
			pGeomInfo->s_pNiGeom->m_pGeometry->SetTranslate( pos );
			pGeomInfo->s_pNiGeom->m_pGeometry->Update( 0.0f );
		}
		else if( strcmp( cToken, "cf" ) == 0 )
		{
			assert_cs( ( pGeomInfo->s_dwType & GTMASK_POS ) == 0 );
			pGeomInfo->s_dwType |= GT_POS_CHAR_FOLLOW;
		}
		else if( strcmp( cToken, "ro" ) == 0 )
		{
			pGeomInfo->s_dwType |= GT_ROTATION;
			pGeomInfo->s_fValue1 = CsD2R( atoi( token.Next( cToken, MAX_FILENAME ) ) )/60.0f;
			assert_csm( pGeomInfo->s_fValue1 != 0.0f, "Sky Box 의 UserDefine 에서 ro 다음에 회전각이 나와야 합니다." );
			pGeomInfo->s_fRotLoopTime = NI_TWO_PI / pGeomInfo->s_fValue1;
			assert_cs( pGeomInfo->s_fRotLoopTime != 0.0f );
			pGeomInfo->s_fAccumeTime = 0.0f;
		}
		else if( strcmp( cToken, "net_ro" ) == 0 )
		{
			pGeomInfo->s_dwType |= GT_NET_ROTATION;
			pGeomInfo->s_fValue1 = CsD2R( atoi( token.Next( cToken, MAX_FILENAME ) ) )/60.0f;
			assert_csm( pGeomInfo->s_fValue1 != 0.0f, "Sky Box 의 UserDefine 에서 ro 다음에 회전각이 나와야 합니다." );			
			pGeomInfo->s_fRotLoopTime = NI_TWO_PI / pGeomInfo->s_fValue1;
			assert_cs( pGeomInfo->s_fRotLoopTime != 0.0f );
			pGeomInfo->s_fAccumeTime = -1.0f;
		}
		else if( strcmp( cToken, "ani" ) == 0 )
		{
			pGeomInfo->s_dwType |= GT_ANIMATION;
			pGeomInfo->s_fAniPlayRate = 1.0f;
			nsCSGBFUNC::InitAnimation( pGeomInfo->s_pNiGeom->m_pGeometry, NiTimeController::APP_TIME, NiTimeController::LOOP );
		}
		else if( strcmp( cToken, "anif" ) == 0 )
		{
			pGeomInfo->s_dwType |= GT_ANIMATION;
			nsCSGBFUNC::InitAnimation( pGeomInfo->s_pNiGeom->m_pGeometry, NiTimeController::APP_TIME, NiTimeController::LOOP );

			pGeomInfo->s_fAniPlayRate = atoi( token.Next( cToken, MAX_FILENAME ) )*0.001f;
		}
		else if( strcmp( cToken, "sort" ) == 0 )
		{
			pGeomInfo->s_dwType |= GT_USER_SORTING;
			int nSortIndex = atoi( token.Next( cToken, MAX_FILENAME ) );
			assert_csm1( pmapSort->find( nSortIndex ) == pmapSort->end(), _T( "sort 유저디파인에 같은 번호가 존재함. 번호 : %d" ), nSortIndex );
			( *pmapSort )[ nSortIndex ] = pGeomInfo;
		}
	}
}

void CsGBSkyBox::_Update_Rotation( float fElapsedTime, sGEOM_INFO* pGeomInfo )
{
	// 아직정의 되지 않았다면
	if( pGeomInfo->s_fAccumeTime == -1.0f )
	{
		pGeomInfo->s_fAccumeTime = nsCsGBTerrain::g_ServerTime.GetInitLoopAccumTime( pGeomInfo->s_fRotLoopTime );
	}

	pGeomInfo->s_fAccumeTime += fElapsedTime;
	while( pGeomInfo->s_fAccumeTime > pGeomInfo->s_fRotLoopTime )
		pGeomInfo->s_fAccumeTime -= pGeomInfo->s_fRotLoopTime;

	pGeomInfo->s_fRotation = pGeomInfo->s_fAccumeTime / pGeomInfo->s_fRotLoopTime * NI_TWO_PI;

	NiMatrix3 matRot;
	matRot.MakeZRotation( pGeomInfo->s_fRotation );
	pGeomInfo->s_pNiGeom->m_pGeometry->SetRotate( matRot );

	if( ( ( pGeomInfo->s_dwType & ~GTD_ROTATION ) & GTMASK_UPDATE ) == 0 )
		pGeomInfo->s_pNiGeom->m_pGeometry->Update( 0.0f, false );
}

void CsGBSkyBox::_Update_Animation( float fElapsedTime, sGEOM_INFO* pGeomInfo )
{
	pGeomInfo->s_fAniTime += fElapsedTime*pGeomInfo->s_fAniPlayRate;
	pGeomInfo->s_pNiGeom->m_pGeometry->Update( pGeomInfo->s_fAniTime );
}


//===============================================================================================
//
//		Extra Data
//
//===============================================================================================

void CsGBSkyBox::SaveExtraData( CsNiNodePtr pBaseNiNode )
{
	// ==== 인포 저장	
	assert_cs( sizeof( char ) == 1 );
	// 저장할 사이즈	
	unsigned int uiSize = sizeof( sINFO );

	// 메모리 할당
	char* pInfo = NiAlloc( char, uiSize );
	memcpy( pInfo, &m_Info, uiSize );

	NiBinaryExtraData* pExtData = NiNew NiBinaryExtraData( uiSize, pInfo );
	pBaseNiNode->AddExtraData( CsGBSkyBox_ExtraKey, pExtData );
}

void CsGBSkyBox::LoadExtraData( CsNiNodePtr pNiNode )
{
	NiBinaryExtraData* pExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBSkyBox_ExtraKey );
	assert_cs( pExtData != NULL );
	unsigned int uiSize;
	char* pData;
	pExtData->GetValue( uiSize, pData );
	memcpy( &m_Info, pData, sizeof( sINFO ) );
}
