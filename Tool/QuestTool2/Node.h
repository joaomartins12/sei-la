

#pragma once 

class cNode
{
public:
	void			Delete();
	void			Init( cNode* pPrevNode_X, cNode* pPrevNode_Y, CsPoint ptNodeIndex );

	void			Update();
	void			UpdateCursor( CsPoint ptCursor );

	void			Render( CsPoint ptViewerPos );
	void			RenderTail( CsPoint ptViewerPos );

	void			ResetDevice();

	void			ResetNodeSize();

protected:
	void			_UpdateNodePos();

	// 노드 인덱스 정보
protected:
	cNode*			m_pPrevNode_X;
	cNode*			m_pPrevNode_Y;
	CsPoint			m_ptNodeIndex;
	CsPoint			m_ptPitchCount;

	// 노드 위치 정보
protected:
	CsPoint			m_ptNodePos;

public:
	CsPoint			GetNodePos(){ return m_ptNodePos; }
	CsPoint			GetNodeIndex(){ return m_ptNodeIndex; }
	CsPoint			GetPitchCount(){ return m_ptPitchCount; }
	void			SetPitchCount_X( int x ){ m_ptPitchCount.x = x; }
	void			SetPitchCount_Y( int y ){ m_ptPitchCount.y = y; }

	bool			IsExistPitch( CsPoint ptToolPos );
	bool			IsExistPitch_X( CsPoint ptToolPos );
	bool			IsExistPitch_Y( CsPoint ptToolPos );

	// 컨트롤
protected:

	// 퀘스트
protected:
	std::list< cQuestObj* >		m_listQuest;
public:
	bool						ResistQuestObj( cQuestObj* pQuestObj );
	void						ReleaseQuestObj( cQuestObj* pQuestObj );
	std::list< cQuestObj* >*	GetQuestList(){ return &m_listQuest; }

protected:
	bool						_AutoPosition( cQuestObj* pQuestObj );
	void						_AutoPosigion_AddBottom( cQuestObj* pQuestObj, int x );

	// Pitch
public:
	void						IncreasePitch_X( int nIncreaseIndex );
	void						IncreasePitch_Y( int nIncreaseIndex );
	void						DecreasePitch_X( int nDecreaseIndex );
	void						DecreasePitch_Y( int nDecreaseIndex );
	cQuestObj*					GetQuest_FromPitch( CsPoint ptPitch );

	// 입력
public:
	void			OnLBtnDown( CsPoint ptWorldClick );
	bool			IsInObject( CsPoint ptWorldClick );
	CsPoint			CalPitch( CsPoint ptWorldClick );	
};