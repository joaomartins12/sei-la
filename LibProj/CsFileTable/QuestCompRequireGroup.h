

#pragma once 

class CsQuestCompRequireGroup
{
public:
	CsQuestCompRequireGroup() {}
	~CsQuestCompRequireGroup() {}

protected:
	CsQuestCompRequire::LIST	m_List;
public:
	void			Delete();
	void			Init();

	void			Add( CsQuestCompRequire::eTYPE type, UINT nTypeID, int nTypeCount, UINT nTargetID, int nSubValue1, int nSubValue2 );
	void			Delete( int nIndex );

	CsQuestCompRequire*			Get( int nIndex );
	int							GetListCount(){ return (int)m_List.size(); }
	CsQuestCompRequire::LIST*	GetList(){ return &m_List; }

	void			Clone( CsQuestCompRequireGroup* pSrcGroup );

	bool			IsCompleate();
	bool			IncreaseCount( CsQuestCompRequire::eTYPE type, UINT nTypeID );
};

