
#ifdef LANGUAGE_KOR

#define NET_FAIL			 _T("네트워크 연결실패")
//#define RUN_FAIL			 _T("파라미터 오류")

#define NO_PATH				 _T("런처 설치 경로가 없습니다.\n새로 받으시겠습니까?")

#define UPDATE_FAIL			 _T("업데이트 실패")
#define UNINST_FAIL			 _T("삭제 실패.")
#define STR_PERSENT			 _T("진행중.. %d%%")
#define STR_UPDATE			 _T("업데이트중입니다.")


#define LAUNCHER_FILE_NOT_FOUND		_T("지정된 파일을 찾을수 없습니다")
#define LAUNCHER_PATH_NOT_FOUND		_T("지정된 경로를 찾을수 없습니다")
#define LAUNCHER_DDE_FAIL			_T("동적 데이터 교환 트랜잭션에 실패했습니다")
#define LAUNCHER_NO_ASSOCIATION		_T("주어진 파일의 확장명과 연결되는 응용 프로그램이 없습니다")
#define LAUNCHER_ACCESS_DENIED		_T("지정된 파일에 대한 액세스가 거부 되었습니다")
#define LAUNCHER_DLL_NOT_FOUND		_T("응용 프로그램을 실행하는 데 필요한 하나의 라이브러리 파일을 찾을 수없습니다")
#define LAUNCHER_CANCELLED			_T("함수가 사용자를 위한 추가정보를 알려주었지만 사용자가 요청을 취소 했습니다")
#define LAUNCHER_NOT_ENOUGH_MEMORY	_T("작업을 수행하기 위한 충분한 메모리가 없습니다")
#define LAUNCHER_SHARING_VIOLATION	_T("공유 위반이 발생했습니다" )

#define STR_LAUNCHER_ERRO_DEFAULT	_T("런처 실행 에러 : 번호 %ld")

#define ERROR_DOWNLOAD_FILE		_T("Download File Error\nFileName : %s")
#define ERROR_UNZIP_FILE		_T("Decompressing Error : %s")

#else

#define NET_FAIL			 _T("Failed to connect to the internet.")
//#define RUN_FAIL			 _T("파라미터 오류")

#define NO_PATH				 _T("Can not find Launcher file\nDo you want to Download File?")

#define UPDATE_FAIL			 _T("Launcher Update Failed")

#define STR_PERSENT			 _T("Updating.. %d%%")
#define STR_UPDATE			 _T("Updating..")


#define LAUNCHER_FILE_NOT_FOUND		_T("Error : exe file does not exist. Please try again.")
#define LAUNCHER_PATH_NOT_FOUND		_T("Error : Incorrect Directory")
#define LAUNCHER_NOT_ENOUGH_MEMORY	_T("Error : Insufficient Memory")

#define STR_LAUNCHER_ERRO_DEFAULT	_T("Error : number %ld")

#define ERROR_DOWNLOAD_FILE			_T("Download File Error\nFileName : %s")
#define ERROR_UNZIP_FILE			_T("Decompressing Error : %s")

#endif
