
#pragma once 

namespace DTALK
{
	enum eTYPE
	{
		TALK_DROP				= 11011,
		TALK_NONDROP			= 11012,
		TALK_DIE				= 12001,
		TALK_HP					= 13000,
		TALK_SKILL				= 13002,
		TALK_SKILLDS			= 13003,
		TALK_SKILLPOINT 		= 13004,
		TALK_EVOLDS				= 13005,
		TALK_QUESTCOM			= 14005,
		TALK_ITEMGET			= 18001,
		TALK_LEVELUP			= 19001,
		TALK_SKILLGET			= 30003,			
		TALK_SKILLUSE			= 30004,
		TALK_ATTACK				= 30005,
		TALK_EVO				= 30010,
		TALK_ITEMUSENO			= 30011,
		TALK_ITEMDIST			= 30013,
		TALK_MOVENOT			= 30016,
		TALK_DIGITALWORLD		= 40000,
		TALK_ELE_ONEWIN			= 50000,
		TALK_ELE_WIN			= 50001,
		TALK_ELE_ONELOSE		= 50002,
		TALK_ELE_LOSE			= 50003,
		TALK_LEV_WIN			= 50004,
		TALK_LEV_LOSE			= 50005,
		TALK_QUEST_REC1			= 50006,
		TALK_QUEST_REC2			= 50007,
		TALK_GOODITEM			= 50008,
		TALK_WAIT1				= 50009,
		TALK_WAIT2				= 50010,
		TALK_WAIT3				= 50011,
		TALK_NPCFIND_START		= 50012,
		TALK_NPCFIND_DIST		= 50013,
		TALK_NPCFIND_END		= 50014,
		TALK_TIRE_0				= 50015,
		TALK_TIRE_40			= 50016,
		TALK_TIRE_80			= 50017,
		TALK_TIRE_MAX			= 50018,
		TALK_TIRE_RISE			= 50019,			
	};
}

#define MAX_TALK	100
#define MAX_LEN		128
class cDigimonTalk_Resist
{
public:
	struct sResist
	{
		int				s_nKey;		
		TCHAR			s_szTalk[ MAX_LEN ];
		int				s_nPersent;
	};

	sResist			m_DigimonTalk[ MAX_TALK ];

public:
	void		Reset();	

	bool		SaveTalk( DWORD dwTalk, TCHAR* szTalk, int nPer );	
	sResist*	GetTalk( DWORD dwTalk );	
};