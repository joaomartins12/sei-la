#pragma once

namespace TB
{
	struct sXAIStruct
	{
		sXAIStruct():s_unMaxXG(0),s_ucMaxCrystal(0)
		{}

		unsigned int	s_unMaxXG;
		unsigned char	s_ucMaxCrystal;
	};

	typedef std::map< DWORD, sXAIStruct >					MAP_XAIITEM_DATA;
	typedef std::map< DWORD, sXAIStruct >::iterator			MAP_XAIITEM_DATA_IT;
	typedef std::map< DWORD, sXAIStruct >::const_iterator	MAP_XAIITEM_DATA_CIT;
}