
#pragma once

#include "ModelBase.h"
#include "ModelRes.h"
#include "ModelNpc.h"

#include "Npc.h"

class cModelMng
{
public:
	cModelMng();

public:
	void				Delete();
	void				Init();

	void				PostViewReset();
	void				PreViewDelete();

	void				HideModelTab();

	void				Update();
	void				Render();

	void				PreSave( const char* cFile, bool bAutoSave );

	//====== ¸ðµ¨ ºä
public:	
	void				SetModelView( DWORD dwNpcID );

protected:
	bool					m_bRenderModelNpc;

	CNpc*					m_pModelNpc;
	CNpc*					m_pSelectNpc;
	std::map< UINT, CNpc* >	m_mapRegistNpc;
	

protected:
	void				_DeleteNpc();

	void				_DeleteResistNpc( DWORD dwNpcID );
	CNpc*				_AddResistNpc( DWORD dwFTID, NiPoint3 ptPos, float fRot, DWORD dwAniID, bool bResistNpc );
	CNpc*				_AddResistNpc( DWORD dwFTID, NiPoint2 ptInitPos, float fRot, DWORD dwAniID = ANI::IDLE_NORMAL );	

public:
	void				_SetSelectNpc( UINT nIDX );

protected:
	void				_MoveModelNpc( CPoint pt );

	void				_PickNpcModel( CPoint pt );

	void				_UpdateNpc();
	void				_RenderNpc();

	void				_MouseMove_Npc( CPoint pt );
	void				_LButtonDown_Npc( CPoint pt );

public:
	void				AddModelNpc( DWORD dwFTID, CNpc* pSrcNpc );
	void				ResetResistMapNpc();

public:
	void				DeleteResistNpc();
	void				DeleteSelectNpc();
	void				DeleteModelNpc();

	void				ModelNpcToSelectNpc();
	void				SelectNpcToResistNpc();
	void				SelectNpcToModelNpc();

	void				SelectNpc_ApplyTransform( NiTransform tr );

	void				RotationModelNpcOnKey( bool bPlus );

	bool				IsResistNpc( DWORD dwNpcID );
	CNpc*				GetModelNpc(){ return m_pModelNpc; }
	CNpc*				GetSelectNpc(){ return m_pSelectNpc; }


	// ======== Mouse
public:
	void				LButtonDown( CPoint pt );
	void				MouseMove( CPoint pt );

public:	
	void				ApplyLight( NiLight* pLight );
	void				RemoveLight( NiLight* pLight );
};

extern cModelMng		g_ModelMng;