// _DlgMTMap_BuffRegion.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTMap_BuffRegion.h"

void Tool_BuffRegion::SetBuffRegion(CsMap_BuffRegion* pkBuffRegion)
{
	m_pkBuffRegion = pkBuffRegion;
}
void Tool_BuffRegion::SetObjUniqID(const int UniqID)
{
	ObjectUniqID = UniqID;
	m_pkBuffRegion->GetInfo()->objuniqid = UniqID;
	CString kDot;
	kDot.Format(L"SetUniqID: X[%d]\n ", UniqID);
	OutputDebugString(kDot);
}

// _DlgMTMap_BuffRegion 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTMap_BuffRegion, CDialog)

_DlgMTMap_BuffRegion::_DlgMTMap_BuffRegion(CWnd* pParent /*=NULL*/)
: _DlgBase(_DlgMTMap_BuffRegion::IDD, pParent),
m_CurrToolBuffRegion(NULL),
MaxRadius(10000.0f)
{

}

_DlgMTMap_BuffRegion::~_DlgMTMap_BuffRegion()
{
	if( m_CurrToolBuffRegion != null)
		SAFE_DELETE(m_CurrToolBuffRegion);

}

void _DlgMTMap_BuffRegion::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_BUFFREGION_CENTERX, mETCenterX);
	DDX_Control(pDX, IDC_EDIT_BUFFREGION_CENTERY, mETCenterY);
	DDX_Control(pDX, IDC_EDIT_BUFFREGION_RADIUS, mETRadius);
	DDX_Control(pDX, IDC_EDIT_BUFFREGION_LAYER, mETLayer);
	DDX_Control(pDX, IDC_EDIT_BUFFREGION_BUFFERIDX, mETBuffIdx);
	DDX_Control(pDX, IDC_EDIT_BUFFREGION_REQUIRE_1, mETRequire1);
	DDX_Control(pDX, IDC_EDIT_BUFFREGION_REQUIRE_2, mETRequire2);
	DDX_Control(pDX, IDC_EDIT_BUFFREGION_TIME, mETTime);


	DDX_Control(pDX, IDC_COMBO_APPLYTYPE, mCBApplyTarget);
	DDX_Control(pDX, IDC_SLIDER_BUFFREGION_RADIUS, mSCBuffregionRadius);
	DDX_Control(pDX, IDC_EDIT_BUFF_REGION_NIF, m_RegionBuffResourcename);
	DDX_Control(pDX, IDC_EDIT_OBJECT_UNIQ_ID, mUniqID);
	DDX_Control(pDX, IDC_LIST, mCheckList);
	DDX_Control(pDX, IDC_COMBO_APPLY_DETAIL, mCBApplyDigimonType);
	DDX_Control(pDX, IDC_EDIT_BUFFREGION_ACT, mED_Act);
	DDX_Control(pDX, IDC_EDIT_BUFFREGION_INDEX, mETBuffRegionIndex);
}


BEGIN_MESSAGE_MAP(_DlgMTMap_BuffRegion, CDialog)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_APPLY_BUFFREGION, &_DlgMTMap_BuffRegion::OnBnClickedButtonApplyBuffregion)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_BUFFREGION_RADIUS, &_DlgMTMap_BuffRegion::OnNMReleasedcaptureSliderBuffregionRadius)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_BUFF_REGION_RESOURCE, &_DlgMTMap_BuffRegion::OnBnClickedButtonOpenBuffRegionResource)
	ON_CBN_SELCHANGE(IDC_COMBO_APPLY_DETAIL, &_DlgMTMap_BuffRegion::OnCbnSelchangeComboApplyDetail)
	ON_CBN_SELCHANGE(IDC_COMBO_APPLYTYPE, &_DlgMTMap_BuffRegion::OnCbnSelchangeComboApplytype)
	ON_LBN_SELCHANGE(IDC_LIST, &_DlgMTMap_BuffRegion::OnLbnSelchangeList)
	ON_EN_CHANGE(IDC_EDIT_BUFFREGION_REQUIRE_1, &_DlgMTMap_BuffRegion::OnEnChangeEditBuffregionRequire1)
	ON_EN_CHANGE(IDC_EDIT_BUFFREGION_INDEX, &_DlgMTMap_BuffRegion::OnEnChangeEditBuffregionIndex)
END_MESSAGE_MAP()


// _DlgMTMap_BuffRegion 메시지 처리기입니다.

void _DlgMTMap_BuffRegion::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
BOOL _DlgMTMap_BuffRegion::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	mCBApplyTarget.AddString(L"조건 없음");
	mCBApplyTarget.AddString(L"디지몬 타입");
	mCBApplyTarget.AddString(L"몬스터 처치");
	mCBApplyTarget.AddString(L"퀘스트 완료");
	mCBApplyTarget.AddString(L"아이템 사용");

	mCBApplyDigimonType.AddString(L"디지몬 IDX");
	mCBApplyDigimonType.AddString(L"디지몬 속성");
	mCBApplyDigimonType.AddString(L"진화 유형");

	mCheckList.AddString(L"NO");
	mCheckList.AddString(L"DA");
	mCheckList.AddString(L"VA");
	mCheckList.AddString(L"VI");
	mCheckList.AddString(L"UN");


	mCheckList.ShowWindow(SW_HIDE);
	mCBApplyDigimonType.ShowWindow(SW_HIDE);
	mSCBuffregionRadius.SetPos(50);

	//mCBApplyType.SetScrollRange(0, 100, 10000, TRUE);
	return TRUE;
}
void _DlgMTMap_BuffRegion::OnBnClickedButtonApplyBuffregion()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_CurrToolBuffRegion != NULL)
	{
		CsMap_BuffRegion* pkBuffRegion = m_CurrToolBuffRegion->GetBuffRegion();
		if(pkBuffRegion != NULL)
		{
			pkBuffRegion->GetInfo()->centerx = _Control2Int(&mETCenterX);
			pkBuffRegion->GetInfo()->centery = _Control2Int(&mETCenterY);
			pkBuffRegion->GetInfo()->radius = _Control2Int(&mETRadius);
			pkBuffRegion->GetInfo()->act = _Control2Int(&mED_Act);
			pkBuffRegion->GetInfo()->buffidx = _Control2Int(&mETBuffIdx);
			pkBuffRegion->GetInfo()->applytype = mCBApplyTarget.GetCurSel();
			pkBuffRegion->GetInfo()->requirement_1 = mCBApplyDigimonType.GetCurSel();
			if(pkBuffRegion->GetInfo()->applytype == APPLY_TARGET::AT_DIGIMON){
				if(pkBuffRegion->GetInfo()->requirement_1 == DIGIMON_IDX)
					pkBuffRegion->GetInfo()->requirement_2 =  _Control2Int(&mETRequire1);
				else if(pkBuffRegion->GetInfo()->requirement_1 == DIGIMON_ATT)
					pkBuffRegion->GetInfo()->requirement_2 =  GetDigimonApplyAttType();
				else if(pkBuffRegion->GetInfo()->requirement_1 == DIGIMON_EVOLVE_TYPE)
					pkBuffRegion->GetInfo()->requirement_2 =  GetDigimonApplyEvolType();
			}
			else if(pkBuffRegion->GetInfo()->applytype == APPLY_TARGET::AT_KILLMONSTER){
			}
			else if(pkBuffRegion->GetInfo()->applytype == APPLY_TARGET::AT_COMPLETE_QUEST){
			}
			else if(pkBuffRegion->GetInfo()->applytype == APPLY_TARGET::AT_COMPLETE_QUEST){
			}
			pkBuffRegion->GetInfo()->layerno = _Control2Int(&mETLayer);
			pkBuffRegion->GetInfo()->time = _Control2Int(&mETTime);
			pkBuffRegion->GetInfo()->regionIdx = _Control2Int(&mETBuffRegionIndex);

			CString kPath;
			m_RegionBuffResourcename.GetWindowText(kPath);
			//pkBuffRegion->GetInfo()->modelpath = kPath.GetString();
			pkBuffRegion->GetInfo()->objuniqid = _Control2Int(&mUniqID);
			UpdateModel(m_CurrToolBuffRegion);
			UpdateApplyViewer();
		}
	}
}
void _DlgMTMap_BuffRegion::_PostRenderView()
{
	if(m_CurrToolBuffRegion != NULL)
	{
		CsMap_Buff_Region* pGroup = GET_MV_MAINTAB_DLG->_GetDlgMap()->_GetBuffRegion();
		CsMap_BuffRegion::LIST* plist = pGroup->GetList();
		CsMap_BuffRegion::LIST_IT it = plist->begin();
		for( ; it!=plist->end(); ++it )
		{
			NiPoint2 ptPos = NiPoint2( (float)(*it)->GetInfo()->centerx, (float)(*it)->GetInfo()->centery);
			if(ptPos.x < 0 && ptPos.y < 0)
			{
				//전역버프는 그리지 않습니다
				continue;
			}
			if( m_CurrToolBuffRegion->GetBuffRegion() == (*it) )
				g_ToolNiNodeMng.GetRegion()->Render( ptPos, (float)(*it)->GetInfo()->radius, true );
			else
				g_ToolNiNodeMng.GetRegion()->Render( ptPos, (float)(*it)->GetInfo()->radius, false );
		}
	}
}
void _DlgMTMap_BuffRegion::UpdateBuffRegionInfo(CsMap_BuffRegion* pkBuffRegion)
{
	_Int2Control( &mETCenterX, pkBuffRegion->GetInfo()->centerx );
	_Int2Control( &mETCenterY, pkBuffRegion->GetInfo()->centery );
	_Int2Control( &mETRadius, pkBuffRegion->GetInfo()->radius );
	float fRadius = pkBuffRegion->GetInfo()->radius;
	mSCBuffregionRadius.SetPos( (int)(fRadius/ MaxRadius));
	_Int2Control( &mED_Act, pkBuffRegion->GetInfo()->act );
	_Int2Control( &mETBuffIdx, pkBuffRegion->GetInfo()->buffidx );
	mCBApplyTarget.SetCurSel(pkBuffRegion->GetInfo()->applytype);
	mCBApplyDigimonType.SetCurSel(pkBuffRegion->GetInfo()->requirement_1);
	if(pkBuffRegion->GetInfo()->requirement_1 == DIGIMON_IDX)
	{
		_Int2Control( &mETRequire1, pkBuffRegion->GetInfo()->requirement_2 );
	}
	else if(pkBuffRegion->GetInfo()->requirement_1 == DIGIMON_EVOLVE_TYPE)
	{
		UpdateDigimonEvolType(pkBuffRegion->GetInfo()->requirement_2);
	}
	else if(pkBuffRegion->GetInfo()->requirement_1 == DIGIMON_ATT)
	{
		UpdateDigimonATTType(pkBuffRegion->GetInfo()->requirement_2);
	}
	_Int2Control( &mETLayer, pkBuffRegion->GetInfo()->layerno );
	_Int2Control( &mETTime, pkBuffRegion->GetInfo()->time );
	_Int2Control( &mETBuffRegionIndex, pkBuffRegion->GetInfo()->regionIdx );
	UpdateApplyViewer();

	_Int2Control( &mUniqID, pkBuffRegion->GetInfo()->objuniqid );

}
void _DlgMTMap_BuffRegion::OnAddBuffRegion(CsMap_BuffRegion* pkBuffRegion)
{
	if(m_CurrToolBuffRegion == NULL)
	{
		m_CurrToolBuffRegion= new Tool_BuffRegion();
	}
	if(pkBuffRegion != NULL)
	{
		m_CurrToolBuffRegion->SetBuffRegion(pkBuffRegion);
		m_CurrToolBuffRegion->SetObjUniqID(pkBuffRegion->GetInfo()->objuniqid);
	}
	UpdateBuffRegionInfo(pkBuffRegion);
}
void _DlgMTMap_BuffRegion::OnSelectBuffRegion(CsMap_BuffRegion* pkBuffRegion)
{
	if(m_CurrToolBuffRegion == NULL)
	{
		m_CurrToolBuffRegion= new Tool_BuffRegion();
	}
	if(pkBuffRegion != NULL)
	{
		m_CurrToolBuffRegion->SetBuffRegion(pkBuffRegion);
		m_CurrToolBuffRegion->SetObjUniqID(pkBuffRegion->GetInfo()->objuniqid);
	}
	UpdateBuffRegionInfo(pkBuffRegion);
}
void _DlgMTMap_BuffRegion::__ReleaseControl()
{
	_Int2Control( &mETCenterX, 0);
	_Int2Control( &mETCenterY, 0);
	_Int2Control( &mETRadius, 0 );
	_Int2Control( &mED_Act, 0 );
	_Int2Control( &mETBuffIdx, 0 );
	_Int2Control( &mETRequire1, 0 );
	_Int2Control( &mETRequire2, 0 );
	_Int2Control( &mETTime, 0 );
	_Int2Control( &mETLayer, 0 );
	_Int2Control( &mUniqID, 0 );
	_Int2Control( &mETBuffRegionIndex, 0 );
	mCBApplyTarget.SetCurSel(0);
	UpdateApplyViewer();
}
bool _DlgMTMap_BuffRegion::_IsLButtonDown( CPoint pt )
{
	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
	if( pRecord == NULL )
		return false;

	if(m_CurrToolBuffRegion != NULL)
	{
		CsMap_BuffRegion* pkBuffRegion = m_CurrToolBuffRegion->GetBuffRegion();
		if(pkBuffRegion != NULL)
		{
			pkBuffRegion->GetInfo()->centerx = (int)pRecord->GetIntersection().x;
			pkBuffRegion->GetInfo()->centery = (int)pRecord->GetIntersection().y;
			OnSelectBuffRegion( pkBuffRegion );
			UpdateModel(m_CurrToolBuffRegion);

		}
	}
	return true;
}
void _DlgMTMap_BuffRegion::OnNMReleasedcaptureSliderBuffregionRadius(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	if(m_CurrToolBuffRegion != NULL)
	{
		CsMap_BuffRegion* pkBuffRegion = m_CurrToolBuffRegion->GetBuffRegion();
		if(pkBuffRegion != NULL)
		{
			int Value = mSCBuffregionRadius.GetPos() * 100;
			_Int2Control(&mETRadius,Value);
			pkBuffRegion->GetInfo()->radius = Value;

			UpdateModel(m_CurrToolBuffRegion);
		}
	}
}


void _DlgMTMap_BuffRegion::LoadModel(Tool_BuffRegion* pkToolRegion)
{
// 	CsMap_BuffRegion* pkBuffRegion = pkToolRegion->GetBuffRegion();
// 	if(pkBuffRegion == NULL)
// 		return;
// 	if(pkBuffRegion->GetInfo()->modelpath != L"" && pkToolRegion->GetObjUniqID() < 0 &&
// 		pkBuffRegion->GetInfo()->centerx >= 0 && pkBuffRegion->GetInfo()->centery >= 0 )
// 	{
// 		NiTransform kTr;
// 		kTr.m_Translate.x = pkBuffRegion->GetInfo()->centerx;
// 		kTr.m_Translate.y = pkBuffRegion->GetInfo()->centery;
// 		kTr.m_Translate.z = nsCsGBTerrain::g_pCurRoot->GetHeight(pkBuffRegion->GetInfo()->centerx, pkBuffRegion->GetInfo()->centery);
// 		kTr.m_Rotate = NiMatrix3::IDENTITY;
// 		kTr.m_fScale = pkBuffRegion->GetInfo()->radius / 2000.0f;
// 
// 		USES_CONVERSION;
// 		string FullPath = W2A(pkBuffRegion->GetInfo()->modelpath.c_str());
// 		DWORD OUID = g_ObjectMng.InsertModel(FullPath, kTr);
// 		//CsGBObject* pkGBObj =g_ObjectMng.CreateBuffRegion(kTr.m_Translate, kTr.m_fScale);
// 
// 		OnSelectBuffRegion( pkBuffRegion );
// 		pkToolRegion->SetObjUniqID(OUID);
// 	}
}

void _DlgMTMap_BuffRegion::UpdateModel(Tool_BuffRegion* pkToolRegion)
{
	USES_CONVERSION;
	CsMap_BuffRegion* pkBuffRegion = pkToolRegion->GetBuffRegion();
	if(pkToolRegion->GetObjUniqID() >= 0)
	{
		NiTransform kTr;
		kTr.m_Translate.x = pkBuffRegion->GetInfo()->centerx;
		kTr.m_Translate.y = pkBuffRegion->GetInfo()->centery;
		kTr.m_Translate.z = nsCsGBTerrain::g_pCurRoot->GetHeight(pkBuffRegion->GetInfo()->centerx, pkBuffRegion->GetInfo()->centery);
		kTr.m_Rotate = NiMatrix3::IDENTITY;
		kTr.m_fScale = pkBuffRegion->GetInfo()->radius / 2000.0f;
		DWORD UOID = pkToolRegion->GetObjUniqID();
		g_ObjectMng.SetTransformByOUID(UOID, kTr);
	}
}


void _DlgMTMap_BuffRegion::OnBnClickedButtonOpenBuffRegionResource()
{
// 	if(m_CurrToolBuffRegion != NULL)
// 	{
// 		CsMap_BuffRegion* pkBuffRegion = m_CurrToolBuffRegion->GetBuffRegion();
// 		if(pkBuffRegion != NULL)
// 		{
// 			// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 			CFileDialog fd( TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST  , L"Nif Files (*.Nif)|*.Nif||" );
// 			int iResult = fd.DoModal();
// 			if(iResult == IDOK)
// 			{
// 				CString kFullPath = fd.GetPathName();
// 				int iFirst = kFullPath.Find(L"\\Data\\", 0);
// 				kFullPath.Delete(0,iFirst + 1);
// 				m_RegionBuffResourcename.SetWindowText(kFullPath);
// 				if(pkBuffRegion->GetInfo()->modelpath != kFullPath.GetString())
// 				{
// 					pkBuffRegion->GetInfo()->modelpath = kFullPath;
// 					LoadModel(m_CurrToolBuffRegion);
// 					UpdateApplyViewer();
// 				}
// 			}
// 		}
// 	}
}
void _DlgMTMap_BuffRegion::OnCbnSelchangeComboApplytype()
{
	if(m_CurrToolBuffRegion != NULL)
	{
		CsMap_BuffRegion* pkBuffRegion = m_CurrToolBuffRegion->GetBuffRegion();
		if(pkBuffRegion != NULL)
		{
			pkBuffRegion->GetInfo()->applytype = mCBApplyTarget.GetCurSel();
		}
	}
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateApplyViewer();

}

void _DlgMTMap_BuffRegion::OnLbnSelchangeList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_CurrToolBuffRegion != NULL)
	{
		CsMap_BuffRegion* pkBuffRegion = m_CurrToolBuffRegion->GetBuffRegion();
		if(pkBuffRegion != NULL)
		{
			if(pkBuffRegion->GetInfo()->applytype == AT_DIGIMON)
			{
				if(pkBuffRegion->GetInfo()->requirement_1 == DIGIMON_EVOLVE_TYPE)
				{
					pkBuffRegion->GetInfo()->requirement_2 = GetDigimonApplyEvolType();
				}
				else if (pkBuffRegion->GetInfo()->requirement_1 == DIGIMON_ATT)
				{
					pkBuffRegion->GetInfo()->requirement_2 = GetDigimonApplyAttType();
				}
			}
		}
	}
	UpdateApplyViewer();
}
void _DlgMTMap_BuffRegion::OnCbnSelchangeComboApplyDetail()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if(m_CurrToolBuffRegion != NULL)
	{
		CsMap_BuffRegion* pkBuffRegion = m_CurrToolBuffRegion->GetBuffRegion();
		if(pkBuffRegion != NULL)
		{
			if(pkBuffRegion->GetInfo()->applytype == AT_DIGIMON)
			{
				pkBuffRegion->GetInfo()->requirement_1 = mCBApplyDigimonType.GetCurSel();
			}
		}
	}
	UpdateApplyViewer();
}
int _DlgMTMap_BuffRegion::GetDigimonApplyEvolType()
{
	DWORD DestValue = 0;
	for(int i= 0 ; i < mCheckList.GetCount(); ++i)
	{
		int Check = mCheckList.GetCheck(i);
		if( Check == TRUE)
		{
			if( i == 0)	DestValue |= ET_DIGITAMA;
			else if( i == 1)	DestValue |= ET_INTRAINING;
			else if( i == 2)	DestValue |= ET_ROOKIE;
			else if( i == 3)	DestValue |= ET_CHAMPION;
			else if( i == 4)	DestValue |= ET_ULTIMATE;
			else if( i == 5)	DestValue |= ET_MEGA;
			else if( i == 6)	DestValue |= ET_BUSTERMODE;
			else if( i == 7)	DestValue |= ET_JOINTPROGRESSMODE;
			else if( i == 8)	DestValue |= ET_CAPSULE;
			else if( i == 9)	DestValue |= ET_SPIRIT;
			else if( i == 10)	DestValue |= ET_X_ROOKIE;
			else if( i == 11)	DestValue |= ET_X_CHAMPION;
			else if( i == 12)	DestValue |= ET_X_ULTIMATE;
			else if( i == 13)	DestValue |= ET_X_MEGA;
			else if( i == 14)	DestValue |= ET_X_BUSTERMODE;
			else if( i == 15)	DestValue |= ET_X_JOGRESS;
			else if( i == 16 )  DestValue |= ET_MIX;
		}
	}
	return DestValue;
}
int _DlgMTMap_BuffRegion::GetDigimonApplyAttType()
{
	DWORD DestValue = 0;
	for(int i= 0 ; i < mCheckList.GetCount(); ++i)
	{
		int Check = mCheckList.GetCheck(i);
		if( Check == TRUE)
		{
			if( i == 0)	DestValue |= ET_NO;
			else if( i == 1)	DestValue |= ET_DATA;
			else if( i == 2)	DestValue |= ET_VACCINE;
			else if( i == 3)	DestValue |= ET_VIRUS;
			else if( i == 4)	DestValue |= ET_UNKNOWN;
		}
	}
	return DestValue;

}

void _DlgMTMap_BuffRegion::UpdateDigimonATTType(int Req_2)
{
	mCheckList.SetCheck(0, (Req_2 & ET_NO) ? TRUE : FALSE);
	mCheckList.SetCheck(1, (Req_2 & ET_DATA) ? TRUE : FALSE);
	mCheckList.SetCheck(2, (Req_2 & ET_VACCINE) ? TRUE : FALSE);
	mCheckList.SetCheck(3, (Req_2 & ET_VIRUS) ? TRUE : FALSE);
	mCheckList.SetCheck(4, (Req_2 & ET_UNKNOWN) ? TRUE : FALSE);
}
void _DlgMTMap_BuffRegion::UpdateDigimonEvolType(int Req_2)
{
	//int Check0 = ((Req_2 & ET_DIGITAMA) ? TRUE : FALSE);
	//int Check1 = ((Req_2 & ET_INTRAINING) ? TRUE : FALSE);
	//int Check2 = ((Req_2 & ET_ROOKIE) ? TRUE : FALSE);
	//int Check3 = ((Req_2 & ET_CHAMPION) ? TRUE : FALSE);
	//int Check4 = ((Req_2 & ET_ULTIMATE) ? TRUE : FALSE);
	//int Check5 = ((Req_2 & ET_MEGA) ? TRUE : FALSE);
	//int Check6 = ((Req_2 & ET_BUSTERMODE) ? TRUE : FALSE);
	//int Check7 = ((Req_2 & ET_JOINTPROGRESSMODE) ? TRUE : FALSE);
	//int Check8 = ((Req_2 & ET_CAPSULE) ? TRUE : FALSE);
	//int Check9 = ((Req_2 & ET_SPIRIT) ? TRUE : FALSE);
	//int Check10 = ((Req_2 & ET_X_ROOKIE) ? TRUE : FALSE);
	//int Check11 = ((Req_2 & ET_X_CHAMPION) ? TRUE : FALSE);
	//int Check12 = ((Req_2 & ET_X_ULTIMATE) ? TRUE : FALSE);
	//int Check13 = ((Req_2 & ET_X_MEGA) ? TRUE : FALSE);
	//int Check14 = ((Req_2 & ET_X_BUSTERMODE) ? TRUE : FALSE);
	//int Check15 = ((Req_2 & ET_X_JOGRESS) ? TRUE : FALSE);
	mCheckList.SetCheck(0, (Req_2 & ET_DIGITAMA) ? TRUE : FALSE);
	mCheckList.SetCheck(1, (Req_2 & ET_INTRAINING) ? TRUE : FALSE);
	mCheckList.SetCheck(2, (Req_2 & ET_ROOKIE) ? TRUE : FALSE);
	mCheckList.SetCheck(3, (Req_2 & ET_CHAMPION) ? TRUE : FALSE);
	mCheckList.SetCheck(4, (Req_2 & ET_ULTIMATE) ? TRUE : FALSE);
	mCheckList.SetCheck(5, (Req_2 & ET_MEGA) ? TRUE : FALSE);
	mCheckList.SetCheck(6, (Req_2 & ET_BUSTERMODE) ? TRUE : FALSE);
	mCheckList.SetCheck(7, (Req_2 & ET_JOINTPROGRESSMODE) ? TRUE : FALSE);
	mCheckList.SetCheck(8, (Req_2 & ET_CAPSULE) ? TRUE : FALSE);
	mCheckList.SetCheck(9, (Req_2 & ET_SPIRIT) ? TRUE : FALSE);
	mCheckList.SetCheck(10, (Req_2 & ET_X_ROOKIE) ? TRUE : FALSE);
	mCheckList.SetCheck(11, (Req_2 & ET_X_CHAMPION) ? TRUE : FALSE);
	mCheckList.SetCheck(12, (Req_2 & ET_X_ULTIMATE) ? TRUE : FALSE);
	mCheckList.SetCheck(13, (Req_2 & ET_X_MEGA) ? TRUE : FALSE);
	mCheckList.SetCheck(14, (Req_2 & ET_X_BUSTERMODE) ? TRUE : FALSE);
	mCheckList.SetCheck(15, (Req_2 & ET_X_JOGRESS) ? TRUE : FALSE);
	mCheckList.SetCheck(16, (Req_2 & ET_MIX) ? TRUE : FALSE);
}



void _DlgMTMap_BuffRegion::UpdateApplyViewer()
{
	if(m_CurrToolBuffRegion != NULL)
	{
		CsMap_BuffRegion* pkBuffRegion = m_CurrToolBuffRegion->GetBuffRegion();
		if(pkBuffRegion != NULL)
		{
			mCBApplyTarget.SetCurSel(pkBuffRegion->GetInfo()->applytype);
			mCBApplyDigimonType.SetCurSel(pkBuffRegion->GetInfo()->requirement_1);

			APPLY_TARGET CurrApplyTarget = (APPLY_TARGET)mCBApplyTarget.GetCurSel();
			switch(CurrApplyTarget)
			{
			case AT_DIGIMON:
				mCBApplyDigimonType.ShowWindow(SW_SHOW);
				break;
			case AT_KILLMONSTER:
			case AT_COMPLETE_QUEST:
			case AT_USE_ITEM:
			default:
				mCBApplyDigimonType.ShowWindow(SW_HIDE);
				break;
			}

			APPLY_DIGIMON_TYPE eApplyDigimonType = (APPLY_DIGIMON_TYPE)mCBApplyDigimonType.GetCurSel();

			switch(eApplyDigimonType)
			{
			case DIGIMON_ATT:
				{
					mCheckList.ShowWindow(SW_SHOW);
					mETRequire1.ShowWindow(SW_HIDE);

					mCheckList.ResetContent();
					mCheckList.AddString(L"NO");
					mCheckList.AddString(L"DA");
					mCheckList.AddString(L"VA");
					mCheckList.AddString(L"VI");
					mCheckList.AddString(L"UN");

					UpdateDigimonATTType(pkBuffRegion->GetInfo()->requirement_2);
				}
				break;
			case DIGIMON_EVOLVE_TYPE:
				{
					mCheckList.ShowWindow(SW_SHOW);
					mETRequire1.ShowWindow(SW_HIDE);
					mCheckList.ResetContent();
					mCheckList.AddString(L"DIGITAMA");
					mCheckList.AddString(L"INTRAINING");
					mCheckList.AddString(L"ROOKIE");
					mCheckList.AddString(L"CHAMPION");
					mCheckList.AddString(L"ULTIMATE");
					mCheckList.AddString(L"MEGA");
					mCheckList.AddString(L"BUSTERMODE");
					mCheckList.AddString(L"JOINTPROGRESSMODE");
					mCheckList.AddString(L"CAPSULE");
					mCheckList.AddString(L"SPIRIT");
					mCheckList.AddString(L"X_ROOKIE");
					mCheckList.AddString(L"X_CHAMPION");
					mCheckList.AddString(L"X_ULTIMATE");
					mCheckList.AddString(L"X_MEGA");
					mCheckList.AddString(L"X_BUSTERMODE");
					mCheckList.AddString(L"X_JOGRESS");
					mCheckList.AddString(L"MIX");

					UpdateDigimonEvolType(pkBuffRegion->GetInfo()->requirement_2);
				}
				break;
			case DIGIMON_IDX:
				{
					mCheckList.ShowWindow(SW_HIDE);
					mETRequire1.ShowWindow(SW_SHOW);
				}
				break;	
			default:
				{
					mCheckList.ShowWindow(SW_HIDE);
					mETRequire1.ShowWindow(SW_HIDE);
				}
				break;
			}


		}
	}


}

void _DlgMTMap_BuffRegion::OnEnChangeEditBuffregionRequire1()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// _DlgBase::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void _DlgMTMap_BuffRegion::OnEnChangeEditBuffregionIndex()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// _DlgBase::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int Inputed = _Control2Int(&mETBuffRegionIndex);

	int a = 10;
}
