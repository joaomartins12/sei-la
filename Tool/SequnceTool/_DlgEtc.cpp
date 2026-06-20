// _DlgAnimation.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SequnceTool.h"
#include "_DlgAnimation.h"
#include "_DlgEtc.h"


// _DlgAnimation 대화 상자입니다.

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
	DDX_Control(pDX, IDC_COMBO_ACTION, m_combo_Action);
}


BEGIN_MESSAGE_MAP(_DlgEtc, CFormView)
	ON_CBN_SELCHANGE(IDC_COMBO_ACTION, &_DlgEtc::OnCbnSelchangeComboAction)
	ON_BN_CLICKED(IDC_BUTTON_ATT1, &_DlgEtc::OnBnClickedButtonAtt1)
	ON_BN_CLICKED(IDC_BUTTON_ATT2, &_DlgEtc::OnBnClickedButtonAtt2)
	ON_BN_CLICKED(IDC_BUTTON_SKILL1, &_DlgEtc::OnBnClickedButtonSkill1)
	ON_BN_CLICKED(IDC_BUTTON_SKILL3, &_DlgEtc::OnBnClickedButtonSkill3)
	ON_BN_CLICKED(IDC_BUTTON_SKILL2, &_DlgEtc::OnBnClickedButtonSkill2)
	ON_BN_CLICKED(IDC_BUTTON_SKILL4, &_DlgEtc::OnBnClickedButtonSkill4)
	ON_BN_CLICKED(IDC_BUTTON_FT_DIGIMON, &_DlgEtc::OnBnClickedButtonFtDigimon)
	ON_BN_CLICKED(IDC_BUTTON_FT_SKILL, &_DlgEtc::OnBnClickedButtonFtSkill)
END_MESSAGE_MAP()


void _DlgEtc::_Delete()
{

}

void _DlgEtc::_Init()
{
	m_combo_Action.InsertString( 0, L"사용 안함" );
	m_combo_Action.SetItemData( 0, ANI::INVALIDE_ID );
	m_combo_Action.InsertString( 1, L"평타 1" );
	m_combo_Action.SetItemData( 1, ANI::ATTACK_ATTACK1 );
	m_combo_Action.InsertString( 2, L"평타 2" );
	m_combo_Action.SetItemData( 2, ANI::ATTACK_ATTACK2 );

	m_combo_Action.InsertString( 3, L"스킬 1" );
	m_combo_Action.SetItemData( 3, ANI::ATTACK_SKILL1 );
	m_combo_Action.InsertString( 4, L"스킬 2" );
	m_combo_Action.SetItemData( 4, ANI::ATTACK_SKILL2 );
	m_combo_Action.InsertString( 5, L"스킬 3" );
	m_combo_Action.SetItemData( 5, ANI::ATTACK_SKILL3 );
	m_combo_Action.InsertString( 6, L"스킬 4" );
	m_combo_Action.SetItemData( 6, ANI::ATTACK_SKILL4 );

	m_combo_Action.SetCurSel( 0 );

	m_fCheckTime = -1.0f;
	m_dwActionAni = ANI::INVALIDE_ID;
}

void _DlgEtc::OnCbnSelchangeComboAction()
{
	m_dwActionAni = ANI::INVALIDE_ID;
	if( _CheckAction() == false )
		return;

	m_dwActionAni = (DWORD)m_combo_Action.GetItemData( m_combo_Action.GetCurSel() );
	g_pMainWnd->_GetDlgAnimation()->_ResetTarget( g_ViewModel.GetSrcMode() );
	g_pMainWnd->_GetDlgAnimation()->_PlayAni();	

	g_ViewModel.GetSrcMode()->GetProp_Animation()->SetStopFrame( true );
	g_ViewModel.GetDestMode()->GetProp_Animation()->SetStopFrame( true );
}

bool _DlgEtc::_CheckAction()
{
	if( m_combo_Action.GetCurSel() == 0 )
	{
		m_dwActionAni = ANI::INVALIDE_ID;
		return false;
	}

	if( g_ViewModel.GetSrcMode() == NULL )
	{
		// 모델이 지정되야만함
		CsMessageBox( MB_OK, L"모델이 지정되야 미리보기를 할 수 있습니다." );
		m_combo_Action.SetCurSel( 0 );
		OnCbnSelchangeComboAction();
		return false;
	}

	if( g_ViewModel.GetDestMode() == NULL )
	{
		// 타깃지정이 되야 함
		CsMessageBox( MB_OK, L"타겟이 지정되야 미리보기를 할 수 있습니다." );
		m_combo_Action.SetCurSel( 0 );
		OnCbnSelchangeComboAction();
		return false;
	}

	int nSel = m_combo_Action.GetCurSel();
	DWORD dwAni = (DWORD)m_combo_Action.GetItemData( nSel );

	// 해당 애니메이션이 존재 하는가
	if( g_ViewModel.GetSrcMode()->GetProp_Animation()->IsSequence( dwAni ) == false )
	{
		CsMessageBox( MB_OK, L"해당 애니메이션이 존재 하지 않습니다." );
		m_combo_Action.SetCurSel( 0 );
		OnCbnSelchangeComboAction();
		return false;
	}

	// 해당 이벤트 키가 존재 하는가
	if( g_ViewModel.GetSrcMode()->GetProp_Animation()->GetSequenceMng()->GetSequence( dwAni )->s_nNAtt_Count == 0 )
	{
		CsMessageBox( MB_OK, L"해당 애니메이션에 이벤트 키가 존재 하지 않습니다." );
		m_combo_Action.SetCurSel( 0 );
		OnCbnSelchangeComboAction();
		return false;
	}
	return true;
}

void _DlgEtc::_Update()
{	
	if( m_dwActionAni == ANI::INVALIDE_ID )
		return;

	m_fCheckTime -= g_pMainWnd->_GetDlgAnimation()->_GetDeltaTime();
	if( m_fCheckTime < 0.0f )
	{
		switch( m_dwActionAni )
		{
		case ANI::ATTACK_ATTACK1:
		case ANI::ATTACK_ATTACK2:
			{
				CsC_AttackProp::eHIT_TYPE ht = CsC_AttackProp::HT_Normal;
				if( rand()%10 == 0 )
					ht = CsC_AttackProp::HT_Critical;

				nsNETWORK::SetAttackInfo( g_ViewModel.GetSrcMode(), g_ViewModel.GetDestMode(), m_dwActionAni, ht, CsC_AttackProp::DT_Normal );
				m_fCheckTime = g_ViewModel.GetSrcMode()->GetAttackSpeed();
			}
			break;
		case ANI::ATTACK_SKILL1:
		case ANI::ATTACK_SKILL2:
		case ANI::ATTACK_SKILL3:
		case ANI::ATTACK_SKILL4:
			{
				nsNETWORK::SetSkillInfo( g_ViewModel.GetSrcMode(), g_ViewModel.GetDestMode(), m_dwActionAni - ANI::ATTACK_SKILL1 );				
				m_fCheckTime = g_ViewModel.GetSrcMode()->GetProp_Animation()->GetSequenceMng()->GetSequence( m_dwActionAni )->s_fEndTime*1.2f;
			}
			break;
		}		
	}
}


void _DlgEtc::_ReleaseAction()
{
	m_combo_Action.SetCurSel( 0 );
	OnCbnSelchangeComboAction();

	if( g_ViewModel.GetSrcMode() )
	{
		g_ViewModel.GetSrcMode()->GetProp_Attack()->DeleteHDList();
		g_ViewModel.GetSrcMode()->SetPause( CsC_AvObject::PAUSE_ANIMATION, false );

		g_ViewModel.GetSrcMode()->GetProp_Animation()->SetStopFrame( false );
	}
	if( g_ViewModel.GetDestMode() )
	{
		g_ViewModel.GetDestMode()->GetProp_Animation()->SetStopFrame( false );
	}

}
void _DlgEtc::OnBnClickedButtonAtt1()
{
	m_combo_Action.SetCurSel( 1 );
	OnCbnSelchangeComboAction();
}

void _DlgEtc::OnBnClickedButtonAtt2()
{
	m_combo_Action.SetCurSel( 2 );
	OnCbnSelchangeComboAction();
}

void _DlgEtc::OnBnClickedButtonSkill1()
{
	m_combo_Action.SetCurSel( 3 );
	OnCbnSelchangeComboAction();
}

void _DlgEtc::OnBnClickedButtonSkill2()
{
	m_combo_Action.SetCurSel( 4 );
	OnCbnSelchangeComboAction();
}

void _DlgEtc::OnBnClickedButtonSkill3()
{
	m_combo_Action.SetCurSel( 5 );
	OnCbnSelchangeComboAction();
}

void _DlgEtc::OnBnClickedButtonSkill4()
{
	m_combo_Action.SetCurSel( 6 );
	OnCbnSelchangeComboAction();
}

void _DlgEtc::OnBnClickedButtonFtDigimon()
{
	char cPath[ MAX_PATH ];
	nsCsFileTable::g_FileTableMng.GetLanguagePath( nsCsFileTable::KOREA_TRANSLATION, cPath );

	nsCsFileTable::g_pDigimonMng->Delete();
	nsCsFileTable::g_pDigimonMng->Init( cPath );

	CsMessageBox( MB_OK, L"재 로드 성공" );
}

void _DlgEtc::OnBnClickedButtonFtSkill()
{
	char cPath[ MAX_PATH ];
	nsCsFileTable::g_FileTableMng.GetLanguagePath( nsCsFileTable::KOREA_TRANSLATION, cPath );

	nsCsFileTable::g_pSkillMng->Delete();
	nsCsFileTable::g_pSkillMng->Init( cPath );

	CsMessageBox( MB_OK, L"재 로드 성공" );
}
