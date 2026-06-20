// _DlgEtc.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EffectSound Tool.h"
#include "_DlgEtc.h"


// _DlgEtc

IMPLEMENT_DYNCREATE(_DlgEtc, CFormView)

_DlgEtc::_DlgEtc()
	: CFormView(_DlgEtc::IDD)
{

}

_DlgEtc::~_DlgEtc()
{
}

void _DlgEtc::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_PLAY, m_Check_Play);
	DDX_Control(pDX, IDC_EDIT_TIME, m_Edit_Time);
	DDX_Control(pDX, IDC_EDIT1, m_Edit_Scale);
	DDX_Control(pDX, IDC_CHECK_MODEL, m_Check_Model);
	DDX_Control(pDX, IDC_CHECK_LOOP, m_Check_Loop);
	DDX_Control(pDX, IDC_SLIDER1, m_sld_Time);
	DDX_Control(pDX, IDC_SPIN_SCALE, m_spin_Scale);	
	DDX_Control(pDX, IDC_COMBO_LIVE, m_Combo_Live);
	DDX_Control(pDX, IDC_COMBO_POS, m_Combo_Pos);
	DDX_Control(pDX, IDC_COMBO_STARTPOS, m_Combo_StartPos);	
	DDX_Control(pDX, IDC_CHECK_OFFSETUSE, m_Check_Offset);
	DDX_Control(pDX, IDC_SPIN_X, m_spin_X);	
	DDX_Control(pDX, IDC_SPIN_Y, m_spin_Y);	
	DDX_Control(pDX, IDC_SPIN_Z, m_spin_Z);	
	DDX_Control(pDX, IDC_EDIT_X, m_Edit_X);
	DDX_Control(pDX, IDC_EDIT_Y, m_Edit_Y);
	DDX_Control(pDX, IDC_EDIT_Z, m_Edit_Z);

	DDX_Control(pDX, IDC_LIST_SOUNDLIST, m_List_Sound);
	DDX_Control(pDX, IDC_COMBO_DIR, m_Combo_Dir);
	DDX_Control(pDX, IDC_EDIT2, m_Edit_CurEffect);
}

BEGIN_MESSAGE_MAP(_DlgEtc, CFormView)
	ON_EN_CHANGE(IDC_EDIT_TIME, &_DlgEtc::OnEnChangeEditTime)
	ON_BN_CLICKED(IDC_CHECK_LOOP, &_DlgEtc::OnBnClickedCheckLoop)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &_DlgEtc::OnNMCustomdrawSlider1)	
	ON_BN_CLICKED(IDC_BUTTON_REOPTION, &_DlgEtc::OnBnClickedButtonReoption)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &_DlgEtc::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &_DlgEtc::OnBnClickedButtonLoad)
	ON_WM_HSCROLL()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SOUNDLIST, &_DlgEtc::OnLvnItemchangedListSoundlist)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_SOUNDLIST, &_DlgEtc::OnLvnColumnclickListSoundlist)
	ON_BN_CLICKED(IDC_BUTTON_DELSOUND, &_DlgEtc::OnBnClickedButtonDelsound)	
	ON_BN_CLICKED(IDC_CHECK_PLAY, &_DlgEtc::OnBnClickedCheckPlay)
END_MESSAGE_MAP()


// _DlgEtc 진단입니다.

#ifdef _DEBUG
void _DlgEtc::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void _DlgEtc::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void _DlgEtc::Init()
{
	m_Check_Play.SetCheck( BST_UNCHECKED );	
	
	m_spin_Scale.SetCsBuddy( &m_Edit_Scale, 0.001f, 3 );
	nsCsMfcFunc::Float2Control( &m_Edit_Scale, 1.0f, 3 );

	m_spin_X.SetCsBuddy( &m_Edit_X, 0.001f, 3 );
	nsCsMfcFunc::Float2Control( &m_Edit_X, 0.0f, 3 );

	m_spin_Y.SetCsBuddy( &m_Edit_Y, 0.001f, 3 );
	nsCsMfcFunc::Float2Control( &m_Edit_Y, 0.0f, 3 );

	m_spin_Z.SetCsBuddy( &m_Edit_Z, 0.001f, 3 );
	nsCsMfcFunc::Float2Control( &m_Edit_Z, 0.0f, 3 );

	m_Combo_Live.InsertString( 0, L"한번" );
	m_Combo_Live.SetItemData( 0, nsEFFECT::LIVE_ONCE );
	m_Combo_Live.InsertString( 1, L"계속" );
	m_Combo_Live.SetItemData( 1, nsEFFECT::LIVE_LOOP );
	m_Combo_Live.SetCurSel( 0 );

	m_Combo_Pos.InsertString( 0, L"제자리" );
	m_Combo_Pos.SetItemData( 0, nsEFFECT::POS_NORMAL );
	m_Combo_Pos.InsertString( 1, L"캐릭터" );
	m_Combo_Pos.SetItemData( 1, nsEFFECT::POS_CHARPOS );	
	m_Combo_Pos.SetCurSel( 0 );

	m_Combo_StartPos.InsertString( 0, L"캐릭터" );
	m_Combo_StartPos.SetItemData( 0, nsEFFECT::SPOS_CHARWORLD );
	m_Combo_StartPos.InsertString( 1, L"브리오중심점" );
	m_Combo_StartPos.SetItemData( 1, nsEFFECT::SPOS_BOUND_CENTER );
	m_Combo_StartPos.InsertString( 2, L"높이의반" );
	m_Combo_StartPos.SetItemData( 2, nsEFFECT::SPOS_HALF_THEIGHT );
	m_Combo_StartPos.InsertString( 3, L"머리위" );
	m_Combo_StartPos.SetItemData( 3, nsEFFECT::SPOS_THEIGHT );
	m_Combo_StartPos.SetCurSel( 0 );

	m_Combo_Dir.InsertString( 0, L"사용안함" );
	m_Combo_Dir.SetItemData( 0, nsEFFECT::DIR_NONE );
	m_Combo_Dir.InsertString( 1, L"캐릭터방향회전" );
	m_Combo_Dir.SetItemData( 1, nsEFFECT::DIR_CHAR );
	m_Combo_Dir.InsertString( 2, L"카메라Z축회전" );
	m_Combo_Dir.SetItemData( 2, nsEFFECT::DIR_CAMERA_AXIS_Z );
	m_Combo_Dir.SetCurSel( 0 );

	DWORD dwExStyle = m_List_Sound.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_List_Sound.SetExtendedStyle( dwExStyle );
	m_List_Sound.InsertColumn( 0, L"시간", LVCFMT_CENTER, 90 );
	m_List_Sound.InsertColumn( 1, L"파일명", LVCFMT_CENTER, 220 );
	
}

void _DlgEtc::Update( float fEffTime )
{	
	if( IsPlay() )
	{
		int nPos = (int)( fEffTime*1000.0f );
		m_sld_Time.SetPos( nPos );			
	}	
}

void _DlgEtc::Reset()
{
	m_Check_Offset.SetCheck( BST_UNCHECKED );	
	m_Check_Model.SetCheck( BST_UNCHECKED );
	m_Check_Play.SetCheck( BST_UNCHECKED );	

	nsCsMfcFunc::Float2Control( &m_Edit_Scale, 1.0f, 3 );	
	nsCsMfcFunc::Float2Control( &m_Edit_X, 0.0f, 3 );	
	nsCsMfcFunc::Float2Control( &m_Edit_Y, 0.0f, 3 );	
	nsCsMfcFunc::Float2Control( &m_Edit_Z, 0.0f, 3 );

	m_Combo_Live.SetCurSel( 0 );
	m_Combo_Pos.SetCurSel( 0 );	
	m_Combo_StartPos.SetCurSel( 0 );

	m_List_Sound.DeleteAllItems();	
}


void _DlgEtc::SetSldRange( float fEndTime )
{ 
	m_sld_Time.SetPos( 0 );
	m_sld_Time.SetRange( 0, (int)( fEndTime*1000.0f ) ); 
}

void _DlgEtc::SetOption( CsEffectSound* Option )
{	
	nsCsMfcFunc::Float2Control( &m_Edit_X, Option->m_sEffectOptin.vOffset.x, 3 );	
	nsCsMfcFunc::Float2Control( &m_Edit_Y, Option->m_sEffectOptin.vOffset.y, 3 );	
	nsCsMfcFunc::Float2Control( &m_Edit_Z, Option->m_sEffectOptin.vOffset.z, 3 );

	if( ( Option->m_sEffectOptin.dwEffectFlag & nsEFFECT::OFFSET_MASK ) == nsEFFECT::OFFSET_USE )
	{
		m_Check_Offset.SetCheck( BST_CHECKED );
	}

	switch( Option->m_sEffectOptin.dwEffectFlag & nsEFFECT::DIR_MASK )
	{
	case nsEFFECT::DIR_NONE:
		m_Combo_Dir.SetCurSel( 0 );
		break;
	case nsEFFECT::DIR_CHAR:
		m_Combo_Dir.SetCurSel( 1 );
		break;
	case nsEFFECT::DIR_CAMERA_AXIS_Z:
		m_Combo_Dir.SetCurSel( 2 );
		break;
	}

	switch( Option->m_sEffectOptin.dwEffectFlag & nsEFFECT::LIVE_MASK )
	{
	case nsEFFECT::LIVE_ONCE:
		m_Combo_Live.SetCurSel( 0 );
		break;
	case nsEFFECT::LIVE_LOOP:
		m_Combo_Live.SetCurSel( 1 );
		break;
	}

	switch( Option->m_sEffectOptin.dwEffectFlag & nsEFFECT::POS_MASK )
	{
	case nsEFFECT::POS_NORMAL:
		m_Combo_Pos.SetCurSel( 0 );
		break;
	case nsEFFECT::POS_CHARPOS:
		m_Combo_Pos.SetCurSel( 1 );
		break;
	case nsEFFECT::POS_SHOT:
		m_Combo_Pos.SetCurSel( 2 );
		break;
	}
	
	switch( Option->m_sEffectOptin.dwEffectFlag & nsEFFECT::SPOS_MASK )
	{
	case nsEFFECT::SPOS_CHARWORLD:
		m_Combo_StartPos.SetCurSel( 0 );
		break;
	case nsEFFECT::SPOS_BOUND_CENTER:
		m_Combo_StartPos.SetCurSel( 1 );
		break;
	case nsEFFECT::SPOS_HALF_THEIGHT:
		m_Combo_StartPos.SetCurSel( 2 );
		break;
	case nsEFFECT::SPOS_THEIGHT:
		m_Combo_StartPos.SetCurSel( 3 );
		break;
	}

	if( Option->m_sEffectOptin.bChrScale )
	{
		m_Check_Model.SetCheck( BST_CHECKED );
	}

	nsCsMfcFunc::Float2Control( &m_Edit_Scale, Option->m_sEffectOptin.fScale, 3 );

}



void _DlgEtc::SetSoundList( CString strSound, float fTime )
{
	TCHAR szTime[64];
	_stprintf_s( szTime, 64, L"%f", fTime );			

	int nIndex = m_List_Sound.GetItemCount();	
	
	m_List_Sound.InsertItem( nIndex, szTime );
	m_List_Sound.SetItem( nIndex, 1, LVIF_TEXT, strSound, 0, 0, 0, 0 );		

}

void _DlgEtc::GetSoundListData( CsEffectSound::sSound_Info* pData, int nIndex )
{
	CString str;	

	//시간
	str = m_List_Sound.GetItemText( nIndex, 0 );
	pData->fSec = (float)_wtof(str.GetBuffer(str.GetLength())); // CString -> float;

	//파일명
	str = m_List_Sound.GetItemText( nIndex, 1 );
	WCHAR *strWC = str.GetBuffer(0);	
	WideCharToMultiByte( CP_ACP, 0, strWC, -1, pData->szFileName, OBJECT_PATH_LEN, NULL, NULL );
	str.ReleaseBuffer();
}

void _DlgEtc::SetControltoData()
{
	DWORD dwFlag = 0;

	int n =m_Combo_Pos.GetCurSel();

	dwFlag &= ~ nsEFFECT::LIVE_MASK;
	dwFlag |= m_Combo_Live.GetItemData( m_Combo_Live.GetCurSel() );

	dwFlag &= ~ nsEFFECT::POS_MASK;
	dwFlag |= m_Combo_Pos.GetItemData( m_Combo_Pos.GetCurSel() );

	dwFlag &= ~ nsEFFECT::SPOS_MASK;
	dwFlag |= m_Combo_StartPos.GetItemData( m_Combo_StartPos.GetCurSel() );

	dwFlag &= ~ nsEFFECT::DIR_MASK;
	int nn = m_Combo_Dir.GetCurSel();
	dwFlag |= m_Combo_Dir.GetItemData( m_Combo_Dir.GetCurSel() );

	dwFlag &= ~ nsEFFECT::OFFSET_MASK;
	dwFlag |= m_Check_Offset.GetCheck() == BST_CHECKED ? nsEFFECT::OFFSET_USE : nsEFFECT::OFFSET_NONE;

	NiPoint3 vOffset( nsCsMfcFunc::Control2Float( &m_Edit_X ), nsCsMfcFunc::Control2Float( &m_Edit_Y ), nsCsMfcFunc::Control2Float( &m_Edit_Z) );
	bool bChar = IsModel();
	float fScale = GetScale();		

	if(	g_ViewEffect.SetOption( dwFlag, vOffset, bChar, fScale ) == false )
	{
		g_ViewEffect.DeleteEffect();
	}		
}




// _DlgEtc 메시지 처리기입니다.

void _DlgEtc::OnEnChangeEditTime()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void _DlgEtc::OnBnClickedCheckLoop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	bool bLoop = ( m_Check_Loop.GetCheck() == BST_CHECKED )? true : false;

	g_ViewEffect.SetLoop( bLoop );	
}


void _DlgEtc::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void _DlgEtc::OnBnClickedButtonReoption()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	g_ViewEffect.ClearOption();
	
	nsCsMfcFunc::Float2Control( &m_Edit_X, 0.0f, 3 );
	nsCsMfcFunc::Float2Control( &m_Edit_Y, 0.0f, 3 );	
	nsCsMfcFunc::Float2Control( &m_Edit_Z, 0.0f, 3 );	
	
	m_Combo_Live.SetCurSel( 0 );	
	m_Combo_Pos.SetCurSel( 0 );	
	m_Combo_StartPos.SetCurSel( 0 );
	m_Combo_Dir.SetCurSel( 0 );
	
	m_Check_Offset.SetCheck( BST_UNCHECKED );

	m_List_Sound.DeleteAllItems();
}

void _DlgEtc::OnBnClickedButtonSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	

	g_EffectSoundMng->InsertData( g_ViewEffect.m_pOption );	
	g_EffectSoundMng->SaveFile();	
}

void _DlgEtc::OnBnClickedButtonLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	_DlgLoad ie;
	if( ie.DoModal() == IDCANCEL )
		return;	
}

void _DlgEtc::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if( pScrollBar->GetSafeHwnd() == m_sld_Time.GetSafeHwnd() )
	{
		m_Check_Play.SetCheck( BST_UNCHECKED );
	}

	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void _DlgEtc::OnLvnItemchangedListSoundlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	

	*pResult = 0;
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

void _DlgEtc::OnLvnColumnclickListSoundlist(NMHDR *pNMHDR, LRESULT *pResult)
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

void _DlgEtc::OnBnClickedButtonDelsound()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_List_Sound.SetFocus();

	int nItemCount = m_List_Sound.GetSelectedCount();
	POSITION pos = m_List_Sound.GetFirstSelectedItemPosition();
	for (int i = 0; i < nItemCount; i++)
	{
		int nIndex = m_List_Sound.GetNextSelectedItem(pos);
		if( nIndex < 0 )
			return;			
		m_List_Sound.DeleteItem( nIndex );				
	}
}

void _DlgEtc::OnBnClickedCheckPlay()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( IsPlay() == true )
	{
		SetControltoData();		
	}	
}
