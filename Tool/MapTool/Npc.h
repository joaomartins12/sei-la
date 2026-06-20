

#pragma once

class CNpc : public CsC_AvObject
{
	THREAD_MEMPOOL_H( CNpc )
	CsCRTTI_H

public:
	virtual void	Init( UINT nIDX, UINT nFileTableID, NiPoint3 vPos, float fRot, sCREATEINFO* pProp );


protected:
	CsNpc*			m_pNpcFT;

public:
	CsNpc*			GetNpcFT(){ return m_pNpcFT; }
	virtual UINT	GetModelID() const;
};