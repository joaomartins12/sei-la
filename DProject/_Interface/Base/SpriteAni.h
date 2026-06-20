
#pragma once 


class cSpriteAni : public cWindow
{
public:
	enum eSPRITE_PLAY{ SPRITE_PAGE, SPRITE_POS };
	cSpriteAni();
	virtual ~cSpriteAni();

public:
	enum ePLAY_TYPE{ LOOP, ONCE, ONCE_LEAVE, LOOP_CNT };	//2017-03-07-nova 애니메이션 종료시 이미지가 보이도록 하는것 추가

	enum SPRITEANI_EVENT
	{
		SPRITEANI_END,
	};
	//=======================================================================================================================
	//
	//		Base
	//
	//=======================================================================================================================
protected:
	CsVectorPB< cSprite* >	m_vpSprite;
	float					m_fSetAniTime;
	float					m_fCurAniTime;
	int						m_nSpriteIndex;
	int						m_nSpriteMaxIndex;

	ePLAY_TYPE				m_ePlayType;
	bool					m_bPlay;

	CsPoint					m_ptTexToken;

	int						m_nLoopCount;

public:
	void			Delete();
	void			Init( ePLAY_TYPE eType, cWindow* pParent, CsPoint pos, CsPoint size, char* cImagePath, char* cExt, int nAniCount, bool bApplyWindowSize, CsPoint textoken = CsPoint::ZERO, eSPRITE_PLAY eSprite = SPRITE_PAGE,int nEnterCount = 0/*스프라이트가 아래로 내려가게하는 조건 수 */);

	void			SetLoopCount( int nLoopCount );
	void			SetAniTime( float fTime ){ m_fCurAniTime = m_fSetAniTime = fTime; }
	cSprite*		AddNode( char const * cImagePath );
	cSprite*		AddNode( char const * cImagePath , int nCntX, int nCntY );

	virtual void	Update(float const& fDeltaTime);
	void			Render();
	void			Render( CsPoint pos );

	virtual eTYPE	GetType(){ return T_SpriteAni; }

	virtual void	SetAlpha( float fAlpha );
	virtual void	ResetDevice();


	void			PlayOff(){ m_bPlay = false; }
	void			PlayOn( bool bResetAni );

	bool			IsPlay(){ return m_bPlay; }

	void			ResetAni(){ m_nSpriteIndex = 0; m_fCurAniTime = m_fSetAniTime; }
	int				GetCurSpriteIndex(){ return m_nSpriteIndex; }

	void			SetTexToken( CsPoint tex );

	void			ChangeTexture( char const* pFileName, char* pExt );
	void			SetPlayType( ePLAY_TYPE eType );
};