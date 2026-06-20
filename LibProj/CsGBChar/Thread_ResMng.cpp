
#include "stdafx.h"
#include "Thread_ResMng.h"

#define REF_CHECK_TIME		300000			// 3분		1000 * 60 * 3
//#define REF_CHECK_TIME		1000

CsC_Thread_ResMng::CsC_Thread_ResMng()
{
	m_pDefaultLight = NULL;
	m_pSettingFog = NULL;

	m_pmapNiPart = NULL;
	m_pmapNiObject = NULL;
	m_pmapKFMObject = NULL;
}

CsC_Thread_ResMng::~CsC_Thread_ResMng()
{
}

void CsC_Thread_ResMng::Init()
{
	assert_cs( m_pmapNiPart == NULL );
	assert_cs( m_pmapNiObject == NULL );
	assert_cs( m_pmapKFMObject == NULL );
	m_pmapNiPart = NiNew NiTStringPointerMap< sLOAD_PART* >;
	m_pmapNiObject = NiNew NiTStringPointerMap< sLOAD_NIF* >;
	m_pmapKFMObject = NiNew NiTStringPointerMap< sLOAD_KFM* >;
}

void CsC_Thread_ResMng::Destroy()
{
	_SYNCHRONIZE_THREAD_( &m_CS );

	m_pDefaultLight = NULL;
	m_pSettingFog = NULL;
	m_SettingLightList.clear();

	if( m_pmapNiPart != NULL )
	{
		sLOAD_PART* pPart = NULL;
		NiTMapIterator itObj = m_pmapNiPart->GetFirstPos();
		while( itObj != NULL )
		{
			const char* pcName;
			m_pmapNiPart->GetNext( itObj, pcName, pPart );
			pPart->Delete();
			SAFE_DELETE( pPart );
		}
		m_pmapNiPart->RemoveAll();
		SAFE_NIDELETE( m_pmapNiPart );
	}


	if( m_pmapNiObject != NULL )
	{
		sLOAD_NIF* pObject = NULL;
		NiTMapIterator itObj = m_pmapNiObject->GetFirstPos();
		while( itObj != NULL )
		{
			const char* pcName;
			m_pmapNiObject->GetNext( itObj, pcName, pObject );

			pObject->Delete();
			SAFE_DELETE( pObject );
		}
		m_pmapNiObject->RemoveAll();
		SAFE_NIDELETE( m_pmapNiObject );
	}

	if( m_pmapKFMObject != NULL )
	{
		sLOAD_KFM* pObject = NULL;
		NiTMapIterator itObj = m_pmapKFMObject->GetFirstPos();
		while( itObj != NULL )
		{
			const char* pcName;
			m_pmapKFMObject->GetNext( itObj, pcName, pObject );

			pObject->Delete();
			SAFE_DELETE( pObject );
		}
		m_pmapKFMObject->RemoveAll();
		SAFE_NIDELETE( m_pmapKFMObject );
	}
}

void CsC_Thread_ResMng::RefCheck()
{
	_SYNCHRONIZE_THREAD_( &m_CS );

	DWORD dwCurTime = GetTickCount();
	
	_RefCheck_Part( dwCurTime );
	_RefCheck_NIF( dwCurTime );
	_RefCheck_KFM( dwCurTime );
}

void CsC_Thread_ResMng::ReleaseImmediatelyResource()
{
	_SYNCHRONIZE_THREAD_( &m_CS );
	if( m_pmapNiPart != NULL )
	{
		sLOAD_PART* pPart = NULL;
		NiTMapIterator itObj = m_pmapNiPart->GetFirstPos();
		while( itObj != NULL )
		{
			const char* pcName;
			m_pmapNiPart->GetNext( itObj, pcName, pPart );
			if( pPart->s_pRefLoad->GetRef() != 0 )
				continue;

			pPart->Delete();
			SAFE_DELETE( pPart );
			m_pmapNiPart->RemoveAt( pcName );
		}
	}

	if( m_pmapNiObject != NULL )
	{
		sLOAD_NIF* pObject = NULL;
		NiTMapIterator itObj = m_pmapNiObject->GetFirstPos();
		while( itObj != NULL )
		{
			const char* pcName;
			m_pmapNiObject->GetNext( itObj, pcName, pObject );
			if( pObject->s_pRefLoad->GetRef() != 0 )
				continue;

			pObject->Delete();
			SAFE_DELETE( pObject );
			m_pmapNiObject->RemoveAt(pcName);
		}
	}

	if( m_pmapKFMObject != NULL )
	{
		sLOAD_KFM* pObject = NULL;
		NiTMapIterator itObj = m_pmapKFMObject->GetFirstPos();
		while( itObj != NULL )
		{
			const char* pcName;
			m_pmapKFMObject->GetNext( itObj, pcName, pObject );
			if( pObject->s_pRefLoad->GetRef() != 0 )
				continue;

			pObject->Delete();
			SAFE_DELETE( pObject );
			m_pmapKFMObject->RemoveAt(pcName);
		}
	}
}

void CsC_Thread_ResMng::ReleaseConnetTerrain()
{
	_SYNCHRONIZE_THREAD_( &m_CS );	
	m_pSettingFog = NULL;
	m_SettingLightList.clear();
}

void CsC_Thread_ResMng::ApplyConnetTerrain()
{
	_SYNCHRONIZE_THREAD_( &m_CS );

	assert_cs( nsCsGBTerrain::g_pCurRoot != NULL );	
	m_pSettingFog = nsCsGBTerrain::g_pCurRoot->GetFogProperty();
	assert_cs( m_pSettingFog != NULL );

	std::list < CsGBLight* >* pList = nsCsGBTerrain::g_pCurRoot->GetLightMng()->GetCharLightList();
	std::list < CsGBLight* >::iterator it = pList->begin();
	std::list < CsGBLight* >::iterator itEnd = pList->end();
	for( ; it!=itEnd; ++it )
	{
		m_SettingLightList.push_back( ( *it )->GetNiLight() );
	}
}

void CsC_Thread_ResMng::_CheckUserDefine( NiAVObject* pNiObject )
{
	assert_cs( pNiObject != NULL );

	NiStringExtraData* pExtraData = (NiStringExtraData*)pNiObject->GetExtraData( "UserPropBuffer" );
	if( pExtraData )
	{
		char cToken[ MAX_FILENAME ];
		const char* cUserDefine = (const char*)pExtraData->GetValue();
		nsCSFILE::sTOKEN_c token( cUserDefine );
		while( token.Next( cToken, MAX_FILENAME ) != NULL )
		{
			if( strcmp( cToken, "default" ) == 0 )
			{				
				nsCSGBFUNC::SetBit( pNiObject, true, NiAVObject::IS_DEFAULT_SHADER );
			}
			else if( strcmp( cToken, "alphatex" ) == 0 )
			{				
				nsCSGBFUNC::SetBit( pNiObject, true, NiAVObject::IS_ALPHATEX_SHADER );
			}			
		}
	}	

	if( pNiObject->IsNode() )
	{
		assert_cs( NiIsKindOf( NiNode, pNiObject ) );
		int nArrayCnt = ( (NiNode*)pNiObject )->GetArrayCount();
		NiAVObject* pChild = NULL;
		for( int i=0; i<nArrayCnt; ++i )
		{
			pChild = ( (NiNode*)pNiObject )->GetAt( i );
			if( pChild )
			{
				_CheckUserDefine( pChild );
			}
		}
	}
}

//=====================================================================================
//
//	Part Object
//
//=====================================================================================
void CsC_Thread_ResMng::GetPart( sLOAD_PART* pDestObject, const char* cObjectPath )
{
	_SYNCHRONIZE_THREAD_( &m_CS );

	sLOAD_PART* pSrcObject;
	bool bFind = m_pmapNiPart->GetAt( cObjectPath, pSrcObject );
	if( bFind == false )
	{
		// NiNide 로딩
		NiStream stream;
		if( stream.Load( cObjectPath ) == false )
		{
			char cNewPath[ MAX_PATH ] = {0, };
			nsCSFILE::GetFilePathOnly( cNewPath, MAX_PATH, (char*)cObjectPath, false );
			strcat_s( cNewPath, MAX_PATH, "\\dummy.nif" );
			if( stream.Load( cNewPath ) == false )
			{
				return;
			}
		}

		pSrcObject = NiNew sLOAD_PART;
		pSrcObject->s_pRefLoad = NiNew sLOAD_THREAD_REF;

		pSrcObject->s_pNiNode = ( NiNode* )stream.GetObjectAt( 0 );
		_CheckUserDefine( pSrcObject->s_pNiNode );

		assert_cs( NiIsKindOf( NiNode, pSrcObject->s_pNiNode ) == true );

		nsCSGBFUNC::SetMaterial_Color( pSrcObject->s_pNiNode, NiColor::WHITE );
		pSrcObject->s_pNiNode->IncRefCount();
		
		m_pmapNiPart->SetAt( cObjectPath, pSrcObject );
	}

	pSrcObject->Clone( pDestObject );
	pSrcObject->s_pRefLoad->IncreaseRef();	

	bool bSelective, bRigid;
	pDestObject->s_pNiNode->SetSelectiveUpdateFlags( bSelective, true, bRigid );
}

void CsC_Thread_ResMng::_RefCheck_Part( DWORD dwCurTime )
{
	sLOAD_PART* pPart = NULL;
	NiTMapIterator itObj = m_pmapNiPart->GetFirstPos();
	while( itObj != NULL )
	{
		const char* pcName;
		m_pmapNiPart->GetNext( itObj, pcName, pPart );

		// 참조가 0이라면
		if( pPart->s_pRefLoad->GetRef() == 0 )
		{
			// 시간 비교 해서 0된지 오래 榮摸?
			if( ( dwCurTime - pPart->s_pRefLoad->GetLastDeleteTime() ) > REF_CHECK_TIME )
			{
				pPart->Delete();
				SAFE_DELETE( pPart );
				m_pmapNiPart->RemoveAt( pcName );
			}
		}
	}
}


//=====================================================================================
//
//	Ni Object
//
//=====================================================================================
void CsC_Thread_ResMng::GetNIFObject( sLOAD_NIF* pDestObject, const char* cObjectPath, sCREATEINFO* pProp )
{
	_SYNCHRONIZE_THREAD_( &m_CS );

	sLOAD_NIF* pSrcObject = NULL;
	bool bFind = m_pmapNiObject->GetAt( cObjectPath, pSrcObject );
	if( bFind == false )
	{
		// NiNide 로딩
		NiStream stream;
		if( stream.Load( cObjectPath ) == false )
		{
			char cNewPath[ MAX_PATH ] = "Data\\Dummy\\dummy.nif";
			if( stream.Load( cNewPath ) == false )
			{
				return;
			}			
		}
		pSrcObject = NiNew sLOAD_NIF;
		pSrcObject->s_pRefLoad = NiNew sLOAD_THREAD_REF;
		pSrcObject->s_pNiNode = ( NiNode* )stream.GetObjectAt( 0 );

		assert_cs( NiIsKindOf( NiNode, pSrcObject->s_pNiNode ) == true );

		NiNode* pNode = (NiNode*)pSrcObject->s_pNiNode;
		_CheckUserDefine( pNode );

		nsCSGBFUNC::SetMaterial_Color( pNode, NiColor::WHITE );

		NiCollisionData* pkCD = NiGetCollisionData( pNode );
		if( pkCD != NULL )
		{
			NiBoundingVolume* pBoundVolume = pkCD->GetModelSpaceABV();
			pSrcObject->s_pABVWire = NiDrawableBV::CreateWireframeBV( pBoundVolume, NiColor( 0, 0, 1 ) );
			assert_csm( pSrcObject->s_pABVWire != NULL, _T( "충돌박스가 없어요~!! 그래픽팀에게 문의" ) );
			pSrcObject->s_pABVWire->SetName( "CS_ABVWire" );
			pSrcObject->s_pABVWire->SetAppCulled(true);
			pNode->AttachChild( pSrcObject->s_pABVWire );
		}

		// 비지블 플래그 설정
		CsGBVisible::GetPlag( pSrcObject->s_dwVisiblePlag, pSrcObject->s_pNiNode );

	/*	pSrcObject->s_pNiNode->UpdateProperties();
		pSrcObject->s_pNiNode->UpdateEffects();*/

		m_pmapNiObject->SetAt( cObjectPath, pSrcObject );
	}
	
	pSrcObject->Clone( pDestObject, pProp->IsOptionPlag( OP_ABVWIRE ) );

	bool bSelective, bRigid;
	pDestObject->s_pNiNode->SetSelectiveUpdateFlags( bSelective, true, bRigid );

	// 포그
	if( pProp->IsOptionPlag( OP_FOG ) )
	{
		if( nsCsGBTerrain::g_Device.g_bUseMyShader )
		{
			if( m_pSettingFog )
				pDestObject->s_pNiNode->AttachProperty( m_pSettingFog );
		}		
	}

	// 라이트
	if( pProp->IsOptionPlag( OP_LIGHT ) == true )
	{
		if( m_SettingLightList.empty() == false )
		{
			std::list< NiLight* >::iterator it = m_SettingLightList.begin();
			std::list< NiLight* >::iterator itEnd = m_SettingLightList.end();
			for( ; it!=itEnd; ++it )
				pDestObject->s_pNiNode->AttachEffect( *it );
		}
		else if( m_pDefaultLight )
		{
			pDestObject->s_pNiNode->AttachEffect( m_pDefaultLight );
		}
	}

	// 반복
	if( pProp->IsPropPlag( PR_ANI_LOOP ) )
	{
		assert_cs( pProp->IsPropPlag( PR_ANI_ENABLE ) );
		nsCSGBFUNC::InitAnimation( pDestObject->s_pNiNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
	}

	pDestObject->s_pNiNode->UpdateEffects();

	// 레퍼런스 카운트 증가
	pSrcObject->s_pRefLoad->IncreaseRef();
}


void CsC_Thread_ResMng::_RefCheck_NIF( DWORD dwCurTime )
{
	sLOAD_NIF* pObject = NULL;
	NiTMapIterator itObj = m_pmapNiObject->GetFirstPos();
	while( itObj != NULL )
	{
		const char* pcName;
		m_pmapNiObject->GetNext( itObj, pcName, pObject );

		// 참조가 0이라면
		if( pObject->s_pRefLoad->GetRef() == 0 )
		{
			// 시간 비교 해서 0된지 오래 榮摸?
			if( ( dwCurTime - pObject->s_pRefLoad->GetLastDeleteTime() ) > REF_CHECK_TIME )
			{
				pObject->Delete();
				SAFE_DELETE( pObject );
				m_pmapNiObject->RemoveAt( pcName );
			}
		}		
	}
}


//=====================================================================================
//
//	Ni Attach
//
//=====================================================================================
void CsC_Thread_ResMng::GetNIFAttach( sLOAD_NIF* pDestObject, const char* cObjectPath, sCREATEINFO* pProp )
{
	_SYNCHRONIZE_THREAD_( &m_CS );

	sLOAD_NIF* pSrcObject = NULL;
	bool bFind = m_pmapNiObject->GetAt( cObjectPath, pSrcObject );
	if( bFind == false )
	{
		// NiNide 로딩
		NiStream stream;
		if( stream.Load( cObjectPath ) == false )
		{
			char cNewPath[ MAX_PATH ] = "Data\\Dummy\\dummy.nif";
			if( stream.Load( cNewPath ) == false )
			{
				return;
			}
		}
		pSrcObject = NiNew sLOAD_NIF;
		pSrcObject->s_pRefLoad = NiNew sLOAD_THREAD_REF;
		pSrcObject->s_pNiNode = ( NiNode* )stream.GetObjectAt( 0 );

		assert_cs( NiIsKindOf( NiNode, pSrcObject->s_pNiNode ) == true );

		NiNode* pNode = (NiNode*)pSrcObject->s_pNiNode;
		m_pmapNiObject->SetAt( cObjectPath, pSrcObject );
	}
	pSrcObject->Clone( pDestObject, false );
	bool bSelective, bRigid;
	pDestObject->s_pNiNode->SetSelectiveUpdateFlags( bSelective, true, bRigid );

	// 반복
	if( pProp->IsPropPlag( PR_ANI_LOOP ) )
	{
		assert_cs( pProp->IsPropPlag( PR_ANI_ENABLE ) );
		nsCSGBFUNC::InitAnimation( pDestObject->s_pNiNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
	}

	pDestObject->s_pNiNode->UpdateEffects();
	// 레퍼런스 카운트 증가
	pSrcObject->s_pRefLoad->IncreaseRef();
}


//=====================================================================================
//
//	KFM Object
// 
//=====================================================================================
void CsC_Thread_ResMng::GetKFMObject( sLOAD_KFM* pDestObject, const char* cObjectPath, sCREATEINFO* pProp )
{
	_SYNCHRONIZE_THREAD_( &m_CS );

	sLOAD_KFM* pSrcObject = NULL;
	bool bFind = m_pmapKFMObject->GetAt( cObjectPath, pSrcObject );

	if( bFind == false )
	{
		pSrcObject = NiNew sLOAD_KFM;
		pSrcObject->s_pRefLoad = NiNew sLOAD_THREAD_REF;

#ifdef CRASHDUMP_0305			//chu8820 - 오브젝트 생성 실패 ( C003 )
		if( cObjectPath == NULL )
		{
			char cNewPath[ MAX_PATH ] = "Data\\Dummy\\dummy.kfm";
			pSrcObject->s_pActorMng = NiActorManager::Create( cNewPath );
			if( pSrcObject->s_pActorMng == NULL )
			{
				DUMPLOG( " C003 " );
				return;
			}
		}
		else
		{
			pSrcObject->s_pActorMng = NiActorManager::Create( cObjectPath );
			if( pSrcObject->s_pActorMng == NULL )
			{
				char cNewPath[ MAX_PATH ] = "Data\\Dummy\\dummy.kfm";
				pSrcObject->s_pActorMng = NiActorManager::Create( cNewPath );
				if( pSrcObject->s_pActorMng == NULL )
				{
					CsMessageBoxA( MB_OK, "\n오브젝트 로드 실패\n\n\nObject Path = %s\n", cObjectPath );
					return;
				}
			}
		}
#else
		pSrcObject->s_pActorMng = NiActorManager::Create( cObjectPath );
		if( pSrcObject->s_pActorMng == NULL )
		{
			char cNewPath[ MAX_PATH ] = "Data\\Dummy\\dummy.kfm";
			pSrcObject->s_pActorMng = NiActorManager::Create( cNewPath );
			if( pSrcObject->s_pActorMng == NULL )
			{
				CsMessageBoxA( MB_OK, "\n오브젝트 로드 실패\n\n\nObject Path = %s\n", cObjectPath );
				return;
			}
		}
#endif
		NiNode* pNode = (NiNode*)pSrcObject->s_pActorMng->GetNIFRoot();
		_CheckUserDefine( pNode );

		nsCSGBFUNC::SetMaterial_Color( pNode, NiColor::WHITE );

		NiCollisionData* pkCD = NiGetCollisionData( pNode );
		if( pkCD != NULL )
		{
			NiBoundingVolume* pBoundVolume = pkCD->GetModelSpaceABV();
			pSrcObject->s_pABVWire = NiDrawableBV::CreateWireframeBV( pBoundVolume, NiColor( 0, 0, 1 ) );
			assert_csm( pSrcObject->s_pABVWire != NULL, _T( "충돌박스가 없어요~!! 그래픽팀에게 문의" ) );
			pSrcObject->s_pABVWire->SetName( "CS_ABVWire" );						
			pSrcObject->s_pABVWire->SetAppCulled(true);
			pNode->AttachChild( pSrcObject->s_pABVWire );
		}

		// 비지블 플래그 설정
		CsGBVisible::GetPlag( pSrcObject->s_dwVisiblePlag, pNode );

		// 시퀀스매니져 설정
		pSrcObject->s_pSequenceMng = NiNew CsC_LoadSequence;
		pSrcObject->s_pSequenceMng->Init( pSrcObject->s_pActorMng, atoi( nsCSFILE::GetFileName( (char*)cObjectPath ) )%100000 );		
		m_pmapKFMObject->SetAt( cObjectPath, pSrcObject );
	}	
	pSrcObject->Clone( pDestObject, pProp->IsOptionPlag( OP_ABVWIRE ) );
	
	bool bSelective, bRigid;
	pDestObject->s_pActorMng->GetNIFRoot()->SetSelectiveUpdateFlags( bSelective, true, bRigid );

	// 포그
	if( pProp->IsOptionPlag( OP_FOG ) )
	{
		if( nsCsGBTerrain::g_Device.g_bUseMyShader )
		{
			if( m_pSettingFog )
				pDestObject->s_pActorMng->GetNIFRoot()->AttachProperty( m_pSettingFog );
		}		
	}

	// 라이트
	if( pProp->IsOptionPlag( OP_LIGHT ) )
	{
		if( m_SettingLightList.empty() == false )
		{
			std::list< NiLight* >::iterator it = m_SettingLightList.begin();
			std::list< NiLight* >::iterator itEnd = m_SettingLightList.end();
			for( ; it!=itEnd; ++it )
				( (NiNode*)pDestObject->s_pActorMng->GetNIFRoot() )->AttachEffect( *it );
		}
		else if( m_pDefaultLight )
		{				
			( (NiNode*)pDestObject->s_pActorMng->GetNIFRoot() )->AttachEffect( m_pDefaultLight );
		}		
	}
	// 라이트
	if( pProp->IsOptionPlag( OP_USERLIGHT ) )
	{		
		if( m_pDefaultLight )
		{		

			NiDirectionalLight*		pDefaultLight = (NiDirectionalLight*)m_pDefaultLight->Clone();
			pDefaultLight->SetAmbientColor( NiColor(1.0f, 1.0f, 1.0f) );
			//pDefaultLight->SetAmbientColor( NiColor(1.0f, 1.0f, 1.0f) );
			//pDefaultLight->SetDiffuseColor( NiColor(1.0f, 1.0f, 1.0f) );
			//pDefaultLight->SetDiffuseColor( NiColor(0.1f, 0.1f, 0.1f) );
			
			//NiPoint3 vPath = pDefaultLight->GetWorldTranslate();
			pDefaultLight->SetWorldTranslate( NiPoint3( 0,0,-10));

			pDefaultLight->Update( 0.0f );

			( (NiNode*)pDestObject->s_pActorMng->GetNIFRoot() )->AttachEffect( pDefaultLight );

// 			m_pDefaultLight->SetAmbientColor( NiColor(0.9f, 0.9f, 0.9f) );
// 			m_pDefaultLight->Update( 0.0f );
		}		
	}
	( (NiNode*)pDestObject->s_pActorMng->GetNIFRoot() )->UpdateEffects();

	// 레퍼런스 카운트 증가
	pSrcObject->s_pRefLoad->IncreaseRef();
}

void CsC_Thread_ResMng::_RefCheck_KFM( DWORD dwCurTime )
{
	sLOAD_KFM* pObject = NULL;
	NiTMapIterator itObj = m_pmapKFMObject->GetFirstPos();
	while( itObj != NULL )
	{
		const char* pcName;
		m_pmapKFMObject->GetNext( itObj, pcName, pObject );

		// 참조가 0이라면
		if( pObject->s_pRefLoad->GetRef() == 0 )
		{
			// 시간 비교 해서 0된지 오래 榮摸?
			if( ( dwCurTime - pObject->s_pRefLoad->GetLastDeleteTime() ) > REF_CHECK_TIME )
			{
				pObject->Delete();
				SAFE_DELETE( pObject );
				m_pmapKFMObject->RemoveAt( pcName );
			}
		}		
	}
}
