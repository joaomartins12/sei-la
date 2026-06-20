//---------------------------------------------------------------------------
//
// 파일명 : DXVersionChecker.h
// 작성일 : 
// 작성자 : 
// 설  명 :
//
//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------

class DXVersionChecker
{
public:
	enum VERSION_RESULT
	{
		DX_RESULT_DO_NOT_DXINSTALL,
		DX_RESULT_NEED_DXVERSION_UP,		
		DX_RESULT_Ok_DXVERSION,
		OS_RESULT_UNSUPPORTED_OS,
		OS_RESULT_OK,
	};

	DXVersionChecker();
	~DXVersionChecker();

	VERSION_RESULT CheckDXVersionRequirement( DWORD p_dwDXVersion , DWORD p_dwRevision );
	VERSION_RESULT CheckOSVersionRequirement();
	bool			DoDxSettup( TCHAR* pDoFileName );
};

//---------------------------------------------------------------------------