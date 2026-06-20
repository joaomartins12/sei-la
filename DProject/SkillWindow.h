#pragma once

#define BASE_SKILL_COUNT				14
#define EMOTION_SKILL_COUNT				28
#define DIGIMON_SKILL_COUNT				5

#define MAX_DIGIMON_COUNT				5
#define BASE_TEXT_COUNT					2
#define DIGIMON_TEXT_COUNT				2

class cSkillWin : public cBaseWindow
{
public:
	cSkillWin(){
		for( int i=0; i<nLimit::EvoUnit; ++i )
		{	
			m_pEvolImeage[ i ] = NULL;
		}
	}

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_SKILL; }
	virtual void			Destroy();
	virtual void			DeleteResource();	

	virtual void			ResetMap() { Close( false ); }
	virtual void			ResetDevice();
	virtual void			Create( int nValue = 0 );

	virtual void			Update();	
	virtual eMU_TYPE		Update_ForMouse();	

	virtual void			Render();		

	virtual bool			OnEscapeKey(){ Close(); return true; }
	virtual void			OnLButtonUp( CsPoint pos );
	virtual void			OnRButtonUp( CsPoint pos );
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );
	virtual	void			OnMoveWindow();				

protected:
	cRadioButton*			m_pTabMenu;
	cSprite*				m_pSkillMark;

	//cScript 사용
protected:
	cButton*				m_pCancelButton;	//닫기	

	//////////////////////////////////////////////////////////////////////////
	//
	//		기본 스킬
	//
	//////////////////////////////////////////////////////////////////////////
protected:
	void			_Delete_BSkill();
	void			_Create_BSkill();	
	
	void			_Update_BSkill();
	void			_Render_BSkill();

	void			_ResetDevice_BSkill();

	bool			_Update_ForMouse_BSkill();
	void			_OnLButtonUp_BSkill( CsPoint pos );
	void			_OnRButtonUp_BSkill( CsPoint pos );
	void			_OnMouseOutWindow_BaseSkill();
	bool			_CursorIconLBtnUp_BSkill( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );


protected:
	cSprite*				m_pBaseSkillBg;

	sIFIcon					m_IFIcon_BaseSkill[ BASE_SKILL_COUNT ];
	sIFIcon					m_IFIcon_EmotionSkill[ EMOTION_SKILL_COUNT ];
	cText*					m_pTextBaseSkill[ BASE_TEXT_COUNT ];	

	//////////////////////////////////////////////////////////////////////////
	//
	//		디지몬 스킬
	//
	//////////////////////////////////////////////////////////////////////////
public:
	void			SetDigimonImeage();

protected:
	void			_Delete_DSkill();
	void			_Create_DSkill();	

	void			_Update_DSkill();
	void			_Render_DSkill();

	void			_ResetDevice_DSkill();

	bool			_Update_ForMouse_DSkill();
	void			_OnLButtonUp_DSkill( CsPoint pos );
	void			_OnRButtonUp_DSkill( CsPoint pos );
	
	bool			_CursorIconLBtnUp_DSkill( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

protected:
	struct sSkillSlot
	{
	public:
		enum eSTATE{ DISABLE, ENABLE, NOT_DIGIMON_LV, NOT_SKILL_LV,  };

	public:
		void				Delete();
		void				Init( cWindow* pParent, int nIdx );
		void				Update( int nDigimonIdx, int nSkillIndex );
		bool				Update_ForMouse( CsPoint ptMouseLocalPos, CsPoint ptPos, int nDigimonIdx, int nSkillIdx );

		void				OnLButtonUp_DSkill( CsPoint pos, int nSkillIdx );
		void				OnRButtonUp_DSkill( CsPoint pos, int nSkillIdx );
		void				OnMouseOutWindow_DSkill( CsPoint pos, int nSkillIdx );
		
		void				Render( CsPoint ptPos, int nDigimonIdx, int nSkillIdx, cSprite* pIconBG, cSprite* pIconBlock );
		void				ResetDevice();	

	protected:
		void				_CheckEnable( int nDigimonIdx, int nSkillIndex );
		bool				_CheckSkillUp( int nDigimonIdx, int nSkillIndex );	

	protected:
		bool				s_bCurDigimonSkill;
		eSTATE				s_eState;

		int					s_nSkillLv;
		sIFIcon				s_IFIcon_DigimonSkill;
		cButton*			s_pPointButton;
		cText*				s_pSkillLVText;
	};

	int						m_nOpenCount;
	cSprite*				m_pDigimonSkillBg;
	cSprite*				m_pCurDigimonSkillMask;
	cSprite*				m_pIconBG;
	cSprite*				m_pIconBlock;
	cSkill*					m_pTargetSkillMng;

	cText*					m_pTextNewSkillPoint;	

	cScrollBar*				m_pScroll;
	cImage*					m_pEvolImeage[ nLimit::EvoUnit ];
	cText*					m_pTextDigimonSkill[ DIGIMON_TEXT_COUNT ];
	sSkillSlot				m_sSkillSlot[ MAX_DIGIMON_COUNT * DIGIMON_SKILL_COUNT ];
};