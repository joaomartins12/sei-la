

#pragma once 

class CsQuestApplyRequiteGroup
{
public:
	CsQuestApplyRequiteGroup() {}
	~CsQuestApplyRequiteGroup() {}

protected:
	CsQuestApplyRequite::LIST	m_List;

public:
	void			Delete();
	void			Init();

	void			Add( CsQuestApplyRequite::eTYPE type, UINT nTypeID, int nTypeCount );
	void			Delete( int nIndex );

	CsQuestApplyRequite*		Get( int nIndex );
	int							GetListCount(){ return (int)m_List.size(); }
	CsQuestApplyRequite::LIST*	GetList(){ return &m_List; }

	void			Clone( CsQuestApplyRequiteGroup* pSrcGroup );
};

