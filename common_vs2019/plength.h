#pragma once

#include "nlib/base.h"
#include "pCountry.h"
#include <vector>
#include <string>
#include <wchar.h>


namespace Language
{

namespace pLength
{
	enum
	{
		str				= 255,

		SecondPass		= Language::pCountry::SizeSecondPass,
		MonsterName		= 63,
		SkillName		= 32,

		ip				= 16,
#if defined VERSION_TH
		map_name		= 64,
#else
		map_name		= 32,
#endif
		Md5				= 33,
		Comment			= 32,

		id				= Language::pCountry::SizeID,
		pass			= Language::pCountry::SizePass,
		AccessToken		= Language::pCountry::SizeAccessToken,

		name			= Language::pCountry::SizeCharName,		

		GuildName		= Language::pCountry::SizeGuildName,
		GuildNotice		= Language::pCountry::SizeGuildNotice,
		GuildClassName	= Language::pCountry::SizeGuildClassName,
		GuildClassNameDesc= Language::pCountry::SizeGuildClassNameDesc,
		GuildMemo		= Language::pCountry::SizeGuildMemo,
		ShopName		= Language::pCountry::SizeShopName,

		BudyMemo		= Language::pCountry::SizeBuddyMemo,	

		email			= 128,
		SteamToken		= 1024,

		COUNTRY			= 3,
	};

}; // namespace pLength


namespace nCode
{
	enum
	{
		none,		// 1byte, 허용하지 아니한 특수 문자 포함
		special,	// 1byte special code
		special2,	// 2byte special code
		special3,	// 2byte special code
		space,		// 스페이스 

		reserve1,	// 특수문자 예약공간
		reserve2,	// 특수문자 예약공간
		reserve3,	// 특수문자 예약공간
		reserve4,	// 특수문자 예약공간
		reserve5,	// 특수문자 예약공간

		english,	// 1byte
		hangle,		// 2byte
		hanja,		// 2byte
		thai,		// 태국 문자	
	};
};


	// 국가별 허용 문자정의
#if defined VERSION_HK
	//id 허용문자
	const int nCodeID[] = {nCode::english, nCode::special};

	//패스워드 허용문자
	const int nCodePW[] = {nCode::english, nCode::special, nCode::special2, nCode::special3};

	//테이머 혹은 디지몬이름 허용문자
	const int nCodeName[] = {nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::special3, nCode::space};

	//길드이름 허용문자
	const int nCodeGuildName[] = {nCode::none, nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::special3, nCode::space};

	//길드 클래스이름 허용문자
	const int nCodeGuildClassName[] = {nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::space};	

	//샵이름 허용문자
	const int nCodeShopName[] = {nCode::none, nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::special3, nCode::space};

	//문자열 디폴트 허용문자
	const int nCodeDefultString[] = {nCode::none, nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::special3, nCode::space};

	//서버 이전시 테이머 이름
	const int nCodeRelocateTamerName[] = {nCode::english, nCode::hanja};
	
#elif defined VERSION_TW	
	const int nCodeID[] = {nCode::english, nCode::special};		
	const int nCodePW[] = {nCode::english, nCode::special, nCode::special2, nCode::special3};
	const int nCodeName[] = {nCode::english, nCode::hanja, nCode::hangle};
	const int nCodeGuildName[] = {nCode::none, nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::special3, nCode::space};
	const int nCodeGuildClassName[] = {nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::space};	
	const int nCodeShopName[] = {nCode::none, nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::special3, nCode::space};
	const int nCodeDefultString[] = {nCode::none, nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::special3, nCode::space};
	//서버 이전시 테이머 이름
	const int nCodeRelocateTamerName[] = {nCode::english, nCode::hanja};
#elif defined VERSION_TH	
	const int nCodeID[] = {nCode::english, nCode::special};
	const int nCodePW[] = {nCode::english, nCode::special, nCode::special2, nCode::special3};
	const int nCodeName[] = {nCode::english, nCode::hanja, nCode::hangle, nCode::thai, nCode::special, nCode::special2};
	const int nCodeGuildName[] = {nCode::none, nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::special3, nCode::space};
	const int nCodeGuildClassName[] = {nCode::none, nCode::english, nCode::hanja, nCode::hangle, nCode::thai, nCode::special, nCode::special2, nCode::space};	
	const int nCodeShopName[] = {nCode::none, nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::special3, nCode::space};
	const int nCodeDefultString[] = {nCode::none, nCode::english, nCode::hanja, nCode::hangle, nCode::thai, nCode::special, nCode::special2, nCode::special3, nCode::space};
	//서버 이전시 테이머 이름
	const int nCodeRelocateTamerName[] = {nCode::english, nCode::thai};
#elif defined VERSION_USA	
	const int nCodeID[] = {nCode::english, nCode::special, nCode::special2};				// USA는 이메일 인증가능
	const int nCodePW[] = {nCode::english, nCode::special, nCode::special2, nCode::special3};
	const int nCodeName[] = {nCode::english, nCode::special};
	const int nCodeGuildName[] = {nCode::none, nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::special3, nCode::space};
	const int nCodeGuildClassName[] = {nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::space};	
	const int nCodeShopName[] = {nCode::english, nCode::space};
	const int nCodeDefultString[] = {nCode::none, nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::special3, nCode::space};
	//서버 이전시 테이머 이름
	const int nCodeRelocateTamerName[] = {nCode::english};
#elif defined VERSION_KOR	
	const int nCodeID[] = {nCode::english, nCode::special};		
	const int nCodePW[] = {nCode::english, nCode::special, nCode::special2, nCode::special3};
	const int nCodeName[] = {nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::special3};
	const int nCodeGuildName[] = {nCode::none, nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::special3, nCode::space};
	const int nCodeGuildClassName[] = {nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::space};	
	const int nCodeShopName[] = {nCode::none, nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::special3, nCode::space};
	const int nCodeDefultString[] = {nCode::none, nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::special3, nCode::space};
	//서버 이전시 테이머 이름
	const int nCodeRelocateTamerName[] = {nCode::english, nCode::hangle};
#else	
	const int nCodeID[] = {nCode::english, nCode::special};		
	const int nCodePW[] = {nCode::english, nCode::special, nCode::special2, nCode::special3};
	const int nCodeName[] = {nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::special3};
	const int nCodeGuildName[] = {nCode::none, nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::special3, nCode::space};
	const int nCodeGuildClassName[] = {nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::space};	
	const int nCodeShopName[] = {nCode::none, nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::special3, nCode::space};
	const int nCodeDefultString[] = {nCode::none, nCode::english, nCode::hanja, nCode::hangle, nCode::special, nCode::special2, nCode::special3, nCode::space};
	//서버 이전시 테이머 이름
	const int nCodeRelocateTamerName[] = {nCode::english};
#endif	

	enum
	{
		SUCCEEDED,
		STRING_EMPTY,
		STRING_SIZE_MIN,
		STRING_SIZE_MAX,
		STRING_UNKNOWN_TYPE,
	};

	int CheckUnicodeChar(wchar c, wchar nc); // nc is next char
	template<size_t N>
	inline int CheckMultibytes(std::string &str, int min, int max, const int (&allowtype)[N], int characterset)
	{
		bool charCheck = true;
		std::vector<int> characterType;
		for (int i = 0; i < N; ++i)
		{
			if (nCode::none == allowtype[i])
			{
				charCheck = false;
				break;
			}
			characterType.push_back(allowtype[i]);
		}

		return __CheckMultibytes(str, min, max, characterType, characterset, charCheck);
	}
	template<size_t N>
	inline int CheckUnicode(std::wstring &str, int min, int max, const int (&allowtype)[N], int characterset)
	{
		bool charCheck = true;
		std::vector<int> characterType;
		for (int i = 0; i < N; ++i)
		{
			if (nCode::none == allowtype[i])
			{
				charCheck = false;
				break;
			}
			characterType.push_back(allowtype[i]);
		}
		return __CheckUnicode(str, min, max, characterType, characterset, charCheck);
	}
	int __CheckMultibytes(std::string &str, int min, int max, std::vector<int> &allowtype, int characterset, bool charCheck);
	int __CheckUnicode(std::wstring &str, int min, int max, std::vector<int> &allowtype, int characterset, bool charCheck);	

	inline int CheckID(std::string &val)
	{
		return CheckMultibytes(val, pCountry::MIN_LenID, pCountry::MAX_LenID, nCodeID, pCountry::CHARACTER_SET);
	}

	inline int CheckID(std::wstring &val)
	{
		return CheckUnicode(val, pCountry::MIN_LenID, pCountry::MAX_LenID, nCodeID, pCountry::CHARACTER_SET);
	}

	inline int CheckPw(std::string &val)
	{		
		return CheckMultibytes(val, pCountry::MIN_LenPass, pCountry::MAX_LenPass, nCodePW, pCountry::CHARACTER_SET);
	}

	inline int CheckPw(std::wstring &val)
	{
		return CheckUnicode(val, pCountry::MIN_LenPass, pCountry::MAX_LenPass, nCodePW, pCountry::CHARACTER_SET);
	}

	inline int CheckTamerName(std::string &val)
	{		
		return CheckMultibytes(val, pCountry::MIN_LenCharName, pCountry::MAX_LenCharName, nCodeName, pCountry::CHARACTER_SET);
	}

	inline int CheckTamerName(std::wstring &val)
	{
		return CheckUnicode(val, pCountry::MIN_LenCharName, pCountry::MAX_LenCharName, nCodeName, pCountry::CHARACTER_SET);
	}

	inline int CheckDigimonName(std::string &val)
	{
		return CheckMultibytes(val, pCountry::MIN_LenCharName, pCountry::MAX_LenCharName, nCodeName, pCountry::CHARACTER_SET);
	}

	inline int CheckDigimonName(std::wstring &val)
	{
		return CheckUnicode(val, pCountry::MIN_LenCharName, pCountry::MAX_LenCharName, nCodeName, pCountry::CHARACTER_SET);
	}

	inline int CheckGuildName(std::string &val)
	{
		return CheckMultibytes(val, pCountry::MIN_LenGuildName, pCountry::MAX_LenGuildName, nCodeGuildName, pCountry::CHARACTER_SET);
	}

	inline int CheckGuildName(std::wstring &val)
	{
		return CheckUnicode(val, pCountry::MIN_LenGuildName, pCountry::MAX_LenGuildName, nCodeGuildName, pCountry::CHARACTER_SET);
	}

	inline int CheckGuildNotice(std::string &val)
	{
		return CheckMultibytes(val, pCountry::MIN_LenGuildNotice, pCountry::MAX_LenGuildNotice, nCodeDefultString, pCountry::CHARACTER_SET);
	}

	inline int CheckGuildNotice(std::wstring &val)
	{
		return CheckUnicode(val, pCountry::MIN_LenGuildNotice, pCountry::MAX_LenGuildNotice, nCodeDefultString, pCountry::CHARACTER_SET);
	}

	inline int CheckGuildClassName(std::string &val)
	{
		return CheckMultibytes(val, pCountry::MIN_LenGuildClassName, pCountry::MAX_LenGuildClassName, nCodeGuildClassName, pCountry::CHARACTER_SET);
	}

	inline int CheckGuildClassName(std::wstring &val)
	{
		return CheckUnicode(val, pCountry::MIN_LenGuildClassName, pCountry::MAX_LenGuildClassName, nCodeGuildClassName, pCountry::CHARACTER_SET);
	}

	inline int CheckGuildClassNameDesc(std::string &val)
	{
		return CheckMultibytes(val, pCountry::MIN_LenGuildClassNameDesc, pCountry::MAX_LenGuildClassNameDesc, nCodeDefultString, pCountry::CHARACTER_SET);
	}

	inline int CheckGuildClassNameDesc(std::wstring &val)
	{
		return CheckUnicode(val, pCountry::MIN_LenGuildClassNameDesc, pCountry::MAX_LenGuildClassNameDesc, nCodeDefultString, pCountry::CHARACTER_SET);
	}	

	inline int CheckGuildMemo(std::string &val)
	{
		return CheckMultibytes(val, pCountry::MIN_LenGuildNotice, pCountry::MAX_LenGuildNotice, nCodeDefultString, pCountry::CHARACTER_SET);
	}

	inline int CheckGuildMemo(std::wstring &val)
	{
		return CheckUnicode(val, pCountry::MIN_LenGuildNotice, pCountry::MAX_LenGuildNotice, nCodeDefultString, pCountry::CHARACTER_SET);
	}

	inline int CheckShopName(std::string &val)
	{
		return CheckMultibytes(val, pCountry::MIN_LenShopName, pCountry::MAX_LenShopName, nCodeShopName, pCountry::CHARACTER_SET);
	}

	inline int CheckShopName(std::wstring &val)
	{
		return CheckUnicode(val, pCountry::MIN_LenShopName, pCountry::MAX_LenShopName, nCodeShopName, pCountry::CHARACTER_SET);
	}

	inline int CheckBudyMemo(std::string &val)
	{
		return CheckMultibytes(val, pCountry::MIN_LenBuddyMemo, pCountry::MAX_LenBuddyMemo, nCodeDefultString, pCountry::CHARACTER_SET);
	}

	inline int CheckBudyMemo(std::wstring &val)
	{
		return CheckUnicode(val, pCountry::MIN_LenBuddyMemo, pCountry::MAX_LenBuddyMemo, nCodeDefultString, pCountry::CHARACTER_SET);
	}
	
	inline int CheckWhisper(std::string &val)
	{
		return CheckMultibytes(val, pCountry::MIN_LenMsg, pCountry::MAX_LenMsg, nCodeDefultString, pCountry::CHARACTER_SET);
	}
	
	inline int CheckWhisper(std::wstring &val)
	{
		return CheckUnicode(val, pCountry::MIN_LenMsg, pCountry::MAX_LenMsg, nCodeDefultString, pCountry::CHARACTER_SET);
	}
	
	inline int CheckMsg(std::string &val)
	{
		return CheckMultibytes(val, pCountry::MIN_LenMsg, pCountry::MAX_LenMsg, nCodeDefultString, pCountry::CHARACTER_SET);
	}
	
	inline int CheckMsg(std::wstring &val)
	{
		return CheckUnicode(val, pCountry::MIN_LenMsg, pCountry::MAX_LenMsg, nCodeDefultString, pCountry::CHARACTER_SET);
	}

	inline int CheckRelocateTamerName(std::string &val)
	{
		return CheckMultibytes(val, pCountry::MIN_LenCharName, pCountry::MAX_LenCharName, nCodeRelocateTamerName, pCountry::CHARACTER_SET);
	}

	inline int CheckRelocateTamerName(std::wstring &val)
	{
		return CheckUnicode(val, pCountry::MIN_LenCharName, pCountry::MAX_LenCharName, nCodeRelocateTamerName, pCountry::CHARACTER_SET);
	}
};