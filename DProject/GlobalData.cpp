//---------------------------------------------------------------------------
//
// 파일명 : NetExtraData.cpp
// 작성일 : 
// 작성자 : 
// 설  명 :
//
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "GlobalData.h"
#include <ifdef.h>
//---------------------------------------------------------------------------
namespace GData
{
	CGlobalData::CGlobalData()
	:m_n2ndPassType(e2ndPassType::eNone)
	,m_nConnectionCountryType(eCountryType::eCountry_None)
	,m_b2ndPasswordInputPassUsing(false)
	,m_nLanguageType(eLanguageType::eLanguage_None)
	,m_nDownLoadVersion(0)
	{
		bWasManuallySet = false;
	}
	//---------------------------------------------------------------------------
	CGlobalData::~CGlobalData()
	{
	}
	//---------------------------------------------------------------------------

	void CGlobalData::Init()
	{
	}
	
	void CGlobalData::Clear2ndPass()							
	{	
		m_str2ndPass.clear();	
	}

	std::string CGlobalData::GetAccountIP() const				
	{	
		return m_Account_IP;	
	}

	unsigned int CGlobalData::GetAccountPort() const			
	{	
		return m_Account_Port;	
	}

	bool CGlobalData::IsCountry( eCountryType eType ) const		
	{	
		return (m_nConnectionCountryType == eType);	
	}

	void CGlobalData::SetLanguage( eLanguageType eType )		
	{	
		m_nLanguageType = eType;	
	}

	std::string CGlobalData::GetSystemFontFileName() const
	{
		switch( m_nLanguageType )
		{
		case eLanguageType::eLanguage_Kor:		return "data\\font\\NanumGothicBold.ttf";
		case eLanguageType::eLanguage_Eng:		return "data\\font\\tahoma.ttf";
		case eLanguageType::eLanguage_Thailand:	return "data\\font\\tahoma.ttf";
		case eLanguageType::eLanguage_Hongkong:	return "data\\font\\msjh.ttc";
		case eLanguageType::eLanguage_Taiwan:	return "data\\font\\msjh.ttc";
		default:								return "";
		}
	}

	std::string CGlobalData::GetTextFontFileName() const
	{
		switch( m_nLanguageType )
		{
		case eLanguageType::eLanguage_Kor:		return "data\\font\\NanumGothicBold.ttf";
		case eLanguageType::eLanguage_Eng:		return "data\\font\\tahoma.ttf";
		case eLanguageType::eLanguage_Thailand:	return "data\\font\\tahoma.ttf";
		case eLanguageType::eLanguage_Hongkong:	return "data\\font\\msjh.ttc";
		case eLanguageType::eLanguage_Taiwan:	return "data\\font\\msjh.ttc";
		default:								return "";
		}
	}

	eLanguageType CGlobalData::GetLanguageType() const				{	return m_nLanguageType;	}

	void CGlobalData::SetConnectionCountryType( eCountryType eType )
	{
		m_nConnectionCountryType = eType;
		m_LoadResourceFileName = "Data\\Pack01";

		switch( m_nConnectionCountryType )
		{
		case eCountry_Kor:		m_LoadTableFileName = "Data\\Pack02";			break;
		case eCountry_GSP:		m_LoadTableFileName = "Data\\Pack03";			break;
		case eCountry_Aeria:	m_LoadTableFileName = "Data\\Pack03";			break;
		case eCountry_Steam:	m_LoadTableFileName = "Data\\Pack03";			break;
		case eCountry_Hongkong:	m_LoadTableFileName = "Data\\Pack07";			break;
		case eCountry_Thailand:	m_LoadTableFileName = "Data\\Pack04";			break;
		case eCountry_Taiwan:	m_LoadTableFileName = "Data\\Pack06";			break;
		}

		_SetNoticeWebUrl(m_nConnectionCountryType);
		_Init2ndPasswordType(m_nConnectionCountryType);
		_SetAccountIP_Port( m_nConnectionCountryType );
	}

	bool CGlobalData::Is2ndPassInputPass() const		{	return m_b2ndPasswordInputPassUsing;	}

	void CGlobalData::_Init2ndPasswordType(eCountryType const& eType)
	{
		switch( eType )
		{
		case eCountry_Kor:		m_n2ndPassType = e2ndNumberPass;	break;
		case eCountry_GSP:		m_n2ndPassType = eEmail;			m_b2ndPasswordInputPassUsing = true;	break;
		case eCountry_Aeria:	m_n2ndPassType = eAccountPass;		break;
		case eCountry_Steam:	m_n2ndPassType = eStringPass;		break;
		case eCountry_Hongkong:	m_n2ndPassType = eAccountPass;		break;
		case eCountry_Thailand:	m_n2ndPassType = eAccountPass;		break;
		case eCountry_Taiwan:	m_n2ndPassType = e2ndNumberPass;	break;
		}
	}

	bool CGlobalData::Is2ndPassEncry() const
	{
		switch( m_n2ndPassType )
		{
		case eEmail:
		case eStringPass:
			return false;	// Email/특정 문자열 은 암호화 하지 않는다.
		case e2ndNumberPass:
			return true;
		case eAccountPass:
			{
				switch( m_nConnectionCountryType )
				{
				case eCountry_Aeria:
				case eCountry_Thailand:
					return false;	// 계정ID를 입력하는 국가중 아에리아,태국은 암호화 하지 않는다.
				}
				return true;
			}
		default:
			return false;
		}
	}

	void CGlobalData::SetSkip2ndPass(void)
	{
		switch( m_nConnectionCountryType )
		{
		case eCountry_GSP:		m_n2ndPassType = eEmail;			break;
		case eCountry_Steam:	m_n2ndPassType = eStringPass;		break;
		case eCountry_Aeria:	m_n2ndPassType = eAccountPass;		break;
		case eCountry_Kor:		m_n2ndPassType = e2ndNumberPass;	break;
		case eCountry_Hongkong:	m_n2ndPassType = eAccountPass;		break;
		case eCountry_Taiwan:	m_n2ndPassType = e2ndNumberPass;	break;
		case eCountry_Thailand:	m_n2ndPassType = eAccountPass;		break;
		}
	}

	#define SKIP_TIME_CHECK_DAY		(24 * 60 * 60)
	bool CGlobalData::Is2ndPassSkiped()
	{
		if( !m_b2ndPasswordInputPassUsing )
			return false;

		if( g_pResist->m_Global.s_SkipSPWTime == 0 )
			return false;

		int nCurDay = g_pResist->m_Global.s_SkipSPWTime / SKIP_TIME_CHECK_DAY;
		int nServerDay = net::net->GetTimeTS() / SKIP_TIME_CHECK_DAY;
		if( nCurDay != nServerDay )
			return false;
		return true;
	}

	void CGlobalData::_SetNoticeWebUrl(eCountryType const& eType)
	{
/*
		switch( eType )
		{
		case eCountry_GSP:	
			{
				m_wsNoticeWebUrl = L"http://dmo.gameking.com/News/NoticeList.aspx";		
				m_wsHomePageWebUrl = L"http://dmo.gameking.com/Main/Main.aspx";
#ifdef _GIVE
	#ifdef VERSION_QA
				m_cCashChargeUrl = "https://dev-dmo.gameking.com/Common/InGameXSolla.aspx?userid=%s&useremail=%s&ticketingtime=%s&accesstoken=%s";
	#else
				m_cCashChargeUrl = "https://dmo.gameking.com/Common/InGameXSolla.aspx?userid=%s&useremail=%s&ticketingtime=%s&accesstoken=%s";
	#endif
#else
				m_cCashChargeUrl = "https://dmo.gameking.com/Common/InGameXSolla.aspx?userid=%s&useremail=%s&ticketingtime=%s&accesstoken=%s";
#endif
			}break;
		case eCountry_Aeria:
			{
				m_wsNoticeWebUrl = L"http://www.aeriagames.com/playnow/dmus";		
				m_cCashChargeUrl = "https://billing.aeriagames.com";	
			}break;
		case eCountry_Steam:		
			{
				m_wsNoticeWebUrl = L"http://dmo.gameking.com/News/NoticeList.aspx";
			}break;
		case eCountry_Kor:
			{
				m_cCashChargeUrl = "http://www.digimonmasters.com/cash/main.aspx?";	
				m_wsHomePageWebUrl = L"http://www.digimonmasters.com/";
			}break;
		case eCountry_Thailand:		
			{
				m_cCashChargeUrl = "https://bill.vplay.in.th/digimon/fillup/digimonfillup.php";
				m_wsHomePageWebUrl = L"http://dmm.vplay.in.th/home.php";
			}break;
		case eCountry_Hongkong:		
			{
				m_wsHomePageWebUrl = L"https://hkdmo.gameking.com";
				m_wsNoticeWebUrl = L"https://hkdmo.gameking.com/News/NoticeList.aspx";		
#ifdef _GIVE
	#ifdef VERSION_QA
				m_cCashChargeUrl = "https://alpha.hkdmo.gameking.com/Common/InGameXSolla.aspx?userid=%s&useremail=%s&ticketingtime=%s&accesstoken=%s";			
	#else
				m_cCashChargeUrl = "https://hkdmo.gameking.com/Common/InGameXSolla.aspx?userid=%s&useremail=%s&ticketingtime=%s&accesstoken=%s";
	#endif
#else
				m_cCashChargeUrl = "https://alpha.hkdmo.gameking.com/Common/InGameXSolla.aspx?userid=%s&useremail=%s&ticketingtime=%s&accesstoken=%s";
#endif
			}break;
		case eCountry_Taiwan:
			{
				m_wsHomePageWebUrl = L"https://twdmo.gameking.com";
				m_wsNoticeWebUrl = L"https://twdmo.gameking.com/News/NoticeList.aspx";		
#ifdef _GIVE
	#ifdef VERSION_QA
				m_cCashChargeUrl = "https://alpha.twdmo.gameking.com/Common/InGameXSolla.aspx?userid=%s&useremail=%s&ticketingtime=%s&accesstoken=%s";			
	#else
				m_cCashChargeUrl = "https://twdmo.gameking.com/Common/InGameXSolla.aspx?userid=%s&useremail=%s&ticketingtime=%s&accesstoken=%s";
	#endif
#else
				m_cCashChargeUrl = "https://alpha.twdmo.gameking.com/Common/InGameXSolla.aspx?userid=%s&useremail=%s&ticketingtime=%s&accesstoken=%s";
#endif
			}break;
		}
		*/
	}

	void CGlobalData::SetUserType(std::string const& cUserType)
	{
		m_cUserType = cUserType;
		/*		if( 0 == m_cUserType.compare( "ZAM" ) || 0 == m_cUserType.compare( "DMOM" ) || 0 == m_cUserType.compare( "DIGI" ) )
		{
			m_wsCashShopWebUrl	= L"http://www.digimonmasters.com/shop/main.aspx";
			m_wsNoticeWebUrl	= L"http://www.digimonmasters.com/news/main.aspx";
			m_wsWebCashPageUrl	= L"http://www.digimonmasters.com/shop/main.aspx";
			m_wsHomePageWebUrl	= L"http://www.digimonmasters.com/";
			m_cCashChargeUrl	= "http://www.digimonmasters.com/cash/main.aspx?";	
		}
		else if( 0 == m_cUserType.compare( "PN" ) || 0 == m_cUserType.compare( "PNET" ) )
		{
			m_wsCashShopWebUrl	= L"http://dm.playnetwork.co.kr/shop/main.aspx";
			m_wsNoticeWebUrl	= L"http://dm.playnetwork.co.kr/news/main.aspx";
			m_wsWebCashPageUrl	= L"http://dm.playnetwork.co.kr/shop/main.aspx";
			m_wsHomePageWebUrl	= L"http://dm.playnetwork.co.kr/";
			m_cCashChargeUrl	= "http://dm.playnetwork.co.kr/cash/main.aspx?";	
		}
// 		else if( 0 == m_cUserType.compare( "TOON" ) )
// 		{
// 			m_wsCashShopWebUrl	= L"http://digimonmasters.tooniland.com/shop/main.aspx";
// 			m_wsNoticeWebUrl	= L"http://digimonmasters.tooniland.com/news/main.aspx";
// 			m_wsWebCashPageUrl	= L"http://digimonmasters.tooniland.com/shop/main.aspx";
// 			m_wsHomePageWebUrl	= L"http://digimonmasters.tooniland.com/";
// 			m_cCashChargeUrl	= "http://digimonmasters.tooniland.com/cash/main.aspx?";	
// 		}
// 		else if( 0 == m_cUserType.compare( "IMBC" ) )
// 		{
// 			m_wsCashShopWebUrl	= L"http://dm.imbc.com/shop/main.aspx";
// 			m_wsNoticeWebUrl	= L"http://dm.imbc.com/news/main.aspx";
// 			m_wsWebCashPageUrl	= L"http://dm.imbc.com/shop/main.aspx";
// 			m_wsHomePageWebUrl	= L"http://dm.imbc.com/index.aspx";
// 			m_cCashChargeUrl	= "http://dm.imbc.com/cash/main.aspx?";	
// 		}
		else if (0 == m_cUserType.compare("MGAME"))
		{
			m_wsCashShopWebUrl	= L"http://dgm.mgame.com/shop/main.aspx";
			m_wsNoticeWebUrl	= L"http://dgm.mgame.com/news/main.aspx";
			m_wsWebCashPageUrl	= L"http://dgm.mgame.com/shop/main.aspx";
			m_wsHomePageWebUrl	= L"http://dgm.mgame.com/index.aspx";	
			m_cCashChargeUrl	= "http://dgm.mgame.com/cash/main.aspx?";			
		}
		else if( 0 == m_cUserType.compare( "HG" ) )
		{
			m_wsCashShopWebUrl	= L"http://dm.hangame.com/shop/main.aspx";
			m_wsNoticeWebUrl	= L"http://dm.hangame.com/news/main.aspx";
			m_wsWebCashPageUrl	= L"http://dm.hangame.com/shop/main.aspx";
			m_wsHomePageWebUrl	= L"http://dm.hangame.com/index.aspx";	
			m_cCashChargeUrl	= "http://dm.hangame.com/cash/main.aspx?";			
		}
*/
	}

	std::string	CGlobalData::GetUserType(void) const	{	return m_cUserType;	}

	bool CGlobalData::IsLoginSkiped(void) const
	{
#ifdef _GIVE
		switch( m_nConnectionCountryType )
		{
		case eCountry_Kor:			
		case eCountry_Aeria:		
		case eCountry_Steam:		
			return true;
		case eCountry_Hongkong:	
		case eCountry_Thailand:	
		case eCountry_Taiwan:		
		case eCountry_GSP:			
			return false;
		default:
			return false;
		}
#else
		switch( m_nConnectionCountryType )
		{
		case eCountry_Steam:		
			return true;
		default:
			return false;
		}
#endif
	}

	bool CGlobalData::IsChannelingService(void) const
	{
#ifdef _GIVE
		switch( m_nConnectionCountryType )
		{
		case eCountry_Kor:
			{
				if( 0 == m_cUserType.compare( "ZAM" ) ||
					0 == m_cUserType.compare( "DMOM" ) ||
					0 == m_cUserType.compare( "DIGI" ) )
					return false;		
				return true;	
			}
			break;
		default:
			return false;
		}	
#else
		return false;
#endif
	}


	eCountryType CGlobalData::GetConnectionCountryType( void ) const	{	return m_nConnectionCountryType;	}
	std::string CGlobalData::GetLoadTableFile(void) const				{	return m_LoadTableFileName;	}
	std::string CGlobalData::GetLoadResourceFile(void) const			{	return m_LoadResourceFileName;	}
	void CGlobalData::SetAuthCode(std::string const& authCode)			{	m_Auth_Code = authCode;	}
	std::string CGlobalData::GetAuthCode(void) const					{	return m_Auth_Code;	}
	void CGlobalData::SetAccountID(std::string const& auID)				{	m_strAccountID = auID;	}
	std::string	CGlobalData::GetAccountID(void) const					{	return m_strAccountID;	}	
	void CGlobalData::SetAccountPW(std::string const& auPW)				{	m_strAccountPW = auPW;	}
	std::string CGlobalData::GetAccountPW(void) const					{	return m_strAccountPW;	}

	std::string CGlobalData::GetAccessToken(void) const					{	return m_cAccessToken;	}
	void CGlobalData::SetAccessToken(std::string const& acctoken)		{	m_cAccessToken = acctoken;	}
	std::string CGlobalData::GetMailAddress(void) const					{	return m_cMailAddress;	}
	void CGlobalData::SetMailAddress(std::string const& mailAddress)	{	m_cMailAddress = mailAddress;	}
	std::string CGlobalData::GetTicketingtime(void) const				{	return m_cTicketingtime;}
	void CGlobalData::SetTicketingtime(std::string const& ticketingTime){	m_cTicketingtime = ticketingTime;}

	std::wstring CGlobalData::GetAccountIDW(void) const
	{
		std::wstring accountIDw;
		DmCS::StringFn::From( accountIDw, m_strAccountID );
		return accountIDw;
	}

	void CGlobalData::ResetAccountInfo(void)
	{
		m_strAccountID.clear();
		m_strAccountPW.clear();
		m_str2ndPass.clear();
		//m_bAutoLogin = false;
	}

	std::string	CGlobalData::GetCountryName(void) const
	{
		switch( m_nConnectionCountryType )
		{
		case eCountry_None:		return "None";
		case eCountry_Kor:		return "Korea";
		case eCountry_GSP:		return "GSP";
		case eCountry_Aeria:	return "Aeria";
		case eCountry_Steam:	return "Steam";
		case eCountry_Hongkong:	return "Hongkong";
		case eCountry_Thailand:	return "Thailand";
		case eCountry_Taiwan:	return "Taiwan";
		}
		return "";
	}

	void CGlobalData::Set2ndPassType( e2ndPassType nType )				{	m_n2ndPassType = nType;	}
	e2ndPassType CGlobalData::Get2ndPassType() const					{	return m_n2ndPassType;	}
	bool CGlobalData::Is2ndPassType( e2ndPassType nType )				{	return m_n2ndPassType == nType;	}
	void CGlobalData::Set2ndPassword( std::string const& strPass )		{	m_str2ndPass = strPass;	}
	std::string	CGlobalData::Get2ndPassword(void) const					{	return m_str2ndPass;	}
	std::wstring CGlobalData::GetCashShopWebUrl(void) const				{	return m_wsCashShopWebUrl;	}
	std::wstring CGlobalData::GetNoticeWebUrl(void) const				{	return m_wsNoticeWebUrl;	}
	std::string	CGlobalData::GetCashChargeUrl(void) const				
	{
		switch( m_nConnectionCountryType )
		{
		case GData::eCountry_GSP:
		case GData::eCountry_Hongkong:
		case GData::eCountry_Taiwan:
			{
				std::string value;
				DmCS::StringFn::FormatA( value, m_cCashChargeUrl.c_str(), m_strAccountID.c_str(), m_cMailAddress.c_str(), m_cTicketingtime.c_str(), m_cAccessToken.c_str() );
				return value;
			}break;
		}
		return m_cCashChargeUrl;	
	}
	std::wstring CGlobalData::GetWebCashPageUrl(void) const				{	return m_wsWebCashPageUrl;	}
	std::wstring CGlobalData::GetHomePageUrl(void) const				{	return m_wsHomePageWebUrl;	}
	void CGlobalData::ClearServerListInfo()								{	m_ServerDataList.clear();	}

	void CGlobalData::AddServerListInfo( int const& nServerIdx, 
										 std::wstring const& serverName, 
										 unsigned short const& nHaveCharacterCount, 
										 unsigned short const& nMaxCharacterCount,
										 unsigned short const& nOpenedSlotCount )
	{
			sServerDataInfo add;
			add.m_nServerIDX = nServerIdx;
			add.m_nHaveCharacterCount = nHaveCharacterCount;
			add.m_ServerName = serverName;
			add.m_nMaxCharacterSlotCount = nMaxCharacterCount;
			add.m_nLockSlotCount = nMaxCharacterCount - nOpenedSlotCount;
			m_ServerDataList.push_back( add );
	}

	void CGlobalData::SetConnetServerInfo( int const& nServerIdx, std::wstring const& serverName )
	{
		m_ConnetServerInfo.m_nServerIDX = nServerIdx;
		m_ConnetServerInfo.m_ServerName = serverName;
	}

	int	CGlobalData::GetCurrentServerIDX() const			{	return m_ConnetServerInfo.m_nServerIDX;	}
	std::wstring CGlobalData::GetCurrentServerName() const	{	return m_ConnetServerInfo.m_ServerName;	}

	int CGlobalData::GetEmptySlotCountFromServer( int const& nServerIDX ) const
	{
		std::list<sServerDataInfo>::const_iterator it = m_ServerDataList.begin();
		for( ; it != m_ServerDataList.end(); ++it )
		{
			if( (*it).m_nServerIDX == nServerIDX )
				return (*it).m_nMaxCharacterSlotCount - ((*it).m_nLockSlotCount + (*it).m_nHaveCharacterCount);
		}
		return 0;
	}

	std::list<sServerDataInfo> const& CGlobalData::GetServerListInfo() const	{	return m_ServerDataList;	}

	int CGlobalData::GetCurrentServerLockSlotCount() const
	{
		std::list<sServerDataInfo>::const_iterator it = m_ServerDataList.begin();
		for( ; it != m_ServerDataList.end(); ++it )
		{
			if( (*it).m_nServerIDX == m_ConnetServerInfo.m_nServerIDX )
				return (*it).m_nLockSlotCount;
		}
		return 0;
	}

	void CGlobalData::GetServerSlotCountInfo( int const& nServerIDX, int & nOpenSlotCount, int & nLockSlotCount)
	{
		nOpenSlotCount = 0;		nLockSlotCount = 0;
		std::list<sServerDataInfo>::const_iterator it = m_ServerDataList.begin();
		for( ; it != m_ServerDataList.end(); ++it )
		{
			if( (*it).m_nServerIDX == nServerIDX )
			{
				nOpenSlotCount = (*it).m_nMaxCharacterSlotCount - (*it).m_nLockSlotCount;
				nLockSlotCount = (*it).m_nLockSlotCount;
				return;
			}
		}
	}

	void CGlobalData::_SetAccountIP_Port( eCountryType const& eType )
	{
		if (eType == eCountry_GSP)
			m_Account_IP = cNetBase::GetIP(ACCOUNT_IP);
		else
			m_Account_IP = cNetBase::GetIP("127.0.0.1");
			m_Account_Port = ACCOUNT_PORT;
	}

	void CGlobalData::ToggleLiveServer()
	{
		
		if (m_Account_Port == 7029) {

			m_Account_IP = cNetBase::GetIP("127.0.0.1");
			m_Account_Port = 7029;
			
		}
		else {
			m_Account_IP = cNetBase::GetIP("127.0.0.1");
			m_Account_Port = 7029;
		
		}

		bWasManuallySet = true;	
	}

	void CGlobalData::LoadInfo_AccountIP_Port()
	{
		#ifdef USE_DMO_INI
			Csini x("./__ChangeIP.ini");
			if (x.IsFile() && (x.GetStr("network", "ip") != NULL))
			{
				m_Account_IP = cNetBase::GetIP(x.GetStr("network", "ip"));
				m_Account_Port = x.GetInt("network", "port");
			}
			else
				_SetAccountIP_Port(m_nConnectionCountryType);
		#else
				_SetAccountIP_Port(m_nConnectionCountryType);
		#endif
	}

	void CGlobalData::SetDownloadPatchVersion( int const& nVersion )
	{
		m_nDownLoadVersion = nVersion;
	}

	int CGlobalData::GetDownloadPatchVersion() const
	{
		return m_nDownLoadVersion;
	}

}
