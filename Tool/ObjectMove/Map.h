
#pragma once 

#include "MoveMng.h"

class cMap
{
public:
	static void		GlobalShotDown();
	static void		GlobalInit();
public:
	void			Delete();
	void			Init();

	void			LoadMap( int nMapID );

	void			Update();
	void			Render();

	void			ResetDevice();	

public:
	void			SetCameraAccel(){ _SetCameraAccel( IsCTRLPressed() ); }
protected:	
	void			_SetCameraAccel( bool bSpeedUp );


protected:
	cMoveMng		m_MoveMng;
	DWORD			m_CurMapID;
public:
	cMoveMng*		GetMoveMng(){ return &m_MoveMng; }
	DWORD			GetCurMapID()	{ return m_CurMapID; }
};

extern CsGBTerrainMng*		g_pTerrainMng;