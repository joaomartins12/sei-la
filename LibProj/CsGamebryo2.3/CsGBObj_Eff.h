
#pragma once 

class CsGBObj_Eff : public CsGBObject
{
	CSGBMEMPOOL_H( CsGBObj_Eff );

public:
	CsGBObj_Eff() {};
	~CsGBObj_Eff() {};

	virtual eOBJ_TYPE	GetType(){ return OT_EFFECT; }

};

