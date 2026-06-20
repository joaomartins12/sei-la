// _Dlg_ModelInfo.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "NpcTool.h"
#include "_Dlg_ModelInfo.h"


// _Dlg_ModelInfo 대화 상자입니다.

IMPLEMENT_DYNAMIC(_Dlg_ModelInfo, CDialog)

_Dlg_ModelInfo::_Dlg_ModelInfo(CWnd* pParent /*=NULL*/)
	: CDialog(_Dlg_ModelInfo::IDD, pParent)
{

}

_Dlg_ModelInfo::~_Dlg_ModelInfo()
{
}

void _Dlg_ModelInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MODEL, m_list_Model);
	DDX_Control(pDX, IDC_EDIT_COMMENT, m_edit_Comment);
	DDX_Control(pDX, IDC_EDIT_OFFSET_X, m_edit_OffsetX);
	DDX_Control(pDX, IDC_EDIT_OFFSET_Y, m_edit_OffsetY);
	DDX_Control(pDX, IDC_EDIT_OFFSET_Z, m_edit_OffsetZ);
}


BEGIN_MESSAGE_MAP(_Dlg_ModelInfo, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &_Dlg_ModelInfo::OnBnClickedButtonSave)
	ON_NOTIFY(NM_CLICK, IDC_LIST_MODEL, &_Dlg_ModelInfo::OnNMClickListModel)
	ON_EN_CHANGE(IDC_EDIT_COMMENT, &_Dlg_ModelInfo::OnEnChangeEditComment)
	ON_EN_CHANGE(IDC_EDIT_OFFSET_X, &_Dlg_ModelInfo::OnEnChangeEditOffsetX)
	ON_EN_CHANGE(IDC_EDIT_OFFSET_Y, &_Dlg_ModelInfo::OnEnChangeEditOffsetY)
	ON_EN_CHANGE(IDC_EDIT_OFFSET_Z, &_Dlg_ModelInfo::OnEnChangeEditOffsetZ)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &_Dlg_ModelInfo::OnBnClickedButtonReset)
END_MESSAGE_MAP()


// _Dlg_ModelInfo 메시지 처리기입니다.

BOOL _Dlg_ModelInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	DWORD dwExStyle = m_list_Model.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Model.SetExtendedStyle( dwExStyle );
	m_list_Model.InsertColumn( 0, L"ID", LVCFMT_RIGHT, 60 );
	m_list_Model.InsertColumn( 1, L"kfm", LVCFMT_RIGHT, 220 );
	m_list_Model.InsertColumn( 2, L"설명", LVCFMT_LEFT, 520 );
	m_list_Model.InsertColumn( 3, L"offset_X", LVCFMT_RIGHT, 60 );
	m_list_Model.InsertColumn( 4, L"offset_Y", LVCFMT_RIGHT, 60 );
	m_list_Model.InsertColumn( 5, L"offset_Z", LVCFMT_RIGHT, 60 );

	__ResetModelInfo();
	__CreateList();	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void _Dlg_ModelInfo::__ResetModelInfo()
{
	// 기존 정보를 여유 공간에 담아 둔다
	std::map< DWORD, CsNpcMng::sMODEL_INFO* > mapTemp;
	std::map< DWORD, CsNpcMng::sMODEL_INFO* >* pMap = nsCsFileTable::g_pNpcMng->GetModelInfoMap();

	{		
		std::map< DWORD, CsNpcMng::sMODEL_INFO* >::iterator it = pMap->begin();
		std::map< DWORD, CsNpcMng::sMODEL_INFO* >::iterator itEnd = pMap->end();
		for( ; it!=itEnd; ++it )
		{
			mapTemp[ it->second->s_nModelID ] = it->second;
		}
		pMap->clear();
	}	

	// Model 정보를 읽어 와서 다시 셋팅
	std::map< DWORD, CsModelData* >* pDataMap = g_pModelDataMng->GetDataMap();
	std::map< DWORD, CsModelData* >::iterator it = pDataMap->begin();
	std::map< DWORD, CsModelData* >::iterator itEnd = pDataMap->end();
	CsModelData* pData = NULL;
	CsNpcMng::sMODEL_INFO* pNew = NULL;
	DWORD nModelID;
	for( ; it!=itEnd; ++it )
	{
		pData = it->second;

		nModelID = pData->GetInfo()->s_dwID;

		// 기존에 존재 한다면
		if( mapTemp.find( nModelID ) != mapTemp.end() )
		{
			pNew = mapTemp[ nModelID ];
			mapTemp.erase( nModelID );			
		}
		else
		{
			pNew = csnew CsNpcMng::sMODEL_INFO;
			pNew->s_nModelID = nModelID;
			pNew->s_nOffset[ 0 ] = 0;
			pNew->s_nOffset[ 1 ] = 0;
			pNew->s_nOffset[ 2 ] = 0;
			pNew->s_szComment[ 0 ] = NULL;			
		}
		(*pMap)[ nModelID ] = pNew;
	}

	{		
		std::map< DWORD, CsNpcMng::sMODEL_INFO* >::iterator itDel = mapTemp.begin();
		std::map< DWORD, CsNpcMng::sMODEL_INFO* >::iterator itEndDel = mapTemp.end();
		for( ; itDel!=itEndDel; ++itDel )
		{
			delete itDel->second;
		}
		mapTemp.clear();
	}
}

void _Dlg_ModelInfo::__CreateList()
{
	m_list_Model.DeleteAllItems();

	std::map< DWORD, CsNpcMng::sMODEL_INFO* >* pMap = nsCsFileTable::g_pNpcMng->GetModelInfoMap();
	std::map< DWORD, CsNpcMng::sMODEL_INFO* >::iterator it = pMap->begin();
	std::map< DWORD, CsNpcMng::sMODEL_INFO* >::iterator itEnd = pMap->end();

	int nIndex = 0;
	TCHAR szMsg[ 256 ];
	for( ; it!=itEnd; ++it )
	{
		CsNpcMng::sMODEL_INFO* pInfo = it->second;

		_stprintf_s( szMsg, 256, L"%d", pInfo->s_nModelID );
		m_list_Model.InsertItem( nIndex, szMsg );

		M2W( szMsg, nsCSFILE::GetFileName( g_pModelDataMng->GetData( pInfo->s_nModelID )->GetInfo()->s_cKfmPath ), 256 );
		m_list_Model.SetItem( nIndex, 1, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
		m_list_Model.SetItem( nIndex, 2, LVIF_TEXT, pInfo->s_szComment, 0, 0, 0, 0 );

		_stprintf_s( szMsg, 256, L"%d", pInfo->s_nOffset[ 0 ] );
		m_list_Model.SetItem( nIndex, 3, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
		_stprintf_s( szMsg, 256, L"%d", pInfo->s_nOffset[ 1 ] );
		m_list_Model.SetItem( nIndex, 4, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
		_stprintf_s( szMsg, 256, L"%d", pInfo->s_nOffset[ 2 ] );
		m_list_Model.SetItem( nIndex, 5, LVIF_TEXT, szMsg, 0, 0, 0, 0 );

		m_list_Model.SetItemData( nIndex, pInfo->s_nModelID );

		++nIndex;
	}

	m_pSelectInfo = NULL;
}

void _Dlg_ModelInfo::OnBnClickedButtonSave()
{
	if( _access_s( nsCsFileTable::GetPath_DM_Npc(), 0 ) == 0 )
	{
		char cNewFile[ MAX_PATH ];

		if( nsCsFileTable::g_eLanguage != nsCsFileTable::ENGLISH )
		{
			// 폴더가 존재 하지 않는다면 폴더 생성
			nsCSFILE::CreateDirectory( L".\\GameData\\Npc_Backup\\Korea" );

			SYSTEMTIME st;
			GetLocalTime( &st );
			sprintf_s( cNewFile, MAX_PATH, ".\\GameData\\Npc_Backup\\Korea\\Npc_%.2d%.2d%.2d_%.2d%.2d%.2d.xls", st.wYear%100, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
		}
		else
		{
			// 폴더가 존재 하지 않는다면 폴더 생성
			nsCSFILE::CreateDirectory( L".\\GameData\\Npc_Backup\\English" );

			SYSTEMTIME st;
			GetLocalTime( &st );
			sprintf_s( cNewFile, MAX_PATH, ".\\GameData\\Npc_Backup\\English\\Npc_%.2d%.2d%.2d_%.2d%.2d%.2d.xls", st.wYear%100, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
		}

		// 이미 파일이 존재 한다면
		if( _access_s( cNewFile, 0 ) == 0 )
			DeleteFileA( cNewFile );

		::CopyFileA( nsCsFileTable::GetPath_DM_Npc(), cNewFile, FALSE );
	}

	nsCsFileTable::g_pNpcMng->Save_ModelInfo();

	CsMessageBoxA( MB_OK, "%s 저장 성공!!", nsCsFileTable::GetPath_DM_Npc() );	
}

void _Dlg_ModelInfo::OnNMClickListModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_list_Model.GetFirstSelectedItemPosition();
	int nItem = m_list_Model.GetNextSelectedItem( pos );
	if( nItem < 0 )
	{
		m_pSelectInfo = NULL;
		return;
	}

	DWORD nModelID = (DWORD)m_list_Model.GetItemData( nItem );
	m_pSelectInfo = nsCsFileTable::g_pNpcMng->GetModelInfo( nModelID );
	m_edit_Comment.SetWindowText( m_pSelectInfo->s_szComment );
	nsCsMfcFunc::Int2Control( &m_edit_OffsetX, m_pSelectInfo->s_nOffset[ 0 ] );
	nsCsMfcFunc::Int2Control( &m_edit_OffsetY, m_pSelectInfo->s_nOffset[ 1 ] );
	nsCsMfcFunc::Int2Control( &m_edit_OffsetZ, m_pSelectInfo->s_nOffset[ 2 ] );

	*pResult = 0;
}

void _Dlg_ModelInfo::OnEnChangeEditComment()
{
	if( m_pSelectInfo == NULL )
		return;

	m_edit_Comment.GetWindowText( m_pSelectInfo->s_szComment, 64 );

	POSITION pos = m_list_Model.GetFirstSelectedItemPosition();
	int nItem = m_list_Model.GetNextSelectedItem( pos );
	m_list_Model.SetItemText( nItem, 2, m_pSelectInfo->s_szComment );
}

void _Dlg_ModelInfo::OnEnChangeEditOffsetX()
{
	if( m_pSelectInfo == NULL )
		return;

	m_pSelectInfo->s_nOffset[ 0 ] = nsCsMfcFunc::Control2Int( &m_edit_OffsetX );

	POSITION pos = m_list_Model.GetFirstSelectedItemPosition();
	int nItem = m_list_Model.GetNextSelectedItem( pos );

	TCHAR szMsg[ 256 ];
	_stprintf_s( szMsg, 256, L"%d", m_pSelectInfo->s_nOffset[ 0 ] );
	m_list_Model.SetItemText( nItem, 3, szMsg );
}

void _Dlg_ModelInfo::OnEnChangeEditOffsetY()
{
	if( m_pSelectInfo == NULL )
		return;

	m_pSelectInfo->s_nOffset[ 1 ] = nsCsMfcFunc::Control2Int( &m_edit_OffsetY );

	POSITION pos = m_list_Model.GetFirstSelectedItemPosition();
	int nItem = m_list_Model.GetNextSelectedItem( pos );
	TCHAR szMsg[ 256 ];
	_stprintf_s( szMsg, 256, L"%d", m_pSelectInfo->s_nOffset[ 1 ] );
	m_list_Model.SetItemText( nItem, 4, szMsg );
}

void _Dlg_ModelInfo::OnEnChangeEditOffsetZ()
{
	if( m_pSelectInfo == NULL )
		return;

	m_pSelectInfo->s_nOffset[ 2 ] = nsCsMfcFunc::Control2Int( &m_edit_OffsetZ );

	POSITION pos = m_list_Model.GetFirstSelectedItemPosition();
	int nItem = m_list_Model.GetNextSelectedItem( pos );
	TCHAR szMsg[ 256 ];
	_stprintf_s( szMsg, 256, L"%d", m_pSelectInfo->s_nOffset[ 2 ] );
	m_list_Model.SetItemText( nItem, 5, szMsg );
}

void _Dlg_ModelInfo::OnBnClickedButtonReset()
{
	if( m_pSelectInfo == NULL )
		return;

	m_pSelectInfo->s_nOffset[ 0 ] = 0;
	m_pSelectInfo->s_nOffset[ 1 ] = 0;
	m_pSelectInfo->s_nOffset[ 2 ] = 0;
	m_pSelectInfo->s_szComment[ 0 ] = NULL;

	m_edit_Comment.SetWindowText( m_pSelectInfo->s_szComment );
	nsCsMfcFunc::Int2Control( &m_edit_OffsetX, m_pSelectInfo->s_nOffset[ 0 ] );
	nsCsMfcFunc::Int2Control( &m_edit_OffsetY, m_pSelectInfo->s_nOffset[ 1 ] );
	nsCsMfcFunc::Int2Control( &m_edit_OffsetZ, m_pSelectInfo->s_nOffset[ 2 ] );

	__CreateList();	
}
