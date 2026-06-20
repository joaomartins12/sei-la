
// FILEVERSION 1.1.0.304 
// 국가코드.QA or Live.채널링타입.리비젼번호

#define COUNTRY_CODE 1		// 미국 국가코드
#define CHANNELING_TYPE 0		// 채널링타입 0 : GSP, 1 : Aeria, 2 : Steam

#ifdef VERSION_QA
#define SERVICES_TYPE 0			// 0 : QA, 1 : Live
#define REVNUMBER_QA 313
#define FileDescription_Str "QA DMO Launcher"
#else
#define SERVICES_TYPE 1
#define REVNUMBER_LIVE 310
#define FileDescription_Str "DMO Launcher"
#endif
