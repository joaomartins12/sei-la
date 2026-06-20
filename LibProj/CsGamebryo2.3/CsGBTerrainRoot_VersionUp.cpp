
#include "stdafx.h"
#include "CsGBTerrainRoot.h"

namespace nsCsGBVerionUp
{
	void SetVersion( CsNiNodePtr pNiNode, DWORD dwVersion )
	{
		DWORD dwOldVersion;

		// Extra Data Load
		unsigned int uiSize;
		char* pData;
		NiBinaryExtraData* pExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTerrainRootExtraKey );
		pExtData->GetValue( uiSize, pData );
		dwOldVersion = ( (sROOTINFO_010000*)pData )->s_dwMapResVersion;
		assert_cs( dwVersion > dwOldVersion );
		( (sROOTINFO_010000*)pData )->s_dwMapResVersion = dwVersion;

		char* pRootInfo = NiAlloc( char, uiSize );
		memcpy( pRootInfo, pData, uiSize );
		// Extra Data Save		
		pExtData = NiNew NiBinaryExtraData( uiSize, (char*)pRootInfo );
		pNiNode->RemoveExtraData( CsGBTerrainRootExtraKey );
		pNiNode->AddExtraData( CsGBTerrainRootExtraKey, pExtData );

		// Message
		TCHAR szMsg[ 1024 ];
		TCHAR szOldVersion[ 128 ];
		TCHAR szCurVersion[ 128 ];
		VERSION_STRING( dwOldVersion, szOldVersion, 128 );
		VERSION_STRING( dwVersion, szCurVersion, 128 );

		_stprintf_s( szMsg, 1024, _T( "%s => %s, 버젼업~!! 맵툴 세이브해서 버젼 유지 해주세요." ), szOldVersion, szCurVersion );
		MessageBox( NULL, szMsg, NULL, MB_OK );
	}

	bool vu010000( CsNiNodePtr pNiNode, const char* cFile )
	{
		SetVersion( pNiNode, 0x010000 );
		return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	}

	bool vu010001( CsNiNodePtr pNiNode, const char* cFile )
	{
		SetVersion( pNiNode, 0x010001 );
		assert_csm( false, _T( "호환 안되는 버젼입니다." ) );
		return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	}

	bool vu010002( CsNiNodePtr pNiNode, const char* cFile )
	{
		SetVersion( pNiNode, 0x010002 );
		assert_csm( false, _T( "호환 안되는 버젼입니다." ) );
		return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	}

	bool vu010003( CsNiNodePtr pNiNode, const char* cFile )
	{
		SetVersion( pNiNode, 0x010003 );

		// Extra Data Load
		unsigned int uiSize;
		char* pData;
		NiBinaryExtraData* pExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTerrainRootExtraKey );
		pExtData->GetValue( uiSize, pData );
		
		int nVertCount = ( ( sROOTINFO_010000* )pData )->s_nXVertCount*( ( sROOTINFO_010000* )pData )->s_nYVertCount;
		// 값할당
		int nNewSize = sizeof( sROOTINFO_010003 );
		// Height Field		
		nNewSize += ( sizeof( float )*nVertCount );
		// Normal Field
		nNewSize += ( sizeof( NiPoint3 )*nVertCount );

		int nDestOffset = 0;
		int nSrcOffset = 0;
		char* pRootInfo = NiAlloc( char, nNewSize );
		memcpy( pRootInfo, pData, sizeof( sROOTINFO_010000 ) );
		nSrcOffset += sizeof( sROOTINFO_010000 );
		( (sROOTINFO_010003*)pRootInfo )->s_cSkyBox[ 0 ] = NULL;
		nDestOffset += sizeof( sROOTINFO_010003 );

		memcpy( &pRootInfo[ nDestOffset ], &pData[ nSrcOffset ], uiSize - nSrcOffset );

		// Extra Data Save
		pExtData = NiNew NiBinaryExtraData( nNewSize, (char*)pRootInfo );
		pNiNode->RemoveExtraData( CsGBTerrainRootExtraKey );
		pNiNode->AddExtraData( CsGBTerrainRootExtraKey, pExtData );

		return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	}

	bool vu010004( CsNiNodePtr pNiNode, const char* cFile )
	{
		SetVersion( pNiNode, 0x010004 );

		// Extra Data Load
		unsigned int uiSize;
		char* pData;
		NiBinaryExtraData* pExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTerrainRootExtraKey );
		pExtData->GetValue( uiSize, pData );

		int nVertCount = ( ( sROOTINFO_010003* )pData )->s_nXVertCount*( ( sROOTINFO_010003* )pData )->s_nYVertCount;
		// 값할당
		int nNewSize = sizeof( sROOTINFO_010004 );
		// Height Field		
		nNewSize += ( sizeof( float )*nVertCount );
		// Normal Field
		nNewSize += ( sizeof( NiPoint3 )*nVertCount );

		int nDestOffset = 0;
		int nSrcOffset = 0;
		char* pRootInfo = NiAlloc( char, nNewSize );
		memcpy( pRootInfo, pData, sizeof( sROOTINFO_010003 ) );
		nSrcOffset += sizeof( sROOTINFO_010003 );
		( (sROOTINFO_010004*)pRootInfo )->s_fFogStart = 10000.0f;
		( (sROOTINFO_010004*)pRootInfo )->s_fFogEndObject = 20000.0f;
		( (sROOTINFO_010004*)pRootInfo )->s_fFogEndTerrain = 30000.0f;
		( (sROOTINFO_010004*)pRootInfo )->s_FogColor = NiColor( 0.1f, 0.1f, 0.1f );
		nDestOffset += sizeof( sROOTINFO_010004 );

		memcpy( &pRootInfo[ nDestOffset ], &pData[ nSrcOffset ], uiSize - nSrcOffset );

		// Extra Data Save
		pExtData = NiNew NiBinaryExtraData( nNewSize, (char*)pRootInfo );
		pNiNode->RemoveExtraData( CsGBTerrainRootExtraKey );
		pNiNode->AddExtraData( CsGBTerrainRootExtraKey, pExtData );

		return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	}

	bool vu010005( CsNiNodePtr pNiNode, const char* cFile )
	{
		SetVersion( pNiNode, 0x010005 );

		NiBinaryExtraData* pRootExtData = NULL;
		NiBinaryExtraData* pLeafExtData = NULL;
		NiBinaryExtraData* pObjectExtData = NULL;
		NiBinaryExtraData* pNewObjectExtData = NULL;
		char* pRootInfo = NULL;
		char* pNiTriInfoData = NULL;
		char* pObjectData = NULL;

		UINT uiRootSize = 0, uiLeafSize = 0, uiObjectSize = 0, uiObjectOffset = 0, uiObjectNewOffset = 0;

		// 루트 정보 및 더미 Child 생성
		nsCsGBTerrain::eCREATE_TYPE ctOrg = nsCsGBTerrain::g_eCreateType;
		pRootExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTerrainRootExtraKey );
		pRootExtData->GetValue( uiRootSize, pRootInfo );
		memcpy( nsCsGBTerrain::g_pCurRoot->GetInfo(), pRootInfo, sizeof( sROOTINFO_010004 ) );
		nsCsGBTerrain::g_pCurRoot->GetFieldInfo()->s_pfHeightField = NULL;
		int nTotalLeafCount = ( (sROOTINFO_010004*)pRootInfo )->s_nTotalLeafCount;
		nsCsGBTerrain::g_eCreateType = nsCsGBTerrain::CT_MAKE;
		nsCsGBTerrain::g_pCurRoot->GetRootNode()->Create( CsGBTerrainNode::QI_ROOT, nsCsGBTerrain::g_pCurRoot->GetBaseNiNode(), 
														  nsCsGBTerrain::g_pCurRoot->GetRootNode(), NULL, ( (sROOTINFO_010004*)pRootInfo )->s_nTreeLevel, 0 );

		char* pTempData = NULL;
		for( int nLeaf=0; nLeaf<nTotalLeafCount; ++nLeaf )
		{
			CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( nLeaf );

			char cQuadData[ 32 ] = {0, };
			sprintf_s( cQuadData, 32, "%s_%d_data", pLeaf->GetParent()->GetCharQuadKey(), pLeaf->GetQuadID() );
			// ==== TriInfo
			pLeafExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( cQuadData );
			assert_cs( pLeafExtData != NULL );
			pLeafExtData->GetValue( uiLeafSize, pNiTriInfoData );
			DWORD dwObjectCount = ( (sNiTriINFO_010000*)pNiTriInfoData )->s_dwObjectCount;
			if( dwObjectCount > 0 )
			{
				char cObjKey[ MAX_FILENAME ];
				sprintf_s( cObjKey, MAX_FILENAME, "%s_%d_object", pLeaf->GetParent()->GetCharQuadKey(), pLeaf->GetQuadID() );
				pObjectExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( cObjKey );
				assert_cs( pObjectExtData != NULL );
				pObjectExtData->GetValue( uiObjectSize, pObjectData );

				// 템프 데이터
				assert_cs( pTempData == NULL );
				pTempData = csnew char[ uiObjectSize*2 ];

				uiObjectNewOffset = 0;
				uiObjectOffset = 0;
				for( DWORD i=0; i<dwObjectCount; ++i )
				{
					nsOBJECT_010000::sFILE_HEADER fh;
					// fh 의 s_dwInfoSize 는 아직 쓰이는데 없으니 예전 값으로 대체하고 넘어가자 - 어차피 세이브할 새로 계산
					memcpy( &fh, &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_010000::sFILE_HEADER ) );
					memcpy( &pTempData[ uiObjectNewOffset ], &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_010000::sFILE_HEADER ) );					
					uiObjectNewOffset += sizeof( nsOBJECT_010005::sFILE_HEADER );
					uiObjectOffset += sizeof( nsOBJECT_010000::sFILE_HEADER );

					if( fh.s_dwSaveType == nsOBJECT_010000::ST_EXIST_OBJECT )
					{
						nsOBJECT_010000::sEXIST_INFO ei;
						memcpy( &ei, &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_010000::sEXIST_INFO ) );
						memcpy( &pTempData[ uiObjectNewOffset ], &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_010000::sEXIST_INFO ) );
						uiObjectNewOffset += sizeof( nsOBJECT_010005::sEXIST_INFO );
						uiObjectOffset += sizeof( nsOBJECT_010000::sEXIST_INFO );
					}
					else
					{
						assert_cs( fh.s_dwSaveType == CsGBObject::ST_FIRST_OBJECT );
						nsOBJECT_010000::sINFO oi;
						memcpy( &oi, &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_010000::sINFO ) );
						memcpy( &pTempData[ uiObjectNewOffset ], &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_010000::sINFO ) );
						( ( nsOBJECT_010005::sINFO* )&pTempData[ uiObjectNewOffset ] )->s_PathInfo.s_uiPEGroupCount = 0;
						( ( nsOBJECT_010005::sINFO* )&pTempData[ uiObjectNewOffset ] )->s_PathInfo.s_uiTotalPECount = 0;
						( ( nsOBJECT_010005::sINFO* )&pTempData[ uiObjectNewOffset ] )->s_PathInfo.s_pptTranse = NULL;
						( ( nsOBJECT_010005::sINFO* )&pTempData[ uiObjectNewOffset ] )->s_PathInfo.s_pPECount = NULL;
						( ( nsOBJECT_010005::sINFO* )&pTempData[ uiObjectNewOffset ] )->s_PathInfo.s_pData = NULL;
						uiObjectNewOffset += sizeof( nsOBJECT_010005::sINFO );
						uiObjectOffset += sizeof( nsOBJECT_010000::sINFO );

						// Path Engine
						if( oi.s_PathInfo.s_usPECount != 0 )
						{							
							uiObjectOffset += ( sizeof( long )*2*oi.s_PathInfo.s_usPECount );
						}

						nsOBJECT_010000::sUSER_DEFINE* pUserDefine =
							(nsOBJECT_010000::sUSER_DEFINE*)nsCsGBTerrain::g_ObjectResMgr.GetUserDefine( (CsGBObject::sINFO*)&oi );						
						// BaseObject						
						if( ( pUserDefine->s_dwPlag & nsOBJECT_010000::sUSER_DEFINE::UD_TERRAIN_BASE ) != 0 )
						{
							UINT usTemp;
							// uv set Load
							memcpy( &usTemp, &pObjectData[ uiObjectOffset ], sizeof( UINT ) );
							uiObjectOffset += sizeof( UINT );					
							uiObjectOffset += sizeof( NiPoint2 )*usTemp;
							// pixel Save
							memcpy( &usTemp, &pObjectData[ uiObjectOffset ], sizeof( UINT ) );
							uiObjectOffset += sizeof( UINT );							
							uiObjectOffset += sizeof( BYTE )*usTemp;
						}
					}
				}
				assert_cs( uiObjectNewOffset < uiObjectSize*2 );
				// ==== 오브젝트 익스트라 데이터 새로 기입
				char* pNewObjectData = NiAlloc( char, uiObjectNewOffset );
				memcpy( pNewObjectData, pTempData, uiObjectNewOffset );
				pNewObjectExtData = NiNew NiBinaryExtraData( uiObjectNewOffset, pNewObjectData );
				// 기존 ExtraData 제거
				pNiNode->RemoveExtraData( cObjKey );
				pNiNode->AddExtraData( cObjKey, pNewObjectExtData );

				SAFE_DELETE_ARRAY( pTempData );
			}
		}
		nsCsGBTerrain::g_pCurRoot->Delete();
		nsCsGBTerrain::g_pCurRoot->Init();
		nsCsGBTerrain::g_eCreateType = ctOrg;
		return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	}

	bool vu010006( CsNiNodePtr pNiNode, const char* cFile )
	{
		SetVersion( pNiNode, 0x010006 );

		// Extra Data Load
		unsigned int uiSize;
		char* pData;
		NiBinaryExtraData* pExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTerrainRootExtraKey );
		pExtData->GetValue( uiSize, pData );

		int nVertCount = ( ( sROOTINFO_010004* )pData )->s_nXVertCount*( ( sROOTINFO_010004* )pData )->s_nYVertCount;
		// 값할당
		int nNewSize = sizeof( sROOTINFO_010006 );
		// Height Field		
		nNewSize += ( sizeof( float )*nVertCount );
		// Normal Field
		nNewSize += ( sizeof( NiPoint3 )*nVertCount );

		int nDestOffset = 0;
		int nSrcOffset = 0;
		char* pRootInfo = NiAlloc( char, nNewSize );
		memcpy( pRootInfo, pData, sizeof( sROOTINFO_010004 ) );
		nSrcOffset += sizeof( sROOTINFO_010004 );
		( (sROOTINFO_010006*)pRootInfo )->s_fClipObject_Far = 15000.0f;
		( (sROOTINFO_010006*)pRootInfo )->s_fClipTerrain_Far = 25000.0f;
		nDestOffset += sizeof( sROOTINFO_010006 );

		memcpy( &pRootInfo[ nDestOffset ], &pData[ nSrcOffset ], uiSize - nSrcOffset );

		// Extra Data Save
		pExtData = NiNew NiBinaryExtraData( nNewSize, (char*)pRootInfo );
		pNiNode->RemoveExtraData( CsGBTerrainRootExtraKey );
		pNiNode->AddExtraData( CsGBTerrainRootExtraKey, pExtData );

		return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	}

	bool vu010007( CsNiNodePtr pNiNode, const char* cFile )
	{
		SetVersion( pNiNode, 0x010007 );

		NiBinaryExtraData* pRootExtData = NULL;
		NiBinaryExtraData* pLeafExtData = NULL;
		NiBinaryExtraData* pObjectExtData = NULL;
		NiBinaryExtraData* pNewObjectExtData = NULL;
		char* pRootInfo = NULL;
		char* pNiTriInfoData = NULL;
		char* pObjectData = NULL;

		UINT uiRootSize = 0, uiLeafSize = 0, uiObjectSize = 0, uiObjectOffset = 0, uiObjectNewOffset = 0;

		// 루트 정보 및 더미 Child 생성
		nsCsGBTerrain::eCREATE_TYPE ctOrg = nsCsGBTerrain::g_eCreateType;
		pRootExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTerrainRootExtraKey );
		pRootExtData->GetValue( uiRootSize, pRootInfo );		
		memcpy( nsCsGBTerrain::g_pCurRoot->GetInfo(), pRootInfo, sizeof( sROOTINFO_010006 ) );
		nsCsGBTerrain::g_pCurRoot->GetFieldInfo()->s_pfHeightField = NULL;
		int nTotalLeafCount = ( (sROOTINFO_010006*)pRootInfo )->s_nTotalLeafCount;
		nsCsGBTerrain::g_eCreateType = nsCsGBTerrain::CT_MAKE;
		nsCsGBTerrain::g_pCurRoot->GetRootNode()->Create( CsGBTerrainNode::QI_ROOT,nsCsGBTerrain::g_pCurRoot->GetBaseNiNode(),nsCsGBTerrain::g_pCurRoot->GetRootNode(), NULL,
			( (sROOTINFO_010006*)pRootInfo )->s_nTreeLevel, 0 );

		// 루트 인포 저장 다시
		char* pNewRootInfo = NiAlloc( char, uiRootSize + sizeof( DWORD ) );
		memcpy( pNewRootInfo, pRootInfo, sizeof( sROOTINFO_010006 ) );
		( ( sROOTINFO_010007* )pNewRootInfo )->s_dwMapID = 0;

		memcpy( &pNewRootInfo[ sizeof( sROOTINFO_010007 ) ], &pRootInfo[ sizeof( sROOTINFO_010006 ) ], uiRootSize - sizeof( sROOTINFO_010006 ) );
		pRootExtData = NiNew NiBinaryExtraData( uiRootSize + sizeof( DWORD ), pNewRootInfo );
		pNiNode->RemoveExtraData( CsGBTerrainRootExtraKey );
		pNiNode->AddExtraData( CsGBTerrainRootExtraKey, pRootExtData );

		char* pTempData = NULL;
		for( int nLeaf=0; nLeaf<nTotalLeafCount; ++nLeaf )
		{
			CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( nLeaf );

			char cQuadData[ 32 ] = {0, };
			sprintf_s( cQuadData, 32, "%s_%d_data", pLeaf->GetParent()->GetCharQuadKey(), pLeaf->GetQuadID() );
			// ==== TriInfo
			pLeafExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( cQuadData );
			assert_cs( pLeafExtData != NULL );
			pLeafExtData->GetValue( uiLeafSize, pNiTriInfoData );
			DWORD dwObjectCount = ( (sNiTriINFO_010000*)pNiTriInfoData )->s_dwObjectCount;
			if( dwObjectCount > 0 )
			{
				char cObjKey[ MAX_FILENAME ];
				sprintf_s( cObjKey, MAX_FILENAME, "%s_%d_object", pLeaf->GetParent()->GetCharQuadKey(), pLeaf->GetQuadID() );
				pObjectExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( cObjKey );
				assert_cs( pObjectExtData != NULL );
				pObjectExtData->GetValue( uiObjectSize, pObjectData );

				// 템프 데이터
				assert_cs( pTempData == NULL );
				pTempData = csnew char[ uiObjectSize*2 ];

				uiObjectNewOffset = 0;
				uiObjectOffset = 0;
				for( DWORD i=0; i<dwObjectCount; ++i )
				{
					nsOBJECT_010006::sFILE_HEADER fh;
					// fh 의 s_dwInfoSize 는 아직 쓰이는데 없으니 예전 값으로 대체하고 넘어가자 - 어차피 세이브할 새로 계산
					memcpy( &fh, &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_010006::sFILE_HEADER ) );
					memcpy( &pTempData[ uiObjectNewOffset ], &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_010006::sFILE_HEADER ) );					
					uiObjectNewOffset += sizeof( nsOBJECT_010007::sFILE_HEADER );
					uiObjectOffset += sizeof( nsOBJECT_010006::sFILE_HEADER );
					if( fh.s_dwSaveType == nsOBJECT_010000::ST_EXIST_OBJECT )
					{
						nsOBJECT_010006::sEXIST_INFO ei;
						memcpy( &ei, &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_010006::sEXIST_INFO ) );
						memcpy( &pTempData[ uiObjectNewOffset ], &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_010006::sEXIST_INFO ) );
						uiObjectNewOffset += sizeof( nsOBJECT_010007::sEXIST_INFO );
						uiObjectOffset += sizeof( nsOBJECT_010006::sEXIST_INFO );
					}
					else
					{
						assert_cs( fh.s_dwSaveType == CsGBObject::ST_FIRST_OBJECT );						
						nsOBJECT_010006::sINFO oi;
						memcpy( &oi, &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_010006::sINFO ) );
						nsOBJECT_010006::sUSER_DEFINE* pUserDefine =
							(nsOBJECT_010006::sUSER_DEFINE*)nsCsGBTerrain::g_ObjectResMgr.GetUserDefine( (CsGBObject::sINFO*)&oi );
						nsOBJECT_010007::sINFO newOi;
						newOi.s_dwUniqID = oi.s_dwUniqID;
						newOi.s_trObject = oi.s_trObject;
						newOi.s_Bound = oi.s_Bound;
						newOi.s_dwTexTypePlag = oi.s_dwTexTypePlag;
						memcpy( &newOi.s_PathInfo, &oi.s_PathInfo, sizeof( nsOBJECT_010006::sPATHINFO ) );
						strcpy_s( newOi.s_cObjectPath, MAX_PATH, oi.s_cObjectPath );
						newOi.s_dwUserDefinePlag = pUserDefine->s_dwPlag;
						newOi.s_dwUserDefinePlag &= ~ nsOBJECT_010006::sUSER_DEFINE::UD_PORTAL;
						memcpy( &pTempData[ uiObjectNewOffset ], &newOi, sizeof( nsOBJECT_010007::sINFO ) );
						uiObjectNewOffset += sizeof( nsOBJECT_010007::sINFO );
						uiObjectOffset += sizeof( nsOBJECT_010006::sINFO );						

						DWORD dwMemcpySize = 0;
						UINT uiOrgObjectOffset = uiObjectOffset;
						// ======= 바꿀 값은 바꾸었다 - 남은 것 바로 저장
						// Path Engine
						if( oi.s_PathInfo.s_uiPEGroupCount != 0 )
						{
							dwMemcpySize += ( sizeof( NiPoint2 )*oi.s_PathInfo.s_uiPEGroupCount );
							dwMemcpySize += ( sizeof( USHORT )*oi.s_PathInfo.s_uiPEGroupCount );
							dwMemcpySize += ( sizeof( long )*2*oi.s_PathInfo.s_uiTotalPECount );
							uiObjectOffset += dwMemcpySize;
						}
						// BaseObject						
						if( ( pUserDefine->s_dwPlag & nsOBJECT_010006::sUSER_DEFINE::UD_TERRAIN_BASE ) != 0 )
						{
							UINT usTemp;
							// uv set Load
							memcpy( &usTemp, &pObjectData[ uiObjectOffset ], sizeof( UINT ) );
							uiObjectOffset += sizeof( UINT );
							uiObjectOffset += sizeof( NiPoint2 )*usTemp;
							dwMemcpySize += sizeof( UINT );
							dwMemcpySize += sizeof( NiPoint2 )*usTemp;
							// pixel Save
							memcpy( &usTemp, &pObjectData[ uiObjectOffset ], sizeof( UINT ) );
							uiObjectOffset += sizeof( UINT );
							uiObjectOffset += sizeof( BYTE )*usTemp;
							dwMemcpySize += sizeof( UINT );
							dwMemcpySize += sizeof( BYTE )*usTemp;
						}
						memcpy( &pTempData[ uiObjectNewOffset ], &pObjectData[ uiOrgObjectOffset ], dwMemcpySize );
						uiObjectNewOffset += dwMemcpySize;
					}
				}
				assert_cs( uiObjectNewOffset < uiObjectSize*2 );
				// ==== 오브젝트 익스트라 데이터 새로 기입
				char* pNewObjectData = NiAlloc( char, uiObjectNewOffset );
				memcpy( pNewObjectData, pTempData, uiObjectNewOffset );
				pNewObjectExtData = NiNew NiBinaryExtraData( uiObjectNewOffset, pNewObjectData );
				// 기존 ExtraData 제거
				pNiNode->RemoveExtraData( cObjKey );
				pNiNode->AddExtraData( cObjKey, pNewObjectExtData );

				SAFE_DELETE_ARRAY( pTempData );
			}
		}
		nsCsGBTerrain::g_pCurRoot->Delete();
		nsCsGBTerrain::g_pCurRoot->Init();
		nsCsGBTerrain::g_eCreateType = ctOrg;
		return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	}

	bool vu010008( CsNiNodePtr pNiNode, const char* cFile )
	{
		SetVersion( pNiNode, 0x010008 );

		// Extra Data Load
		unsigned int uiSize;
		char* pData;
		NiBinaryExtraData* pExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTerrainRootExtraKey );
		pExtData->GetValue( uiSize, pData );

		int nVertCount = ( ( sROOTINFO_010007* )pData )->s_nXVertCount*( ( sROOTINFO_010007* )pData )->s_nYVertCount;
		// 값할당
		int nNewSize = sizeof( sROOTINFO_010008 );
		// Height Field		
		nNewSize += ( sizeof( float )*nVertCount );
		// Normal Field
		nNewSize += ( sizeof( NiPoint3 )*nVertCount );

		int nDestOffset = 0;
		int nSrcOffset = 0;
		char* pRootInfo = NiAlloc( char, nNewSize );
		memcpy( pRootInfo, pData, sizeof( sROOTINFO_010007 ) );
		nSrcOffset += sizeof( sROOTINFO_010007 );
		( (sROOTINFO_010008*)pRootInfo )->s_PathInfo.Reset();		
		nDestOffset += sizeof( sROOTINFO_010008 );

		memcpy( &pRootInfo[ nDestOffset ], &pData[ nSrcOffset ], uiSize - nSrcOffset );

		// Extra Data Save
		pExtData = NiNew NiBinaryExtraData( nNewSize, (char*)pRootInfo );
		pNiNode->RemoveExtraData( CsGBTerrainRootExtraKey );
		pNiNode->AddExtraData( CsGBTerrainRootExtraKey, pExtData );

		return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	}

	bool vu010009( CsNiNodePtr pNiNode, const char* cFile )
	{
		SetVersion( pNiNode, 0x010009 );

		NiBinaryExtraData* pRootExtData = NULL;
		NiBinaryExtraData* pLeafExtData = NULL;
		NiBinaryExtraData* pObjectExtData = NULL;
		NiBinaryExtraData* pNewObjectExtData = NULL;
		char* pRootInfo = NULL;
		char* pNiTriInfoData = NULL;
		char* pObjectData = NULL;

		UINT uiRootSize = 0, uiLeafSize = 0, uiObjectSize = 0, uiObjectOffset = 0, uiObjectNewOffset = 0;

		// 루트 정보 및 더미 Child 생성
		nsCsGBTerrain::eCREATE_TYPE ctOrg = nsCsGBTerrain::g_eCreateType;
		pRootExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTerrainRootExtraKey );
		pRootExtData->GetValue( uiRootSize, pRootInfo );

		int nTotalLeafCount = ( (sROOTINFO_010008*)pRootInfo )->s_nTotalLeafCount;
		int nTreeLevel = ( (sROOTINFO_010008*)pRootInfo )->s_nTreeLevel;

		nsCsGBTerrain::g_eCreateType = nsCsGBTerrain::CT_MAKE;
		nsCsGBTerrain::g_pCurRoot->Delete();
		nsCsGBTerrain::g_pCurRoot->Init();
		memcpy( nsCsGBTerrain::g_pCurRoot->GetInfo(), pRootInfo, sizeof( sROOTINFO_010008 ) );
		nsCsGBTerrain::g_pCurRoot->GetFieldInfo()->s_pfHeightField = NULL;
		nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->s_PathInfo.Reset();
							
		nsCsGBTerrain::g_pCurRoot->GetRootNode()->Create( CsGBTerrainNode::QI_ROOT,nsCsGBTerrain::g_pCurRoot->GetBaseNiNode(), nsCsGBTerrain::g_pCurRoot->GetRootNode(), NULL,
			nTreeLevel, 0 );

		char* pTempData = NULL;
		for( int nLeaf=0; nLeaf<nTotalLeafCount; ++nLeaf )
		{
			CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( nLeaf );

			char cQuadData[ 32 ] = {0, };
			sprintf_s( cQuadData, 32, "%s_%d_data", pLeaf->GetParent()->GetCharQuadKey(), pLeaf->GetQuadID() );
			// ==== TriInfo
			pLeafExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( cQuadData );
			assert_cs( pLeafExtData != NULL );
			pLeafExtData->GetValue( uiLeafSize, pNiTriInfoData );
			DWORD dwObjectCount = ( (sNiTriINFO_010000*)pNiTriInfoData )->s_dwObjectCount;
			if( dwObjectCount > 0 )
			{
				char cObjKey[ MAX_FILENAME ];
				sprintf_s( cObjKey, MAX_FILENAME, "%s_%d_object", pLeaf->GetParent()->GetCharQuadKey(), pLeaf->GetQuadID() );
				pObjectExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( cObjKey );
				assert_cs( pObjectExtData != NULL );
				pObjectExtData->GetValue( uiObjectSize, pObjectData );

				// 템프 데이터
				assert_cs( pTempData == NULL );
				pTempData = csnew char[ uiObjectSize ];
				uiObjectOffset = 0;
				for( DWORD i=0; i<dwObjectCount; ++i )
				{
					nsOBJECT_010007::sFILE_HEADER fh;
					// fh 의 s_dwInfoSize 는 아직 쓰이는데 없으니 예전 값으로 대체하고 넘어가자 - 어차피 세이브할 새로 계산
					memcpy( &fh, &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_010007::sFILE_HEADER ) );
					memcpy( &pTempData[ uiObjectOffset ], &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_010007::sFILE_HEADER ) );										
					uiObjectOffset += sizeof( nsOBJECT_010007::sFILE_HEADER );
					if( fh.s_dwSaveType == nsOBJECT_010007::ST_EXIST_OBJECT )
					{
						nsOBJECT_010007::sEXIST_INFO ei;
						memcpy( &ei, &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_010007::sEXIST_INFO ) );
						memcpy( &pTempData[ uiObjectOffset ], &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_010007::sEXIST_INFO ) );						
						uiObjectOffset += sizeof( nsOBJECT_010006::sEXIST_INFO );
					}
					else
					{
						assert_cs( fh.s_dwSaveType == CsGBObject::ST_FIRST_OBJECT );						
						nsOBJECT_010007::sINFO oi;
						memcpy( &oi, &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_010007::sINFO ) );
						if( ( ( oi.s_dwUserDefinePlag & sUSER_DEFINE_010008::UD_TERRAIN_BASE ) != 0 )||
							( ( oi.s_dwUserDefinePlag & sUSER_DEFINE_010008::UD_TERRAIN_BASE_PICK ) != 0 ) )
						{
							oi.s_dwUserDefinePlag |= sUSER_DEFINE_010008::UD_DARKMAP;
							memcpy( &pTempData[ uiObjectOffset ], &oi, sizeof( nsOBJECT_010007::sINFO ) );
						}
						else
						{
							memcpy( &pTempData[ uiObjectOffset ], &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_010007::sINFO ) );
						}
						uiObjectOffset += sizeof( nsOBJECT_010007::sINFO );

						DWORD dwMemcpySize = 0;
						UINT uiOrgObjectOffset = uiObjectOffset;
						// ======= 바꿀 값은 바꾸었다 - 남은 것 바로 저장
						// Path Engine
						if( oi.s_PathInfo.s_uiPEGroupCount != 0 )
						{
							dwMemcpySize += ( sizeof( NiPoint2 )*oi.s_PathInfo.s_uiPEGroupCount );
							dwMemcpySize += ( sizeof( USHORT )*oi.s_PathInfo.s_uiPEGroupCount );
							dwMemcpySize += ( sizeof( long )*2*oi.s_PathInfo.s_uiTotalPECount );
							uiObjectOffset += dwMemcpySize;
						}
						// BaseObject						
						if( ( oi.s_dwUserDefinePlag & sUSER_DEFINE_010008::UD_DARKMAP ) != 0 )
						{
							UINT usTemp;
							// uv set Load
							memcpy( &usTemp, &pObjectData[ uiObjectOffset ], sizeof( UINT ) );
							uiObjectOffset += sizeof( UINT );
							uiObjectOffset += sizeof( NiPoint2 )*usTemp;
							dwMemcpySize += sizeof( UINT );
							dwMemcpySize += sizeof( NiPoint2 )*usTemp;
							// pixel Save
							memcpy( &usTemp, &pObjectData[ uiObjectOffset ], sizeof( UINT ) );
							uiObjectOffset += sizeof( UINT );
							uiObjectOffset += sizeof( BYTE )*usTemp;
							dwMemcpySize += sizeof( UINT );
							dwMemcpySize += sizeof( BYTE )*usTemp;
						}
						// Portal
						if( ( oi.s_dwUserDefinePlag & sUSER_DEFINE_010008::UD_PORTAL ) != 0 )
						{
							UINT uiPortalSize = sizeof( DWORD ) + sizeof( DWORD ) + sizeof( CsGBObj_Portal::sTARGET )*2;
							uiObjectOffset += uiPortalSize;
							dwMemcpySize += uiPortalSize;
						}
						memcpy( &pTempData[ uiOrgObjectOffset ], &pObjectData[ uiOrgObjectOffset ], dwMemcpySize );						
					}
				}
				assert_cs( uiObjectNewOffset < uiObjectSize*2 );
				// ==== 오브젝트 익스트라 데이터 새로 기입
				char* pNewObjectData = NiAlloc( char, uiObjectOffset );
				memcpy( pNewObjectData, pTempData, uiObjectOffset );
				pNewObjectExtData = NiNew NiBinaryExtraData( uiObjectOffset, pNewObjectData );
				// 기존 ExtraData 제거
				pNiNode->RemoveExtraData( cObjKey );
				pNiNode->AddExtraData( cObjKey, pNewObjectExtData );

				SAFE_DELETE_ARRAY( pTempData );
			}
		}
		nsCsGBTerrain::g_pCurRoot->Delete();
		nsCsGBTerrain::g_pCurRoot->Init();
		nsCsGBTerrain::g_eCreateType = ctOrg;
		return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	}

	bool vu01000a( CsNiNodePtr pNiNode, const char* cFile )
	{
		SetVersion( pNiNode, 0x01000a );

		NiBinaryExtraData* pRootExtData = NULL;
		char* pData = NULL;
		UINT uiRootSize = 0;
		UINT uiOldOffset = 0;

		//====================================================================================================
		//
		//		RootInfo
		//
		//====================================================================================================
		pRootExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTerrainRootExtraKey );
		pRootExtData->GetValue( uiRootSize, pData );
		sROOTINFO_010008* pOldRootInfo = ( sROOTINFO_010008* )pData;
		uiOldOffset += sizeof( sROOTINFO_010008 );

		int nNewRootSize = sizeof( sROOTINFO_01000a );		
		sROOTINFO_01000a* pRootInfo = (sROOTINFO_01000a*)NiAlloc( char, nNewRootSize );
		pRootInfo->s_dwMapResVersion		=	pOldRootInfo->s_dwMapResVersion;
		pRootInfo->s_nXVertCount			=	pOldRootInfo->s_nXVertCount;
		pRootInfo->s_nXVertCountInNode		=	pOldRootInfo->s_nXVertCountInNode;
		pRootInfo->s_nYVertCount			=	pOldRootInfo->s_nYVertCount;
		pRootInfo->s_nYVertCountInNode		=	pOldRootInfo->s_nYVertCountInNode;
		pRootInfo->s_fHeightRate			=	pOldRootInfo->s_fHeightRate;
		pRootInfo->s_nGridSize				=	pOldRootInfo->s_nGridSize;
		pRootInfo->s_nTreeLevel				=	pOldRootInfo->s_nTreeLevel;
		pRootInfo->s_nNodeCountInSide		=	pOldRootInfo->s_nNodeCountInSide;
		pRootInfo->s_nTotalLeafCount		=	pOldRootInfo->s_nTotalLeafCount;
		_tcscpy_s( pRootInfo->s_szHeightMapFile, MAX_PATH, pOldRootInfo->s_szHeightMapFile );
		pRootInfo->s_nInitMapCoverSize		=	pOldRootInfo->s_nInitMapCoverSize;
		pRootInfo->s_nInitMapSize			=	pOldRootInfo->s_nInitMapSize;
		pRootInfo->s_fDetailMapSize			=	pOldRootInfo->s_fDetailMapSize;
		pRootInfo->s_nBaseMPPG				=	pOldRootInfo->s_nBaseMPPG;
		pRootInfo->s_nAlphaMPPG				=	pOldRootInfo->s_nAlphaMPPG;
		pRootInfo->s_nShadowMPPG			=	pOldRootInfo->s_nShadowMPPG;
		pRootInfo->s_nShadowConstant		=	pOldRootInfo->s_nShadowConstant;
		pRootInfo->s_btObjectShadowColor	=	pOldRootInfo->s_btObjectShadowColor;
		pRootInfo->s_btTerrainShadowColor	=	pOldRootInfo->s_btTerrainShadowColor;
		pRootInfo->s_nEffectMPPG			=	pOldRootInfo->s_nEffectMPPG;
		pRootInfo->s_dwUniqObjectID			=	pOldRootInfo->s_dwUniqObjectID;
		pRootInfo->s_FogColor				=	pOldRootInfo->s_FogColor;
		pRootInfo->s_fFogStart				=	pOldRootInfo->s_fFogStart;
		pRootInfo->s_fFogEndTerrain			=	pOldRootInfo->s_fFogEndTerrain;
		pRootInfo->s_fFogEndObject			=	pOldRootInfo->s_fFogEndObject;
		pRootInfo->s_fClipObject_Far		=	pOldRootInfo->s_fClipObject_Far;
		pRootInfo->s_fClipTerrain_Far		=	pOldRootInfo->s_fClipTerrain_Far;
		pRootInfo->s_dwMapID				=	pOldRootInfo->s_dwMapID;
		memset( pRootInfo->s_Dummy, 0, 64 );

		//====================================================================================================
		//
		//		FieldInfo
		//
		//====================================================================================================
		int nVertCount = pOldRootInfo->s_nXVertCount * pOldRootInfo->s_nYVertCount;
		int nNewFieldSize = sizeof( float )*nVertCount + sizeof( NiPoint3 )*nVertCount;
		char* pFieldData = NiAlloc( char, nNewFieldSize );

		int nFieldOffset = 0;
		memcpy( &pFieldData[ nFieldOffset ], &pData[ uiOldOffset ], sizeof( float )*nVertCount );
		nFieldOffset += sizeof( float )*nVertCount;
		uiOldOffset += sizeof( float )*nVertCount;
		memcpy( &pFieldData[ nFieldOffset ], &pData[ uiOldOffset ], sizeof( NiPoint3 )*nVertCount );
		uiOldOffset += sizeof( NiPoint3 )*nVertCount;

		//====================================================================================================
		//
		//		PathInfo
		//
		//====================================================================================================
		assert_cs( sizeof( nsOBJECT_010007::sPATHINFO ) == sizeof( sTR_PATHINFO_01000a ) );
		UINT uiPEGroupCount = pOldRootInfo->s_PathInfo.s_uiPEGroupCount;		
		int nNewPathSize = sizeof( sTR_PATHINFO_01000a ) + sizeof( NiPoint2 )*uiPEGroupCount + sizeof( long )*uiPEGroupCount*8;
				
		char* pPathData = NiAlloc( char, nNewPathSize );		
		memcpy( pPathData, &pOldRootInfo->s_PathInfo, sizeof( nsOBJECT_010007::sPATHINFO ) );

		int nPathOffset = sizeof( nsOBJECT_010007::sPATHINFO );
		memcpy( &pPathData[ nPathOffset ], &pData[ uiOldOffset ], sizeof( NiPoint2 )*uiPEGroupCount );
		nPathOffset += sizeof( NiPoint2 )*uiPEGroupCount;
		uiOldOffset += sizeof( NiPoint2 )*uiPEGroupCount;

		memcpy( &pPathData[ nPathOffset ], &pData[ uiOldOffset ], sizeof( long )*uiPEGroupCount*8 );

		//====================================================================================================
		//
		//		SkyBox
		//
		//====================================================================================================
		int nNewSkySize = sizeof( sSKYBOXINFO_01000a );

		char* pSkyBoxData = NiAlloc( char, nNewSkySize );
		( (sSKYBOXINFO_01000a*)pSkyBoxData )->s_fHeight = 0.0f;
		strcpy_s( ( (sSKYBOXINFO_01000a*)pSkyBoxData )->s_cSkyBox, MAX_PATH, pOldRootInfo->s_cSkyBox );

		// Extra Data Save
		pRootExtData = NiNew NiBinaryExtraData( nNewRootSize, (char*)pRootInfo );
		pNiNode->RemoveExtraData( CsGBTerrainRootExtraKey );
		pNiNode->AddExtraData( CsGBTerrainRootExtraKey, pRootExtData );

		// Extra Data Save
		pRootExtData = NiNew NiBinaryExtraData( nNewFieldSize, pFieldData );
		pNiNode->RemoveExtraData( CsGBTR_FieldExtraKey );
		pNiNode->AddExtraData( CsGBTR_FieldExtraKey, pRootExtData );

		// Extra Data Save
		pRootExtData = NiNew NiBinaryExtraData( nNewPathSize, pPathData );
		pNiNode->RemoveExtraData( CsGBTR_PathExtraKey );
		pNiNode->AddExtraData( CsGBTR_PathExtraKey, pRootExtData );

		// Extra Data Save
		pRootExtData = NiNew NiBinaryExtraData( nNewSkySize, pSkyBoxData );
		pNiNode->RemoveExtraData( CsGBSkyBox_ExtraKey );
		pNiNode->AddExtraData( CsGBSkyBox_ExtraKey, pRootExtData );

		return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	}

	bool vu01000b( CsNiNodePtr pNiNode, const char* cFile )
	{
		SetVersion( pNiNode, 0x01000b );

		NiBinaryExtraData* pRootExtData = NULL;
		NiBinaryExtraData* pLeafExtData = NULL;
		NiBinaryExtraData* pNewExtData = NULL;
		char* pRootInfo = NULL;
		char* pNiTriInfoData = NULL;
		UINT uiRootSize = 0, uiLeafSize = 0;

		// 루트 정보 및 더미 Child 생성
		nsCsGBTerrain::eCREATE_TYPE ctOrg = nsCsGBTerrain::g_eCreateType;
		pRootExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTerrainRootExtraKey );
		pRootExtData->GetValue( uiRootSize, pRootInfo );

		int nTotalLeafCount = ( (sROOTINFO_01000a*)pRootInfo )->s_nTotalLeafCount;
		int nTreeLevel = ( (sROOTINFO_01000a*)pRootInfo )->s_nTreeLevel;

		nsCsGBTerrain::g_eCreateType = nsCsGBTerrain::CT_MAKE;
		nsCsGBTerrain::g_pCurRoot->Delete();
		nsCsGBTerrain::g_pCurRoot->Init();
		memcpy( nsCsGBTerrain::g_pCurRoot->GetInfo(), pRootInfo, sizeof( sROOTINFO_01000a ) );
		nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->s_PathInfo.Reset();

		nsCsGBTerrain::g_pCurRoot->GetRootNode()->Create( CsGBTerrainNode::QI_ROOT,nsCsGBTerrain::g_pCurRoot->GetBaseNiNode(), nsCsGBTerrain::g_pCurRoot->GetRootNode(), NULL,
			nTreeLevel, 0 );

		sNiTriINFO_01000b NewTriInfo;		
		for( int nLeaf=0; nLeaf<nTotalLeafCount; ++nLeaf )
		{
			CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( nLeaf );

			char cQuadData[ 32 ] = {0, };
			UINT nOffset = 0;
			sprintf_s( cQuadData, 32, "%s_%d_data", pLeaf->GetParent()->GetCharQuadKey(), pLeaf->GetQuadID() );
			
			// ==== TriInfo
			pLeafExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( cQuadData );
			assert_cs( pLeafExtData != NULL );
			pLeafExtData->GetValue( uiLeafSize, pNiTriInfoData );

			NewTriInfo.s_ptOffsetPos = ( (sNiTriINFO_010000*)pNiTriInfoData )->s_ptOffsetPos;
			NewTriInfo.s_nVertCount = ( (sNiTriINFO_010000*)pNiTriInfoData )->s_nVertCount;
			NewTriInfo.s_nTemp1 = ( (sNiTriINFO_010000*)pNiTriInfoData )->s_nTemp1;
			NewTriInfo.s_fTemp2 = ( (sNiTriINFO_010000*)pNiTriInfoData )->s_fTemp2;
			NewTriInfo.s_usNiTerrainIndex = ( (sNiTriINFO_010000*)pNiTriInfoData )->s_usNiTerrainIndex;
			NewTriInfo.s_dwObjectCount = ( (sNiTriINFO_010000*)pNiTriInfoData )->s_dwObjectCount;

			strcpy_s( NewTriInfo.s_cDetailMap0, MAX_PATH, ( (sNiTriINFO_010000*)pNiTriInfoData )->s_cDetailMap0 );
			strcpy_s( NewTriInfo.s_cDetailMap1, MAX_PATH, ( (sNiTriINFO_010000*)pNiTriInfoData )->s_cDetailMap1 );
			strcpy_s( NewTriInfo.s_cDetailMap2, MAX_PATH, ( (sNiTriINFO_010000*)pNiTriInfoData )->s_cDetailMap2 );
			strcpy_s( NewTriInfo.s_cDetailMap3, MAX_PATH, ( (sNiTriINFO_010000*)pNiTriInfoData )->s_cDetailMap3 );
			memset( NewTriInfo.s_cDetailMap4, 0, sizeof( char )*MAX_PATH );

			NewTriInfo.s_nShaderCord = ( (sNiTriINFO_010000*)pNiTriInfoData )->s_nShaderCord*10;

			// ==== NiTriInfo 익스트라 데이터 새로 기입
			char* pNewData = NiAlloc( char, uiLeafSize + sizeof( char )*MAX_PATH );
			memcpy( pNewData, &NewTriInfo, sizeof( sNiTriINFO_01000b ) );
			memcpy( &pNewData[ sizeof( sNiTriINFO_01000b ) ], &pNiTriInfoData[ sizeof( sNiTriINFO_010000 ) ], sizeof( float )*NewTriInfo.s_nVertCount );
			pNewExtData = NiNew NiBinaryExtraData( uiLeafSize + sizeof( char )*MAX_PATH, pNewData );
			// 기존 ExtraData 제거
			pNiNode->RemoveExtraData( cQuadData );
			pNiNode->AddExtraData( cQuadData, pNewExtData );			
		}

		// ========================================================================================================
		//		알파맵 - 24 비트에서 32 비트로
		// ========================================================================================================
		char cStr[ MAX_PATH ];
		strcpy_s( cStr, MAX_PATH, cFile );
		size_t nLen = strlen( cStr );
		sprintf_s( &cStr[ nLen - 4 ], MAX_PATH - (nLen-4), "_AM.cst" );
		NiStream stream;
		if( stream.Load( cStr ) == true )
		{
			bool bSave = false;
			NiPixelData** pDestPixelData = csnew NiPixelData*[ nTotalLeafCount ];
			{
				NiStream saveStream;			
				for( int i=0; i<nTotalLeafCount; ++i )
				{
					NiPixelData* pSrcPixelData = (NiPixelData*)stream.GetObjectAt( i );
					if( pSrcPixelData->GetPixelFormat() != ALPHA_MAP_FORMAT )
					{
						pDestPixelData[ i ] = NiNew NiPixelData( pSrcPixelData->GetWidth(), pSrcPixelData->GetHeight(), ALPHA_MAP_FORMAT );
						NiImageConverter::GetImageConverter()->ConvertPixelData( *pSrcPixelData, ALPHA_MAP_FORMAT, pDestPixelData[ i ], false );

						int nPCount = pSrcPixelData->GetWidth()*pSrcPixelData->GetHeight();
						for( int p=0; p<nPCount; ++p )
						{
							( (BYTE*)pDestPixelData[ i ]->GetPixels() )[ p*4 + 3 ] = 0;
						}						
						saveStream.InsertObject( pDestPixelData[ i ] );
						bSave = true;
					}					
				}
				if( bSave == true )
				{
					saveStream.Save( cStr );
				}
			}
			SAFE_DELETE_ARRAY( pDestPixelData );
		}

		nsCsGBTerrain::g_pCurRoot->Delete();
		nsCsGBTerrain::g_pCurRoot->Init();
		nsCsGBTerrain::g_eCreateType = ctOrg;
		return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	}

	bool vu01000c( CsNiNodePtr pNiNode, const char* cFile )
	{
		SetVersion( pNiNode, 0x01000c );

		NiStream stream;	
		stream.Load( cFile );
		NiDirectionalLightPtr pLight = (NiDirectionalLight*)stream.GetObjectAt( 0 );
		pLight->UpdateProperties();
		pLight->Update( 0.0f );

		CsGBLightMng::sINFO		mngInfo;
		mngInfo.s_nLightCount = 1;		

		CsGBLight::sBASE_INFO	baseInfo;
		CsGBLight_Dir::sINFO	Info;
		baseInfo.s_eType = CsGBLight::DIRECTION;
		baseInfo.s_Diffuse = pLight->GetDiffuseColor();
		baseInfo.s_Ambient = pLight->GetAmbientColor();
		memset( baseInfo.s_Dummy, 0, sizeof( char )* 28 );
		Info.s_Rotation = pLight->GetRotate();

		UINT nNewSize = sizeof( CsGBLightMng::sINFO ) + sizeof( CsGBLight::sBASE_INFO ) + sizeof( CsGBLight_Dir::sINFO );
		char* pRootInfo = NiAlloc( char, nNewSize );
		UINT nOffset = 0;
		// 헤더 입력
		memcpy( pRootInfo, &mngInfo, sizeof( CsGBLightMng::sINFO) );
		nOffset += sizeof( CsGBLightMng::sINFO );

		// 실 라이트 입력
		memcpy( &pRootInfo[ nOffset ], &baseInfo, sizeof( CsGBLight::sBASE_INFO) );
		nOffset += sizeof( CsGBLight::sBASE_INFO );
		memcpy( &pRootInfo[ nOffset ], &Info, sizeof( CsGBLight_Dir::sINFO ) );

		// 다시 저장		
		NiBinaryExtraData* pNewExtData = NiNew NiBinaryExtraData( nNewSize, pRootInfo );
		pNiNode->AddExtraData( CsGBLight_ExtraKey, pNewExtData );		

		return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	}

	bool vu01000d( CsNiNodePtr pNiNode, const char* cFile )
	{
		SetVersion( pNiNode, 0x01000d );

		return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );

		//NiBinaryExtraData* pRootExtData = NULL;
		//NiBinaryExtraData* pLeafExtData = NULL;
		//NiBinaryExtraData* pObjectExtData = NULL;
		//NiBinaryExtraData* pNewObjectExtData = NULL;
		//char* pRootInfo = NULL;
		//char* pNiTriInfoData = NULL;
		//char* pObjectData = NULL;

		//UINT uiRootSize = 0, uiLeafSize = 0, uiObjectSize = 0, uiObjectOffset = 0, uiObjectNewOffset = 0;

		//// 루트 정보 및 더미 Child 생성
		//nsCsGBTerrain::eCREATE_TYPE ctOrg = nsCsGBTerrain::g_eCreateType;
		//pRootExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTerrainRootExtraKey );
		//pRootExtData->GetValue( uiRootSize, pRootInfo );

		//int nTotalLeafCount = ( (sROOTINFO_01000a*)pRootInfo )->s_nTotalLeafCount;
		//int nTreeLevel = ( (sROOTINFO_01000a*)pRootInfo )->s_nTreeLevel;

		//nsCsGBTerrain::g_eCreateType = nsCsGBTerrain::CT_MAKE;
		//nsCsGBTerrain::g_pCurRoot->Delete();
		//nsCsGBTerrain::g_pCurRoot->Init();
		//memcpy( nsCsGBTerrain::g_pCurRoot->GetInfo(), pRootInfo, sizeof( sROOTINFO_01000a ) );
		//nsCsGBTerrain::g_pCurRoot->GetFieldInfo()->s_pfHeightField = NULL;
		//nsCsGBTerrain::g_pCurRoot->GetFieldInfo()->s_pptNormalField = NULL;
		//nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->s_PathInfo.Reset();

		//nsCsGBTerrain::g_pCurRoot->GetRootNode()->Create( CsGBTerrainNode::QI_ROOT, nsCsGBTerrain::g_pCurRoot->GetRootNode(), NULL,
		//	nTreeLevel, 0 );

		//for( int nLeaf=0; nLeaf<nTotalLeafCount; ++nLeaf )
		//{
		//	CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( nLeaf );

		//	char cQuadData[ 32 ] = {0, };
		//	sprintf_s( cQuadData, 32, "%s_%d_data", pLeaf->GetParent()->GetQuadKey(), pLeaf->GetQuadID() );
		//	// ==== TriInfo
		//	pLeafExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( cQuadData );
		//	assert_cs( pLeafExtData != NULL );
		//	pLeafExtData->GetValue( uiLeafSize, pNiTriInfoData );
		//	DWORD dwObjectCount = ( (sNiTriINFO_01000b*)pNiTriInfoData )->s_dwObjectCount;
		//	if( dwObjectCount > 0 )
		//	{
		//		char cObjKey[ MAX_FILENAME ];
		//		sprintf_s( cObjKey, MAX_FILENAME, "%s_%d_object", pLeaf->GetParent()->GetQuadKey(), pLeaf->GetQuadID() );
		//		pObjectExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( cObjKey );
		//		assert_cs( pObjectExtData != NULL );
		//		pObjectExtData->GetValue( uiObjectSize, pObjectData );
		//		
		//		char* pNewObjectData = NiAlloc( char, uiObjectSize );
		//		memcpy( pNewObjectData, pObjectData, uiObjectSize );				
		//		UINT nOffset = 0, nDataOffset = 0;
		//		nsOBJECT_01000c::sFILE_HEADER* fh;
		//		nsOBJECT_01000c::sINFO* objInfo;
		//		for( DWORD i=0; i<dwObjectCount; ++i )
		//		{
		//			nOffset = nDataOffset;
		//			fh = (nsOBJECT_01000c::sFILE_HEADER*)&pNewObjectData[ nOffset ];
		//			nOffset += sizeof( nsOBJECT_01000c::sFILE_HEADER );
		//			if( fh->s_dwSaveType == CsGBObject::ST_FIRST_OBJECT )
		//			{
		//				objInfo = (nsOBJECT_01000c::sINFO*)&pNewObjectData[ nOffset ];
		//				objInfo->s_fScale = 1.0f;
		//			}
		//			nDataOffset += fh->s_dwInfoSize;
		//		}
		//		// ==== 오브젝트 익스트라 데이터 새로 기입
		//		pNewObjectExtData = NiNew NiBinaryExtraData( uiObjectOffset, pNewObjectData );
		//		// 기존 ExtraData 제거
		//		pNiNode->RemoveExtraData( cObjKey );
		//		pNiNode->AddExtraData( cObjKey, pNewObjectExtData );
		//	}
		//}
		//nsCsGBTerrain::g_pCurRoot->Delete();
		//nsCsGBTerrain::g_pCurRoot->Init();
		//nsCsGBTerrain::g_eCreateType = ctOrg;
		//return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	}

	bool vu01000e( CsNiNodePtr pNiNode, const char* cFile )
	{
		SetVersion( pNiNode, 0x01000e );

		char* pRootInfo = NULL;
		UINT uiRootSize = 0;
		NiBinaryExtraData* pRootExtData = pRootExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTerrainRootExtraKey );
		pRootExtData->GetValue( uiRootSize, pRootInfo );
		int nTotalLeafCount = ( (sROOTINFO_01000a*)pRootInfo )->s_nTotalLeafCount;

		// ========================================================================================================
		//		쉐도우맵 8비트 -> 24비트
		// ========================================================================================================
		char cStr[ MAX_PATH ];
		strcpy_s( cStr, MAX_PATH, cFile );
		size_t nLen = strlen( cStr );
		sprintf_s( &cStr[ nLen - 4 ], MAX_PATH - (nLen-4), "_SMorg.cst" );
		{
			NiStream stream;
			if( stream.Load( cStr ) == true )
			{
				bool bSave = false;
				NiPixelData** pDestPixelData = csnew NiPixelData*[ nTotalLeafCount ];
				{
					NiStream saveStream;			
					for( int i=0; i<nTotalLeafCount; ++i )
					{
						NiPixelData* pSrcPixelData = (NiPixelData*)stream.GetObjectAt( i );
						if( pSrcPixelData->GetPixelFormat() != SHADOW_MAP_FORMAT )
						{
							pDestPixelData[ i ] = NiNew NiPixelData( pSrcPixelData->GetWidth(), pSrcPixelData->GetHeight(), SHADOW_MAP_FORMAT );
							//NiImageConverter::GetImageConverter()->ConvertPixelData( *pSrcPixelData, SHADOW_MAP_FORMAT, pDestPixelData[ i ], false );
							int nPCount = pSrcPixelData->GetWidth()*pSrcPixelData->GetHeight();
							BYTE* pSrcPixel = pSrcPixelData->GetPixels();
							BYTE destColor;
							for( int p=0; p<nPCount; ++p )
							{
								destColor = pSrcPixel[ p ];
								( (BYTE*)pDestPixelData[ i ]->GetPixels() )[ p*3 + 0 ] = destColor;
								( (BYTE*)pDestPixelData[ i ]->GetPixels() )[ p*3 + 1 ] = destColor;
								( (BYTE*)pDestPixelData[ i ]->GetPixels() )[ p*3 + 2 ] = destColor;
							}
							saveStream.InsertObject( pDestPixelData[ i ] );
							bSave = true;
						}
					}
					if( bSave == true )
					{
						saveStream.Save( cStr );
					}
				}
				SAFE_DELETE_ARRAY( pDestPixelData );
			}
		}	
		

		// ========================================================================================================
		//		이펙트맵 - R값 BG에 복사
		// ========================================================================================================
		strcpy_s( cStr, MAX_PATH, cFile );		
		sprintf_s( &cStr[ nLen - 4 ], MAX_PATH - (nLen-4), "_EM.cst" );
		{
			NiStream stream;
			if( stream.Load( cStr ) == true )
			{
				NiStream saveStream;			
				for( int i=0; i<nTotalLeafCount; ++i )
				{
					NiPixelData* pSrcPixelData = (NiPixelData*)stream.GetObjectAt( i );
					int nPCount = pSrcPixelData->GetWidth()*pSrcPixelData->GetHeight();
					BYTE* pPixel = (BYTE*)pSrcPixelData->GetPixels();
					BYTE destColor;
					for( int p=0; p<nPCount; ++p )
					{
						destColor = pPixel[ p*4 + 0 ];
						pPixel[ p*4 + 1 ] = destColor;
						pPixel[ p*4 + 2 ] = destColor;
					}
					pSrcPixelData->MarkAsChanged();
					saveStream.InsertObject( pSrcPixelData );
				}
				saveStream.Save( cStr );
			}
		}
		
		return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	}

	bool vu01000f( CsNiNodePtr pNiNode, const char* cFile )
	{
		SetVersion( pNiNode, 0x01000f );

		NiBinaryExtraData* pRootExtData = NULL;
		NiBinaryExtraData* pLeafExtData = NULL;
		NiBinaryExtraData* pObjectExtData = NULL;
		NiBinaryExtraData* pNewObjectExtData = NULL;
		char* pRootInfo = NULL;
		char* pNiTriInfoData = NULL;
		char* pObjectData = NULL;

		UINT uiRootSize = 0, uiLeafSize = 0, uiObjectSize = 0, uiObjectOffset = 0, uiObjectNewOffset = 0;

		// 루트 정보 및 더미 Child 생성
		nsCsGBTerrain::eCREATE_TYPE ctOrg = nsCsGBTerrain::g_eCreateType;
		pRootExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTerrainRootExtraKey );
		pRootExtData->GetValue( uiRootSize, pRootInfo );

		int nTotalLeafCount = ( (sROOTINFO_01000a*)pRootInfo )->s_nTotalLeafCount;
		int nTreeLevel = ( (sROOTINFO_01000a*)pRootInfo )->s_nTreeLevel;

		nsCsGBTerrain::g_eCreateType = nsCsGBTerrain::CT_MAKE;
		nsCsGBTerrain::g_pCurRoot->Delete();
		nsCsGBTerrain::g_pCurRoot->Init();
		memcpy( nsCsGBTerrain::g_pCurRoot->GetInfo(), pRootInfo, sizeof( sROOTINFO_01000a ) );
		nsCsGBTerrain::g_pCurRoot->GetFieldInfo()->s_pfHeightField = NULL;
		nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->s_PathInfo.Reset();

		nsCsGBTerrain::g_pCurRoot->GetRootNode()->Create( CsGBTerrainNode::QI_ROOT,nsCsGBTerrain::g_pCurRoot->GetBaseNiNode(), nsCsGBTerrain::g_pCurRoot->GetRootNode(), NULL, nTreeLevel, 0 );

		char* pTempData = NULL;
		for( int nLeaf=0; nLeaf<nTotalLeafCount; ++nLeaf )
		{
			CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( nLeaf );

			char cQuadData[ 32 ] = {0, };
			sprintf_s( cQuadData, 32, "%s_%d_data", pLeaf->GetParent()->GetCharQuadKey(), pLeaf->GetQuadID() );
			// ==== TriInfo
			pLeafExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( cQuadData );
			assert_cs( pLeafExtData != NULL );
			pLeafExtData->GetValue( uiLeafSize, pNiTriInfoData );
			DWORD dwObjectCount = ( (sNiTriINFO_01000b*)pNiTriInfoData )->s_dwObjectCount;
			if( dwObjectCount > 0 )
			{
				char cObjKey[ MAX_FILENAME ];
				sprintf_s( cObjKey, MAX_FILENAME, "%s_%d_object", pLeaf->GetParent()->GetCharQuadKey(), pLeaf->GetQuadID() );
				pObjectExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( cObjKey );
				assert_cs( pObjectExtData != NULL );
				pObjectExtData->GetValue( uiObjectSize, pObjectData );

				// 템프 데이터
				assert_cs( pTempData == NULL );
				pTempData = csnew char[ uiObjectSize*3 ];
				uiObjectOffset = 0;
				uiObjectNewOffset = 0;
				for( DWORD i=0; i<dwObjectCount; ++i )
				{
					nsOBJECT_01000c::sFILE_HEADER fh;
					// fh 의 s_dwInfoSize 는 아직 쓰이는데 없으니 예전 값으로 대체하고 넘어가자 - 어차피 세이브할 새로 계산
					memcpy( &fh, &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_01000c::sFILE_HEADER ) );
					memcpy( &pTempData[ uiObjectNewOffset ], &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_01000c::sFILE_HEADER ) );										
					uiObjectOffset += sizeof( nsOBJECT_01000c::sFILE_HEADER );
					uiObjectNewOffset += sizeof( nsOBJECT_01000c::sFILE_HEADER );

					if( fh.s_dwSaveType == nsOBJECT_01000c::ST_EXIST_OBJECT )
					{
						nsOBJECT_01000c::sEXIST_INFO ei;
						memcpy( &ei, &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_01000c::sEXIST_INFO ) );
						memcpy( &pTempData[ uiObjectOffset ], &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_01000c::sEXIST_INFO ) );						
						uiObjectOffset += sizeof( nsOBJECT_01000c::sEXIST_INFO );
						uiObjectNewOffset += sizeof( nsOBJECT_01000c::sEXIST_INFO );
					}
					else
					{
						assert_cs( fh.s_dwSaveType == nsOBJECT_01000c::ST_FIRST_OBJECT );						
						nsOBJECT_01000c::sINFO oi;
						memcpy( &oi, &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_01000c::sINFO ) );
						memcpy( &pTempData[ uiObjectNewOffset ], &pObjectData[ uiObjectOffset ], sizeof( nsOBJECT_01000c::sINFO ) );
						uiObjectOffset += sizeof( nsOBJECT_01000c::sINFO );
						uiObjectNewOffset += sizeof( nsOBJECT_01000c::sINFO );

						DWORD dwMemcpySize = 0;
						UINT uiOrgObjectOffset = uiObjectOffset;
						UINT uiOrgObjectNewOffset = uiObjectNewOffset;
						// ======= 바꿀 값은 바꾸었다 - 남은 것 바로 저장
						// Path Engine
						if( oi.s_PathInfo.s_uiPEGroupCount != 0 )
						{
							dwMemcpySize += ( sizeof( NiPoint2 )*oi.s_PathInfo.s_uiPEGroupCount );
							dwMemcpySize += ( sizeof( USHORT )*oi.s_PathInfo.s_uiPEGroupCount );
							dwMemcpySize += ( sizeof( long )*2*oi.s_PathInfo.s_uiTotalPECount );
							uiObjectOffset += dwMemcpySize;
							uiObjectNewOffset += dwMemcpySize;
						}
						// BaseObject						
						if( ( oi.s_dwUserDefinePlag & sUSER_DEFINE_010008::UD_DARKMAP ) != 0 )
						{
							UINT usTemp;
							// uv set Load
							memcpy( &usTemp, &pObjectData[ uiObjectOffset ], sizeof( UINT ) );
							uiObjectOffset += sizeof( UINT );
							uiObjectOffset += sizeof( NiPoint2 )*usTemp;
							uiObjectNewOffset += sizeof( UINT );
							uiObjectNewOffset += sizeof( NiPoint2 )*usTemp;
							dwMemcpySize += sizeof( UINT );
							dwMemcpySize += sizeof( NiPoint2 )*usTemp;
							// pixel Save
							memcpy( &usTemp, &pObjectData[ uiObjectOffset ], sizeof( UINT ) );
							uiObjectOffset += sizeof( UINT );
							uiObjectOffset += sizeof( BYTE )*usTemp;
							uiObjectNewOffset += sizeof( UINT );
							uiObjectNewOffset += sizeof( BYTE )*3*usTemp;
							dwMemcpySize += sizeof( UINT );
							dwMemcpySize += sizeof( BYTE )*usTemp;
						}
						// Portal
						if( ( oi.s_dwUserDefinePlag & sUSER_DEFINE_010008::UD_PORTAL ) != 0 )
						{
							UINT uiPortalSize = sizeof( DWORD ) + sizeof( DWORD ) + sizeof( CsGBObj_Portal::sTARGET )*2;
							uiObjectOffset += uiPortalSize;
							uiObjectNewOffset += uiPortalSize;
							dwMemcpySize += uiPortalSize;
						}
						memcpy( &pTempData[ uiOrgObjectNewOffset ], &pObjectData[ uiOrgObjectOffset ], dwMemcpySize );						
					}
				}
				assert_cs( uiObjectNewOffset < uiObjectSize*3 );
				// ==== 오브젝트 익스트라 데이터 새로 기입
				char* pNewObjectData = NiAlloc( char, uiObjectNewOffset );
				memcpy( pNewObjectData, pTempData, uiObjectNewOffset );
				pNewObjectExtData = NiNew NiBinaryExtraData( uiObjectNewOffset, pNewObjectData );
				// 기존 ExtraData 제거
				pNiNode->RemoveExtraData( cObjKey );
				pNiNode->AddExtraData( cObjKey, pNewObjectExtData );

				SAFE_DELETE_ARRAY( pTempData );
			}
		}
		nsCsGBTerrain::g_pCurRoot->Delete();
		nsCsGBTerrain::g_pCurRoot->Init();
		nsCsGBTerrain::g_eCreateType = ctOrg;

		return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	}

	bool vu010010( CsNiNodePtr pNiNode, const char* cFile )
	{
		SetVersion( pNiNode, 0x010010 );

		NiBinaryExtraData* pRootExtData = NULL;
		NiBinaryExtraData* pLeafExtData = NULL;
		NiBinaryExtraData* pObjectExtData = NULL;
		NiBinaryExtraData* pNewObjectExtData = NULL;
		char* pRootInfo = NULL;
		char* pNiTriInfoData = NULL;
		char* pObjectOrgData = NULL;

		UINT uiRootSize = 0, uiLeafSize = 0, uiObjectSize = 0, uiObjectOffset = 0, uiObjectNewOffset = 0;

		// 루트 정보 및 더미 Child 생성
		nsCsGBTerrain::eCREATE_TYPE ctOrg = nsCsGBTerrain::g_eCreateType;
		pRootExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTerrainRootExtraKey );
		pRootExtData->GetValue( uiRootSize, pRootInfo );

		int nTotalLeafCount = ( (sROOTINFO_01000a*)pRootInfo )->s_nTotalLeafCount;
		int nTreeLevel = ( (sROOTINFO_01000a*)pRootInfo )->s_nTreeLevel;

		nsCsGBTerrain::g_eCreateType = nsCsGBTerrain::CT_MAKE;
		nsCsGBTerrain::g_pCurRoot->Delete();
		nsCsGBTerrain::g_pCurRoot->Init();
		memcpy( nsCsGBTerrain::g_pCurRoot->GetInfo(), pRootInfo, sizeof( sROOTINFO_01000a ) );
		nsCsGBTerrain::g_pCurRoot->GetFieldInfo()->s_pfHeightField = NULL;
		nsCsGBTerrain::g_pCurRoot->GetTrPathInfo()->s_PathInfo.Reset();

		nsCsGBTerrain::g_pCurRoot->GetRootNode()->Create( CsGBTerrainNode::QI_ROOT,nsCsGBTerrain::g_pCurRoot->GetBaseNiNode(), nsCsGBTerrain::g_pCurRoot->GetRootNode(), NULL, nTreeLevel, 0 );

		char* pTempData = NULL;
		for( int nLeaf=0; nLeaf<nTotalLeafCount; ++nLeaf )
		{
			CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( nLeaf );

			char cQuadData[ 32 ] = {0, };
			sprintf_s( cQuadData, 32, "%s_%d_data", pLeaf->GetParent()->GetCharQuadKey(), pLeaf->GetQuadID() );
			// ==== TriInfo
			pLeafExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( cQuadData );
			assert_cs( pLeafExtData != NULL );
			pLeafExtData->GetValue( uiLeafSize, pNiTriInfoData );
			DWORD dwObjectCount = ( (sNiTriINFO_01000b*)pNiTriInfoData )->s_dwObjectCount;
			if( dwObjectCount > 0 )
			{
				char cObjKey[ MAX_FILENAME ];
				sprintf_s( cObjKey, MAX_FILENAME, "%s_%d_object", pLeaf->GetParent()->GetCharQuadKey(), pLeaf->GetQuadID() );
				pObjectExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( cObjKey );
				assert_cs( pObjectExtData != NULL );
				pObjectExtData->GetValue( uiObjectSize, pObjectOrgData );

				// 템프 데이터
				assert_cs( pTempData == NULL );
				pTempData = csnew char[ uiObjectSize ];
				memcpy( pTempData, pObjectOrgData, uiObjectSize );
				uiObjectOffset = 0;
				for( DWORD i=0; i<dwObjectCount; ++i )
				{
					nsOBJECT_010010::sFILE_HEADER fh;
					// fh 의 s_dwInfoSize 는 아직 쓰이는데 없으니 예전 값으로 대체하고 넘어가자 - 어차피 세이브할 새로 계산
					memcpy( &fh, &pTempData[ uiObjectOffset ], sizeof( nsOBJECT_010010::sFILE_HEADER ) );					
					uiObjectOffset += sizeof( nsOBJECT_010010::sFILE_HEADER );

					if( fh.s_dwSaveType == nsOBJECT_010010::ST_EXIST_OBJECT )
					{
						uiObjectOffset += sizeof( nsOBJECT_010010::sEXIST_INFO );						
					}
					else
					{
						assert_cs( fh.s_dwSaveType == nsOBJECT_010010::ST_FIRST_OBJECT );

						nsOBJECT_010010::sINFO oi;
						memcpy( &oi, &pTempData[ uiObjectOffset ], sizeof( nsOBJECT_010010::sINFO ) );
						oi.s_bCheckDoAnimation = true;
						oi.s_fAniSpeed = 1.0f;
						oi.s_fStopAniTime = 0.0f;
						memcpy( &pTempData[ uiObjectOffset ], &oi, sizeof( nsOBJECT_010010::sINFO ) );
						uiObjectOffset += sizeof( nsOBJECT_010010::sINFO );		

						// ======= 바꿀 값은 바꾸었다 - 남은 것 바로 저장
						// Path Engine
						if( oi.s_PathInfo.s_uiPEGroupCount != 0 )
						{
							uiObjectOffset += ( sizeof( NiPoint2 )*oi.s_PathInfo.s_uiPEGroupCount );
							uiObjectOffset += ( sizeof( USHORT )*oi.s_PathInfo.s_uiPEGroupCount );
							uiObjectOffset += ( sizeof( long )*2*oi.s_PathInfo.s_uiTotalPECount );
						}
						// BaseObject						
						if( ( oi.s_dwUserDefinePlag & sUSER_DEFINE_010008::UD_DARKMAP ) != 0 )
						{
							UINT usTemp;
							// uv set Load
							memcpy( &usTemp, &pTempData[ uiObjectOffset ], sizeof( UINT ) );
							uiObjectOffset += sizeof( UINT );
							uiObjectOffset += sizeof( NiPoint2 )*usTemp;
							// pixel Save
							memcpy( &usTemp, &pTempData[ uiObjectOffset ], sizeof( UINT ) );
							uiObjectOffset += sizeof( UINT );
							uiObjectOffset += sizeof( BYTE )*3*usTemp;
						}
						// Portal
						if( ( oi.s_dwUserDefinePlag & sUSER_DEFINE_010008::UD_PORTAL ) != 0 )
						{
							UINT uiPortalSize = sizeof( DWORD ) + sizeof( DWORD ) + sizeof( CsGBObj_Portal::sTARGET )*2;
							uiObjectOffset += uiPortalSize;							
						}
					}
				}

				// ==== 오브젝트 익스트라 데이터 새로 기입
				char* pNewObjectData = NiAlloc( char, uiObjectOffset );
				memcpy( pNewObjectData, pTempData, uiObjectOffset );
				pNewObjectExtData = NiNew NiBinaryExtraData( uiObjectOffset, pNewObjectData );
				// 기존 ExtraData 제거
				pNiNode->RemoveExtraData( cObjKey );
				pNiNode->AddExtraData( cObjKey, pNewObjectExtData );

				SAFE_DELETE_ARRAY( pTempData );
			}
		}
		nsCsGBTerrain::g_pCurRoot->Delete();
		nsCsGBTerrain::g_pCurRoot->Init();
		nsCsGBTerrain::g_eCreateType = ctOrg;

		return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	}

	bool vu010011( CsNiNodePtr pNiNode, const char* cFile )
	{
		SetVersion( pNiNode, 0x010011 );

		NiBinaryExtraData* pRootExtData = NULL;
		char* pRootInfo = NULL;
		UINT uiRootSize = 0;

		// 루트 정보 및 더미 Child 생성		
		pRootExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTerrainRootExtraKey );
		pRootExtData->GetValue( uiRootSize, pRootInfo );

		( (sROOTINFO_010011*)pRootInfo )->s_nClipCellObject = 20;
		( (sROOTINFO_010011*)pRootInfo )->s_nCellObject_Size = 500;

		char* pNewData = NiAlloc( char, uiRootSize );
		memcpy( pNewData, pRootInfo, uiRootSize );
		NiBinaryExtraData* pNewExtData = NiNew NiBinaryExtraData( uiRootSize, pNewData );
		// 기존 ExtraData 제거
		pNiNode->RemoveExtraData( CsGBTerrainRootExtraKey );
		pNiNode->AddExtraData( CsGBTerrainRootExtraKey, pNewExtData );

		return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	}

	bool vu010012( CsNiNodePtr pNiNode, const char* cFile )
	{
		SetVersion( pNiNode, 0x010012 );

		NiBinaryExtraData* pExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBCELL_ExtraKey );
		if( pExtData == NULL )
		{
			return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
		}

		UINT nSize;
		UINT nOffset = 0;
		char* pData;
		pExtData->GetValue( nSize, pData );

		int nBaseObjectKind;
		memcpy( &nBaseObjectKind, &pData[ nOffset ], sizeof( int ) );
		nOffset += sizeof( int );

		UINT nNewOffset = 0;
		UINT nNewSize = nSize+nBaseObjectKind*( sizeof( nsSPEEDCELL_010012::sOBJECT_INFO ) - sizeof( nsSPEEDCELL_010011::sOBJECT_INFO ) );
		char* pNewData = csnew char[ nNewSize ];
		memcpy( &pNewData[ nNewOffset ], &nBaseObjectKind, sizeof( int ) );
		nNewOffset += sizeof( int );

		for( int i=0; i<nBaseObjectKind; ++i )
		{
			nsSPEEDCELL_010011::sOBJECT_INFO obj;
			memcpy( &obj, &pData[ nOffset ], sizeof( nsSPEEDCELL_010011::sOBJECT_INFO ) );
			nOffset += sizeof( nsSPEEDCELL_010011::sOBJECT_INFO );
			memcpy( &pNewData[ nNewOffset ], &obj, sizeof( nsSPEEDCELL_010011::sOBJECT_INFO ) );
			( ( nsSPEEDCELL_010012::sOBJECT_INFO* )&pNewData[ nNewOffset ] )->s_fCheckTimeMin = 2.0f;
			( ( nsSPEEDCELL_010012::sOBJECT_INFO* )&pNewData[ nNewOffset ] )->s_fCheckTimeMax = 5.0f;
			( ( nsSPEEDCELL_010012::sOBJECT_INFO* )&pNewData[ nNewOffset ] )->s_fAniSpeedMin = 0.2f;
			( ( nsSPEEDCELL_010012::sOBJECT_INFO* )&pNewData[ nNewOffset ] )->s_fAniSpeedMax = 1.2f;
			nNewOffset += sizeof( nsSPEEDCELL_010012::sOBJECT_INFO );
		}

		memcpy( &pNewData[ nNewOffset ], &pData[ nOffset ], nSize-nOffset );

		char* pNewSaveData = NiAlloc( char, nNewSize );
		memcpy( pNewSaveData, pNewData, nNewSize );
		NiBinaryExtraData* pNewExtData = NiNew NiBinaryExtraData( nNewSize, pNewSaveData );
		// 기존 ExtraData 제거
		pNiNode->RemoveExtraData( CsGBCELL_ExtraKey );
		pNiNode->AddExtraData( CsGBCELL_ExtraKey, pNewExtData );

		return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	}

	bool vu010013( CsNiNodePtr pNiNode, const char* cFile )
	{
		SetVersion( pNiNode, 0x010013 );

		NiBinaryExtraData* pExtData = NULL;
		char* pInfo = NULL;
		UINT uiSize = 0;

		//======================================================================================================
		//
		//		루드 인포
		//
		//======================================================================================================
		pExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTerrainRootExtraKey );
		pExtData->GetValue( uiSize, pInfo );

		int nVertCount = ( (sROOTINFO_010011*)pInfo )->s_nXVertCount*( (sROOTINFO_010011*)pInfo )->s_nYVertCount;

		//======================================================================================================
		//
		//		필드 인포
		//
		//======================================================================================================
		{
			pExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTR_FieldExtraKey );
			assert_cs( pExtData != NULL );	
			pExtData->GetValue( uiSize, pInfo );

			uiSize = sizeof( float )*nVertCount;
			char* pField = NiAlloc( char, sizeof( float )*nVertCount );
			memcpy( pField, pInfo, sizeof( float )*nVertCount );

			NiBinaryExtraData* pNewExtData = NiNew NiBinaryExtraData( uiSize, pField );
			// 기존 ExtraData 제거
			pNiNode->RemoveExtraData( CsGBTR_FieldExtraKey );
			pNiNode->AddExtraData( CsGBTR_FieldExtraKey, pNewExtData );
		}

		//======================================================================================================
		//
		//		패쓰 인포
		//
		//======================================================================================================
		{
			pExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTR_PathExtraKey );
			assert_cs( pExtData != NULL );	
			pExtData->GetValue( uiSize, pInfo );

			sTR_PATHINFO_01000a TrPathInfo;
			int nOffset = 0;
			int nNewOffset = 0;
			memcpy( &TrPathInfo, &pInfo[ nOffset ], sizeof( sTR_PATHINFO_01000a ) );			

			// 메모리 할당
			UINT uiPEGroupCount = TrPathInfo.s_PathInfo.s_uiPEGroupCount;
			UINT nNewSize = sizeof( sTR_PATHINFO_010013 ) + sizeof( NiPoint2 )*uiPEGroupCount + sizeof( long )*uiPEGroupCount*8 + sizeof( char )*uiPEGroupCount;			 
			char* pNewData = NiAlloc( char, nNewSize );

			// Info 데이타 입력
			memcpy( &pNewData[ nNewOffset ], &pInfo[ nOffset ], sizeof( sTR_PATHINFO_01000a ) );
			( (sTR_PATHINFO_010013*)&pNewData[ nNewOffset ] )->s_nDataTotalCount = uiPEGroupCount*8;
			nOffset += sizeof( sTR_PATHINFO_01000a );
			nNewOffset += sizeof( sTR_PATHINFO_010013 );

			memset( &pNewData[ nNewOffset ], 0, sizeof( char )*uiPEGroupCount );
			nNewOffset += sizeof( char )*uiPEGroupCount;

			memcpy( &pNewData[ nNewOffset ], &pInfo[ nOffset ], sizeof( NiPoint2 )*uiPEGroupCount );
			nOffset += sizeof( NiPoint2 )*uiPEGroupCount;
			nNewOffset += sizeof( NiPoint2 )*uiPEGroupCount;

			memcpy( &pNewData[ nNewOffset ], &pInfo[ nOffset ], sizeof( long )*uiPEGroupCount*8 );

			pExtData = NiNew NiBinaryExtraData( nNewSize, pNewData );
			pNiNode->RemoveExtraData( CsGBTR_PathExtraKey );
			pNiNode->AddExtraData( CsGBTR_PathExtraKey, pExtData );

		}		
		return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	}

	//// 이펙트 파일 - 그레이마스크 채널 0x00 -> 0xff 로
	//bool vu000402( NiNode* pNiNode, const char* cFile )
	//{
	//	SetVersion( pNiNode, 0x0403 );

	//	// ==== 이펙트 파일 
	//	char cStr[ MAX_PATH ];
	//	strcpy_s( cStr, MAX_PATH, cFile );
	//	size_t nLen = strlen( cStr );
	//	sprintf_s( &cStr[ nLen - 4 ], MAX_PATH - (nLen-4), "_EM.cst" );

	//	NiStream LoadStream;
	//	NiStream SaveStream;
	//	bool bLoad = LoadStream.Load( cStr );
	//	if( bLoad == true )
	//	{
	//		int nStreamCount = LoadStream.GetObjectCount();
	//		NiPixelData* pPixelData = NULL;
	//		BYTE* pPixel = NULL;
	//		int nPixelCount, nStride;
	//		for( int i=0; i<nStreamCount; ++i )
	//		{
	//			pPixelData = (NiPixelData*)LoadStream.GetObjectAt( i );
	//			nPixelCount = pPixelData->GetWidth()*pPixelData->GetHeight();
	//			pPixel = (BYTE*)pPixelData->GetPixels();
	//			nStride = pPixelData->GetPixelStride();
	//			for( int nPixel=0; nPixel<nPixelCount; ++nPixel )
	//				pPixel[ nPixel*nStride + EFFECT_GRAYMASK_MAP_CHANNEL ] = 0xff;

	//			SaveStream.InsertObject( pPixelData );
	//		}
	//		SaveStream.Save( cStr );
	//	}		
	//	return nsCsGBTerrain::g_pCurRoot->LoadExtraData( pNiNode, cFile );
	//}
}


bool CsGBTerrainRoot::_VersionUp( CsNiNodePtr pNiNode, const char* cFile, DWORD dwVersion )
{
	switch( dwVersion )
	{
	// 걍 버젼업
	case 0x00000402:		return nsCsGBVerionUp::vu010000( pNiNode, cFile );
	// 오브젝트별 ExtraData -> 노드별 통합 오브젝트 ExtraData
	case 0x00010000:		return nsCsGBVerionUp::vu010001( pNiNode, cFile );
	// 여러노드에 걸쳐 존재하는 오브젝트일 경우 - 처음 한번만 저장, 다른 노드에선 ID만 저장, 오브젝트 앞에 식별자 넣자
	case 0x00010001:		return nsCsGBVerionUp::vu010002( pNiNode, cFile );
	// 룻트 인포에 스카이박스 경로 포함
	case 0x00010002:		return nsCsGBVerionUp::vu010003( pNiNode, cFile );
	// 포그
	case 0x00010003:		return nsCsGBVerionUp::vu010004( pNiNode, cFile );
	// 오브젝트 - 패쓰엔진 구조체 변경
	case 0x00010004:		return nsCsGBVerionUp::vu010005( pNiNode, cFile );
	// 룻트인포에 클리핑 정보 포함
	case 0x00010005:		return nsCsGBVerionUp::vu010006( pNiNode, cFile );
	// 오브젝트 타입별 정리
	case 0x00010006:		return nsCsGBVerionUp::vu010007( pNiNode, cFile );
	// 지형 자체에 패쓰 정보 포함
	case 0x00010007:		return nsCsGBVerionUp::vu010008( pNiNode, cFile );
	// TB, TBP 에서 DarkMap 분리
	case 0x00010008:		return nsCsGBVerionUp::vu010009( pNiNode, cFile );
	// RootInfo 에서 Info 부분과 동적 메모리 할당 부분들 전부 분리
	// SkyBox Info 부분 추가
	case 0x00010009:		return nsCsGBVerionUp::vu01000a( pNiNode, cFile );
	// Mapping 증가 4 -> 5
	case 0x0001000a:		return nsCsGBVerionUp::vu01000b( pNiNode, cFile );
	// Light 정보 별도로 추가
	case 0x0001000b:		return nsCsGBVerionUp::vu01000c( pNiNode, cFile );
	// Object 에 Scale 값 추가
	case 0x0001000c:		return nsCsGBVerionUp::vu01000d( pNiNode, cFile );
	// 쉐도우맵 및 다크맵 8 -> 24
	case 0x0001000d:		return nsCsGBVerionUp::vu01000e( pNiNode, cFile );
	// DM 오브젝트 8 -> 24
	case 0x0001000e:		return nsCsGBVerionUp::vu01000f( pNiNode, cFile );
	// 오브젝트 정보에 애니메이션 정보 포함 ( 애니메이션 컨트롤 )
	case 0x0001000f:		return nsCsGBVerionUp::vu010010( pNiNode, cFile );
	// 루트 인포에 ClipObject size 와 Range 정보 포함
	case 0x00010010:		return nsCsGBVerionUp::vu010011( pNiNode, cFile );
	// 스피드 셀 정보에 랜덤 애니메이션 조절 값 추가
	case 0x00010011:		return nsCsGBVerionUp::vu010012( pNiNode, cFile );
	// 룻트노드에 pptNormal 없애고, 맵툴패쓰정보에 타입정보 추가
	case 0x00010012:		return nsCsGBVerionUp::vu010013( pNiNode, cFile );
	}

	assert_cs( false );
	return false;
}