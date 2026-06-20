#pragma once

#include "../14.MainFrameUI/TamerStatusUI_SubViewer.h"

class cTamerStatusUI_Skill : public cTamerStatusUI_SubViewer
{
public:
	cTamerStatusUI_Skill();
	~cTamerStatusUI_Skill();

	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create(cWindow* pkRoot, int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual BOOL			Update_ForMouse();
	virtual void			RenderScript();

	virtual void			ResetDevice();

	virtual void			ReleaseSelect();
	virtual void			UpdateViewer();
	virtual void			OnLButtonUp( CsPoint pos );
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

private:
	enum eSkillType{ eBasicSkill, eActiveSkill, ePassiveSkill };
	struct sSkillInfo : public CUSTOMDATA
	{
		sSkillInfo( int nIconType, int nIndex, eSkillType eType ) 
			: nSkillIconType( nIconType ), nSkillIndex( nIndex ), eSType( eType ) {}
		~sSkillInfo() {}

		int					nSkillIconType;
		int					nSkillIndex;
		eSkillType			eSType;
	};
	struct sSkill
	{
		sSkill() : pSkillIcon( NULL ), pSkillDelBtn( NULL ) {}
		~sSkill() {}

		void				ChangeSkill( int nSkillType, int nSkillIndex, CsCoolTimeSeq* pCoolTime );
		void				SetDeleteButton( bool bEnable );

		cString::sICON*		pSkillIcon;
		cString::sBUTTON*	pSkillDelBtn;
		cGridListBoxItem*	pGridItem;
	};
	std::map< int, sSkill >	m_mapBasicSkill;
	std::map< int, sSkill > m_mapActiveSkill;
	std::map< int, sSkill > m_mapPassiveSkill;
	cGridListBox*			m_pBasicSkill;
	cGridListBox*			m_pActiveSkill;
	cGridListBox*			m_pPassiveSkill;

	cText*					m_pBasicText;
	cText*					m_pActiveText;
	cText*					m_pPassiveText;

private:
	BOOL					_SetTooltip(cGridListBoxItem const* pOverItem);

	void					_MakeBasicSkill();
	void					_AddBasicSkill( int nIndex );

	void					_MakeActiveSkill();
	void					_AddActiveSkill( int nIndex );

	void					_MakePassiveSkill();
	void					_AddPassiveSkill( int nIndex, int nIconType );

	void					_OnLClickDownSkill( void* pSender, void* pData );
	void					_OnRClickSkill( void* pSender, void* pData );
	void					_OnClickDeleteSkill( void* pSender, void* pData );
	bool					_OnClickSkill( cGridListBoxItem const* pOverItem );
};

