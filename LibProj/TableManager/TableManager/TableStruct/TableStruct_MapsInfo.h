#pragma once

BEGIN_NAME_TABLE

struct sMapLimitInfo
{
	sMapLimitInfo():s_nShoutSec(0), s_bEnableCheckMacro(true)
	{}
	
	int		s_nShoutSec;
	bool	s_bEnableCheckMacro;
};

END_NAME_TABLE
