
#pragma once

class cModel_Npc : public cModel_Base
{
public:
	cModel_Npc();
protected:
	NiPoint2			m_ptInitPos;

	DWORD				m_dwNpcID;

public:
	virtual void	Delete();
	void			Init( DWORD dwNpcID, cModel_Res* pModelRes );

	cModel_Npc*		Clone( bool bResist );
	
	void		SetTransform( NiTransform tr );
	void		SetPos( NiPoint3 pos );

	void		SetInitPos( NiPoint2 pos ){ m_ptInitPos = pos; }
	NiPoint2	GetInitPos(){ return m_ptInitPos; }

	void		CalInitPos();

	DWORD		GetNpcID(){ return m_dwNpcID; }
	void		SetNpcID( DWORD dwNpcID ){ m_dwNpcID = dwNpcID; }

	void		SetSequence( DWORD dwSequ ){ m_pAM->SetTargetAnimation( dwSequ ); }
};

