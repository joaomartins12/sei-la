#pragma once
#include "afxwin.h"
#include "afxcmn.h"

namespace BUFF_REGION
{


}

enum BUFF_REGION_APPLYTYPE_EVOLUTION{
	ET_DIGITAMA					= 0x00000001,
	ET_INTRAINING				= 0x00000002,
	ET_ROOKIE					= 0x00000004,
	ET_CHAMPION					= 0x00000008,
	ET_ULTIMATE					= 0x00000010,
	ET_MEGA						= 0x00000020,
	ET_BUSTERMODE				= 0x00000040,
	ET_JOINTPROGRESSMODE		= 0x00000080,
	ET_CAPSULE					= 0x00000100,
	ET_SPIRIT					= 0x00000200,
	ET_X_ROOKIE					= 0x00000400,
	ET_X_CHAMPION				= 0x00000800,
	ET_X_ULTIMATE				= 0x00001000,
	ET_X_MEGA					= 0x00002000,
	ET_X_BUSTERMODE				= 0x00004000,
	ET_X_JOGRESS				= 0x00008000,
	ET_MIX						= 0x00010000,
	ET_MAX,
};

enum BUFF_REGION_APPLYTYPE_ATTRIBUTE{
	ET_NO							= 0x00000001,
	ET_DATA							= 0x00000002,
	ET_VACCINE						= 0x00000004,
	ET_VIRUS							= 0x00000008,
	ET_UNKNOWN					= 0x00000010,
};

enum APPLY_TARGET
{
	AT_ANIWAY,
	AT_DIGIMON,
	AT_KILLMONSTER,
	AT_COMPLETE_QUEST,
	AT_USE_ITEM,
	AT_END,
};

enum APPLY_DIGIMON_TYPE
{
	DIGIMON_IDX,
	DIGIMON_ATT,
	DIGIMON_EVOLVE_TYPE,
};
// _DlgMTMap_BuffRegion 대화 상자입니다.
class Tool_BuffRegion
{
public:
	void SetBuffRegion(CsMap_BuffRegion* pkBuffRegion);
	void SetObjUniqID(const int UniqID);

	CsMap_BuffRegion* GetBuffRegion(void){return m_pkBuffRegion;}
	int GetObjUniqID(void){return ObjectUniqID;}

private:
	CsMap_BuffRegion* m_pkBuffRegion;
	int ObjectUniqID;

public:
	Tool_BuffRegion()
		:m_pkBuffRegion(NULL),
		ObjectUniqID(-1)
	{


	}
	~Tool_BuffRegion()
	{
		m_pkBuffRegion = NULL;
		ObjectUniqID = 0;
	}
};
class _DlgMTMap_BuffRegion : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTMap_BuffRegion)

public:
	_DlgMTMap_BuffRegion(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTMap_BuffRegion();

	bool _IsLButtonDown(CPoint kPt);
	virtual void	_PostRenderView();
	CsMap_BuffRegion* GetBuffRegion(void)
	{
		if(m_CurrToolBuffRegion != NULL)
			return m_CurrToolBuffRegion->GetBuffRegion();

		return NULL;
	}
	

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_MAP_BUFF_REGION };

protected:
	void ReleaseControls(void);
	void __ReleaseControl();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonApplyBuffregion();
	afx_msg void OnNMReleasedcaptureSliderBuffregionRadius(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonOpenBuffRegionResource();
	afx_msg void OnCbnSelchangeComboApplyDetail();
	afx_msg void OnCbnSelchangeComboApplytype();
	afx_msg void OnLbnSelchangeList();
	afx_msg void OnEnChangeEditBuffregionRequire1();

	void LoadModel(Tool_BuffRegion* pkBuffRegion);
	void UpdateModel(Tool_BuffRegion* pkBuffRegion);	
	void OnSelectBuffRegion(CsMap_BuffRegion* pkBuffRegion);
	void UpdateBuffRegionInfo(CsMap_BuffRegion* pkBuffRegion);
	void OnAddBuffRegion(CsMap_BuffRegion* pkBuffRegion);
	void UpdateDigimonEvolType(int Req_2);
	void UpdateDigimonATTType(int Req_2);
	void UpdateApplyViewer();
	int GetDigimonApplyEvolType();
	int GetDigimonApplyAttType();
private:
	Tool_BuffRegion* m_CurrToolBuffRegion;
	CsEditFosus mETCenterX;
	CsEditFosus mETCenterY;
	CsEditFosus mETRadius;
	CsEditFosus mED_Act;
	CsEditFosus mETBuffIdx;
	CsEditFosus mETRequire1;
	CsEditFosus mETRequire2;
	CsEditFosus mETTime;
	CsEditFosus mETLayer;
	CsEditFosus mUniqID;

	float MaxRadius;
	CComboBox mCBApplyTarget;
	CSliderCtrl mSCBuffregionRadius;
	CEdit m_RegionBuffResourcename;
	CCheckListBox mCheckList;
	CComboBox mCBApplyDigimonType;

public:
	CsEditFosus mETBuffRegionIndex;
	afx_msg void OnEnChangeEditBuffregionIndex();
};
