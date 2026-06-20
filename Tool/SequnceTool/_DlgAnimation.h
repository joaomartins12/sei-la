#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// _DlgAnimation 대화 상자입니다.

class _DlgAnimation : public CFormView
{
	DECLARE_DYNCREATE(_DlgAnimation)


	enum { INVALIDE_LISTID = (DWORD)(-1), };


protected:
	bool					m_bReadInfo;
	CsC_AvObject*			m_pTarget;
	CsC_AniProp_Sequence*	m_pCsSequenceMng;

	float					m_fDeltaTime;
	float					m_fAniDeltaTime;

public:
	void			_Delete();
	void			_Init();

	void			_Update();

	float			_GetDeltaTime();
	float			_GetAniTime();

	DWORD			_GetSelAniID(){ return m_dwSelectSequenceInfo; }
	CsC_AvObject*	_GetTarget() { return m_pTarget; }

	void			_ReleaseTarget();
	void			_ResetTarget( CsC_AvObject* pActor );
	bool			_IsLoop(){ return ( m_chk_LoopPlay.GetCheck() == BST_CHECKED ); }
	void			_PlayAni(){ m_chk_Play.SetCheck( BST_CHECKED ); }

	bool			_IsPlanView(){ return ( m_chk_PlanView.GetCheck() == BST_CHECKED ); } 
	bool			_IsUseOffset(){ return ( m_chk_Offset.GetCheck() == BST_CHECKED ); } 
	bool			_IsViewOffset(){ return ( m_chk_ViewOffset.GetCheck() == BST_CHECKED ); } 

protected:
	void			__ReadTargetInfo();
	void			__WriteSequenceList( bool bSelectExist = false );
	void			__ResetLoopCount();



protected:
	void			__ListDeleteAllItem();




protected:
	CsC_LoadSequence::sSEQUENCE_INFO*					m_pSelectSequenceInfo;
	DWORD												m_dwSelectSequenceInfo;
	DWORD												m_dwSelectEventInfo;	


protected:
	void			__EventDeleteAllItem();
	void			__ResetEventList();

	void			__WriteEventList();

	void			__ChangeEvent();
	void			__SetEventControl();


	void			__EnableOffset( BOOL bEnable );	
	void			__SetOffset( NiPoint3 vOffset );
	void			__ChangeOffset();
	
	NiPoint3		__GetEffect0();
	void			__SetEffect0( NiPoint3 v );
	void			__ChangeEffect0();

	NiPoint3		__GetEffect1();
	void			__SetEffect1( NiPoint3 v );
	void			__ChangeEffect1();

	void			__SetShowControl_Eff0_Eff1( int nCmdShow );

	void			__ChangeEffectScale();


protected:
	void			__ShaderDeleteAllItem();
	void			__ResetShaderList();

	void			__WriteShaderList();

	
public:
	NiPoint3		_GetOffset();

protected:
	_DlgAnimation();   // 표준 생성자입니다.
	virtual ~_DlgAnimation();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_ANIMATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnOK(){}
	virtual void OnCancel(){}

protected:
	CListCtrl m_list_Sequence;
	CListCtrl m_list_Event;
	CSliderCtrl m_sld_Time;
	CEdit m_edit_Time;
	CButton	m_chk_Play;
	CButton m_btn_SetTime;

	CButton m_rdo_Loop;
	CButton m_rdo_Once;	
	CButton m_chk_CharPos;
	CButton m_chk_GameScale;
	CButton m_chk_Offset;

	CsEdit m_edit_Speed;
	CsSpinCtrl m_spin_Speed;

	CsEdit m_edit_OffsetX;
	CsSpinCtrl m_spin_OffsetX;
	CsEdit m_edit_OffsetY;
	CsSpinCtrl m_spin_OffsetY;
	CsEdit m_edit_OffsetZ;
	CsSpinCtrl m_spin_OffsetZ;
	CsEdit m_edit_EffectScale;
	CsSpinCtrl m_spin_EffectScale;

	CsEdit m_edit_Eff0X;
	CsSpinCtrl m_spin_Eff0X;
	CsEdit m_edit_Eff0Y;
	CsSpinCtrl m_spin_Eff0Y;
	CsEdit m_edit_Eff0Z;
	CsSpinCtrl m_spin_Eff0Z;

	CsEdit m_edit_Eff1X;
	CsSpinCtrl m_spin_Eff1X;
	CsEdit m_edit_Eff1Y;
	CsSpinCtrl m_spin_Eff1Y;
	CsEdit m_edit_Eff1Z;
	CsSpinCtrl m_spin_Eff1Z;

	CsEdit m_edit_FadeTime;
	CsSpinCtrl m_spin_FadeTime;
	
public:
	afx_msg void OnNMClickListSequence(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnNMClickListEvent(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSettime();
	afx_msg void OnBnClickedButtonNewEvent();
	afx_msg void OnBnClickedRadioLoop();
	afx_msg void OnBnClickedRadioOnce();
	afx_msg void OnEnChangeEditSpeed();
	afx_msg void OnBnClickedButtonDelEvent();
protected:
	CButton m_chk_LoopPlay;
public:
	afx_msg void OnLvnItemchangedListSequence(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListEvent(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckUseoffset();
	afx_msg void OnEnChangeEditOffsetX();
	afx_msg void OnEnChangeEditOffsetY();
	afx_msg void OnEnChangeEditOffsetZ();
	afx_msg void OnBnClickedCheckCharPos();
	afx_msg void OnBnClickedCheckGameScale();	
	afx_msg void OnEnChangeEditEffectscale();
	afx_msg void OnEnChangeEditEff0X();
	afx_msg void OnEnChangeEditEff0Y();
	afx_msg void OnEnChangeEditEff0Z();
	afx_msg void OnEnChangeEditEff1X();
	afx_msg void OnEnChangeEditEff1Y();
	afx_msg void OnEnChangeEditEff1Z();
	afx_msg void OnEnChangeEditFadeTime();
protected:
	CComboBox m_combo_StartPos;
public:
	afx_msg void OnCbnSelchangeComboStartpos();
protected:
	CButton m_chk_PlanView;
	CButton m_chk_Plan_TwoSide;
	CButton m_chk_Plan_Wire;
public:
	afx_msg void OnBnClickedCheckPlanTowside();
	afx_msg void OnBnClickedCheckPlanWire();
protected:
	CButton m_chk_ViewOffset;
	CsEdit m_edit_LoopCnt;
public:
	afx_msg void OnEnChangeEditAniloop();
	CComboBox m_EventType;
	afx_msg void OnCbnSelchangeComboEventtype();
	CComboBox m_ComboEventDel;
	afx_msg void OnCbnSelchangeComboDel();
protected:
	CComboBox m_cmb_EffLevel;
public:
	afx_msg void OnCbnSelchangeComboEffLevel();
	CComboBox m_Combo_Dir;
	afx_msg void OnCbnSelchangeComboRot();
	afx_msg void OnBnClickedButtonDelEffect();
protected:
	CListCtrl m_list_Shader;
public:
	afx_msg void OnBnClickedButtonNewShader();
	afx_msg void OnBnClickedButtonDelShader();
	afx_msg void OnNMDblclkListShader(NMHDR *pNMHDR, LRESULT *pResult);
};


