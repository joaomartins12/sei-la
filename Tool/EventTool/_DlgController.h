#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// _DlgController 폼 뷰입니다.

class _DlgController : public CFormView
{
	DECLARE_DYNCREATE(_DlgController)
public:
	void	_Init();
	void	_Update();
	void	_Render();
	void	_Delete();
	void	_ResetAll();
	bool	CreateEventData();

	void	_SetAniList();
	void	_SetTarGet( CsC_AvObject* pTarget );
	void	_SetEventTime( float fStartTime, float fEndTime );

	void	_SetPlayListSort();
	void	_SetAniEvent( DWORD dwAniId );
	void	_SetMoveEvent( NiPoint3 vPos );
	float	_GetDist( NiPoint3 vPos );
	void	_SetTalkEvent( DWORD dwTalkNum );
	void	_SetSoundEvent( CString strFile, CEventData::sEventSound_Info::eEventSound eType );
	void	_SetEffectEvent( DWORD dwEffectNum );	
	void	_SetStartCharEvent();

	void	_SetPlayListReset() { m_ListEventPlay.DeleteAllItems(); }

	bool	_IsPlay(){ return ( m_Check_Play.GetCheck() == BST_CHECKED ); }
	void	_SetPlay( bool bPlay ); 
	void	_SetEventButton( bool bEnable ) { GetDlgItem( IDC_SETEVENT )->EnableWindow( bEnable ); }
	bool	_IsArea(){ return ( m_Check_AreaSet.GetCheck() == BST_CHECKED ); }	
	//bool	_IsPlayList(){ return ( m_Check_PlayList.GetCheck() == BST_CHECKED ); }
	bool	_IsDirectPlay(){ return ( m_CheckPlay_Direct.GetCheck() == BST_CHECKED ); }
	bool	_IsChrCamera() { return ( m_CheckChrCamera.GetCheck() == BST_CHECKED ); }
	void	_SetChrCamera( bool bUse ) { bUse == true ? m_CheckChrCamera.SetCheck( BST_CHECKED ) : m_CheckChrCamera.SetCheck( BST_UNCHECKED );}	
	bool	_IsOnlyArea(){ return ( m_CheckOnlyArea.GetCheck() == BST_CHECKED ); }	
	void	_SetOnlyArea( bool bUse ) { bUse == true ? m_CheckOnlyArea.SetCheck( BST_CHECKED ) : m_CheckOnlyArea.SetCheck( BST_UNCHECKED );	}
	bool	_IsConLoop(){ return ( m_CheckConLoop.GetCheck() == BST_CHECKED ); }
	void	_SetConLoop( bool bUse ) { bUse == true ? m_CheckConLoop.SetCheck( BST_CHECKED ) : m_CheckConLoop.SetCheck( BST_UNCHECKED ); }
	bool	_IsUseCamera(){ return ( m_Check_EventCamera.GetCheck() == BST_CHECKED ); }

	void	_SetInsertListPlayer( DWORD dwID, CString str );
	void	_SetSceneNum( DWORD dwID ) { m_nSceneNum = (UINT)dwID; }
	void	_NewEventChar( DWORD dwModelID );
	void	_SetEffectPos( NiPoint3 vPos );

	CsC_AvObject*	_GetTarget() { return m_pTarget; }
	int		_GetType() { return m_nType; }
	bool	_GetNewEvent() { return m_bNewObjectLoad;  }
	void	_SetNewEvent( bool bNew ) { m_bNewObjectLoad = bNew; }
	void	_SetNewPos( NiPoint3 vpos ) { m_vPos = vpos; }
	NiPoint3 _GetNewPos() { return m_vPos; }
	void	_SetNewRot( float fRot ) { m_fRot = fRot; }	
	float   _GetNewRot() { return m_fRot; }
	float	_GetAreaRad() { return nsCsMfcFunc::Control2Float( &m_EditAreaRad ); }

	void	_SetEventMoveSpeed( float fSpeed ) { m_fMoveSpeed = fSpeed; }

	//이벤트 수정용 설정 함수
	void	_SetEventModify( DWORD dwEventID );
	void	_SetAniEvent( DWORD dwAniId, float fTime );
	void	_SetMoveEvent( NiPoint3 vPos, float fTime, float fMoveSpeed );
	void	_SetTalkEvent( DWORD dwTalkNum, float fTime );
	void	_SetSoundEvent( CString strFile, float fTime, CEventData::sEventSound_Info::eEventSound eType );
	void	_SetEffectEvent( DWORD dwEffectNum, float fTime );
	void	_NewEventChar( DWORD dwModelID, NiPoint3 vPos, float vRot );	

 
	float	m_fTime; //현재 이벤트시간
	float	m_fEndTime; //이벤트 총시간
	float	m_fStartTime; //이벤트 시작시간 (딜레이 타임) 툴에선 적용안함	
	bool	m_bSetArea; //영역을 사용할것인가?
	bool	m_bArea;		//영역지정 사용중인가?
	bool	m_bMoveEvent;	//위치이동 이벤트사용중인가?	
	bool	m_bNewObjectLoad;	//신규이벤트 로드중인가?
	bool	m_bCharCamera;	//캐릭터 카메라모드 사용
	bool	m_bEffectPos;	//이펙트 위치 사용
	
	bool	m_bMouseMove;
	UINT	m_nSceneNum;

protected:
	int		m_nType;		

	CsC_AvObject*	m_pTarget;	//현재 이벤트의 주체	
	NiPoint3		m_vPos;		//신규 이벤트시 초기위치
	float			m_fRot;		//신규 이벤트시 초기회전값

	float			m_fMoveSpeed; //이벤트 주체 이동이벤트시 움직이는속도

protected:
	_DlgController();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~_DlgController();

public:
	enum { IDD = IDD_CONTROL };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMapload();
	afx_msg void OnBnClickedInsertEvent();
	afx_msg void OnBnClickedInsertEvent2();
	afx_msg void OnLvnItemchangedListAnimation(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListAnimation(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnBnClickedSettime();
	afx_msg void OnBnClickedListdelete();
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedSetevent();
	afx_msg void OnBnClickedCheckPlay();	
	afx_msg void OnNMCustomdrawSliderEventtime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedMoveevent();
	afx_msg void OnBnClickedEventtalk();
	afx_msg void OnLvnItemchangedListEventplay(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSetarea();
	afx_msg void OnBnClickedCheckSetarea();
	afx_msg void OnBnClickedEventsave();
	afx_msg void OnBnClickedEventload();
	afx_msg void OnBnClickedCheckListplay();
	afx_msg void OnBnClickedEventplayer();
	afx_msg void OnBnClickedEventtalkload();	
	afx_msg void OnBnClickedEventpaste();
	afx_msg void OnNMRclickListEventplay(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditEventscenenum();
	afx_msg void OnBnClickedCheckUsecon();
	afx_msg void OnEnChangeEditEventnum();
	afx_msg void OnBnClickedEventsavetemp();
	afx_msg void OnBnClickedEventsound();
	afx_msg void OnBnClickedEventeffect();
	afx_msg void OnBnClickedCheckFreemove();	
	afx_msg void OnBnClickedGetpos();
	afx_msg void OnBnClickedSceneDel();	
	afx_msg void OnBnClickedCheckDirectplay();	
	afx_msg void OnLbnSelchangeListPlayer();
	afx_msg void OnEnChangeEditZdelta();
protected:	
	CSliderCtrl m_SldEventtime;
	CButton m_Check_Play;
	CEdit m_EditTime;
	CEdit m_EditStartTime;

	CListCtrl m_List_Animation;
	CListCtrl m_ListEventPlay;
		
	CsEdit m_EditEventNum;
	CEdit m_EditEventTitle;

	CButton m_Check_AreaSet;
	CsSpinCtrl m_SpinAreaRad;
	CsEdit m_EditAreaRad;	
	CButton m_btArea;

	CListBox m_List_Player;
	
	CsEdit m_EditSceneNum;	

	CButton m_CheckChrCamera;
	CEdit m_EditEventCount;
	CButton m_CheckOnlyArea;
	CButton m_CheckConLoop;
	
	CEdit m_EditCharName;

	CsEdit m_EditScale;
	CsSpinCtrl m_SpinScale;

	CsEdit m_EditEventNumUse;

	CListBox m_List_Scene;

	CEdit m_Edit_Pos;

	CButton m_Check_EventCamera;
	CEdit m_Edit_TargetPos;

	CButton m_CheckPlay_Direct;

	CsEdit m_Edit_ZOffset;
	CsSpinCtrl m_Spin_ZOffset;
	
public:
	afx_msg void OnBnClickedSetcameraButton();
	afx_msg void OnEnChangeEditScale();
};


