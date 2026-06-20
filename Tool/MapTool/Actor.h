
#pragma once 

#include "Actor_Char.h"

class cActor
{
public:
	cActor();

protected:
	bool				m_bEnableActor;
	float				s_fMoveSpeedRate;
	CDigimon			m_Digimon;
	CTamer				m_Tamer;

public:
	void				Delete();
	void				Init();

	void				ReleaseVisible();
	void				ResetVisible( NiPoint3 pos );

	void				Update();
	void				Render();	

public:
	void				SetMoveDest( NiPoint3 pos );
	NiPoint3			GetPos(){ return m_Tamer.GetPos(); }
	void				SetMoveRate(float fRate);

	CTamer*				GetTamer(){ return &m_Tamer; }

	//========== State
public:
	bool				IsEnableActor(){ return m_bEnableActor; }
};

extern cActor	g_Actor;
