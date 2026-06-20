// _Dlg_Copy_Digimon.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SequnceTool.h"
#include "_Dlg_Copy_Digimon.h"

int CopyFolder(LPCTSTR from, LPCTSTR to)
{
	SHFILEOPSTRUCT fileOp = {0};
	fileOp.wFunc = FO_COPY;
	TCHAR newFrom[MAX_PATH];
	_tcscpy_s(newFrom, from);
	newFrom[_tcsclen(from) + 1] = NULL;
	fileOp.pFrom = newFrom;
	TCHAR newTo[MAX_PATH];
	_tcscpy_s(newTo, to);
	newTo[_tcsclen(to) + 1] = NULL;
	fileOp.pTo = newTo;
	fileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;
	return SHFileOperation(&fileOp);
} 
int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch (uMsg) {
		// 폴더선택 다이얼로그의 초기화가 끝난 경우
	case BFFM_INITIALIZED:
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		break;
	}
	return 0;
}
// _Dlg_Copy_Digimon 대화 상자입니다.

IMPLEMENT_DYNAMIC(_Dlg_Copy_Digimon, CDialog)

_Dlg_Copy_Digimon::_Dlg_Copy_Digimon(CWnd* pParent /*=NULL*/)
	: CDialog(_Dlg_Copy_Digimon::IDD, pParent)
{

}

_Dlg_Copy_Digimon::~_Dlg_Copy_Digimon()
{
}
BOOL _Dlg_Copy_Digimon::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
void _Dlg_Copy_Digimon::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_COPY_SRC_PATH, mEB_SrcPath);
	DDX_Control(pDX, IDC_EDIT_COPY_SRC_DIGIMONID, mEB_SrcDigimonID);
	DDX_Control(pDX, IDC_EDIT_COPY_SRC_DIGIMON_NAME, mEB_SrcName);
	DDX_Control(pDX, IDC_EDIT_COPY_DEST_DIGIMON_ID, mEBDestDigimonID);
	DDX_Control(pDX, IDC_EDIT_COPY_DEST_DIGIMON_NAME, mEBDestDigimonName);

	InitBasePath();
}


BEGIN_MESSAGE_MAP(_Dlg_Copy_Digimon, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_DIALOG, &_Dlg_Copy_Digimon::OnBnClickedButtonOpenDialog)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_DIGIMON_FROM_BASE, &_Dlg_Copy_Digimon::OnBnClickedButtonCreateDigimonFromBase)
END_MESSAGE_MAP()

// _Dlg_Copy_Digimon 메시지 처리기입니다.
void _Dlg_Copy_Digimon::OnBnClickedButtonOpenDialog()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString SelectedPath;
	SelectedPath = GetSelectFolderPath();
	if(SelectedPath.IsEmpty()){
		OutputResultMessage(FAIL_NOT_SELECT_SRC_DIGIMON_PATH);
		return;
	}

	COPY_RESUTL eResult = SetSrcDigimonInfo(SelectedPath);
	OutputResultMessage(eResult);
}
void _Dlg_Copy_Digimon::OnBnClickedButtonCreateDigimonFromBase()
{
	SetDigimonPath();
	COPY_RESUTL eResult = CheckingCopyable();
	if(eResult == ISCOPYABLE)
	{
		eResult = CopyDigimonFolder();
		if(	eResult == SUCCESS_COPY_FOLDER)
		{
			eResult = RenameAnimFiles();
			if(eResult == SUCCESS_RENAME_FILES)
			{
				eResult = CopyDigimonData(false);
			}
		}
	}
	OutputResultMessage(eResult);
}


COPY_RESUTL _Dlg_Copy_Digimon::SetSrcDigimonInfo(CString Path)
{
	SrcPath = Path;
	CFileFind finder;
	CString FinalPath = Path + _T("\\*.*");
	BOOL bWorking = finder.FindFile(FinalPath); //
	CString fileName;
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.IsArchived())
		{
			CString _fileName =  finder.GetFileName();

			if( _fileName == _T(".") || _fileName == _T("..") || _fileName == _T("Thumbs.db") ) continue;
			fileName =  finder.GetFileTitle();
			int i = _fileName.ReverseFind(_T('.'));
			CString exeName = _fileName.Right(_fileName.GetLength() - _fileName.ReverseFind('.') - 1);
			if(exeName == _T("kfm"))
			{
				CString PureFileName = finder.GetFileTitle();

				int i = PureFileName.ReverseFind('_');
				CString ModelID = PureFileName.Left(i);
				int iModelID  = _ttoi(ModelID.GetBuffer());
				char szBuf[MAX_PATH] = {0,};
				_itoa_s(iModelID, szBuf, 10);
				ModelID =szBuf;
				CString ModelName = PureFileName.Right(PureFileName.GetLength() - _fileName.ReverseFind('_') - 1);

				mEB_SrcPath.SetWindowText(finder.GetFilePath());
				mEB_SrcDigimonID.SetWindowText(ModelID);
				mEB_SrcName.SetWindowText(ModelName);

				return SUCCESS_SELECT_SRC_DIGIMON_PATH;
			}
		}
	}
	return FAIL_WRONG_SELECT_SRC_DIGIMON_PATH;
}

COPY_RESUTL _Dlg_Copy_Digimon::CheckingCopyable(void)
{
	USES_CONVERSION;
	CFileFind finder;
	CString FinalPath = CopyFromPath;

	BOOL bChecking = finder.FindFile(FinalPath); //
	int KfmCount = 0;
	int NifCount =0;
	while (bChecking)
	{
		bChecking = finder.FindNextFile();
		if (finder.IsArchived())
		{
			CString _fileName =  finder.GetFileName();
			CString exeName = _fileName.Right(_fileName.GetLength() - _fileName.ReverseFind('.') - 1);
			if( _fileName == _T(".") || _fileName == _T("..") || _fileName == _T("Thumbs.db") ) continue;

			if(exeName == _T("kfm"))
			{
				KfmCount++;
			}	
			else if(exeName == _T("nif"))
			{
				NifCount++;
			}
		}
	}
	if(KfmCount != 1)
		return DUPLICATE_KFM;
	if(NifCount != 1)
		return DUPLICATE_NIF;

	wchar_t NifPath[MAX_PATH];
	std::wstring kNifName = mSrcName.GetBuffer();
	swprintf_s( NifPath, MAX_PATH, L".\\%s.nif", kNifName.c_str());
	BOOL FindRes = finder.FindFile(NifPath);
	if( FindRes = FALSE)
	{
		return NO_NIFFILE;
	}
	return ISCOPYABLE;
}
void	_Dlg_Copy_Digimon::SetDigimonPath(void)
{
	TCHAR str[ MAX_PATH ] = {0, };
	mEBDestDigimonID.GetWindowText(str, MAX_PATH);
	mDestID = str;
	mEB_SrcDigimonID.GetWindowText(str, MAX_PATH);
	mSrcID = str;
	mEBDestDigimonName.GetWindowText(str, MAX_PATH);
	mDestName = str;
	mEB_SrcName.GetWindowText(str, MAX_PATH);
	mSrcName = str;

	iSrcID = _ttoi(mSrcID.GetBuffer());
	iDestID  = _ttoi(mDestID.GetBuffer());

	CopyNewPath = ObsoluteDigimonPath + "\\" + mDestName;//mDestID + _T("_") + mDestName;
	CopyFromPath = SrcPath + L"\\*.*";
}

COPY_RESUTL _Dlg_Copy_Digimon::CopyDigimonFolder()
{
	int i = CopyFolder(SrcPath, CopyNewPath);
	if(i == 0 )
		return SUCCESS_COPY_FOLDER;

	return FAIL_COPY_FOLDER;
}
COPY_RESUTL _Dlg_Copy_Digimon::RenameAnimFiles(void)
{
	USES_CONVERSION;
	CFileFind finder;
	CString FinalPath = CopyFromPath;
///////////////////////////////////////////////////////////////////////////
	BOOL bWorking = finder.FindFile(FinalPath); //

	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.IsArchived())
		{
			CString _fileName =  finder.GetFileName();
			CString _NewName =  finder.GetFileName();

			_NewName.Replace(mSrcName, mDestName);
			_NewName.Replace(mSrcID, mDestID);

			CString exeName = _fileName.Right(_fileName.GetLength() - _fileName.ReverseFind('.') - 1);
			if( _fileName == _T(".") || _fileName == _T("..") || _fileName == _T("Thumbs.db") ) continue;

			CString SrcFilePath = CopyNewPath + "\\" + _fileName;
			CString DestFilePath = CopyNewPath + "\\" + _NewName;

			MoveFile(SrcFilePath, DestFilePath);
			if(exeName == _T("kfm"))
			{
				RelateveDigimonPath += (mDestName + "\\"  +_NewName);
				std::string kTest = W2A(RelateveDigimonPath.GetBuffer());
				NiActorManager* pkActorMng = NiActorManager::Create(kTest.c_str(), false, true, NULL);
				char NifPath[MAX_PATH];
				std::string kNifName = W2A(mDestName.GetBuffer());
				sprintf_s( NifPath, MAX_PATH, ".\\%s.nif", kNifName.c_str());

				pkActorMng->GetKFMTool()->SetModelPath(NifPath);
				pkActorMng->GetKFMTool()->SaveFile(kTest.c_str());
				NiDelete(pkActorMng);
			}
		}
	}
	return SUCCESS_RENAME_FILES;

}

COPY_RESUTL _Dlg_Copy_Digimon::CopyDigimonData(bool ForceCopy)
{
	CString szisKFM;
	CString exeName = RelateveDigimonPath.Right(RelateveDigimonPath.GetLength() - RelateveDigimonPath.ReverseFind('.') - 1);
	if(exeName != _T("kfm"))
		return WRONG_KMF_FILE_NAME;
	if(iSrcID == iDestID )
		return DUPLICATED_DIGIMON_ID;

	USES_CONVERSION;
	std::string kPath = W2A(RelateveDigimonPath.GetBuffer());
	int Res = g_pModelDataMng->CreateDataFromAnother( iSrcID, iDestID, kPath, ForceCopy);
	if(Res == 0)
	{
		g_pMainWnd->_GetDlgController()->OnBnClickedButtonResetmodel();
		g_pModelDataMng->Save();
		return SUCCESS_COPY_FINAL;
	}
	else if( Res == 1)
	{
		return WRONG_BASE_DIGIMONID;

	}
	else if( Res == 2)
	{
		return DUPLICATED_NEW_DIGIMONID;
	}
	return FAIL_COPY_FINAL;

}
CString _Dlg_Copy_Digimon::GetSelectFolderPath()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ITEMIDLIST  *pidlBrowse;
	WCHAR       pszPathname[MAX_PATH];

	BROWSEINFO  BrInfo;
	BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.pidlRoot = NULL;

	memset(&BrInfo, 0x00, sizeof(BrInfo));
	BrInfo.pszDisplayName = pszPathname;
	BrInfo.lpszTitle = _T("Select folder");
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;

	// 이벤트에 대한 사용자정의 함수
	BrInfo.lpfn = BrowseCallbackProc;
	BrInfo.lParam = (LPARAM)ObsoluteDigimonPath.GetBuffer();

	pidlBrowse = ::SHBrowseForFolder(&BrInfo);
	if(pidlBrowse != NULL){
		// 선택된 폴더 경로얻음
		SHGetPathFromIDList(pidlBrowse, pszPathname);
	}
	CString strFolderPath;
	strFolderPath.Format(L"%s",pszPathname);

	return strFolderPath;
}

int _Dlg_Copy_Digimon::InitBasePath(void)
{
	RelateveDigimonPath = L"Data\\Digimon\\";
	WCHAR       pszPathname[MAX_PATH];
	GetModuleFileName(NULL, pszPathname, MAX_PATH);
	ObsoluteDigimonPath = pszPathname;
	int i = ObsoluteDigimonPath.ReverseFind('\\');
	ObsoluteDigimonPath = ObsoluteDigimonPath.Left(i);
	ObsoluteDigimonPath += L"\\" +	RelateveDigimonPath;
	return 0;
}

void _Dlg_Copy_Digimon::OutputResultMessage(COPY_RESUTL eResult)
{
	switch(eResult)
	{
	case SUCCESS:
		break;
	case FAIL_NOT_SELECT_SRC_DIGIMON_PATH:
		AfxMessageBox(_T("베이스 디지몬의 경로가 선택되지 않았습니다."));
		break;
	case SUCCESS_SELECT_SRC_DIGIMON_PATH:
		AfxMessageBox(_T("Load 성공, 새롭게 만들 디지몬의 ID, 이름을 입력한 후 <복사하기>버튼을 눌러주세요!"));
		break;
	case FAIL_WRONG_SELECT_SRC_DIGIMON_PATH:
		AfxMessageBox(_T("해당 폴에더 KFM 파일이 없거나 이름, ID가 잘못되었습니다."));
		break;
	case FAIL_COPY_FOLDER:
		AfxMessageBox(_T("생성할 디지몬의 폴더가 이미 있습니다. 삭제해 주시고 다시 시도해 주세요!"));
		break;
	case SUCCESS_RENAME_FILES:
		AfxMessageBox(_T("파일 복사 성공"));
		break;
	case WRONG_KMF_FILE_NAME:
		AfxMessageBox(_T("SOURCE DIGIMON의 잘못된 KFM 파일 입니다."));
		break;
	case DUPLICATED_DIGIMON_ID:
		AfxMessageBox(_T("디지몬 아이디가 잘못되었습니다."));
		break;
	case SUCCESS_COPY_FINAL:
		AfxMessageBox(_T("새로운 디지몬 생성을 성공했습니다."));
		break;
	case WRONG_BASE_DIGIMONID:
		AfxMessageBox(_T("베이스 번호가 기존 Model.Dat에 저장되지 않았습니다.."));
		break;
	case DUPLICATED_NEW_DIGIMONID:
		if(IDYES == AfxMessageBox(L"새롭게 만든 번호가 이미 기존의 Model.Dat에 저장되어 있습니다 삭제하고 다시 저장하시겠습니까?", MB_YESNO))
		{
			CopyDigimonData(true);
		}
		break;
	case FAIL_COPY_FINAL:
		AfxMessageBox(_T("알수 없는 에러입니다.처음 부터 다시 실행해주세요"));
		break;
	case DUPLICATE_KFM:
		AfxMessageBox(_T("원본 폴더에 KMF 파일이 두개 이상입니다."));
		break;
	case DUPLICATE_NIF:
		AfxMessageBox(_T("원본 폴더에 NIF 파일이 두개 이상입니다."));
		break;
	case NO_NIFFILE:
		AfxMessageBox(_T("NIF 파일이 없습니다. 혹은 파일명을 확인해주세요"));
		break;


	}

}