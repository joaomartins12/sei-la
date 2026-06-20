// _DlgSetup.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgSetup.h"


// _DlgSetup 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgSetup, CDialog)

_DlgSetup::_DlgSetup(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgSetup::IDD, pParent)
{

}

_DlgSetup::~_DlgSetup()
{
}

void _DlgSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SLIDER_GIZMO_SENCE, m_GizmoSensor);
	DDX_Control(pDX, IDC_SLIDER_RATE_CAM_HEIGHT2, m_CamAutoMoveHeightRate);
	DDX_Control(pDX, IDC_CHECK_SHOW_COLLISIONBOX, m_Chk_Show_CollisionBox);
	DDX_Control(pDX, IDC_CHECK_RENDEROBJECT, m_check_RenderObject);
	DDX_Control(pDX, IDC_CHECK_RENDER_MONSTERREGION, m_check_RenderMonsterRegion);
	DDX_Control(pDX, IDC_BASIC_COORD, m_CoordBox);
}


BEGIN_MESSAGE_MAP(_DlgSetup, CDialog)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_GIZMO_SENCE, &_DlgSetup::OnNMReleasedcaptureSliderGizmoSence)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_RATE_CAM_HEIGHT2, &_DlgSetup::OnNMReleasedcaptureSliderRateCamHeight2)
	ON_BN_CLICKED(IDC_CHECK_SHOW_COLLISIONBOX, &_DlgSetup::OnBnClickedCheckShowCollisionbox)
	ON_BN_CLICKED(IDC_CHECK_RENDEROBJECT, &_DlgSetup::OnBnClickedCheckRenderobject)
	ON_BN_CLICKED(IDC_CHECK_RENDER_MONSTERREGION, &_DlgSetup::OnBnClickedCheckRenderMonsterregion)
	ON_CBN_SELCHANGE(IDC_BASIC_COORD, &_DlgSetup::OnCbnSelchangeBasicCoord)
END_MESSAGE_MAP()


// _DlgSetup 메시지 처리기입니다.
BOOL _DlgSetup::OnInitDialog()
{
	_DlgBase::OnInitDialog();
	m_GizmoSensor.SetPos(50);
	g_ToolNiNodeMng.OnChangeGizmoEditSense(50);

	float CurrRate = GET_MV_VIEW->GetCurrResetCamHeightRate() * 100.0f;
	m_CamAutoMoveHeightRate.SetPos(CurrRate);

	m_check_RenderObject.SetCheck(TRUE);

	m_CoordBox.AddString(L"World");
	m_CoordBox.AddString(L"Model");
	m_CoordBox.AddString(L"Cam");
	m_CoordBox.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
}
void _DlgSetup::OnNMReleasedcaptureSliderGizmoSence(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	int CurrPos = m_GizmoSensor.GetPos();
	g_ToolNiNodeMng.OnChangeGizmoEditSense(CurrPos);

}

void _DlgSetup::OnNMReleasedcaptureSliderRateCamHeight2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	float fRate = (float)m_CamAutoMoveHeightRate.GetPos() / 100.0f;
	GET_MV_VIEW->UpdateResetCamHeightRate(fRate);

}

void _DlgSetup::OnBnClickedCheckShowCollisionbox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_Chk_Show_CollisionBox.GetCheck() == TRUE)
		g_ObjectMng.ShowCollisionBox(true);
	else
		g_ObjectMng.ShowCollisionBox(false);
}

void _DlgSetup::OnBnClickedCheckRenderobject()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( m_check_RenderObject.GetCheck() == BST_CHECKED )
	{
		if(nsCsGBTerrain::g_pCurRoot)
			nsCsGBTerrain::g_pCurRoot->SetRenderObject_ByTool( true );
	}
	else
	{
		if(nsCsGBTerrain::g_pCurRoot)
			nsCsGBTerrain::g_pCurRoot->SetRenderObject_ByTool( false );
	}
}

void _DlgSetup::OnBnClickedCheckRenderMonsterregion()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_check_RenderMonsterRegion.GetCheck() == TRUE)
	{
		theApp.SetShowMonsterRegion(true);
	}
	else
		theApp.SetShowMonsterRegion(false);

}

void _DlgSetup::OnCbnSelchangeBasicCoord()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int CurSel = m_CoordBox.GetCurSel();
	g_ToolNiNodeMng.OnChangeGizmoCoord(CurSel, &g_ObjectMng);

	GetDlgItem(IDC_BASIC_COORD)->SendMessage(WM_KILLFOCUS, NULL); 
}
