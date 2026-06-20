// Lan_To_HeaderDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"

#define RESIST_FILE_PATH		L"ToolRgs\\Lan_To_Header.rgs"

struct sRESIST
{
	TCHAR		s_szWorkDir[ MAX_PATH ];
	void		Load();
	void		Save();
};


// CLan_To_HeaderDlg 대화 상자
class CLan_To_HeaderDlg : public CDialog
{
protected:
	void				__Init();
	void				__Delete();

	void				__NewLine( FILE* fp, int nNum );

protected:
	sRESIST				m_Resist;



	// 체크 소스
public:
	struct sWRITE_INFO
	{
		FILE*	s_pWriteFile;
		int		s_nLine;
		TCHAR*	s_szFileName;
	};

protected:
	CStringArray		m_CheckFileArray;
protected:
	void				__SourceCheck();
	void				__Serch_SourceExist( FILE* fp, CsLocalizing::MAP* pMapOrg, CsLocalizing::MAP* pMap, CString* pFile );
	void				__Serch_SourceExist_Line( sWRITE_INFO* wi, CsLocalizing::MAP* pMapOrg, CsLocalizing::MAP* pMap, char* cLine );
	void				__FindCheckSubDir(CString strDir, CStringArray &FileArray );


	// 토탈 엑셀
protected:	
	void				__DM_Addeffect_E( YExcel::BasicExcelWorksheet* pSheet );
	void				__DM_Digimon_List_E_Skill( YExcel::BasicExcelWorksheet* pSheet );
	void				__DM_Item_List_E_Skill( YExcel::BasicExcelWorksheet* pSheet );
	void				__DM_Evolution_Digitama_E( YExcel::BasicExcelWorksheet* pSheet );
	void				__DM_Item_List_E( YExcel::BasicExcelWorksheet* pSheet );
	void				__DM_QuestItem_List_E( YExcel::BasicExcelWorksheet* pSheet );
	void				__DM_Riding_E( YExcel::BasicExcelWorksheet* pSheet );
	void				__DM_WorldMap_E( YExcel::BasicExcelWorksheet* pSheet );
	void				__DM_Talk_E( YExcel::BasicExcelWorksheet* pSheet );
	void				__DM_Map_E( YExcel::BasicExcelWorksheet* pSheet );
	void				__DM_ParcingWord_E( YExcel::BasicExcelWorksheet* pSheet );
	void				__Npc_E( YExcel::BasicExcelWorksheet* pSheet );
	void				__Quest_00000_E( YExcel::BasicExcelWorksheet* pSheet );
	void				__Quest_02501_E( YExcel::BasicExcelWorksheet* pSheet );
	void				__QuestAchieve_E( YExcel::BasicExcelWorksheet* pSheet );





protected:
	void				__TitleTab( YExcel::BasicExcelWorksheet* pSheet, int nIndex );



// 생성입니다.
public:
	CLan_To_HeaderDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LAN_TO_HEADER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


	virtual void	OnOK(){}


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	afx_msg void OnBnClickedButtonSetdir();
protected:
	CEdit m_edit_WorkDir;
public:
	afx_msg void OnBnClickedButtonEnglish();
	afx_msg void OnBnClickedButtonThailand();
	afx_msg void OnBnClickedButtonTotalexcel();
	afx_msg void OnBnClickedButtonTempQuest();
};
