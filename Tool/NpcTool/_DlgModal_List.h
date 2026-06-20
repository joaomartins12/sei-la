#pragma once


// _DlgModal_List 대화 상자입니다.

class _DlgModal_List : public CDialog
{
	DECLARE_DYNAMIC(_DlgModal_List)


public:
	enum eLIST_TYPE{ LT_MONSTER, LT_NPC, LT_ITEM, LT_DIGIMON, LT_QUEST, };
public:
	_DlgModal_List( eLIST_TYPE type, void* pReturnValue, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgModal_List();

protected:
	eLIST_TYPE		m_eListType;
	void*			m_pReturnValue;

	// ====		Npc
protected:
	void			__Npc_Init();
	void			__Npc_Read();
	void			__Npc_Sort( int nColume, bool bIncrease );
	void			__Npc_InsertList( CsNpc* pFT );

	// ====		Monster
protected:
	void			__Monster_Init();
	void			__Monster_Read();
	void			__Monster_Sort( int nColume, bool bIncrease );
	void			__Monster_InsertList( CsMonster* pFT );

	// ====		Item
protected:
	void			__Item_Init();
	void			__Item_Read();
	void			__Item_Sort( int nColume, bool bIncrease );
	void			__Item_InsertList( CsItem* pFT );

	// ====		Digimon
protected:
	void			__Digimon_Init();
	void			__Digimon_Read();
	void			__Digimon_Sort( int nColume, bool bIncrease );
	void			__Digimon_InsertList( CsDigimon* pFT );

	// ====		Quest
protected:
	void			__Quest_Init();
	void			__Quest_Read();
	void			__Quest_Sort( int nColume, bool bIncrease );
	void			__Quest_InsertList( CsQuest* pFT );




// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	CListCtrl m_list_Main;
public:
	afx_msg void OnLvnColumnclickListMain(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListMain(NMHDR *pNMHDR, LRESULT *pResult);
};
