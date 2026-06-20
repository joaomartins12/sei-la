#pragma once
#include "afxwin.h"
#include "cseditfocus.h"


// _DlgMTMap 대화 상자입니다.

class _DlgMTMap : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTMap)

	// =================================================================================================
	//
	//		virtual
	//
	// =================================================================================================
public:
	virtual void		_PreSave( const char* cFile, bool bAutoSave );
	virtual void		_PostRenderView();
	virtual void		_ViewInit();
	virtual void		_Delete();
	virtual void		_PreViewReset();
	virtual void		_PostViewReset();
	virtual void		_OnLButtonDown( CPoint pt );
	

	// =================================================================================================
	//
	//		Base
	//
	// =================================================================================================

	// =================================================================================================
	//
	//		list
	//
	// =================================================================================================
//public:
// 	enum eMAP_ID
// 	{
// 		MI_NONE						= 0,
// 		MI_DOTS						= 1,
// 		MI_DTERMINAL				= 2,
// 		MI_DATSTEST					= 3,
// 		MI_DATSUNDERGROUND			= 4,
// 		MI_COLOSSEUM				= 5,
// 		MI_GM_ROOM					= 10,
// 		MI_DTERMINAL_MAZE_1F		= 11,
// 		MI_DTERMINAL_MAZE_2F		= 12,
// 		MI_DTERMINAL_MAZE_3F		= 13,
// 		MI_GM_ROOM2					= 14,
// 		MI_YOKOHAMA					= 100,
// 		MI_RT1_FIELD1_1				= 101,
// 		MI_RT1_FIELD1_2				= 102,
// 		MI_RT1_FIELD1_3				= 103,
// 		MI_RT1_FIELD1_4				= 104,
// 		MI_YOKOHAMA_PT2				= 105,
// 		MI_YOKOHAMA_EAST			= 110,
// 		MI_RT1_FIELD2_1				= 111,
// 		MI_RT1_FIELD2_2				= 112,
// 		MI_RT1_FIELD2_3				= 113,		
// 		MI_CAMPGROUND				= 150,
// 		MI_HIKARIGAOKA				= 151,
// 		MI_SHIBAURA					= 152,
// 		MI_ODAIBA					= 153,
// 		MI_RAINBOW_BRIDGE			= 154,
// 		MI_RAINBOW_BRIDGE_HERO		= 155,
// 		MI_WESTTOWN1				= 1100,
// 		MI_WEST_BATTLE1				= 1101,
// 		MI_DIGIMON_FORM1			= 1102,
// 		MI_WEST_BATTLE2				= 1103,
// 		MI_WEST_BATTLE3				= 1104,
// 		MI_WESTTOWN_EAST			= 1105,
// 		MI_WESTTOWN_OUT				= 1106,
// 		MI_WESTTOWN_WEST			= 1107,
// 		MI_DIGIMON_MAZE				= 1108,
// 		MI_WEST_DARKTOWER			= 1109,
// 		MI_KAISER_LABORATORY_NORMAL	= 1110,
// 		MI_KAISER_LABORATORY_HERO	= 1111,
// 		MI_DIGIMON_MAZE_B2			= 1201,
// 		MI_DIGIMON_MAZE_B1			= 1202,		
// 		MI_DIGIMON_MAZE_F1			= 1203,	
// 		MI_DIGIMON_MAZE_F2			= 1204,	
// 		MI_DIGIMON_MAZE_F3			= 1205,
// 		MI_DIGIMON_MAZE_F4			= 1206,		
// 		FL_ISLAND_STARTVILLAGE		= 1300,
// 		FL_SILVER_LAKE				= 1301,
// 		FL_SLIENT_FOREST			= 1302,
// 		FL_LOST_TEMPLE				= 1303,
// 		FL_IDG_LOST_TEMPLE			= 1304,
// 		FL_FILE_ISLAND				= 1305,
// 		FL_MT_INFINITY				= 1306,
// 		FL_DG_INFINITY_				= 1307,
// 		FL_IDG_INFINITY_			= 1308,
// 		CW_SERVER_PANGAEA			= 1400,
// 		CW_SERVER_CANYON			= 1401,
// 		CW_SERVER_PYRAMID			= 1402,
// 		CW_SERVER_PYRAMID_NORMAL	= 1403,		// 데이터몬 미궁 일반
// 		CW_SERVER_PYRAMID_HERO		= 1404,		// 데이터몬 미궁 영웅
// 		CW_SERVER_PYRAMID_EVENT		= 1405,		// 데이터몬 미궁 이벤트
// 		CW_GREEN_ZONE				= 1500,
// 		CW_GREEN_ZONE_TOWN			= 1501,
// 		XUANWUMON_DUNGEON			= 1600,		// 현무몬 인던
// 		Zhuqiaomon_DUNGEON			= 1601,		// 주작몬 인던
// 		Baihumon_DUNGEON			= 1602,		// 백호몬 인던
// 		Qinglongmon_DUNGEON			= 1603,		// 청룡몬 인던
// 		MI_ICE_TOWN					= 2100,
// 		MI_ICE_1					= 2101,
// 		MI_ICE_2					= 2102,
// 		MI_ICE_3					= 2103,
// 		MI_ICE_4					= 2104,
// 		MI_BATTLE_S_1				= 9101,
// 		MI_BATTLE_S_2				= 9102,
// 		MI_BATTLE_S_3				= 9103,
// 		BATTLE_ZONE_A08				= 9104,
// 		BATTLE_ZONE_A16				= 9105,
// 		MI_BATTLE_G_1				= 9201,
// 		MI_BATTLE_G_2				= 9202,
// 		MI_BATTLE_G_3				= 9203,
// 		HOLY_POINT					= 9301,
// 		MI_TESTMAP_1				= 10000,
// 		MI_TESTMAP_2				= 10001,
// 	};
protected:	
	CsMapList*				m_pCurList;
	CsMapListMng			m_MapListFT;
public:
	void			SetCurList( CsMapList* pMapList );
	//	MapID
protected:
	//void			__InitMapIDCombo( CComboBox* pComboBox );
	void			__ResetComboMapID( DWORD dwMapID );
	void			__ReadBGSound();

	// =================================================================================================
	//
	//		Uniq Object
	//
	// =================================================================================================
public:
	enum eUNIQOBJ_TYPE
	{
		UT_NONE		= 0,
		UT_START	= 1,
		UT_REGION	= 2,
		UT_RESURRECTION = 3,
		UT_LIMIT_EVO	= 4,
		UT_BUFF_REGION = 5,
	};

	// =================================================================================================
	//
	//		Start
	//
	// =================================================================================================

protected:
	CsMapStartGroup		m_StartGroup;
	_DlgMTMap_Start		m_DlgStart;
	
protected:
	void				__EnableUniqObj( bool bEnable );
public:
	void				_ResetUniqObjList_ForStart();
	CsMapStartGroup*	_GetStartGroup(){ return &m_StartGroup; }


	// =================================================================================================
	//
	//		Region
	//
	// =================================================================================================
protected:
	CsMapRegionGroup	m_RegionGroup;
	_DlgMTMap_Region	m_DlgRegion;

public:
	void				_ResetUniqObjList_ForRegion();
	CsMapRegionGroup*	_GetRegionGroup(){ return &m_RegionGroup; }

	// =================================================================================================
	//
	//		Resurrection
	//
	// =================================================================================================
protected:
	CsMapResurrectionGroup	m_ResurrectionGroup;
	_DlgMTMap_Resurrection	m_DlgResurrection;

public:
	void				_ResetUniqObjList_ForResurrection();
	CsMapResurrectionGroup*	_GetResurrectionGroup(){ return &m_ResurrectionGroup; }	

	// =================================================================================================
	//
	//		Limit Evo
	//
	// =================================================================================================
protected:
	CsMap_LimitEvoGroup		m_LimitEvoGroup;
	_DlgMTMap_LimitEvo		m_DlgLimitEvo;

public:
	void					_ResetUniqObjList_ForLimitEvo();
	CsMap_LimitEvoGroup*	_GetLimitEvGoroup(){ return &m_LimitEvoGroup; }		

	// =================================================================================================
	//
	//		Buffer Region
	//
	// =================================================================================================
protected:
	CsMap_Buff_Region		m_BuffRegion;
	_DlgMTMap_BuffRegion m_DlgBuffRegion;
public:
	void					_ResetUniqObjList_ForBuffRegion();
	CsMap_Buff_Region*	_GetBuffRegion(){ return &m_BuffRegion; }		

public:
	_DlgMTMap(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTMap();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_MAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CComboBox m_combo_MapID;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
protected:
	CsEditFosus m_edit_MapName;
	CsEditFosus m_edit_MapDescription;
public:
	afx_msg void OnCbnSelchangeComboMapid();
protected:
	CComboBox m_combo_UniqObj;
	CListBox m_list_UniqObj;
public:
	afx_msg void OnCbnSelchangeComboUniqObj();
	afx_msg void OnBnClickedButtonAddUniqobj();
protected:
	CButton m_btn_AddUniqObj;
public:
	afx_msg void OnLbnSelchangeListUniqObj();
	afx_msg void OnBnClickedButtonDelUniqobj();
	afx_msg void OnEnChangeEditMapDescription();
	afx_msg void OnEnChangeEditMapDescriptionEng();
	afx_msg void OnBnClickedButtonLoadbgsound();
protected:
	CEdit m_edit_MapBGSound;
public:
	afx_msg void OnBnClickedButtonDelBgsound();
protected:
	CComboBox m_combo_Resurrection;
public:
	afx_msg void OnCbnSelchangeComboResurrection();
protected:
	CsEditFosus m_edit_FG_Type;
	CsEditFosus m_edit_FG_DBuff;
	CsEditFosus m_edit_FG_STime;
	CsEditFosus m_edit_FG_ATime;
	CsEditFosus m_edit_FG_Point;
public:
	afx_msg void OnEnChangeEditFatigueType();
	afx_msg void OnEnChangeEditFatigueDebuff();
	afx_msg void OnEnChangeEditFatigueStime();
	afx_msg void OnEnChangeEditFatigueAtime();
	afx_msg void OnEnChangeEditFatiguePoint();
protected:
	CEdit m_edit_MapDescription_Eng;
protected:
	CsEditFosus m_edit_Camera_Max_Level;
public:
	afx_msg void OnEnChangeEditCameraLevel();

public:
	afx_msg void OnBnClickedButtonAddMapID();
protected:
	CButton		m_btn_AddMapID;
	CsEditFosus m_edit_AddMapID;
	CsEditFosus m_edit_AddToolMapName;

	CStatic		m_Static_AddMapID;
	CStatic		m_Static_ToolMapName;
	CStatic		m_Static_AddMapID_Group;
public:
	afx_msg void OnBnClickedCheckXgconsume();
	afx_msg void OnBnClickedCheckBattletagUse();

private:
	CButton		m_check_Xgconsume;	
	CButton		m_check_BattletagUse;
};
