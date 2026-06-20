
// Jenkins를 통해 SVN 업데이트 후 빌드 리비젼 번호를 셋팅 후 빌드함.
// ClientVersion.h 파일은 Jenkins로 빌드 하지 않은 용으로 사용
// Jenkins로 빌드시 ClientVersion.h 파일의 내용을 지우고 
// 여기에 있는 내용에 리비전 셋팅 후 ClientVersion.h에 넣고 빌드함.

#ifdef VERSION_USA
	#define COUNTRY_CODE		1		// 미국(GSP/STEAM)국가코드34936
#elif VERSION_HK
	#define COUNTRY_CODE		852		// 홍콩 국가코드
#elif VERSION_TW
	#define COUNTRY_CODE		886		// 대만 국가코드
#elif VERSION_TH
	#define COUNTRY_CODE		66		// 태국 국가코드
#else 
	#define COUNTRY_CODE		82		// 한국 국가코드
#endif

#ifdef VERSION_STEAM
#define CHANNELING_TYPE		1		// 채널링 타입 0 : 기본, 1 : Steam
#else
#define CHANNELING_TYPE		0		// 채널링 타입 0 : 기본, 1 : Steam
#endif

#ifdef _GIVE
	#ifdef VERSION_QA
		#define SERVICES_TYPE		1			// 0 : Test, 1 : QA, 2 : Live
	#else
		#define SERVICES_TYPE		2			// 0 : Test, 1 : QA, 2 : Live
	#endif
#else
	#define SERVICES_TYPE		0			// 0 : Test, 1 : QA, 2 : Live
#endif

#define SVN_REVNUMBER		34936	// 빌드 SVN 리비전 번호

