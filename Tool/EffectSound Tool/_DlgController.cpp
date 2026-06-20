// _DlgController.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EffectSound Tool.h"
#include "_DlgController.h"


// _DlgController

IMPLEMENT_DYNCREATE(_DlgController, CFormView)

_DlgController::_DlgController()
	: CFormView(_DlgController::IDD)
{

}

_DlgController::~_DlgController()
{
}

void _DlgController::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);				
	DDX_Control(pDX, IDC_LIST_EFFECTLIST, m_List_EffectList);	
	DDX_Control(pDX, IDC_LIST_SOUNDTLIST, m_List_Sound);	
	DDX_Control(pDX, IDC_EDIT_FIND_EFFECT, m_EditSerch);
}

BEGIN_MESSAGE_MAP(_DlgController, CFormView)			
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_EFFECTLIST, &_DlgController::OnLvnItemchangedListEffect)	
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SOUNDTLIST, &_DlgController::OnLvnItemchangedListSoundtlist)
	ON_BN_CLICKED(IDC_BUTTON_FILEEFFECT, &_DlgController::OnBnClickedButtonFileeffect)
	ON_BN_CLICKED(IDC_BUTTON_FILESOUND, &_DlgController::OnBnClickedButtonFilesound)
	ON_BN_CLICKED(IDC_BUTTON_INSERTSOUND, &_DlgController::OnBnClickedButtonInsertsound)	
	ON_EN_CHANGE(IDC_EDIT_TIME, &_DlgController::OnEnChangeEditTime)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_SOUNDTLIST, &_DlgController::OnLvnColumnclickListSoundtlist)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_EFFECTLIST, &_DlgController::OnLvnColumnclickListEffectlist)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SOUNDTLIST, &_DlgController::OnNMClickListSoundtlist)
	ON_NOTIFY(NM_CLICK, IDC_LIST_EFFECTLIST, &_DlgController::OnNMClickListEffectlist)
//	ON_WM_KEYDOWN()
ON_BN_CLICKED(IDC_BUTTON_SERCH, &_DlgController::OnBnClickedButtonSerch)
END_MESSAGE_MAP()


// _DlgController 진단입니다.

#ifdef _DEBUG
void _DlgController::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void _DlgController::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void _DlgController::Init()
{	
	//Effect 리스트 생성
	DWORD dwExStyle = m_List_EffectList.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_List_EffectList.SetExtendedStyle( dwExStyle );
	m_List_EffectList.InsertColumn( 0, L"파일명", LVCFMT_CENTER, 125 );
	m_List_EffectList.InsertColumn( 1, L"경로", LVCFMT_CENTER, 175 );		
	
	//사운드리스트
	dwExStyle = m_List_Sound.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_List_Sound.SetExtendedStyle( dwExStyle );
	m_List_Sound.InsertColumn( 0, L"파일명", LVCFMT_CENTER, 125 );
	m_List_Sound.InsertColumn( 1, L"경로", LVCFMT_CENTER, 175 );			
}

void _DlgController::FindSerch()
{
	m_List_EffectList.DeleteAllItems();
	m_List_Sound.DeleteAllItems();

	CString strSerch;
	m_EditSerch.GetWindowText( strSerch );

	// 이펙트 파일 검색
	CString strDrt( ".\\Data\\Effect" );	
	FindSubDirEffect( strDrt, m_EffectFile );	

	for( int i = 0; i < m_EffectFile.GetSize(); i++ )
	{
		int nIndex = m_List_EffectList.GetItemCount();

		int nStrLen = m_EffectFile[i].GetLength();
		int nPathPos = m_EffectFile[i].ReverseFind( _T('\\') );

		CString strPathname = m_EffectFile[i].Mid( 0, nPathPos );                            // C:\temp\test\ 까지만 남습니다.
		CString strFilename = m_EffectFile[i].Mid( nPathPos+1, nStrLen - nPathPos );    // test.txt이 남습니다.

		if( strFilename.Find( _T(".nif") ) == -1 )
			continue;	

		if( strFilename.Find( strSerch ) == -1 )
			continue;

		m_List_EffectList.InsertItem( nIndex, strFilename );
		m_List_EffectList.SetItem( nIndex, 1, LVIF_TEXT, strPathname, 0, 0, 0, 0 );
	}

	// 사운드 파일 검색
	strDrt = ".\\Data\\Sound";	
	FindSubDirSound( strDrt, m_SoundFile );	

	for( int i = 0; i < m_SoundFile.GetSize(); i++ )
	{
		int nIndex = m_List_Sound.GetItemCount();

		int nStrLen = m_SoundFile[i].GetLength();
		int nPathPos = m_SoundFile[i].ReverseFind( _T('\\') );

		CString strPathname = m_SoundFile[i].Mid( 0, nPathPos );                            // C:\temp\test\ 까지만 남습니다.
		CString strFilename = m_SoundFile[i].Mid( nPathPos+1, nStrLen - nPathPos );    // test.txt이 남습니다.

		if( strFilename.Find( _T(".wav") ) == -1  && strFilename.Find( _T(".mp3") ) == -1 )
			continue;	

		if( strFilename.Find( strSerch ) == -1 )
			continue;

		m_List_Sound.InsertItem( nIndex, strFilename );
		m_List_Sound.SetItem( nIndex, 1, LVIF_TEXT, strPathname, 0, 0, 0, 0 );
	}


}

void _DlgController::FindSubDirEffect( CString strDir, CStringArray &FileArray )
{
	strDir += "\\*.*";	
	
	CFileFind ff;
	BOOL bFound = ff.FindFile(strDir);	

	while(bFound)
	{
		bFound = ff.FindNextFile();

		if(ff.IsDots()) continue;
		if(ff.IsDirectory())		
		{			
			FindSubDirEffect( ff.GetFilePath(), FileArray );					
		}
		else	
		{
			if( ff.GetFilePath().Find( _T(".nif") ) != -1 )				
			{			
				FileArray.Add( ff.GetFilePath() );
			}
		}
	}
}

void _DlgController::FindSubDirSound( CString strDir, CStringArray &FileArray )
{
	strDir += "\\*.*";	

	CFileFind ff;
	BOOL bFound = ff.FindFile(strDir);	

	while(bFound)
	{
		bFound = ff.FindNextFile();

		if(ff.IsDots()) continue;
		if(ff.IsDirectory())		
		{			
			FindSubDirSound( ff.GetFilePath(), FileArray );					
		}
		else	
		{
			if( ff.GetFilePath().Find( _T(".wav") ) != -1  || ff.GetFilePath().Find( _T(".mp3") ) != -1 )
			{
				FileArray.Add( ff.GetFilePath() );
			}			
		}
	}
}

void _DlgController::OnLvnItemchangedListEffect(NMHDR *pNMHDR, LRESULT *pResult)
{	
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	*pResult = 0;	
}

void _DlgController::OnLvnItemchangedListSoundtlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	

	*pResult = 0;
}

void _DlgController::OnBnClickedButtonFileeffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
#define EFFECT_INIT_PATH		L"Data\\Effect\\"

	TCHAR szOut[ MAX_PATH ] = {0, };
	if( nsCSFILE::FileOpen( szOut, MAX_PATH, L"Nif File(*.nif)\0*.nif\0", EFFECT_INIT_PATH ) == false )
		return;

	// 하위폴더 여야만 한다.
	if( nsCSFILE::IsSubDirectory( szOut ) == false )
	{
		MessageBox( L"현재 디렉토리의 하위 디렉토리에 존재해야만 합니다." );
		return;
	}

	if( szOut[ 0 ] == NULL )
		return;

	TCHAR szRelativePath[ MAX_PATH ];
	nsCSFILE::GetRelativePath( szRelativePath, MAX_PATH, szOut );

	TCHAR szResult[ MAX_TEXTKEY_LEN ];
	size_t nLen = _tcslen( EFFECT_INIT_PATH );
	_tcscpy_s( szResult, MAX_TEXTKEY_LEN, &szRelativePath[ nLen ] );

	CString strFilePath(EFFECT_INIT_PATH);
	strFilePath += szResult;

	int nStrLen = strFilePath.GetLength();
	int nPathPos = strFilePath.Find( _T("Effect") );
	CString strPathname = strFilePath.Mid( nPathPos + 7, nStrLen ); 

	WCHAR *strWC = strPathname.GetBuffer(0);
	CHAR strMB[OBJECT_PATH_LEN];
	WideCharToMultiByte( CP_ACP, 0, strWC, -1, strMB, OBJECT_PATH_LEN, NULL, NULL );
	strPathname.ReleaseBuffer();

	size_t CodeKey = CsFPS::GetHashCode( strMB );
	if( g_EffectSoundMng->IsData( CodeKey ) == true )
	{
		if( CsMessageBox( MB_YESNO, L"작업된 이펙트가 존재합니다. 저장된 데이타로 로드하시겠습니까?" ) == IDYES )
		{
			g_ViewEffect.LoadEffect( CodeKey );			
		}
		else
		{
			g_ViewEffect.SetEffect( strFilePath );
			g_ViewEffect.ClearOption();	
		}
	}	
	else
	{
		g_ViewEffect.SetEffect( strFilePath );
		g_ViewEffect.ClearOption();	
	}
}

void _DlgController::OnBnClickedButtonFilesound()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

#define SOUND_INIT_PATH		L"Data\\Sound\\"

	TCHAR szOut[ MAX_PATH ] = {0, };	

	if( nsCSFILE::FileOpen( szOut, MAX_PATH, L"Sound File(*.*)\0*.*\0", SOUND_INIT_PATH ) == false )
		return;

	// 하위폴더 여야만 한다.
	if( nsCSFILE::IsSubDirectory( szOut ) == false )
	{
		MessageBox( L"현재 디렉토리의 하위 디렉토리에 존재해야만 합니다." );
		return;
	}

	if( szOut[ 0 ] == NULL )
		return;

	TCHAR szRelativePath[ MAX_PATH ];
	nsCSFILE::GetRelativePath( szRelativePath, MAX_PATH, szOut );	

	TCHAR szResult[ MAX_TEXTKEY_LEN ];
	size_t nLen = _tcslen( SOUND_INIT_PATH );
	_tcscpy_s( szResult, MAX_TEXTKEY_LEN, &szRelativePath[ nLen ] );

	CString strFilePath;
	strFilePath += szResult;

	WCHAR *strWC = strFilePath.GetBuffer(0);
	CHAR strMB[OBJECT_PATH_LEN];
	WideCharToMultiByte( CP_ACP, 0, strWC, -1, strMB, OBJECT_PATH_LEN, NULL, NULL );
	strFilePath.ReleaseBuffer();	

	CString SoundFile( strMB );	
	g_ViewEffect.SetSoundList( SoundFile );
	
}

void _DlgController::OnBnClickedButtonInsertsound()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	POSITION pos = m_List_Sound.GetFirstSelectedItemPosition();
	int nSelect = m_List_Sound.GetNextSelectedItem( pos );
	if( nSelect < 0 )
		return;

	CString SoundFile = m_List_Sound.GetItemText( nSelect, 1 );
	SoundFile += "\\";
	SoundFile += m_List_Sound.GetItemText( nSelect, 0 );	

	int nStrLen = SoundFile.GetLength();
	int nPathPos = SoundFile.Find( _T("Sound") );

	CString strDrtname = SoundFile.Mid( nPathPos+6, nStrLen - nPathPos );  	
	g_ViewEffect.SetSoundList( strDrtname );	
}

void _DlgController::OnEnChangeEditTime()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CString* strItem1 = (CString*)lParam1;
	CString* strItem2 = (CString*)lParam2;
	BOOL bSort = (BOOL)lParamSort;

	int result = _tcscmp(*strItem1, *strItem2);    

	if( bSort == TRUE )
	{
		return result;
	}
	else
	{
		return -result;
	}

	return 0; 
}

void _DlgController::OnLvnColumnclickListSoundtlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int iColumn = pNMLV->iSubItem;
	if( iColumn == m_nSortColumn )
	{
		m_bSort = !m_bSort;
	}
	else
	{
		m_bSort = TRUE;
	}
	m_nSortColumn = iColumn;

	int nitem = m_List_Sound.GetItemCount();
	CString** arStr =  new CString*[ nitem ];
	for( int i = 0; i < nitem; i++ )
	{	
		arStr[ i ]= new CString( m_List_Sound.GetItemText( i, iColumn ) );
		m_List_Sound.SetItemData( i, (LPARAM)arStr[ i ] );
	}

	m_List_Sound.SortItems( CompareFunc, m_bSort );

	for(int i = 0; i < nitem; i++)
	{
		delete arStr[ i ];
	}
	delete []arStr;

	*pResult = 0;
}

void _DlgController::OnLvnColumnclickListEffectlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int iColumn = pNMLV->iSubItem;
	if( iColumn == m_nSortColumn1 )
	{
		m_bSort1 = !m_bSort1;
	}
	else
	{
		m_bSort1 = TRUE;
	}
	m_nSortColumn1 = iColumn;

	int nitem = m_List_EffectList.GetItemCount();
	CString** arStr =  new CString*[ nitem ];
	for( int i = 0; i < nitem; i++ )
	{	
		arStr[ i ]= new CString( m_List_EffectList.GetItemText( i, iColumn ) );
		m_List_EffectList.SetItemData( i, (LPARAM)arStr[ i ] );
	}

	m_List_EffectList.SortItems( CompareFunc, m_bSort1 );

	for(int i = 0; i < nitem; i++)
	{
		delete arStr[ i ];
	}
	delete []arStr;

	*pResult = 0;
}

  


void _DlgController::OnNMClickListSoundtlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_List_Sound.SetFocus();

	POSITION pos = m_List_Sound.GetFirstSelectedItemPosition();
	int nSelect = m_List_Sound.GetNextSelectedItem( pos );
	if( nSelect < 0 )
		return;

	CString SoundFile( m_List_Sound.GetItemText( nSelect, 1 ) );
	SoundFile += L"\\";
	SoundFile += m_List_Sound.GetItemText( nSelect, 0 );

	int nStrLen = SoundFile.GetLength();
	int nPathPos = SoundFile.Find( _T("Sound") );

	CString strDrtname = SoundFile.Mid( nPathPos+6, nStrLen - nPathPos );  

	WCHAR *strWC = strDrtname.GetBuffer(0);
	CHAR strMB[OBJECT_PATH_LEN];
	WideCharToMultiByte( CP_ACP, 0, strWC, -1, strMB, OBJECT_PATH_LEN, NULL, NULL );
	strDrtname.ReleaseBuffer();

	g_pSoundMgr->PlaySound( strMB );

	*pResult = 0;
}

void _DlgController::OnNMClickListEffectlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_List_EffectList.SetFocus();

	POSITION pos = m_List_EffectList.GetFirstSelectedItemPosition();
	int nSelect = m_List_EffectList.GetNextSelectedItem( pos );
	if( nSelect < 0 )
	{
		g_ViewEffect.DeleteEffect();
		return;
	}

	int nID = (int)m_List_EffectList.GetItemData( nSelect );

	CString strFilePath( m_List_EffectList.GetItemText( nSelect, 1 ) );
	strFilePath += L"\\";
	strFilePath += m_List_EffectList.GetItemText( nSelect, 0 );

	int nStrLen = strFilePath.GetLength();
	int nPathPos = strFilePath.Find( _T("Effect") );
	CString strPathname = strFilePath.Mid( nPathPos + 7, nStrLen ); 

	WCHAR *strWC = strPathname.GetBuffer(0);
	CHAR strMB[OBJECT_PATH_LEN];
	WideCharToMultiByte( CP_ACP, 0, strWC, -1, strMB, OBJECT_PATH_LEN, NULL, NULL );
	strPathname.ReleaseBuffer();

	size_t CodeKey = CsFPS::GetHashCode( strMB );
	if( g_EffectSoundMng->IsData( CodeKey ) == true )
	{
		if( CsMessageBox( MB_YESNO, L"작업된 이펙트가 존재합니다. 저장된 데이타로 로드하시겠습니까?" ) == IDYES )
		{
			g_ViewEffect.LoadEffect( CodeKey );			
		}		
		else
		{
			g_ViewEffect.SetEffect( strFilePath );
			g_ViewEffect.ClearOption();	
		}
	}	
	else
	{
		g_ViewEffect.SetEffect( strFilePath );
		g_ViewEffect.ClearOption();	
	}

	*pResult = 0;
}

void _DlgController::OnBnClickedButtonSerch()
{
	FindSerch();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
