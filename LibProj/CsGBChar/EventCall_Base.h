#pragma once

class CEventCall_Base : public NiMemObject
{
public:
	CEventCall_Base();
	virtual ~CEventCall_Base();

public:
	virtual void	EventStart() { assert_cs( false ); }
	virtual void	EventEnd()	 { assert_cs( false ); }

	virtual void	PlaySound( char* szSound ) { assert_cs( false ); } 
	virtual void	PlayMusic( char* szMusic ) { assert_cs( false ); }

	virtual void	EventTalk( UINT nTargetUID, TCHAR* szMsg, bool bExistDelete, int ObjType ) { assert_cs( false ); }
	virtual void    EventSave( int nSceneNum ) { assert_cs( false ); } 
};

extern CEventCall_Base* g_pEventCall;



