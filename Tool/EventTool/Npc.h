
#pragma once 

class CNpc		:	public	CsC_AvObject
{
	THREAD_MEMPOOL_H( CNpc )
	CsCRTTI_H

public:
	CNpc();

public:
	virtual void		Delete();
	virtual void		PreDelete();
	virtual void		Init( uint nUID, uint nFileTableID, sCREATEINFO* pProp );
	virtual void		Init_NoMapFT( uint nUID, uint nFileTableID, sCREATEINFO* pProp );

	virtual void		Update( float fDeltaTime );	
	virtual void		Render();	

	virtual void		ThreadCallBack_LoadKFM( sLOAD_KFM* pLoad );


	//====================================================================================================
	// 파일 테이블
	//====================================================================================================
protected:
	CsMapNpc*		m_pFT_MapNpc;
	CsNpc*			m_pFT_Npc;
public:
	CsNpc*			GetFTNpc(){ return m_pFT_Npc; }


};