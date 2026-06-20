

#pragma once 

class CsQuestPreRequireGroup
{
public:
	CsQuestPreRequireGroup() {}
	~CsQuestPreRequireGroup() {}

protected:
	CsQuestPreRequire::LIST	m_List;
public:
	void			Delete();
	void			Init();

	void			Add( CsQuestPreRequire::eTYPE type, UINT nTypeID, int nTypeCount );
	void			Delete( int nIndex );

	CsQuestPreRequire*			Get( int nIndex );
	int							GetListCount(){ return (int)m_List.size(); }
	CsQuestPreRequire::LIST*	GetList(){ return &m_List; }

	void			Clone( CsQuestPreRequireGroup* pSrcGroup );
};

