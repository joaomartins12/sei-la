
#pragma once 

class CsQuestRequiteGroup
{
public:
	CsQuestRequiteGroup() {}
	~CsQuestRequiteGroup() {}

protected:
	CsQuestRequite::LIST	m_List;
public:
	void			Delete();
	void			Init();

	void			Add( CsQuestRequite::eMETHOD method, CsQuestRequite::eTYPE type, UINT nTypeID, int nTypeCount );
	void			Delete( int nIndex );

	CsQuestRequite*			Get( int nIndex );
	CsQuestRequite::LIST*	GetList(){ return &m_List; }
	int						GetListCount(){ return (int)m_List.size(); }

	void			Clone( CsQuestRequiteGroup* pSrcGroup );
};