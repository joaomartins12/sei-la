
#pragma once

class cMoveNpc : public CsC_AvObject
{
	CsCRTTI_H

public:
	cMoveNpc(){}
	virtual ~cMoveNpc(){}


	virtual UINT		GetModelID(){ return nsCsFileTable::g_pNpcMng->GetNpc( GetFTID() )->GetInfo()->s_dwModelID; }
};
class cMoveMonster : public CsC_AvObject
{
	CsCRTTI_H

public:
	cMoveMonster(){}
	virtual ~cMoveMonster(){}


	virtual UINT		GetModelID(){ return nsCsFileTable::g_pMonsterMng->GetMonster( GetFTID() )->GetInfo()->s_dwModelID; }
};
