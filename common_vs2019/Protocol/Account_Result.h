namespace eLOGINRESULT
{
	const int SUCCESS							= 0;
	
	const int SERVER_ERROR						= 10034;			//서버측 오류
	const int AUTH_FAILED						= 10035;			//인증실패
	const int SERVER_NOT_OPEN					= 10015;			//Account서버가 열려있지 않음
	const int ACCOUNT_BANNED					= 10052;			//계정 밴상태
	const int ALREADY_LOGIN						= 10014;			//이미 접속중입니다
	const int IPBLOCKED							= 10036;			//IP블록됨
	const int MACBLOCKED						= 10037;			//MAC블록됨
	const int VIRTUALMACHINE					= 10038;			//가상머신에서 접속
	const int NO_AVAILABE_CORE					= 10039;			//접속가능한 코어서버 없음
	const int VERSION_NOT_MATCH					= 19999;			//버전이 다름
	const int PASS_NOT_MATCH					= 20052;			//2차비밀번호 틀림

	const int HTTP_ERROR						= 10054;			//인증 HTTP에러
	const int NOT_AGREEMENT						= 10055;			//약관 동의안함
	const int BLOCKING							= 10056;			//계정 블록
	const int ERROR_ID							= 10058;			//인증과정중 에러
	const int ERROR_LOGINPASS					= 10057;			//계정 정보가 일치하지 않음

	const int NOT_AGREEMENT_EMAIL				= 10059;			//이메일 동의 안함
	const int NOT_AGREEMENT_USER				= 10060;			//유저 동의 안함
	const int NOT_AGREEMENT_GAME				= 10061;			//게임 동의 안함

	const int SERVER_IS_MAINTENANCE				= 10011;			// 서버 점검중
	const int SERVER_CONNECT_USER_FULL			= 10012;			// 서버 정원 초과
	const int SERVER_IS_NOT_READY				= 10015;			// 서버 준비중
	
	const int RELOCATE_BANNED                   = 10120;            // 이전 진행중인 아이디 입니다.
}

//
//enum eLOGINRESULT
//{	
//	SUCCESS = 0,
//	DO_LOGIN = 1,
//	CHECK_2NDPASS = 2,
//
//	ERRORSTART = 10,		//여기서부터 에러코드
//
//	SERVER_ERROR,			//서버측 오류
//	AUTH_FAILED,			//인증실패
//	SERVER_NOT_OPEN,		//Account서버가 열려있지 않음
//	ACCOUNT_BANED,			//계정 밴상태
//	ALREADY_LOGIN,			//이미 접속중입니다
//	IPBLOCKED,				//IP블록됨
//	MACBLOCKED,				//MAC블록됨
//	VIRTUALMACHINE,			//가상머신에서 접속
//	NO_AVAILABE_CORE,		//접속가능한 코어서버 없음
//	VERSION_NOT_MATCH,		//버전이 다름
//};

enum eRESULT
{
	SUCCESS = 0,
	FAILED = 1,
};
