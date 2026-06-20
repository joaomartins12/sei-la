#include "stdafx.h"
#include "RollBack.h"

cRollBack	g_RollBack;

void cRollBack::sRB_DATA::Delete()
{
	LIST_RT_BASE_IT it = s_List.begin();
	for( ; it!=s_List.end(); ++it )
	{
		switch( ( *it )->s_eType )
		{
		case RT_ALPHA_MAPPING:
			g_RollBack.m_poolAM.push( (sRT_AM_INFO*)( *it ) );
			break;
		case RT_EFFECT_MAPPING:
			g_RollBack.m_poolEM.push( (sRT_EM_INFO*)( *it ) );
			break;
		case RT_OBJ_RESIST:
			{
				sRT_OBJ_RESIST_INFO* pObjResist = (sRT_OBJ_RESIST_INFO*)(*it);
				pObjResist->s_pObject->DecreaseRefCount();
				pObjResist->s_pObject = NULL;
				g_RollBack.m_poolObjResist.Push( pObjResist );
			}			
			break;
		case RT_OBJ_DELETE:
			{
				sRT_OBJ_DELETE_INFO* pObjResist = (sRT_OBJ_DELETE_INFO*)(*it);
				pObjResist->s_pObject->DecreaseRefCount();
				pObjResist->s_pObject = NULL;
				g_RollBack.m_poolObjDelete.Push( pObjResist );
			}
			break;
		case RT_HEIGHT:
			g_RollBack.m_poolHeight.push( (sRT_HEIGHT_INFO*)( *it ) );
			break;
		case RT_CELL_RESIST:
			{
				sRT_CELL_RESIST_INFO* pCellResist = (sRT_CELL_RESIST_INFO*)(*it);
				g_RollBack.m_poolCellResist.Push( pCellResist );
			}
			break;
		case RT_CELL_DELETE:
			{
				sRT_CELL_DELETE_INFO* pCellDelete = (sRT_CELL_DELETE_INFO*)(*it);
				g_RollBack.m_poolCellDelete.Push( pCellDelete );
			}
			break;
		default:
			assert_cs( false );
		}
	}
	s_List.clear();
}

cRollBack::cRollBack()
{
	m_ppUnDoData = NULL;
	m_ppReDoData = NULL;
	m_pTempData = NULL;
}

void cRollBack::Delete()
{
	assert_cs( m_bCheckLeak == false );

	_DeleteUnDo();
	_DeleteReDo();
	_DeleteMemPool();
}

void cRollBack::_DeleteMemPool()
{
	while( m_poolAM.empty() == false )
	{
		sRT_AM_INFO* pInfo = m_poolAM.top();
		m_poolAM.pop();
		delete[] pInfo->s_pData;
		delete pInfo;
	}

	while( m_poolEM.empty() == false )
	{
		sRT_EM_INFO* pInfo = m_poolEM.top();
		m_poolEM.pop();
		delete[] pInfo->s_pData;
		delete pInfo;
	}

	while( m_poolHeight.empty() == false )
	{
		sRT_HEIGHT_INFO* pInfo = m_poolHeight.top();
		m_poolHeight.pop();
		for( int i=0; i<pInfo->s_npfHeightCount; ++i )
		{
			delete[] pInfo->s_ppfHeight[ i ];
		}
		delete[] pInfo->s_ppfHeight;
		delete pInfo;
	}
}

void cRollBack::Init()
{
	m_bCheckLeak = false;	

	_InitUnDo();
	_InitReDo();

	Reset();
}

void cRollBack::Reset()
{
	assert_cs( m_bCheckLeak == false );

	m_nUnDoCount = 0;
	m_nRollBackIndex = 0;
	m_bBegin = false;
	m_pTempData = NULL;
}

void cRollBack::Begin()
{
	assert_cs( m_bBegin == false );
	assert_cs( m_bCheckLeak == false );
	m_bCheckLeak = true;

	// 현재 인덱스 뒷부분 날려주고 앞으로 땡겨준다
	if( ( m_nRollBackIndex != 0 )&&( m_nUnDoCount != 0 ) )
	{
		// 롤백 인덱스 만큼 날려준다
		for( int i=0; i<m_nRollBackIndex; ++i )
		{
			m_ppUnDoData[ i ]->Delete();
			m_poolRBData.Push( m_ppUnDoData[ i ] );
			m_ppUnDoData[ i ] = NULL;
		}

		for( int i=m_nRollBackIndex; i<m_nUnDoCount; ++i )
		{
			m_ppUnDoData[ i-m_nRollBackIndex ] = m_ppUnDoData[ i ];
			m_ppUnDoData[ i ] = NULL;
		}
		m_nUnDoCount -= m_nRollBackIndex;
	}

	// 모든 리두 삭제
	_EmptyReDo();

	m_nRollBackIndex = 0;

	assert_cs( m_pTempData == NULL );
	m_pTempData = m_poolRBData.Pop();	

	m_bBegin = true;
}

void cRollBack::Push( sRT_BASE* pData )
{
	assert_cs( m_bCheckLeak == true );
	assert_cs( pData != NULL );
	switch( pData->s_eType )
	{
	case RT_ALPHA_MAPPING:
		_PushAM( (sRT_AM_INFO* )pData );
		break;
	case RT_EFFECT_MAPPING:
		_PushEM( (sRT_EM_INFO* )pData );
		break;
	case RT_OBJ_RESIST:
		_PushObjResist( (sRT_OBJ_RESIST_INFO*)pData );
		break;
	case RT_OBJ_DELETE:
		_PushObjDelete( (sRT_OBJ_DELETE_INFO*)pData );
		break;
	case RT_HEIGHT:
		_PushHeight( (sRT_HEIGHT_INFO*)pData );
		break;
	case RT_CELL_RESIST:
		_PushCellResist( (sRT_CELL_RESIST_INFO*)pData );
		break;
	case RT_CELL_DELETE:
		_PushCellDelete( (sRT_CELL_DELETE_INFO*)pData );
		break;
	default:
		assert_cs( false );
	}	
}

void cRollBack::End()
{
	if( m_bBegin == false )
		return;

	m_bBegin = false;

	assert_cs( m_bCheckLeak == true );
	m_bCheckLeak = false;

	if( m_pTempData->s_List.size() == 0 )
	{
		m_poolRBData.Push( m_pTempData );
		m_pTempData = NULL;
		return;
	}

	// 데이터가 다 찼다면 마지막 데이터 제거
	if( m_nUnDoCount == g_Config.GetRollBack()->s_nRollBackCount )
	{
		m_ppUnDoData[ m_nUnDoCount - 1 ]->Delete();
		m_poolRBData.Push( m_ppUnDoData[ m_nUnDoCount - 1 ] );
		m_ppUnDoData[ m_nUnDoCount - 1 ] = NULL;
		--m_nUnDoCount;
	}

	++m_nUnDoCount;

	// 뒤에서 부터 앞에 있던 내용을 뒤로 미룬다.
	for( int i=m_nUnDoCount-1; i>0; --i )
	{
		assert_cs( m_ppUnDoData[ i ] == NULL );
		m_ppUnDoData[ i ] = m_ppUnDoData[ i-1 ];
		m_ppUnDoData[ i - 1 ] = NULL;
	}
	assert_cs( m_ppUnDoData[ 0 ] == NULL );
	m_ppUnDoData[ 0 ] = m_pTempData;
	m_pTempData = NULL;
}

//==========================================================================================
//
//		UnDo
//
//==========================================================================================

void cRollBack::_DeleteUnDo()
{
	assert_cs( m_ppUnDoData != NULL );
	for( int i=0; i<g_Config.GetRollBack()->s_nRollBackCount; ++i )
	{
		if( m_ppUnDoData[ i ] != NULL )
		{
			m_ppUnDoData[ i ]->Delete();
			m_poolRBData.Push( m_ppUnDoData[ i ] );			
		}		
	}
	SAFE_DELETE_ARRAY( m_ppUnDoData );
}

void cRollBack::_InitUnDo()
{
	assert_cs( m_ppUnDoData == NULL );

	m_ppUnDoData = csnew sRB_DATA*[ g_Config.GetRollBack()->s_nRollBackCount ];
	for( int i=0; i<g_Config.GetRollBack()->s_nRollBackCount; ++i )
	{
		m_ppUnDoData[ i ] = NULL;
	}
}

void cRollBack::UnDo()		//컨트롤 z 했을 때 여기로 들어온다
{
	// 저장 된 정보보다 더이상 롤백 할게 없다면 리턴
	if( m_nRollBackIndex == m_nUnDoCount )
		return;	

	// 인덱스의 포인터 얻어온다
	sRB_DATA* pRBData = m_ppUnDoData[ m_nRollBackIndex ];

	_WriteReDo();

	LIST_RT_BASE_IT it = pRBData->s_List.begin();
	for( ; it!=pRBData->s_List.end(); ++it )
	{
		switch( (*it)->s_eType )
		{
		case RT_ALPHA_MAPPING:			
			_DoAM( (sRT_AM_INFO*)(*it) );
			break;
		case RT_EFFECT_MAPPING:			
			_DoEM( (sRT_EM_INFO*)(*it) );
			break;
		case RT_OBJ_RESIST:
			_UnDoObjResist( (sRT_OBJ_RESIST_INFO*)(*it) );
			break;
		case RT_OBJ_DELETE:
			_UnDoObjDelete( (sRT_OBJ_DELETE_INFO*)(*it) );
			break;
		case RT_HEIGHT:
			_UnDoHeight( (sRT_HEIGHT_INFO*)(*it) );
			break;
		case RT_CELL_RESIST:
			_UnDoCellResist( (sRT_CELL_RESIST_INFO*)(*it) );
			break;
		case RT_CELL_DELETE:
			_UnDoCellDelete( (sRT_CELL_DELETE_INFO*)(*it) );
			break;
		default:
			assert_cs( false );
		}
	}
	
	// 인덱스 증가
	++m_nRollBackIndex;
}

//==========================================================================================
//
//		ReDo
//
//==========================================================================================

void cRollBack::_DeleteReDo()
{
	_EmptyReDo();
	SAFE_DELETE_ARRAY( m_ppReDoData );
}

void cRollBack::_EmptyReDo()
{
	assert_cs( m_ppReDoData != NULL );
	for( int i=0; i<g_Config.GetRollBack()->s_nRollBackCount; ++i )
	{
		if( m_ppReDoData[ i ] != NULL )
		{
			m_ppReDoData[ i ]->Delete();
			m_poolRBData.Push( m_ppReDoData[ i ] );
			m_ppReDoData[ i ] = NULL;
		}
	}
}

void cRollBack::_InitReDo()
{
	assert_cs( m_ppReDoData == NULL );

	m_ppReDoData = csnew sRB_DATA*[ g_Config.GetRollBack()->s_nRollBackCount ];
	for( int i=0; i<g_Config.GetRollBack()->s_nRollBackCount; ++i )
	{
		m_ppReDoData[ i ] = NULL;
	}
}

void cRollBack::_WriteReDo()
{
	if( m_ppReDoData[ m_nRollBackIndex ] != NULL )
		return;

	m_ppReDoData[ m_nRollBackIndex ] = m_poolRBData.Pop();
	sRB_DATA* pRBData = m_ppUnDoData[ m_nRollBackIndex ];
	LIST_RT_BASE_IT it = pRBData->s_List.begin();
	for( ; it!=pRBData->s_List.end(); ++it )
	{
		switch( (*it)->s_eType )
		{
		case RT_ALPHA_MAPPING:
			m_ppReDoData[ m_nRollBackIndex ]->s_List.push_back( _GetCurTRMappingInfo( (sRT_AM_INFO*)(*it) ) );
			break;
		case RT_EFFECT_MAPPING:
			m_ppReDoData[ m_nRollBackIndex ]->s_List.push_back( _GetCurTRMappingInfo( (sRT_EM_INFO*)(*it) ) );
			break;
		case RT_OBJ_RESIST:
			m_ppReDoData[ m_nRollBackIndex ]->s_List.push_back( _GetNewObjResistInfo( (sRT_OBJ_RESIST_INFO*)(*it) ) );
			break;
		case RT_OBJ_DELETE:
			m_ppReDoData[ m_nRollBackIndex ]->s_List.push_back( _GetNewObjDeleteInfo( (sRT_OBJ_DELETE_INFO*)(*it) ) );
			break;
		case RT_HEIGHT:
			m_ppReDoData[ m_nRollBackIndex ]->s_List.push_back( _GetNewHeightInfo( (sRT_HEIGHT_INFO*)(*it), false ) );
			break;
		case RT_CELL_RESIST:
			m_ppReDoData[ m_nRollBackIndex ]->s_List.push_back( _GetNewCellResistInfo( (sRT_CELL_RESIST_INFO*)(*it) ) );
			break;
		case RT_CELL_DELETE:
			m_ppReDoData[ m_nRollBackIndex ]->s_List.push_back( _GetNewCellDeleteInfo( (sRT_CELL_DELETE_INFO*)(*it) ) );
			break;
		default:
			assert_cs( false );
		}
	}
}

void cRollBack::ReDo()
{
	if( m_nRollBackIndex == 0 )
		return;

	// 인덱스의 포인터 얻어온다	
	sRB_DATA* pRBData = m_ppReDoData[ m_nRollBackIndex - 1 ];

	LIST_RT_BASE_IT it = pRBData->s_List.begin();
	for( ; it!=pRBData->s_List.end(); ++it )
	{
		switch( (*it)->s_eType )
		{
		case RT_ALPHA_MAPPING:
			_DoAM( (sRT_AM_INFO*)(*it) );
			break;
		case RT_EFFECT_MAPPING:
			_DoEM( (sRT_EM_INFO*)(*it) );
			break;
		case RT_OBJ_RESIST:
			_ReDoObjResist( (sRT_OBJ_RESIST_INFO*)(*it) );
			break;
		case RT_OBJ_DELETE:
			_ReDoObjDelete( (sRT_OBJ_DELETE_INFO*)(*it) );
			break;
		case RT_HEIGHT:
			_ReDoHeight( (sRT_HEIGHT_INFO*)(*it) );
			break;
		case RT_CELL_RESIST:
			_ReDoCellResist( (sRT_CELL_RESIST_INFO*)(*it) );
			break;
		case RT_CELL_DELETE:
			_ReDoCellDelete( (sRT_CELL_DELETE_INFO*)(*it) );
			break;
		default:
			assert_cs( false );
		}
	}

	// 인덱스 감소
	--m_nRollBackIndex;
}

//==========================================================================================
//
//		Alpha Mapping
//
//==========================================================================================

cRollBack::sRT_AM_INFO* cRollBack::_GetCurTRMappingInfo( sRT_AM_INFO* pInfo )
{
	// Texture정보를 알아 내자
	NiPixelData* pPixelData = GET_SHADERMAP_PIXELDATA(	nsCsGBTerrain::g_pCurRoot->GetNode( pInfo->s_nObjectID )->GetNiNode(),
														pInfo->s_nNTM );
	unsigned int uiPixelCount = pPixelData->GetWidth()*pPixelData->GetHeight()*pInfo->s_nStride;

	// 메모리 풀이 비여 있다면 새로 생성 하자
	sRT_AM_INFO* pNewInfo;
	if( m_poolAM.empty() == true )
	{
		pNewInfo = csnew sRT_AM_INFO;
		pNewInfo->s_pData = csnew BYTE[ uiPixelCount ];
	}
	else
	{
		pNewInfo = m_poolAM.top();
		m_poolAM.pop();
	}
	assert_cs( pNewInfo != NULL );

	// 정보 복사
	pNewInfo->s_nObjectID	= pInfo->s_nObjectID;
	pNewInfo->s_nNTM		= pInfo->s_nNTM;
	pNewInfo->s_nStride		= pInfo->s_nStride;
	memcpy( pNewInfo->s_pData, pPixelData->GetPixels(), sizeof( BYTE )*uiPixelCount );
	return pNewInfo;
}

void cRollBack::_PushAM( sRT_AM_INFO* pInfo )
{
	assert_cs( m_pTempData != NULL );
	m_pTempData->s_List.push_back( _GetCurTRMappingInfo( pInfo ) );
}

void cRollBack::_DoAM( sRT_AM_INFO* pInfo )
{
	NiPixelData* pPixelData = GET_SHADERMAP_PIXELDATA(	nsCsGBTerrain::g_pCurRoot->GetNode( pInfo->s_nObjectID )->GetNiNode(),
														pInfo->s_nNTM );
	unsigned int uiPixelCount = pPixelData->GetWidth()*pPixelData->GetHeight()*pInfo->s_nStride;
	BYTE* pPixel = (BYTE*)pPixelData->GetPixels();
	memcpy( pPixel, pInfo->s_pData, sizeof( BYTE )*uiPixelCount );
	pPixelData->MarkAsChanged();
}

//==========================================================================================
//
//		Effect Mapping
//
//==========================================================================================

cRollBack::sRT_EM_INFO* cRollBack::_GetCurTRMappingInfo( sRT_EM_INFO* pInfo )
{
	// Texture정보를 알아 내자
	NiPixelData* pPixelData = GET_SHADERMAP_PIXELDATA(	nsCsGBTerrain::g_pCurRoot->GetNode( pInfo->s_nObjectID )->GetNiNode(),
														pInfo->s_nNTM );
	unsigned int uiPixelCount = pPixelData->GetWidth()*pPixelData->GetHeight()*pInfo->s_nStride;

	// 메모리 풀이 비여 있다면 새로 생성 하자
	sRT_EM_INFO* pNewInfo;
	if( m_poolEM.empty() == true )
	{
		pNewInfo = csnew sRT_EM_INFO;
		pNewInfo->s_pData = csnew BYTE[ uiPixelCount ];
	}
	else
	{
		pNewInfo = m_poolEM.top();
		m_poolEM.pop();
	}
	assert_cs( pNewInfo != NULL );

	// 정보 복사
	pNewInfo->s_nObjectID	= pInfo->s_nObjectID;
	pNewInfo->s_nNTM		= pInfo->s_nNTM;
	pNewInfo->s_nStride		= pInfo->s_nStride;
	memcpy( pNewInfo->s_pData, pPixelData->GetPixels(), sizeof( BYTE )*uiPixelCount );
	return pNewInfo;
}

void cRollBack::_PushEM( sRT_EM_INFO* pInfo )
{
	assert_cs( m_pTempData != NULL );
	m_pTempData->s_List.push_back( _GetCurTRMappingInfo( pInfo ) );
}

void cRollBack::_DoEM( sRT_EM_INFO* pInfo )
{
	NiPixelData* pPixelData = GET_SHADERMAP_PIXELDATA(	nsCsGBTerrain::g_pCurRoot->GetNode( pInfo->s_nObjectID )->GetNiNode(),
														pInfo->s_nNTM );
	unsigned int uiPixelCount = pPixelData->GetWidth()*pPixelData->GetHeight()*pInfo->s_nStride;
	BYTE* pPixel = (BYTE*)pPixelData->GetPixels();
	memcpy( pPixel, pInfo->s_pData, sizeof( BYTE )*uiPixelCount );
	pPixelData->MarkAsChanged();
}

//==========================================================================================
//
//		Object Resist
//
//==========================================================================================

cRollBack::sRT_OBJ_RESIST_INFO* cRollBack::_GetNewObjResistInfo( sRT_OBJ_RESIST_INFO* pInfo )
{
	sRT_OBJ_RESIST_INFO* pNewInfo = m_poolObjResist.Pop();
	pNewInfo->s_pObject = pInfo->s_pObject;
	pNewInfo->s_pObject->IncreaseRefCount();
	return pNewInfo;
}

void cRollBack::_PushObjResist( sRT_OBJ_RESIST_INFO* pInfo )
{
	assert_cs( pInfo->s_eType == RT_OBJ_RESIST );
	assert_cs( m_pTempData != NULL );
	
	m_pTempData->s_List.push_back( _GetNewObjResistInfo( pInfo ) );
}

void cRollBack::_UnDoObjResist( sRT_OBJ_RESIST_INFO* pInfo )
{
	// 셀렉트 오브젝트 인가
	if( g_ObjectMng.DeleteSelectModel( pInfo->s_pObject->GetInfo()->s_dwUniqID ) == true )
		return;

	nsCsGBTerrain::g_pCurRoot->DeleteCsGBObject( pInfo->s_pObject->GetInfo()->s_dwUniqID );
}

void cRollBack::_ReDoObjResist( sRT_OBJ_RESIST_INFO* pInfo )
{
	pInfo->s_pObject->GetInfo()->s_Bound.Reset();			
	if( nsCSGBFUNC::CalMinMaxBound( pInfo->s_pObject->GetNiNode(), pInfo->s_pObject->GetNiNode()->GetWorldTransform(), &pInfo->s_pObject->GetInfo()->s_Bound ) == false )
	{
		pInfo->s_pObject->GetInfo()->s_Bound.s_ptMin = NiPoint2( pInfo->s_pObject->GetNiNode()->GetWorldTransform().m_Translate.x, pInfo->s_pObject->GetNiNode()->GetWorldTransform().m_Translate.y );
		pInfo->s_pObject->GetInfo()->s_Bound.s_ptMax = pInfo->s_pObject->GetInfo()->s_Bound.s_ptMin;
	}
	if( pInfo->s_pObject->GetInfo()->s_Bound.s_ptMin.x > pInfo->s_pObject->GetInfo()->s_Bound.s_ptMax.x )
	{
		pInfo->s_pObject->GetInfo()->s_Bound.s_ptMin.x = pInfo->s_pObject->GetInfo()->s_Bound.s_ptMax.x = pInfo->s_pObject->GetNiNode()->GetWorldTransform().m_Translate.x;
		pInfo->s_pObject->GetInfo()->s_Bound.s_ptMin.y = pInfo->s_pObject->GetInfo()->s_Bound.s_ptMax.y = pInfo->s_pObject->GetNiNode()->GetWorldTransform().m_Translate.y;
	}

	nsCsGBTerrain::g_pCurRoot->ResistObject_ChangeRotation( pInfo->s_pObject->GetInfo() );
}

//==========================================================================================
//
//		Object Delete
//
//==========================================================================================

cRollBack::sRT_OBJ_DELETE_INFO* cRollBack::_GetNewObjDeleteInfo( sRT_OBJ_DELETE_INFO* pInfo )
{
	sRT_OBJ_DELETE_INFO* pNewInfo = m_poolObjDelete.Pop();
	pNewInfo->s_pObject = pInfo->s_pObject;
	pNewInfo->s_pObject->IncreaseRefCount();
	return pNewInfo;
}

void cRollBack::_PushObjDelete( sRT_OBJ_DELETE_INFO* pInfo )
{
	assert_cs( pInfo->s_eType == RT_OBJ_DELETE );
	assert_cs( m_pTempData != NULL );

	m_pTempData->s_List.push_back( _GetNewObjDeleteInfo( pInfo ) );
}

void cRollBack::_UnDoObjDelete( sRT_OBJ_DELETE_INFO* pInfo )
{
	pInfo->s_pObject->GetInfo()->s_Bound.Reset();			
	if( nsCSGBFUNC::CalMinMaxBound( pInfo->s_pObject->GetNiNode(), pInfo->s_pObject->GetNiNode()->GetWorldTransform(), &pInfo->s_pObject->GetInfo()->s_Bound ) == false )
	{
		pInfo->s_pObject->GetInfo()->s_Bound.s_ptMin = NiPoint2( pInfo->s_pObject->GetNiNode()->GetWorldTransform().m_Translate.x, pInfo->s_pObject->GetNiNode()->GetWorldTransform().m_Translate.y );
		pInfo->s_pObject->GetInfo()->s_Bound.s_ptMax = pInfo->s_pObject->GetInfo()->s_Bound.s_ptMin;
	}
	if( pInfo->s_pObject->GetInfo()->s_Bound.s_ptMin.x > pInfo->s_pObject->GetInfo()->s_Bound.s_ptMax.x )
	{
		pInfo->s_pObject->GetInfo()->s_Bound.s_ptMin.x = pInfo->s_pObject->GetInfo()->s_Bound.s_ptMax.x = pInfo->s_pObject->GetNiNode()->GetWorldTransform().m_Translate.x;
		pInfo->s_pObject->GetInfo()->s_Bound.s_ptMin.y = pInfo->s_pObject->GetInfo()->s_Bound.s_ptMax.y = pInfo->s_pObject->GetNiNode()->GetWorldTransform().m_Translate.y;
	}

	nsCsGBTerrain::g_pCurRoot->ResistObject_ChangeRotation( pInfo->s_pObject->GetInfo() );
}

void cRollBack::_ReDoObjDelete( sRT_OBJ_DELETE_INFO* pInfo )
{
	// 셀렉트 오브젝트 인가
	if( g_ObjectMng.DeleteSelectModel( pInfo->s_pObject->GetInfo()->s_dwUniqID ) == true )
		return;

	nsCsGBTerrain::g_pCurRoot->DeleteCsGBObject( pInfo->s_pObject->GetInfo()->s_dwUniqID );
}



//==========================================================================================
//
//		Height
//
//==========================================================================================

cRollBack::sRT_HEIGHT_INFO* cRollBack::_GetNewHeightInfo( sRT_HEIGHT_INFO* pInfo, bool bSrcTerrain )
{
	// Height 정보를 알아내자
	int nHeightCount =	nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nXVertCountInNode*
						nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nYVertCountInNode;

	// 메모리 풀이 비여 있다면 새로 생성 하자
	sRT_HEIGHT_INFO* pNewInfo;
	if( m_poolHeight.empty() == true )
	{
		pNewInfo = csnew sRT_HEIGHT_INFO;
		pNewInfo->s_ppfHeight = csnew float*[ pInfo->s_npfHeightCount ];
		for( int i=0; i<pInfo->s_npfHeightCount; ++i )
		{
			pNewInfo->s_ppfHeight[ i ] = csnew float[ nHeightCount ];
		}
	}
	else
	{
		pNewInfo = m_poolHeight.top();
		m_poolHeight.pop();
	}
	assert_cs( pNewInfo != NULL );

	// 정보 복사
	pNewInfo->s_npfHeightCount	= pInfo->s_npfHeightCount;
	//if( bSrcTerrain == true )
	{
		CsGBTerrainLeaf* pLeaf = NULL;
		for( int i=0; i<pInfo->s_npfHeightCount; ++i )
		{
			pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( i );
			memcpy( pNewInfo->s_ppfHeight[ i ], pLeaf->GetNiTriInfo()->s_pfHeight, sizeof( float )*nHeightCount );
		}
	}
	/*else
	{
		for( int i=0; i<pInfo->s_npfHeightCount; ++i )
		{
			memcpy( pNewInfo->s_ppfHeight[ i ], pInfo->s_ppfHeight[ i ], sizeof( float )*nHeightCount );
		}
	}*/
	
	return pNewInfo;
}

void cRollBack::_PushHeight( sRT_HEIGHT_INFO* pInfo )
{
	assert_cs( m_pTempData != NULL );
	m_pTempData->s_List.push_back( _GetNewHeightInfo( pInfo, true ) );
}

void cRollBack::_UnDoHeight( sRT_HEIGHT_INFO* pInfo )
{
	// Height 정보를 알아내자
	int nHeightCount =	nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nXVertCountInNode*
						nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nYVertCountInNode;
	
	CsGBTerrainLeaf* pLeaf = NULL;
	for( int i=0; i<pInfo->s_npfHeightCount; ++i )
	{
		pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( i );
		memcpy( pLeaf->GetNiTriInfo()->s_pfHeight, pInfo->s_ppfHeight[ i ], sizeof( float )*nHeightCount );
		NiGeometryData* pGeomData = pLeaf->GetNiTriStrip()->GetModelData();
		NiPoint3* pPos = pGeomData->GetVertices();
		for( int nVert=0; nVert<nHeightCount; ++nVert )
		{
			pPos[ nVert ].z = pInfo->s_ppfHeight[ i ][ nVert ];
		}
		pGeomData->MarkAsChanged( NiGeometryData::VERTEX_MASK );
		pGeomData->GetBound().ComputeFromData( nHeightCount, pPos );
	}

	nsCsGBTerrain::g_pCurRoot->MakeHeightField();
	//nsCsGBTerrain::g_pCurRoot->CalculateNormal();
}

void cRollBack::_ReDoHeight( sRT_HEIGHT_INFO* pInfo )
{
	// Height 정보를 알아내자
	int nHeightCount =	nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nXVertCountInNode*
						nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nYVertCountInNode;

	CsGBTerrainLeaf* pLeaf = NULL;
	for( int i=0; i<pInfo->s_npfHeightCount; ++i )
	{
		pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( i );
		memcpy( pLeaf->GetNiTriInfo()->s_pfHeight, pInfo->s_ppfHeight[ i ], sizeof( float )*nHeightCount );
		NiGeometryData* pGeomData = pLeaf->GetNiTriStrip()->GetModelData();
		NiPoint3* pPos = pGeomData->GetVertices();
		for( int nVert=0; nVert<nHeightCount; ++nVert )
		{
			pPos[ nVert ].z = pInfo->s_ppfHeight[ i ][ nVert ];
		}
		pGeomData->MarkAsChanged( NiGeometryData::VERTEX_MASK );
		pGeomData->GetBound().ComputeFromData( nHeightCount, pPos );
	}

	nsCsGBTerrain::g_pCurRoot->MakeHeightField();
	//nsCsGBTerrain::g_pCurRoot->CalculateNormal();
}



//==========================================================================================
//
//		Cell Resist
//
//==========================================================================================

cRollBack::sRT_CELL_RESIST_INFO* cRollBack::_GetNewCellResistInfo( sRT_CELL_RESIST_INFO* pInfo )
{
	sRT_CELL_RESIST_INFO* pNewInfo = m_poolCellResist.Pop();
	memcpy( &pNewInfo->s_Info, &pInfo->s_Info, sizeof( CsGBCell::sINFO ) );
	pNewInfo->s_nBaseIndex = pInfo->s_nBaseIndex;
	return pNewInfo;
}

void cRollBack::_PushCellResist( sRT_CELL_RESIST_INFO* pInfo )
{
	assert_cs( m_pTempData != NULL );
	m_pTempData->s_List.push_back( _GetNewCellResistInfo( pInfo ) );
}

void cRollBack::_UnDoCellResist( sRT_CELL_RESIST_INFO* pInfo )
{
	GET_MV_MAINTAB_DLG->_GetDlgCellObject()->__ReleaseSelectObject();
	nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->DeleteCellObject( pInfo->s_nBaseIndex, &pInfo->s_Info );
	GET_MV_MAINTAB_DLG->_GetDlgCellObject()->__ResetListInfo();
}

void cRollBack::_ReDoCellResist( sRT_CELL_RESIST_INFO* pInfo )
{
	GET_MV_MAINTAB_DLG->_GetDlgCellObject()->__ReleaseSelectObject();
	nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->ResistObject( pInfo->s_nBaseIndex, &pInfo->s_Info.s_trParent, &pInfo->s_Info.s_trObject );
	GET_MV_MAINTAB_DLG->_GetDlgCellObject()->__ResetListInfo();
}


//==========================================================================================
//
//		Cell Delete
//
//==========================================================================================

cRollBack::sRT_CELL_DELETE_INFO* cRollBack::_GetNewCellDeleteInfo( sRT_CELL_DELETE_INFO* pInfo )
{
	sRT_CELL_DELETE_INFO* pNewInfo = m_poolCellDelete.Pop();
	memcpy( &pNewInfo->s_Info, &pInfo->s_Info, sizeof( CsGBCell::sINFO ) );
	pNewInfo->s_nBaseIndex = pInfo->s_nBaseIndex;
	return pNewInfo;
}

void cRollBack::_PushCellDelete( sRT_CELL_DELETE_INFO* pInfo )
{
	assert_cs( m_pTempData != NULL );
	m_pTempData->s_List.push_back( _GetNewCellDeleteInfo( pInfo ) );
}

void cRollBack::_UnDoCellDelete( sRT_CELL_DELETE_INFO* pInfo )
{
	GET_MV_MAINTAB_DLG->_GetDlgCellObject()->__ReleaseSelectObject();
	nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->ResistObject( pInfo->s_nBaseIndex, &pInfo->s_Info.s_trParent, &pInfo->s_Info.s_trObject );
	GET_MV_MAINTAB_DLG->_GetDlgCellObject()->__ResetListInfo();
}

void cRollBack::_ReDoCellDelete( sRT_CELL_DELETE_INFO* pInfo )
{
	GET_MV_MAINTAB_DLG->_GetDlgCellObject()->__ReleaseSelectObject();
	nsCsGBTerrain::g_pCurRoot->GetSpeedCell()->DeleteCellObject( pInfo->s_nBaseIndex, &pInfo->s_Info );	
	GET_MV_MAINTAB_DLG->_GetDlgCellObject()->__ResetListInfo();
}
