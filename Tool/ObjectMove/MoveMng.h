
#pragma once 

#include "MoveNpc.h"

enum eRTTI_MOTOOL
{
	RTTI_MOVE_NPC = RTTI_LIB_MAXCOUNT,
	RTTI_MOVE_MON
};

class cMoveMng
{
public:
	cMoveMng();


public:
	void			Delete();
	void			Init();

	void			DeleteObject();	
	CsC_AvObject*	LoadNpc( DWORD dwNpcID );
	CsC_AvObject*	LoadMonster( DWORD dwMonID );
	void			Update();
	void			Render();

	void			OnLButtonDown( CsPoint pt );

public:
	void			Set_PickPos( bool bEnable, CsPoint pt );


protected:
	struct sSIGN
	{
		sSIGN(){ s_bView = false; }
		void	Delete(){ s_Node.Delete(); }
		void	SetTranslate( NiPoint3 vPos );
		void	SetTranslate( int x, int y );
		void	Update();
		void	UpdateRender();
		CsNodeObj		s_Node;
		bool			s_bView;
	};
	
	sSIGN				m_PickSign;
	sSIGN				m_CheckPointSign;
	sSIGN				m_ObjectCurPosSign;
	CsC_AvObject*		m_pMoveObject;


public:
	CsC_AvObject*		GetMoveObject(){ return m_pMoveObject; }
};