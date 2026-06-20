
#pragma once 

class CsQuestRequite
{
public:
	enum eMETHOD{ RM_GIVE = 0, RM_SEL1, };
	enum eTYPE{	MONEY = 0, EXP, ITEM, EVOSLOT};
	typedef	std::list< CsQuestRequite* >				LIST;
	typedef	std::list< CsQuestRequite* >::iterator	LIST_IT;

public:
	CsQuestRequite() {}
	~CsQuestRequite(){ Delete(); }

protected:
	eMETHOD			m_eMethod;
	eTYPE			m_eType;
	UINT			m_nTypeID;
	int				m_nTypeCount;

public:
	void			Delete(){}
	void			Init( eMETHOD method, eTYPE type, UINT nTypeID, int nTypeCount );
	void			Reset(){ m_eMethod = RM_GIVE; m_eType = MONEY; m_nTypeID = 0; m_nTypeCount = 0; }

	void			SetMethod( eMETHOD method ){ m_eMethod = method;}
	void			SetType( eTYPE type ){ m_eType = type;}
	void			SetTypeID( UINT nTypeID ){ m_nTypeID = nTypeID; }
	void			SetTypeCount( int nTypeCount ){ m_nTypeCount = nTypeCount; }	

	eMETHOD			GetMethodID(){ return m_eMethod; }
	eTYPE			GetType(){ return m_eType; }
	UINT			GetTypeID(){ return m_nTypeID; }	
	int				GetTypeCount(){ return m_nTypeCount; }

	CsQuestRequite*	Clone();
};
