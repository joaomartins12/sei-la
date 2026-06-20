#pragma once

#ifdef VERSION_KOR
#define VERSION_KOR_VALUE 11051401
const unsigned int g_nManagerVersion = VERSION_KOR_VALUE;
#elif defined VERSION_USA
#define VERSION_USA_VALUE 10051401
const unsigned int g_nManagerVersion = VERSION_USA_VALUE;
#elif defined VERSION_TH
#define VERSION_TH_VALUE 19030801
const unsigned int g_nManagerVersion = VERSION_TH_VALUE;
#elif defined VERSION_HK
#define VERSION_HK_VALUE 19041001
const unsigned int g_nManagerVersion = VERSION_HK_VALUE;
#elif defined VERSION_TW
#define VERSION_TW_VALUE 19042401
const unsigned int g_nManagerVersion = VERSION_TW_VALUE;
#else
const unsigned int g_nManagerVersion = 12102301;
#endif

namespace cManagerVer
{
	inline unsigned int GetVersion()
	{
		return unsigned int(g_nManagerVersion / 100);
	}
	inline unsigned int GetPatch()
	{
		return (g_nManagerVersion % 100);
	}

	inline bool isVersion(unsigned int version)
	{
		return GetVersion() == unsigned int(version / 100);
	}
	inline bool isPatch(unsigned int version)
	{
		return GetPatch() == (version % 100);
	}

	inline bool isVersion(unsigned int oldVersion, unsigned int newVersion)
	{
		return unsigned int(oldVersion / 100) == unsigned int(newVersion / 100);
	}
	inline bool isPatch(unsigned int oldVersion, unsigned int newVersion)
	{
		return unsigned int(oldVersion % 100) == unsigned int(newVersion % 100);
	}
}