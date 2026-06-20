
#pragma once

class cNode;
class cQuestMng : public NiMemObject
{
public:
	void			Delete();
	void			Init();
	
	void			CreateMiddleType( cQuestObj::eMIDDLETAIL_TYPE eType );

	cQuestObj*		AddQuest( cQuestObj* pObject );
	cQuestObj*		AddQuest( CsQuest* pFTQuest );

	void			DeleteQuestObj( cQuestObj* pObject );

	cQuestObj*		FindNode( CsQuest* pFTQuest );
	cQuestObj*		FindNode( DWORD dwQuestID );

	cQuestObj::eMIDDLETAIL_TYPE			GetQuestMiddleType(){ return m_eMiddleType; }


protected:
	void			_ResistQuestMap( CsQuest::MAP* pMap );
	void			_CheckQuestMap();

protected:
	std::map< DWORD, cQuestObj* >		m_mapQuest;
	cQuestObj::eMIDDLETAIL_TYPE			m_eMiddleType;
};