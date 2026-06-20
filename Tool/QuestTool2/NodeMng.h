
#pragma once 

#define MAX_SIZE				2080

#define START_POS_X				200
#define START_POS_Y				32

#define PITCH_SIZE_X			400
#define PITCH_SIZE_Y			200


class cNodeMng
{
public:
	cNodeMng();

public:
	enum eEXT_NODE{	EN_DIGIVICE, EN_COUNT, };

public:
	void			Delete();
	void			Init();

	void			Update();
	void			UpdateCursor( CsPoint ptCursor );
	void			Render( CsPoint ptViewerPos );

	void			ResetDevice();

	// 팝업
protected:
	cPopup*			m_pPopup;
public:
	cPopup*			GetPopup(){ return m_pPopup; }

	// 퀘스트
protected:
	int				_MapID_To_Index( DWORD dwMapID );
public:
	bool			ResistQuestObj( cQuestObj* pQuestObj, bool bResetSize );
	void			ReleaseQuestObj( cQuestObj* pQuestObj, bool bResetSize );

	void			ResetSize_AllNode();
	void			ResetSize();

	// 노드
protected:
	cNode**			m_ppNodeArray;	
	CsPoint			m_ptNodeCount;

public:
	CsPoint			GetNodeCount(){ return m_ptNodeCount; }
	cNode*			GetNode( CsPoint ptNodeIndex ){ return &m_ppNodeArray[ ptNodeIndex.y ][ ptNodeIndex.x ]; }
	cNode*			FindNode( CsPoint ptWorld );


protected:
	struct sMAP_NAME : public NiMemObject
	{
		void		Init( int nMapIndex );
		void		Render( CsPoint pos, float fAlpha );
		void		ResetDevice(){ s_BG.ResetDevice(); s_Text.ResetDevice(); }

		cSprite		s_BG;
		cText		s_Text;
	};

	// 컨트롤
protected:
	cSprite			m_LevelBGBar;
	cSprite			m_MapBGBar;

	cSprite*		m_pMapBGArray;
	cText*			m_pLevelTextArray;
	sMAP_NAME*		m_pMapTextArray;


	// 입력
public:
	void			OnLBtnDown( CsPoint ptWorldClick );
	bool			IsInObject( CsPoint ptWorldClick );
};