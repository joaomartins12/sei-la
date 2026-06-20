//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------

namespace GData
{
	enum eCountryType
	{
		eCountry_None,
		eCountry_Kor,
		eCountry_GSP,
		eCountry_Aeria,
		eCountry_Steam,
		eCountry_Hongkong,
		eCountry_Thailand,
		eCountry_Taiwan,
	};

	enum eLanguageType
	{
		eLanguage_None,
		eLanguage_Kor,
		eLanguage_Eng,
		eLanguage_Thailand,
		eLanguage_Hongkong,
		eLanguage_Taiwan,		
	};

	enum e2ndPassType
	{
		eNone,
		eEmail,
		eAccountPass,
		e2ndNumberPass,
		eStringPass,
	};

	struct sServerDataInfo
	{
		sServerDataInfo():m_nServerIDX(0),m_nHaveCharacterCount(0),m_nMaxCharacterSlotCount(0),m_nLockSlotCount(0)
		{}

		~sServerDataInfo() {};
		int				m_nServerIDX;
		std::wstring	m_ServerName;				// 서버 이름
		unsigned short	m_nHaveCharacterCount;		// 생성된 캐릭터 갯수
		unsigned short	m_nMaxCharacterSlotCount;	// 생성할 수 있는 캐릭터 최대 갯수
		unsigned short	m_nLockSlotCount;			// 잠겨 있는 슬롯 갯수
	};

	struct sConnetServerInfo
	{
		sConnetServerInfo():m_nServerIDX(0)
		{}

		~sConnetServerInfo() {};

		int				m_nServerIDX;
		std::wstring	m_ServerName;				// 서버 이름
	};

	class CGlobalData : public CSingleton<CGlobalData>
	{
	public:
		CGlobalData();
		~CGlobalData();

		void			Init();
		void			Clear2ndPass();

		std::string		GetAccountIP() const;
		unsigned int	GetAccountPort() const;

		void			SetLanguage(eLanguageType eType);
		eLanguageType	GetLanguageType() const;
		std::string		GetSystemFontFileName() const;
		std::string		GetTextFontFileName() const;

		void			SetConnectionCountryType(eCountryType eType);// 국가 셋팅
		eCountryType	GetConnectionCountryType(void) const;
		std::string		GetCountryName(void) const;
		bool			IsCountry(eCountryType eType) const;
		bool			IsLoginSkiped(void) const;				// 서비스 지역별 로그인 페이지 스킵 기능 확인.

		std::string		GetLoadTableFile(void) const;			// 서비스 지역별 액셀 테이블 파일명
		std::string		GetLoadResourceFile(void) const;		// 서비스 지역별 리소스 파일명

		void			SetAuthCode(std::string const& authCode);
		std::string		GetAuthCode(void) const;

		void			SetAccountID(std::string const& auID);
		std::string		GetAccountID(void) const;
		std::wstring	GetAccountIDW(void) const;
		void			ResetAccountInfo(void);

		void			SetAccountPW(std::string const& auPW);
		std::string		GetAccountPW(void) const;

		void			SetUserType(std::string const& cChannelName);
		std::string		GetUserType(void) const;
		bool			IsChannelingService(void) const;

		void			Set2ndPassType(e2ndPassType nType);
		e2ndPassType	Get2ndPassType(void) const;
		bool			Is2ndPassType(e2ndPassType nType);

		void			Set2ndPassword(std::string const& strPass);
		std::string		Get2ndPassword(void) const;
		void			SetSkip2ndPass(void);
		bool			Is2ndPassSkiped();
		bool			Is2ndPassInputPass() const;// 2차 비밀번호 입력을 스킵할 수 있는가?
		bool			Is2ndPassEncry() const;

		std::wstring	GetCashShopWebUrl(void) const;
		std::wstring	GetNoticeWebUrl(void) const;
		std::string		GetCashChargeUrl(void) const;
		std::wstring	GetWebCashPageUrl(void) const;
		std::wstring	GetHomePageUrl(void) const;


		void			ClearServerListInfo();
		void			AddServerListInfo(int const& nServerIdx,
			std::wstring const& serverName,
			unsigned short const& nHaveCharacterCount,
			unsigned short const& nMaxCharacterCount,
			unsigned short const& nOpenedSlotCount);
		void			SetConnetServerInfo(int const& nServerIdx, std::wstring const& serverName);

		int				GetCurrentServerIDX() const;
		std::wstring	GetCurrentServerName() const;

		// 서버 인덱스로 그 서버에 생성 가능한 캐릭터 슬롯의 갯수를 리턴한다.
		// 전체 캐릭터 슬롯 수 - (잠긴 슬롯 수 + 생성된 슬롯 수)
		int				GetEmptySlotCountFromServer(int const& nServerIDX) const;

		std::list<sServerDataInfo> const& GetServerListInfo() const;
		int				GetCurrentServerLockSlotCount() const;

		void			GetServerSlotCountInfo(int const& nServerIDX, int& nOpenSlotCount, int& nLockSlotCount);

		void			LoadInfo_AccountIP_Port();

		void			SetDownloadPatchVersion(int const& nVersion);
		int				GetDownloadPatchVersion() const;

		std::string		GetAccessToken(void) const;
		std::string		GetMailAddress(void) const;
		std::string		GetTicketingtime(void) const;

		void			SetTicketingtime(std::string const& ticketingTime);
		void			SetMailAddress(std::string const& mailAddress);
		void			SetAccessToken(std::string const& acctoken);

		void			ToggleLiveServer();
	 
	private:
		void			_SetNoticeWebUrl(eCountryType const& eType);
		void			_Init2ndPasswordType(eCountryType const& eType);
		void			_SetAccountIP_Port(eCountryType const& eType);
	
	private:
		bool			bWasManuallySet;
		int				m_nDownLoadVersion;
		eCountryType	m_nConnectionCountryType;	// 접속 지역
		eLanguageType	m_nLanguageType;			// 언어 타입

		std::string		m_Account_IP;
		unsigned int	m_Account_Port;

		std::string		m_strAccountID;		// 계정 ID
		std::string		m_strAccountPW;		// 계정 Password

		e2ndPassType	m_n2ndPassType;		// 2nd PasswordType
		std::string		m_str2ndPass;		// 2nd Password

		std::string		m_LoadTableFileName;
		std::string		m_LoadResourceFileName;

		std::string		m_Auth_Code;
		std::string		m_cAccessToken;
		std::string		m_cMailAddress;
		std::string		m_cTicketingtime;
		
		std::string		m_cUserType;

		std::wstring	m_wsCashShopWebUrl;
		std::wstring	m_wsNoticeWebUrl;
		std::string		m_cCashChargeUrl;
		std::wstring	m_wsWebCashPageUrl;
		std::wstring	m_wsHomePageWebUrl;
		std::string		m_wsHashDataUrl;

		// 2차 비밀번호 입력을 스킵할 수 있는가?
		// 현재는 GSP 국가만 적용함. 
		// 다른 국가도 2차 비밀번호 입력 기능이 들어가게 되면 
		// 국가별로 셋팅 해주자
		bool			m_b2ndPasswordInputPassUsing;
		std::list<sServerDataInfo> m_ServerDataList;
		sConnetServerInfo		m_ConnetServerInfo;
	};
};

// Singleton
#define GLOBALDATA_ST			GData::CGlobalData::GetSingleton()
#define GLOBALDATA_STPTR		GData::CGlobalData::GetSingletonPtr()
//---------------------------------------------------------------------------