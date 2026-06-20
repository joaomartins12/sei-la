#pragma once

class CEventCall : public CEventCall_Base
{
public:
	CEventCall(void);
	~CEventCall(void);

public:	
	virtual void	PlaySound( char* szSound );
	virtual void	PlayMusic( char* szMusic );

	virtual void	EventTalk( UINT nTargetUID, TCHAR* szMsg, bool bExistDelete, int ObjType );
};
