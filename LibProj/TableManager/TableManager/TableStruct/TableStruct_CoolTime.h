
#pragma once 

BEGIN_NAME_TABLE

struct sItemCoolTime
{
	sItemCoolTime():s_bNetwork(false),s_fTimeSec(0) {}
	
	bool	s_bNetwork;
	double	s_fTimeSec;
};

END_NAME_TABLE