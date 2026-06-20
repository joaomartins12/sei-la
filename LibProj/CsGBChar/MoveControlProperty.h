
#pragma once 

class CsC_MoveControlProp : public CsC_Property
{
	THREAD_MEMPOOL_H( CsC_MoveControlProp )

public:
	CsC_MoveControlProp();
	~CsC_MoveControlProp();


	virtual void			Delete();
	bool					IsDelete();

	void					Init( CsC_AvObject* pParent );
	void					ResetAccumTime();

	void					PreUpdate( float fDeltaTime );

protected:
	CsMoveObject*			m_pFTMoveObject;
	float					m_fAccumTime;
	bool					m_bMove;

public:
	void					SetFTMoveObject( CsMoveObject* pFTMO );	
	void					SetFTEventMoveObject( CsMoveObject* pFTMO );
	CsMoveObject*			GetFTMoveObject() { return m_pFTMoveObject; }
	float					GetAccumTime(){ return m_fAccumTime; }
	void					SetAccumTime( float fTime ) { m_fAccumTime = fTime; }
	bool					IsMove() { return m_bMove; }
};