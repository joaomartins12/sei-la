
#pragma once

#define IF_QUESTNODE_X				200
#define IF_QUESTNODE_Y				50
#define IF_TAIL_LEN					20

class cQuestObj : public cWindow
{
public:
	cQuestObj(){}
	virtual ~cQuestObj(){ Delete( true ); }

public:
	void				Delete( bool bDeleteMiddle );
	void				DeleteMiddle_Tail();
	void				Init( CsQuest* pFTQuest );

	void				InitControl();
	void				ResetControl();

	bool				UpdateCursor( CsPoint ptLocalCursor );

	void				RenderTail( CsPoint ptNodePos );
	void				Render( CsPoint ptNodePos );
	void				RenderPos( CsPoint ptPos );
	void				ResetDevice();

	void				SetAlpha( float fAlpha );
	void				ResetBG();


	// 선행 조건 관련
protected:
	void				_CheckParent();					// 선행 조건 체크


	// 컨트롤
protected:
	cSprite				m_SpriteBG;
	cStringList			m_TitleList;

	cSprite*			m_pStartTargetBG;
	cText*				m_pStartTarget;
	cSprite*			m_pEndTargetBG;
	cText*				m_pEndTarget;
	cStringList			m_EtcList;


	// 꼬리들
public:
	enum eMIDDLETAIL_TYPE{ MT_LINE, MT_JUMP, MT_LINEJUMP, MT_NONE, };
	enum eMIDDLETAIL_ELEMENTTYPE{ MTE_LINE = 100, MTE_JUMP_CHILD, MTE_JUMP_PARENT };
protected:
	cSprite*			m_pParentTail;
	cSprite*			m_pChildTail;
	
	eMIDDLETAIL_TYPE		m_eMiddleType;
	struct sMIDDLE_INFO : public NiMemObject
	{
		bool		IsClick( CsPoint ptLocal ){ return ( (s_ptDelta.x < ptLocal.x)&&(s_ptDelta.x+20 > ptLocal.x)&&(s_ptDelta.y < ptLocal.y)&&(s_ptDelta.y+20 > ptLocal.y) ); }

		cSprite					s_Sprite;
		eMIDDLETAIL_ELEMENTTYPE	s_eType;
		CsPoint					s_ptDelta;
		cQuestObj*				s_pLinkQuestObj;
	};
	CsVectorPB< sMIDDLE_INFO* >	m_vpMiddleTail;

	// 보이기
protected:
	void					_AddEtcString( cText::sTEXTINFO* ti );

	// 정보
protected:
	CsQuest*				m_pFTQuest;
	CsPoint					m_ptNodeIndex;

public:
	CsQuest*				GetFT(){ return m_pFTQuest; }
	void					SetNodeIndex( CsPoint ptNodeIndex ){ m_ptNodeIndex = ptNodeIndex; }
	CsPoint					GetNodeIndex(){ return m_ptNodeIndex; }
	CsPoint					GetPitchPos();
	CsPoint					GetSpritePos();

	// 부모와 자식관계 - 퀘스트의 선행 조건에 따른 연관 관계
protected:
	CsVectorPB< cQuestObj* >	m_vpParentQuest;
	CsVectorPB< cQuestObj* >	m_vpChildQuest;
public:
	void					AddParent( cQuestObj* pParent );
	void					DelParent( cQuestObj* pParent );
	void					DelAllParent();
	void					UnlinkParent();

	void					AddChild( cQuestObj* pChild );
	void					DelChild( cQuestObj* pChild );
	void					DelAllChild();
	void					UnlinkChild();

	void					ResetMiddleTail();
	void					ResetMiddleTail_ParentChild();
	void					CreateMiddle_LineTail();
	void					CreateMiddle_JumpTail();
	void					CreateMiddle_LineJumpTail();

	int						FindChild_Index( cQuestObj* pChild );
	int						GetParentCount(){ return m_vpParentQuest.Size(); }
	cQuestObj*				GetParent( int nIndex ){ assert_cs( m_vpParentQuest.IsExistElement( nIndex ) ); return m_vpParentQuest[ nIndex ]; }
	int						GetChildCount(){ return m_vpChildQuest.Size(); }
	cQuestObj*				GetChild( int nIndex ){ assert_cs( m_vpChildQuest.IsExistElement( nIndex ) ); return m_vpChildQuest[ nIndex ]; }


	CsPoint					GetChildTailPos();
	CsPoint					GetParentTailPos();



	// 입력
public:
	bool					OnLBtnDown( CsPoint ptLocalClick );
	bool					IsInObject( CsPoint ptLocalClick );
};