

#pragma once 

class CsQuestApplyRequite
{
public:
	enum eTYPE{	ITEM = 0, /*TIME*/ };
	typedef	std::list< CsQuestApplyRequite* >			LIST;
	typedef	std::list< CsQuestApplyRequite* >::iterator	LIST_IT;
public:
	CsQuestApplyRequite() {}
	~CsQuestApplyRequite(){ Delete(); }

protected:
	eTYPE			m_eType;
	UINT			m_nTypeID;
	int				m_nTypeCount;

public:
	void			Delete(){}
	void			Init( eTYPE type, UINT nTypeID, int nTypeCount );
	void			Reset(){ m_eType = CsQuestApplyRequite::ITEM; m_nTypeID = 0; m_nTypeCount = 0; }

	void			SetType( eTYPE type ){ m_eType = type;}
	void			SetTypeID( UINT nTypeID ){ m_nTypeID = nTypeID; }
	void			SetTypeCount( int nTypeCount ){ m_nTypeCount = nTypeCount; }

	eTYPE			GetType(){ return m_eType; }
	UINT			GetTypeID(){ return m_nTypeID; }	
	int				GetTypeCount(){ return m_nTypeCount; }

	CsQuestApplyRequite*	Clone();
};

