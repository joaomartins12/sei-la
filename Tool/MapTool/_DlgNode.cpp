// _DlgNode.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgNode.h"


// _DlgNode 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgNode, CDialog)

_DlgNode::_DlgNode(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgNode::IDD, pParent)
{

}

_DlgNode::~_DlgNode()
{
}

void _DlgNode::_ResistNode()
{
	CsGBTerrainNode* pRootNode = nsCsGBTerrain::g_pCurRoot->GetRootNode();
	if( pRootNode->IsExistTerrain() == false )
		return;

	m_tree_Node.DeleteAllItems();	

	
	HTREEITEM hRootItem = m_tree_Node.InsertItem( L"QI_ROOT", 0, 0 );
	m_tree_Node.SetItemData( hRootItem, (DWORD_PTR)pRootNode );

	assert_cs( pRootNode->IsLeaf() == false );
	for( int i=0; i<CsGBTerrainNode::QI_COUNT; ++i )
	{
		__ResistNode( hRootItem, pRootNode->GetChildNode( i ) );
	}

	m_tree_Node.SelectItem( hRootItem );
}

void _DlgNode::__ResistNode( HTREEITEM hParent, CsGBTerrainNode* pChild )
{
	switch( pChild->GetQuadID() )
	{
	case CsGBTerrainNode::QI_LD:	hParent = m_tree_Node.InsertItem( L"QI_LD", hParent );		break;
	case CsGBTerrainNode::QI_RD:	hParent = m_tree_Node.InsertItem( L"QI_RD", hParent );		break;
	case CsGBTerrainNode::QI_LT:	hParent = m_tree_Node.InsertItem( L"QI_LT", hParent );		break;
	case CsGBTerrainNode::QI_RT:	hParent = m_tree_Node.InsertItem( L"QI_RT", hParent );		break;
	default:						assert_cs( false );
	}

	m_tree_Node.SetItemData( hParent, (DWORD_PTR)pChild );

	if( pChild->IsLeaf() == false )
	{
		for( int i=0; i<CsGBTerrainNode::QI_COUNT; ++i )
		{
			__ResistNode( hParent, pChild->GetChildNode( i ) );
		}
	}
}

CsGBTerrainLeaf* _DlgNode::__GetFirstLeaf( HTREEITEM hSelItem )
{
	CsGBTerrainNode* pSelNode = (CsGBTerrainNode*)m_tree_Node.GetItemData( hSelItem );
	CsGBTerrainNode* pChild = pSelNode;
	while( pChild->IsLeaf() == false )
	{
		pChild = pSelNode->GetChildNode( 0 );
		pSelNode = pChild;
	}
	return (CsGBTerrainLeaf*)pChild;
}

//============================================================================================================
//
// Select Node
//
//============================================================================================================
void _DlgNode::_SelectChildNode( CPoint pt )
{
	// 이미 최하위 노드라면 리턴
	assert_cs( m_tree_Node.GetSelectedItem() != NULL );
	HTREEITEM hItem = m_tree_Node.GetSelectedItem();
	CsGBTerrainNode* pTreeNode = (CsGBTerrainNode*)m_tree_Node.GetItemData( hItem );
	if( pTreeNode->IsLeaf() == true )
		return;

	// 선택 된 노드를 알아 본다
	CsGBPick pick;
	NiPoint3 origin, dir;
	CAMERA_ST.WindowPointToRay( pt.x, pt.y, origin, dir );
	if( pick.Pick( nsCsGBTerrain::g_pCurRoot->GetRootNode()->GetNiNode(), origin, dir, false ) == false )
		return;

	// 이름 검사해서 오브젝트 판별
	NiFixedString fixStr = pick.GetRecord()->GetAVObject()->GetName();
	int nLeafIndex = atoi( fixStr );
	CsGBTerrainNode* pNode = nsCsGBTerrain::g_pCurRoot->GetNode( nLeafIndex );
	assert_cs( pTreeNode != pNode );

	// 자식중의 하나인가 검색
	HTREEITEM hChild = m_tree_Node.GetChildItem( hItem );
	HTREEITEM hSrc = hChild;
	while( 1 )
	{		
		if( pNode == (CsGBTerrainNode*)m_tree_Node.GetItemData( hChild ) )
		{
			m_tree_Node.SelectItem( hChild );
			break;
		}
		hChild = m_tree_Node.GetNextSiblingItem( hChild );
		if( hChild == NULL )
		{
			pNode = pNode->GetParent();
			hChild = hSrc;
		}
	}	
}

void _DlgNode::_SelectParentNode()
{
	HTREEITEM hTreeItem = m_tree_Node.GetSelectedItem();
	HTREEITEM hParentItem = m_tree_Node.GetParentItem( hTreeItem );
	if( hParentItem != NULL )
		m_tree_Node.SelectItem( hParentItem );
}

//============================================================================================================
//
// ==== Detail Map0
//
//============================================================================================================

void _DlgNode::_SetDetail_Map0( LPCTSTR szTexPath )
{
	char cTexPath[ MAX_PATH ] = {0, };
	W2M( cTexPath, szTexPath, MAX_PATH );
	__SetDetail_Map0( (CsGBTerrainNode*)m_tree_Node.GetItemData( m_tree_Node.GetSelectedItem() ), cTexPath );
}

void _DlgNode::__SetDetail_Map0( CsGBTerrainNode* pChild, char* cpTexPath )
{
	if( pChild->IsLeaf() == false )
	{
		for( int i=0; i<CsGBTerrainNode::QI_COUNT; ++i )
		{
			__SetDetail_Map0( pChild->GetChildNode( i ), cpTexPath );				
		}
	}
	else
	{
		( (CsGBTerrainLeaf*)pChild )->SetDetail_Map0_ByTool( cpTexPath );		
	}
}

char* _DlgNode::__GetSameFile_Detail_Map0( HTREEITEM hSelItem )
{
	CsGBTerrainLeaf* pSelLeaf = __GetFirstLeaf( hSelItem );
	char* cpSrcName = pSelLeaf->GetNiTriInfo()->s_cDetail_Map0;

	if( __IsSameFile_Detail_Map0( (CsGBTerrainNode*)m_tree_Node.GetItemData( hSelItem ), cpSrcName ) == false )
		return NULL;
	
	return cpSrcName;
}

bool _DlgNode::__IsSameFile_Detail_Map0( CsGBTerrainNode* pChild, char* pSrcName )
{
	if( pChild->IsLeaf() == false )
	{
		for( int i=0; i<CsGBTerrainNode::QI_COUNT; ++i )
		{
			if( __IsSameFile_Detail_Map0( pChild->GetChildNode( i ), pSrcName ) == false )
				return false;
		}
	}
	else
	{
		if( strcmp( pSrcName, ( (CsGBTerrainLeaf*)pChild )->GetNiTriInfo()->s_cDetail_Map0 ) != 0 )
			return false;
	}

	return true;
}

//============================================================================================================
//
// ==== Detail Map1
//
//============================================================================================================

void _DlgNode::_SetDetail_Map1( LPCTSTR szTexPath )
{
	char cTexPath[ MAX_PATH ] = {0, };
	W2M( cTexPath, szTexPath, MAX_PATH );
	__SetDetail_Map1( (CsGBTerrainNode*)m_tree_Node.GetItemData( m_tree_Node.GetSelectedItem() ), cTexPath );
}

void _DlgNode::__SetDetail_Map1( CsGBTerrainNode* pChild, char* cpTexPath )
{
	if( pChild->IsLeaf() == false )
	{
		for( int i=0; i<CsGBTerrainNode::QI_COUNT; ++i )
		{
			__SetDetail_Map1( pChild->GetChildNode( i ), cpTexPath );				
		}
	}
	else
	{
		( (CsGBTerrainLeaf*)pChild )->SetDetail_Map1_ByTool( cpTexPath );		
	}
}

char* _DlgNode::__GetSameFile_Detail_Map1( HTREEITEM hSelItem )
{
	CsGBTerrainLeaf* pSelLeaf = __GetFirstLeaf( hSelItem );
	char* cpSrcName = pSelLeaf->GetNiTriInfo()->s_cDetail_Map1;

	if( __IsSameFile_Detail_Map1( (CsGBTerrainNode*)m_tree_Node.GetItemData( hSelItem ), cpSrcName ) == false )
		return NULL;

	return cpSrcName;
}

bool _DlgNode::__IsSameFile_Detail_Map1( CsGBTerrainNode* pChild, char* pSrcName )
{
	if( pChild->IsLeaf() == false )
	{
		for( int i=0; i<CsGBTerrainNode::QI_COUNT; ++i )
		{
			if( __IsSameFile_Detail_Map1( pChild->GetChildNode( i ), pSrcName ) == false )
				return false;
		}
	}
	else
	{
		if( strcmp( pSrcName, ( (CsGBTerrainLeaf*)pChild )->GetNiTriInfo()->s_cDetail_Map1 ) != 0 )
			return false;
	}

	return true;
}

//============================================================================================================
//
// ==== Detail Map2
//
//============================================================================================================

void _DlgNode::_SetDetail_Map2( LPCTSTR szTexPath )
{
	char cTexPath[ MAX_PATH ] = {0, };
	W2M( cTexPath, szTexPath, MAX_PATH );
	__SetDetail_Map2( (CsGBTerrainNode*)m_tree_Node.GetItemData( m_tree_Node.GetSelectedItem() ), cTexPath );
}

void _DlgNode::__SetDetail_Map2( CsGBTerrainNode* pChild, char* cpTexPath )
{
	if( pChild->IsLeaf() == false )
	{
		for( int i=0; i<CsGBTerrainNode::QI_COUNT; ++i )
		{
			__SetDetail_Map2( pChild->GetChildNode( i ), cpTexPath );				
		}
	}
	else
	{
		( (CsGBTerrainLeaf*)pChild )->SetDetail_Map2_ByTool( cpTexPath );		
	}
}

char* _DlgNode::__GetSameFile_Detail_Map2( HTREEITEM hSelItem )
{
	CsGBTerrainLeaf* pSelLeaf = __GetFirstLeaf( hSelItem );
	char* cpSrcName = pSelLeaf->GetNiTriInfo()->s_cDetail_Map2;

	if( __IsSameFile_Detail_Map2( (CsGBTerrainNode*)m_tree_Node.GetItemData( hSelItem ), cpSrcName ) == false )
		return NULL;

	return cpSrcName;
}

bool _DlgNode::__IsSameFile_Detail_Map2( CsGBTerrainNode* pChild, char* pSrcName )
{
	if( pChild->IsLeaf() == false )
	{
		for( int i=0; i<CsGBTerrainNode::QI_COUNT; ++i )
		{
			if( __IsSameFile_Detail_Map2( pChild->GetChildNode( i ), pSrcName ) == false )
				return false;
		}
	}
	else
	{
		if( strcmp( pSrcName, ( (CsGBTerrainLeaf*)pChild )->GetNiTriInfo()->s_cDetail_Map2 ) != 0 )
			return false;
	}

	return true;
}

//============================================================================================================
//
// ==== Detail Map3
//
//============================================================================================================

void _DlgNode::_SetDetail_Map3( LPCTSTR szTexPath )
{
	char cTexPath[ MAX_PATH ] = {0, };
	W2M( cTexPath, szTexPath, MAX_PATH );
	__SetDetail_Map3( (CsGBTerrainNode*)m_tree_Node.GetItemData( m_tree_Node.GetSelectedItem() ), cTexPath );
}

void _DlgNode::__SetDetail_Map3( CsGBTerrainNode* pChild, char* cpTexPath )
{
	if( pChild->IsLeaf() == false )
	{
		for( int i=0; i<CsGBTerrainNode::QI_COUNT; ++i )
		{
			__SetDetail_Map3( pChild->GetChildNode( i ), cpTexPath );				
		}
	}
	else
	{
		( (CsGBTerrainLeaf*)pChild )->SetDetail_Map3_ByTool( cpTexPath );		
	}
}

char* _DlgNode::__GetSameFile_Detail_Map3( HTREEITEM hSelItem )
{
	CsGBTerrainLeaf* pSelLeaf = __GetFirstLeaf( hSelItem );
	char* cpSrcName = pSelLeaf->GetNiTriInfo()->s_cDetail_Map3;

	if( __IsSameFile_Detail_Map3( (CsGBTerrainNode*)m_tree_Node.GetItemData( hSelItem ), cpSrcName ) == false )
		return NULL;

	return cpSrcName;
}

bool _DlgNode::__IsSameFile_Detail_Map3( CsGBTerrainNode* pChild, char* pSrcName )
{
	if( pChild->IsLeaf() == false )
	{
		for( int i=0; i<CsGBTerrainNode::QI_COUNT; ++i )
		{
			if( __IsSameFile_Detail_Map3( pChild->GetChildNode( i ), pSrcName ) == false )
				return false;
		}
	}
	else
	{
		if( strcmp( pSrcName, ( (CsGBTerrainLeaf*)pChild )->GetNiTriInfo()->s_cDetail_Map3 ) != 0 )
			return false;
	}

	return true;
}


//============================================================================================================
//
// ==== Detail Map4
//
//============================================================================================================

void _DlgNode::_SetDetail_Map4( LPCTSTR szTexPath )
{
	char cTexPath[ MAX_PATH ] = {0, };
	W2M( cTexPath, szTexPath, MAX_PATH );
	__SetDetail_Map4( (CsGBTerrainNode*)m_tree_Node.GetItemData( m_tree_Node.GetSelectedItem() ), cTexPath );
}

void _DlgNode::__SetDetail_Map4( CsGBTerrainNode* pChild, char* cpTexPath )
{
	if( pChild->IsLeaf() == false )
	{
		for( int i=0; i<CsGBTerrainNode::QI_COUNT; ++i )
		{
			__SetDetail_Map4( pChild->GetChildNode( i ), cpTexPath );				
		}
	}
	else
	{
		( (CsGBTerrainLeaf*)pChild )->SetDetail_Map4_ByTool( cpTexPath );		
	}
}

char* _DlgNode::__GetSameFile_Detail_Map4( HTREEITEM hSelItem )
{
	CsGBTerrainLeaf* pSelLeaf = __GetFirstLeaf( hSelItem );
	char* cpSrcName = pSelLeaf->GetNiTriInfo()->s_cDetail_Map4;

	if( __IsSameFile_Detail_Map4( (CsGBTerrainNode*)m_tree_Node.GetItemData( hSelItem ), cpSrcName ) == false )
		return NULL;

	return cpSrcName;
}

bool _DlgNode::__IsSameFile_Detail_Map4( CsGBTerrainNode* pChild, char* pSrcName )
{
	if( pChild->IsLeaf() == false )
	{
		for( int i=0; i<CsGBTerrainNode::QI_COUNT; ++i )
		{
			if( __IsSameFile_Detail_Map4( pChild->GetChildNode( i ), pSrcName ) == false )
				return false;
		}
	}
	else
	{
		if( strcmp( pSrcName, ( (CsGBTerrainLeaf*)pChild )->GetNiTriInfo()->s_cDetail_Map4 ) != 0 )
			return false;
	}

	return true;
}

void _DlgNode::_AlphaSwap12()
{
	__AlphaSwap12( (CsGBTerrainNode*)m_tree_Node.GetItemData( m_tree_Node.GetSelectedItem() ) );
}

void _DlgNode::__AlphaSwap12( CsGBTerrainNode* pChild )
{
	if( pChild->IsLeaf() == false )
	{
		for( int i=0; i<CsGBTerrainNode::QI_COUNT; ++i )
		{
			__AlphaSwap12( pChild->GetChildNode( i ) );			
		}
	}
	else
	{
		NiNode* pNode = pChild->GetNiNode();
		// 쉐이더 맵이 존재한다면
		NiPixelData* pPData = GET_SHADERMAP_PIXELDATA( pNode, ALPHA_MAP_SHADER_NTM );
		int nBtCount = pPData->GetWidth()*pPData->GetHeight();

		BYTE btTemp;
		BYTE* pByte = (BYTE*)pPData->GetPixels();
		for( int i=0; i<nBtCount; ++i )
		{
			btTemp = pByte[ i*ALPHA_MAP_STRIDE ];
			pByte[ i*ALPHA_MAP_STRIDE ] = pByte[ i*ALPHA_MAP_STRIDE + 1 ];			
			pByte[ i*ALPHA_MAP_STRIDE + 1 ] = btTemp;
		}
		pPData->MarkAsChanged();
	}
}

void _DlgNode::_AlphaSwap23()
{
	__AlphaSwap23( (CsGBTerrainNode*)m_tree_Node.GetItemData( m_tree_Node.GetSelectedItem() ) );
}

void _DlgNode::__AlphaSwap23( CsGBTerrainNode* pChild )
{
	if( pChild->IsLeaf() == false )
	{
		for( int i=0; i<CsGBTerrainNode::QI_COUNT; ++i )
		{
			__AlphaSwap23( pChild->GetChildNode( i ) );			
		}
	}
	else
	{
		NiNode* pNode = pChild->GetNiNode();
		// 쉐이더 맵이 존재한다면
		NiPixelData* pPData = GET_SHADERMAP_PIXELDATA( pNode, ALPHA_MAP_SHADER_NTM );
		int nBtCount = pPData->GetWidth()*pPData->GetHeight();

		BYTE btTemp;
		BYTE* pByte = (BYTE*)pPData->GetPixels();
		for( int i=0; i<nBtCount; ++i )
		{
			btTemp = pByte[ i*ALPHA_MAP_STRIDE + 1 ];
			pByte[ i*ALPHA_MAP_STRIDE + 1 ] = pByte[ i*ALPHA_MAP_STRIDE + 2 ];			
			pByte[ i*ALPHA_MAP_STRIDE + 2 ] = btTemp;
		}
		pPData->MarkAsChanged();
	}
}

void _DlgNode::_AlphaSwap13()
{
	__AlphaSwap13( (CsGBTerrainNode*)m_tree_Node.GetItemData( m_tree_Node.GetSelectedItem() ) );
}

void _DlgNode::__AlphaSwap13( CsGBTerrainNode* pChild )
{
	if( pChild->IsLeaf() == false )
	{
		for( int i=0; i<CsGBTerrainNode::QI_COUNT; ++i )
		{
			__AlphaSwap13( pChild->GetChildNode( i ) );			
		}
	}
	else
	{
		NiNode* pNode = pChild->GetNiNode();
		// 쉐이더 맵이 존재한다면
		NiPixelData* pPData = GET_SHADERMAP_PIXELDATA( pNode, ALPHA_MAP_SHADER_NTM );
		int nBtCount = pPData->GetWidth()*pPData->GetHeight();

		BYTE btTemp;
		BYTE* pByte = (BYTE*)pPData->GetPixels();
		for( int i=0; i<nBtCount; ++i )
		{
			btTemp = pByte[ i*ALPHA_MAP_STRIDE ];
			pByte[ i*ALPHA_MAP_STRIDE ] = pByte[ i*ALPHA_MAP_STRIDE + 2 ];			
			pByte[ i*ALPHA_MAP_STRIDE + 2 ] = btTemp;
		}
		pPData->MarkAsChanged();
	}
}

void _DlgNode::__SetSiblingItemVisibleOff( HTREEITEM hItem )
{
	if( hItem == NULL )
		return;

	// 같은 레벨의 당사자 제외 전부 false
	HTREEITEM hSiblingItem = m_tree_Node.GetPrevSiblingItem( hItem );
	while( hSiblingItem != NULL )
	{
		( ( CsGBTerrainNode* )m_tree_Node.GetItemData( hSiblingItem ) )->SetVisible( false );
		hSiblingItem = m_tree_Node.GetPrevSiblingItem( hSiblingItem );
	}
	hSiblingItem = m_tree_Node.GetNextSiblingItem( hItem );
	while( hSiblingItem != NULL )
	{
		( ( CsGBTerrainNode* )m_tree_Node.GetItemData( hSiblingItem ) )->SetVisible( false );
		hSiblingItem = m_tree_Node.GetNextSiblingItem( hSiblingItem );
	}

	__SetSiblingItemVisibleOff( m_tree_Node.GetParentItem( hItem ) );
}

void _DlgNode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_NODE, m_tree_Node);
}


BEGIN_MESSAGE_MAP(_DlgNode, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_NODE, &_DlgNode::OnTvnSelchangedTreeNode)
END_MESSAGE_MAP()


// _DlgNode 메시지 처리기입니다.

void _DlgNode::OnTvnSelchangedTreeNode(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	// 전부 다 붙이자
	for( int i=0; i<nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount; ++i )
	{
		nsCsGBTerrain::g_pCurRoot->GetNode( i )->SetVisible( true );
	}
	__SetSiblingItemVisibleOff( pNMTreeView->itemNew.hItem );


	// Map 비교 -> 아래로 같은거 쓰고 있나
	{
		char* pcBaseName = __GetSameFile_Detail_Map0( pNMTreeView->itemNew.hItem );
		if( pcBaseName == NULL )
		{
			GET_MV_MAINTAB_DLG->_GetDlgMapping()->_SetDetail_Map0Text( L"..." );
		}
		else
		{
			TCHAR szMap[ MAX_PATH ] = {0, };
			M2W( szMap, pcBaseName, MAX_PATH );		
			GET_MV_MAINTAB_DLG->_GetDlgMapping()->_SetDetail_Map0Text( nsCSFILE::GetFileName( szMap ) );
		}
	}

	{
		char* pcDetailMap1Name = __GetSameFile_Detail_Map1( pNMTreeView->itemNew.hItem );
		if( pcDetailMap1Name == NULL )
		{
			GET_MV_MAINTAB_DLG->_GetDlgMapping()->_SetDetail_Map1Text( L"..." );
		}
		else
		{
			TCHAR szMap[ MAX_PATH ] = {0, };
			M2W( szMap, pcDetailMap1Name, MAX_PATH );		
			GET_MV_MAINTAB_DLG->_GetDlgMapping()->_SetDetail_Map1Text( nsCSFILE::GetFileName( szMap ) );
		}
	}

	{
		char* pcDetailMap2Name = __GetSameFile_Detail_Map2( pNMTreeView->itemNew.hItem );
		if( pcDetailMap2Name == NULL )
		{
			GET_MV_MAINTAB_DLG->_GetDlgMapping()->_SetDetail_Map2Text( L"..." );
		}
		else
		{
			TCHAR szMap[ MAX_PATH ] = {0, };
			M2W( szMap, pcDetailMap2Name, MAX_PATH );		
			GET_MV_MAINTAB_DLG->_GetDlgMapping()->_SetDetail_Map2Text( nsCSFILE::GetFileName( szMap ) );
		}
	}

	{
		char* pcDetailMap3Name = __GetSameFile_Detail_Map3( pNMTreeView->itemNew.hItem );
		if( pcDetailMap3Name == NULL )
		{
			GET_MV_MAINTAB_DLG->_GetDlgMapping()->_SetDetail_Map3Text( L"..." );
		}
		else
		{
			TCHAR szMap[ MAX_PATH ] = {0, };
			M2W( szMap, pcDetailMap3Name, MAX_PATH );		
			GET_MV_MAINTAB_DLG->_GetDlgMapping()->_SetDetail_Map3Text( nsCSFILE::GetFileName( szMap ) );
		}
	}

	{
		char* pcDetailMap4Name = __GetSameFile_Detail_Map4( pNMTreeView->itemNew.hItem );
		if( pcDetailMap4Name == NULL )
		{
			GET_MV_MAINTAB_DLG->_GetDlgMapping()->_SetDetail_Map4Text( L"..." );
		}
		else
		{
			TCHAR szMap[ MAX_PATH ] = {0, };
			M2W( szMap, pcDetailMap4Name, MAX_PATH );		
			GET_MV_MAINTAB_DLG->_GetDlgMapping()->_SetDetail_Map4Text( nsCSFILE::GetFileName( szMap ) );
		}
	}

	GET_MV_MAINTAB_DLG->_GetDlgMapping()->_ResetPreviewTexture();

	*pResult = 0;
}
