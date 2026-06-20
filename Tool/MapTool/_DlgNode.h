#pragma once
#include "afxcmn.h"


// _DlgNode 대화 상자입니다.

class _DlgNode : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgNode)

public:
	_DlgNode(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgNode();

public:
	void		_ResistNode();
protected:
	void		__ResistNode( HTREEITEM hParent, CsGBTerrainNode* pChild );
	CsGBTerrainLeaf* __GetFirstLeaf( HTREEITEM hSelItem );

	// ==== Select
public:
	void		_SelectChildNode( CPoint pt );
	void		_SelectParentNode();

	// ==== Detail Map0
public:
	void		_SetDetail_Map0( LPCTSTR szTexPath );
protected:
	void		__SetDetail_Map0( CsGBTerrainNode* pChild, char* cpTexPath );
	char*		__GetSameFile_Detail_Map0( HTREEITEM hSelItem );
	bool		__IsSameFile_Detail_Map0( CsGBTerrainNode* pChild, char* pSrcName );

	// ==== Detail Map1
public:
	void		_SetDetail_Map1( LPCTSTR szTexPath );
protected:
	void		__SetDetail_Map1( CsGBTerrainNode* pChild, char* cpTexPath );
	char*		__GetSameFile_Detail_Map1( HTREEITEM hSelItem );
	bool		__IsSameFile_Detail_Map1( CsGBTerrainNode* pChild, char* pSrcName );

	// ==== Detail Map2
public:
	void		_SetDetail_Map2( LPCTSTR szTexPath );
protected:
	void		__SetDetail_Map2( CsGBTerrainNode* pChild, char* cpTexPath );
	char*		__GetSameFile_Detail_Map2( HTREEITEM hSelItem );
	bool		__IsSameFile_Detail_Map2( CsGBTerrainNode* pChild, char* pSrcName );

	// ==== Detail Map3
public:
	void		_SetDetail_Map3( LPCTSTR szTexPath );
protected:
	void		__SetDetail_Map3( CsGBTerrainNode* pChild, char* cpTexPath );
	char*		__GetSameFile_Detail_Map3( HTREEITEM hSelItem );
	bool		__IsSameFile_Detail_Map3( CsGBTerrainNode* pChild, char* pSrcName );

	// ==== Detail Map4
public:
	void		_SetDetail_Map4( LPCTSTR szTexPath );
protected:
	void		__SetDetail_Map4( CsGBTerrainNode* pChild, char* cpTexPath );
	char*		__GetSameFile_Detail_Map4( HTREEITEM hSelItem );
	bool		__IsSameFile_Detail_Map4( CsGBTerrainNode* pChild, char* pSrcName );

	// ==== Swap 12
public:
	void		_AlphaSwap12();
protected:
	void		__AlphaSwap12( CsGBTerrainNode* pChild );
	// ==== Swap 23
public:
	void		_AlphaSwap23();
protected:
	void		__AlphaSwap23( CsGBTerrainNode* pChild );
	// ==== Swap 13
public:
	void		_AlphaSwap13();
protected:
	void		__AlphaSwap13( CsGBTerrainNode* pChild );

	// ====	같은 레벨의 당사자 제외 트리 전부 비지블 오프
protected:
	void		__SetSiblingItemVisibleOff( HTREEITEM hItem );

// 대화 상자 데이터입니다.
public:
	enum { IDD = IDD_DLG_NODE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
protected:
	CTreeCtrl m_tree_Node;
public:
	afx_msg void OnTvnSelchangedTreeNode(NMHDR *pNMHDR, LRESULT *pResult);
};
