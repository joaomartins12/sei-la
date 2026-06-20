// _DlgInsertEvent.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SequnceTool.h"
#include "_DlgInsertEvent.h"


// _DlgInsertEvent 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgInsertEvent, CDialog)

_DlgInsertEvent::_DlgInsertEvent( int* nType, char* cTextKey, int* nStaticIndex, CWnd* pParent /*=NULL*/)
	: CDialog(_DlgInsertEvent::IDD, pParent)
{
	m_nType = nType;
	m_cTextKey = cTextKey;
	m_nStaticIndex = nStaticIndex;
}

_DlgInsertEvent::~_DlgInsertEvent()
{
}

void _DlgInsertEvent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DY_EFFECT, m_list_Effect);
	DDX_Control(pDX, IDC_LIST_DY_SOUND, m_list_Sound);
	DDX_Control(pDX, IDC_LIST_FE_LIST, m_list_FE);
}


BEGIN_MESSAGE_MAP(_DlgInsertEvent, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_EFFECT_FILE, &_DlgInsertEvent::OnBnClickedButtonEffectFile)
	ON_BN_CLICKED(IDC_BUTTON_SOUND_FILE, &_DlgInsertEvent::OnBnClickedButtonSoundFile)
	ON_LBN_DBLCLK(IDC_LIST_DY_EFFECT, &_DlgInsertEvent::OnLbnDblclkListDyEffect)
	ON_LBN_DBLCLK(IDC_LIST_DY_SOUND, &_DlgInsertEvent::OnLbnDblclkListDySound)
	ON_BN_CLICKED(IDC_BUTTON_FE_DEL, &_DlgInsertEvent::OnBnClickedButtonFeDel)
	ON_BN_CLICKED(IDC_BUTTON_FE_LOAD, &_DlgInsertEvent::OnBnClickedButtonFeLoad)
	ON_LBN_DBLCLK(IDC_LIST_FE_LIST, &_DlgInsertEvent::OnLbnDblclkListFeList)
END_MESSAGE_MAP()


// _DlgInsertEvent 메시지 처리기입니다.

#define FE_PATH		L"ToolRgs\\SequenceFE.rgs"

BOOL _DlgInsertEvent::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_list_Effect.InsertString( -1, L"타격 공격 (E_NAtt)" );
	m_list_Effect.SetItemData( m_list_Effect.GetCount()-1, nsSTATIC_EVENT::DYE_NORMAL_ATTACK );

	m_list_Effect.InsertString( -1, L"타격 공격 파일 (E_NAtt_File)" );
	m_list_Effect.SetItemData( m_list_Effect.GetCount()-1, nsSTATIC_EVENT::DYE_NORMAL_ATTACK_FILE );

	m_list_Effect.InsertString( -1, L"공격 충돌 이펙트" );
	m_list_Effect.SetItemData( m_list_Effect.GetCount()-1, nsSTATIC_EVENT::DYE_ATTACK_CLASH_FILE );

	m_list_Effect.InsertString( -1, L"원거리 공격 시작 (E_RAtt_Start)" );
	m_list_Effect.SetItemData( m_list_Effect.GetCount()-1, nsSTATIC_EVENT::DYE_RANGE_ATTACK_START );

	m_list_Effect.InsertString( -1, L"원거리 공격 이펙트" );
	m_list_Effect.SetItemData( m_list_Effect.GetCount()-1, nsSTATIC_EVENT::DYE_RANGE_ATTACK_FILE );	

	m_list_Effect.InsertString( -1, L"원거리 공격 끝 (E_RAtt_End)" );
	m_list_Effect.SetItemData( m_list_Effect.GetCount()-1, nsSTATIC_EVENT::DYE_RANGE_ATTACK_END );

	




	m_list_Sound.InsertString( -1, L"맞는 소리 약 (S_Att_L)" );
	m_list_Sound.SetItemData( m_list_Sound.GetCount()-1, nsSTATIC_EVENT::DYS_DMG_ATT_L );

	m_list_Sound.InsertString( -1, L"맞는 소리 중 (S_Att_M)" );
	m_list_Sound.SetItemData( m_list_Sound.GetCount()-1, nsSTATIC_EVENT::DYS_DMG_ATT_M );

	m_list_Sound.InsertString( -1, L"맞는 소리 강 (S_Att_H)" );
	m_list_Sound.SetItemData( m_list_Sound.GetCount()-1, nsSTATIC_EVENT::DYS_DMG_ATT_H );

	m_list_Sound.InsertString( -1, L"메탈공격음 (S_Att_Metal)" );
	m_list_Sound.SetItemData( m_list_Sound.GetCount()-1, nsSTATIC_EVENT::DYS_DMG_ATT_METAL );

	m_list_Sound.InsertString( -1, L"날카로운 찌르기 (S_Att_Thrust)" );
	m_list_Sound.SetItemData( m_list_Sound.GetCount()-1, nsSTATIC_EVENT::DYS_DMG_ATT_THRUST );

	m_list_Sound.InsertString( -1, L"채찍맞는 소리 (S_Att_Wip)" );
	m_list_Sound.SetItemData( m_list_Sound.GetCount()-1, nsSTATIC_EVENT::DYS_DMG_ATT_WIP );

	m_list_Sound.InsertString( -1, L"둔탁한것으로 (S_Att_Stick)" );
	m_list_Sound.SetItemData( m_list_Sound.GetCount()-1, nsSTATIC_EVENT::DYS_DMG_ATT_STICK );

	m_list_Sound.InsertString( -1, L"매우강한 (S_Att_Power)" );
	m_list_Sound.SetItemData( m_list_Sound.GetCount()-1, nsSTATIC_EVENT::DYS_DMG_ATT_POWER );

	m_list_Sound.InsertString( -1, L"테이머발자국 (S_T_Foot_N1)" );
	m_list_Sound.SetItemData( m_list_Sound.GetCount()-1, nsSTATIC_EVENT::DYS_T_Foot_N_01 );

	m_list_Sound.InsertString( -1, L"테이머발자국 (S_T_Foot_N2)" );
	m_list_Sound.SetItemData( m_list_Sound.GetCount()-1, nsSTATIC_EVENT::DYS_T_Foot_N_02 );

	m_list_Sound.InsertString( -1, L"원거리발사 (S_Range_Start)" );
	m_list_Sound.SetItemData( m_list_Sound.GetCount()-1, nsSTATIC_EVENT::DYS_RANGE_ATTACK_START );

	m_list_Sound.InsertString( -1, L"원거리충돌 (S_Range_Clash)" );
	m_list_Sound.SetItemData( m_list_Sound.GetCount()-1, nsSTATIC_EVENT::DYS_RANGE_ATTACK_CLASH );

	m_list_Sound.InsertString( -1, L"번역,원본 (S_Lan_Sound)" );
	m_list_Sound.SetItemData( m_list_Sound.GetCount()-1, nsSTATIC_EVENT::DYS_LANGUAGE_SOUND );

	m_list_Sound.InsertString( -1, L"공통 (S_Lan_Common)" );
	m_list_Sound.SetItemData( m_list_Sound.GetCount()-1, nsSTATIC_EVENT::DYS_LANGUAGE_COMMON );

	m_list_Sound.InsertString( -1, L"나만들려 (S_UserOnly)" );
	m_list_Sound.SetItemData( m_list_Sound.GetCount()-1, nsSTATIC_EVENT::DYS_USER_ONLY );	

	m_list_Sound.InsertString( -1, L"나만들려 번역,원본 (S_LUO_Sound)" );
	m_list_Sound.SetItemData( m_list_Sound.GetCount()-1, nsSTATIC_EVENT::DYS_USER_ONLY_LANGUAGE_SOUND );

	m_list_Sound.InsertString( -1, L"나만들려 공통 (S_LUO_Common)" );
	m_list_Sound.SetItemData( m_list_Sound.GetCount()-1, nsSTATIC_EVENT::DYS_USER_ONLY_LANGUAGE_COMMON );

	if( _taccess_s( FE_PATH, 0 ) == 0 )
	{
		FILE* fp;
		_tfopen_s( &fp, FE_PATH, L"rb" );

		int nCount;
		fread( &nCount, sizeof( int ), 1, fp );
		for( int i=0; i<nCount; ++i )
		{
			sFE* pFE = NiNew sFE;
			fread( pFE, sizeof( sFE ), 1, fp );
			m_listFileEffect.push_back( pFE );
		}

		fclose( fp );

		_ReadListFileEffect();
	}

	m_nSelectList = -1;

	m_list_Effect.SetFocus();
	m_list_Effect.SetCurSel( 0 );

	return TRUE;  // return TRUE unless you set the focus to a control
}

void _DlgInsertEvent::OnBnClickedButtonEffectFile()
{
	if( _GetEffectFileOpen( m_cTextKey ) == false )
		return;

	*m_nType = CsC_LoadSequence::sTEXTKEY_EVENT::ET_E_FILE;
	*m_nStaticIndex = -1;

	_DeleteListFileEffect();
	CDialog::OnOK();
}

void _DlgInsertEvent::OnLbnDblclkListDyEffect()
{
	m_nSelectList = m_list_Effect.GetCurSel();

	if( m_nSelectList == -1 )
		return;

	_SetDyEffect();
}

void _DlgInsertEvent::_SetDyEffect()
{
	*m_nType = CsC_LoadSequence::sTEXTKEY_EVENT::ET_DE_STATIC;
	*m_nStaticIndex = (int)m_list_Effect.GetItemData( m_nSelectList );

	switch( *m_nStaticIndex )
	{
	case nsSTATIC_EVENT::DYE_NORMAL_ATTACK:
		sprintf_s( m_cTextKey, MAX_TEXTKEY_LEN, "E_NAtt" );
		break;
	case nsSTATIC_EVENT::DYE_NORMAL_ATTACK_FILE:
		if( _GetEffectFileOpen( m_cTextKey ) == false )
			return;
		break;
	case nsSTATIC_EVENT::DYE_ATTACK_CLASH_FILE:
		if( _GetEffectFileOpen( m_cTextKey ) == false )
			return;
		break;

	case nsSTATIC_EVENT::DYE_RANGE_ATTACK_START:
		sprintf_s( m_cTextKey, MAX_TEXTKEY_LEN, "E_RAtt_Start" );
		break;
	case nsSTATIC_EVENT::DYE_RANGE_ATTACK_FILE:
		if( _GetEffectFileOpen( m_cTextKey ) == false )
			return;
		break;	
	case nsSTATIC_EVENT::DYE_RANGE_ATTACK_END:
		sprintf_s( m_cTextKey, MAX_TEXTKEY_LEN, "E_RAtt_End" );
		break;
	default:
		assert_cs( false );
	}	

	_DeleteListFileEffect();
	CDialog::OnOK();
}

void _DlgInsertEvent::OnBnClickedButtonSoundFile()
{
	if( _GetSoundFileOpen( m_cTextKey ) == false )
		return;

	*m_nType = CsC_LoadSequence::sTEXTKEY_EVENT::ET_S_FILE;
	*m_nStaticIndex = -1;

	_DeleteListFileEffect();
	CDialog::OnOK();
}

void _DlgInsertEvent::OnLbnDblclkListDySound()
{
	m_nSelectList = m_list_Sound.GetCurSel();

	if( m_nSelectList == -1 )
		return;

	_SetDySound();
}

void _DlgInsertEvent::OnLbnDblclkListFeList()
{
	m_nSelectList = m_list_FE.GetCurSel();
	if( m_nSelectList == -1 )
		return;

	sFE* pSelFE = (sFE*)m_list_FE.GetItemData( m_nSelectList );
	strcpy_s( m_cTextKey, MAX_TEXTKEY_LEN, pSelFE->s_cFile );
	*m_nType = CsC_LoadSequence::sTEXTKEY_EVENT::ET_E_FILE;
	*m_nStaticIndex = -1;

	_DeleteListFileEffect();
	CDialog::OnOK();
}

void _DlgInsertEvent::_SetDySound()
{
	*m_nType = CsC_LoadSequence::sTEXTKEY_EVENT::ET_DS_STATIC;
	*m_nStaticIndex = (int)m_list_Sound.GetItemData( m_nSelectList );

	switch( *m_nStaticIndex )
	{
	case nsSTATIC_EVENT::DYS_DMG_ATT_L:
		sprintf_s( m_cTextKey, MAX_TEXTKEY_LEN, "S_Att_L" );
		break;
	case nsSTATIC_EVENT::DYS_DMG_ATT_M:
		sprintf_s( m_cTextKey, MAX_TEXTKEY_LEN, "S_Att_M" );
		break;
	case nsSTATIC_EVENT::DYS_DMG_ATT_H:
		sprintf_s( m_cTextKey, MAX_TEXTKEY_LEN, "S_Att_H" );
		break;
	case nsSTATIC_EVENT::DYS_DMG_ATT_METAL:
		sprintf_s( m_cTextKey, MAX_TEXTKEY_LEN, "S_Att_Metal" );
		break;
	case nsSTATIC_EVENT::DYS_DMG_ATT_THRUST:
		sprintf_s( m_cTextKey, MAX_TEXTKEY_LEN, "S_Att_Thrust" );
		break;
	case nsSTATIC_EVENT::DYS_DMG_ATT_WIP:
		sprintf_s( m_cTextKey, MAX_TEXTKEY_LEN, "S_Att_Wip" );
		break;
	case nsSTATIC_EVENT::DYS_DMG_ATT_STICK:
		sprintf_s( m_cTextKey, MAX_TEXTKEY_LEN, "S_Att_Stick" );
		break;
	case nsSTATIC_EVENT::DYS_DMG_ATT_POWER:
		sprintf_s( m_cTextKey, MAX_TEXTKEY_LEN, "S_Att_Power" );
		break;
	case nsSTATIC_EVENT::DYS_T_Foot_N_01:
		sprintf_s( m_cTextKey, MAX_TEXTKEY_LEN, "S_T_Foot_N1" );
		break;
	case nsSTATIC_EVENT::DYS_T_Foot_N_02:
		sprintf_s( m_cTextKey, MAX_TEXTKEY_LEN, "S_T_Foot_N2" );
		break;
	
	case nsSTATIC_EVENT::DYS_LANGUAGE_SOUND:
	case nsSTATIC_EVENT::DYS_USER_ONLY_LANGUAGE_SOUND:
		if( _GetLanguageSoundFileOpen( m_cTextKey, L"Language\\Kor_Tr\\" ) == false )
			return;
		break;

	case nsSTATIC_EVENT::DYS_LANGUAGE_COMMON:
	case nsSTATIC_EVENT::DYS_USER_ONLY_LANGUAGE_COMMON:
		if( _GetLanguageSoundFileOpen( m_cTextKey, L"Language\\Kor_Common\\" ) == false )
			return;
		break;

	case nsSTATIC_EVENT::DYS_USER_ONLY:
		if( _GetSoundFileOpen( m_cTextKey ) == false )
			return;
		break;

	case nsSTATIC_EVENT::DYS_RANGE_ATTACK_START:
		if( _GetSoundFileOpen( m_cTextKey ) == false )
			return;
		break;
	case nsSTATIC_EVENT::DYS_RANGE_ATTACK_CLASH:
		if( _GetSoundFileOpen( m_cTextKey ) == false )
			return;
		break;
	default:
		assert_cs( false );
	}	

	_DeleteListFileEffect();
	CDialog::OnOK();
}

bool _DlgInsertEvent::_GetEffectFileOpen( char* cFile )
{
	#define EFFECT_INIT_PATH		L"Data\\Effect\\"

	TCHAR szOut[ MAX_PATH ] = {0, };
	if( nsCSFILE::FileOpen( szOut, MAX_PATH, L"Nif File(*.nif)\0*.nif\0", EFFECT_INIT_PATH ) == false )
		return false;

	// 하위폴더 여야만 한다.
	if( nsCSFILE::IsSubDirectory( szOut ) == false )
	{
		MessageBox( L"현재 디렉토리의 하위 디렉토리에 존재해야만 합니다." );
		return false;
	}

	if( szOut[ 0 ] == NULL )
		return false;

	TCHAR szRelativePath[ MAX_PATH ];
	nsCSFILE::GetRelativePath( szRelativePath, MAX_PATH, szOut );

	TCHAR szResult[ MAX_TEXTKEY_LEN ];
	size_t nLen = _tcslen( EFFECT_INIT_PATH );
	_tcscpy_s( szResult, MAX_TEXTKEY_LEN, &szRelativePath[ nLen ] );

	W2M( cFile, szResult, MAX_TEXTKEY_LEN );

	return true;
}


bool _DlgInsertEvent::_GetSoundFileOpen( char* cFile )
{
	#define SOUND_INIT_PATH		L"Data\\Sound\\"

	TCHAR szOut[ MAX_PATH ] = {0, };
	if( nsCSFILE::FileOpen( szOut, MAX_PATH, L"Sound File(*.*)\0*.*\0", SOUND_INIT_PATH ) == false )
		return false;

	// 하위폴더 여야만 한다.
	if( nsCSFILE::IsSubDirectory( szOut ) == false )
	{
		MessageBox( L"현재 디렉토리의 하위 디렉토리에 존재해야만 합니다." );
		return false;
	}

	if( szOut[ 0 ] == NULL )
		return false;

	TCHAR szRelativePath[ MAX_PATH ];
	nsCSFILE::GetRelativePath( szRelativePath, MAX_PATH, szOut );

	TCHAR szResult[ MAX_TEXTKEY_LEN ];
	size_t nLen = _tcslen( SOUND_INIT_PATH );
	_tcscpy_s( szResult, MAX_TEXTKEY_LEN, &szRelativePath[ nLen ] );

	W2M( cFile, szResult, MAX_TEXTKEY_LEN );

	return true;
}

bool _DlgInsertEvent::_GetLanguageSoundFileOpen( char* cFile, TCHAR* szLanguage )
{
#define SOUND_INIT_PATH		L"Data\\Sound\\"

	TCHAR szInitPath[ MAX_PATH ];
	_stprintf_s( szInitPath, MAX_PATH, L"%s%s", SOUND_INIT_PATH, szLanguage );

	TCHAR szOut[ MAX_PATH ] = {0, };
	if( nsCSFILE::FileOpen( szOut, MAX_PATH, L"Sound File(*.*)\0*.*\0", szInitPath ) == false )
		return false;

	// 하위폴더 여야만 한다.
	if( nsCSFILE::IsSubDirectory( szOut ) == false )
	{
		MessageBox( L"현재 디렉토리의 하위 디렉토리에 존재해야만 합니다." );
		return false;
	}

	if( szOut[ 0 ] == NULL )
		return false;

	TCHAR szRelativePath[ MAX_PATH ];
	nsCSFILE::GetRelativePath( szRelativePath, MAX_PATH, szOut );

	TCHAR szResult[ MAX_TEXTKEY_LEN ];
	size_t nLen = _tcslen( szInitPath );
	_tcscpy_s( szResult, MAX_TEXTKEY_LEN, &szRelativePath[ nLen ] );

	W2M( cFile, szResult, MAX_TEXTKEY_LEN );

	return true;
}

void _DlgInsertEvent::OnBnClickedButtonFeDel()
{
	if( CsMessageBox( MB_OKCANCEL, L"저장된 리스트를 삭제 합니다" ) == IDOK )
	{
		_DeleteListFileEffect();
		_SaveListFileEffect();
		_ReadListFileEffect();
	}	
}

void _DlgInsertEvent::OnBnClickedButtonFeLoad()
{
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

	sFE* pFE = NiNew sFE;
	W2M( pFE->s_cFile, szResult, MAX_TEXTKEY_LEN );
	m_listFileEffect.push_back( pFE );

	_SaveListFileEffect();
	_ReadListFileEffect();
}

void _DlgInsertEvent::OnCancel()
{
	_DeleteListFileEffect();
	CDialog::OnCancel();
}

void _DlgInsertEvent::_ReadListFileEffect()
{
	m_list_FE.ResetContent();

	TCHAR str[ MAX_PATH ];
	int nIndex = 0;
	std::list< sFE* >::iterator it = m_listFileEffect.begin();
	std::list< sFE* >::iterator itEnd = m_listFileEffect.end();
	for( ; it!=itEnd; ++it )
	{
		M2W( str, (*it)->s_cFile, MAX_PATH );
		m_list_FE.InsertString( nIndex, str );
		m_list_FE.SetItemData( nIndex, (DWORD_PTR)(*it) );
		++nIndex;
	}
}

void _DlgInsertEvent::_DeleteListFileEffect()
{
	std::list< sFE* >::iterator it = m_listFileEffect.begin();
	std::list< sFE* >::iterator itEnd = m_listFileEffect.end();
	for( ; it!=itEnd; ++it )
	{
		SAFE_NIDELETE( *it );
	}
	m_listFileEffect.clear();
}

void _DlgInsertEvent::_SaveListFileEffect()
{
	FILE* fp;
	_tfopen_s( &fp, FE_PATH, L"wb" );

	int nCount = (int)m_listFileEffect.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	std::list< sFE* >::iterator it = m_listFileEffect.begin();
	std::list< sFE* >::iterator itEnd = m_listFileEffect.end();
	for( ; it!=itEnd; ++it )
	{
		fwrite( *it, sizeof( sFE ), 1, fp );
	}

	fclose( fp );
}

void _DlgInsertEvent::OnOK()
{
	if( m_list_Effect.GetSafeHwnd() == GetFocus()->GetSafeHwnd() )
	{
		if( m_list_Effect.GetCurSel() == -1 )
			return;

		OnLbnDblclkListDyEffect();
	}
	else if( m_list_Sound.GetSafeHwnd() == GetFocus()->GetSafeHwnd() )
	{
		if( m_list_Sound.GetCurSel() == -1 )
			return;

		OnLbnDblclkListDySound();
	}
	else if( m_list_FE.GetSafeHwnd() == GetFocus()->GetSafeHwnd() )
	{
		if( m_list_FE.GetCurSel() == -1 )
			return;

		OnLbnDblclkListFeList();
	}
	else
	{
		return;
	}
}
