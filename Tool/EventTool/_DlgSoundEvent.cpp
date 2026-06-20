// _DlgSoundEvent.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EventTool.h"
#include "_DlgSoundEvent.h"


// _DlgSoundEvent 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgSoundEvent, CDialog)

_DlgSoundEvent::_DlgSoundEvent(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgSoundEvent::IDD, pParent)	
{

}

_DlgSoundEvent::~_DlgSoundEvent()
{
}

void _DlgSoundEvent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTSOUND, m_ListSound);	
	DDX_Control(pDX, IDC_RADIO_MUSIC, m_RadioMusic);
}


BEGIN_MESSAGE_MAP(_DlgSoundEvent, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTSOUND, &_DlgSoundEvent::OnLvnItemchangedListsound)
	ON_NOTIFY(NM_CLICK, IDC_LISTSOUND, &_DlgSoundEvent::OnNMClickListsound)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTSOUND, &_DlgSoundEvent::OnNMDblclkListsound)
	ON_BN_CLICKED(IDC_RADIO_MUSIC, &_DlgSoundEvent::OnBnClickedRadioMusic)
	ON_BN_CLICKED(IDC_RADIO_SOUND, &_DlgSoundEvent::OnBnClickedRadioSound)
END_MESSAGE_MAP()


// _DlgSoundEvent 메시지 처리기입니다.

BOOL _DlgSoundEvent::OnInitDialog()
{	
	CDialog::OnInitDialog();

	//사운드리스트
	DWORD dwExStyle = m_ListSound.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_ListSound.SetExtendedStyle( dwExStyle );
	m_ListSound.InsertColumn( 0, L"파일명", LVCFMT_CENTER, 170 );
	m_ListSound.InsertColumn( 1, L"경로", LVCFMT_CENTER, 200 );
	
	CString strDrt( ".\\Data\\Sound\\EventSound" );	
	_FindSubDirSound( strDrt, m_SoundFile );	

	for( int i = 0; i < m_SoundFile.GetSize(); i++ )
	{
		int nIndex = m_ListSound.GetItemCount();

		int nStrLen = m_SoundFile[i].GetLength();
		int nPathPos = m_SoundFile[i].ReverseFind( _T('\\') );

		CString strPathname = m_SoundFile[i].Mid( 0, nPathPos );                            // C:\temp\test\ 까지만 남습니다.
		CString strFilename = m_SoundFile[i].Mid( nPathPos+1, nStrLen - nPathPos );    // test.txt이 남습니다.

		if( strFilename.Find( _T(".wav") ) == -1  && strFilename.Find( _T(".mp3") ) == -1 )
			continue;			

		m_ListSound.InsertItem( nIndex, strFilename );
		m_ListSound.SetItem( nIndex, 1, LVIF_TEXT, strPathname, 0, 0, 0, 0 );
	}

	return TRUE;  // return TRUE unless you set the focus to a control	
}

void _DlgSoundEvent::_FindSubDirSound( CString strDir, CStringArray &FileArray )
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
			_FindSubDirSound( ff.GetFilePath(), FileArray );					
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

void _DlgSoundEvent::OnLvnItemchangedListsound(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void _DlgSoundEvent::OnNMClickListsound(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_ListSound.SetFocus();

	POSITION pos = m_ListSound.GetFirstSelectedItemPosition();
	int nSelect = m_ListSound.GetNextSelectedItem( pos );
	if( nSelect < 0 )
		return;

	int nID = (int)m_ListSound.GetItemData( nSelect );

	int nStrLen = m_SoundFile[nSelect].GetLength();
	int nPathPos = m_SoundFile[nSelect].Find( _T("Sound") );

	CString strDrtname = m_SoundFile[nSelect].Mid( nPathPos+6, nStrLen - nPathPos );  

	WCHAR *strWC = strDrtname.GetBuffer(0);
	CHAR strMB[OBJECT_PATH_LEN];
	WideCharToMultiByte( CP_ACP, 0, strWC, -1, strMB, OBJECT_PATH_LEN, NULL, NULL );
	strDrtname.ReleaseBuffer();

	g_pSoundMgr->PlaySound( strMB );

	*pResult = 0;
}

void _DlgSoundEvent::OnNMDblclkListsound(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_ListSound.SetFocus();

	POSITION pos = m_ListSound.GetFirstSelectedItemPosition();
	int nSelect = m_ListSound.GetNextSelectedItem( pos );
	if( nSelect < 0 )
		return;	

	CString str; 
	str = m_ListSound.GetItemText( nSelect, 0 );

	if( m_RadioMusic.GetCheck() ==  BST_CHECKED )	
		g_pMainWnd->_GetDlgController()->_SetSoundEvent( str, CEventData::sEventSound_Info::ES_MUSIC );
	else
		g_pMainWnd->_GetDlgController()->_SetSoundEvent( str, CEventData::sEventSound_Info::ES_SOUND );

	*pResult = 0;

	OnCancel();
}

void _DlgSoundEvent::OnBnClickedRadioMusic()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void _DlgSoundEvent::OnBnClickedRadioSound()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
