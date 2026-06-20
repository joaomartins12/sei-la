

#pragma once 

#include "Party_Contents.h"

class cPartyWindow : public cBaseWindow, public CPartyContents::ObserverType
{
	struct sMemberIdx : public CUSTOMDATA
	{
		sMemberIdx(int nIdx) : m_nMemberIdx(nIdx)
		{}
		~sMemberIdx() {}

		int		m_nMemberIdx;
	};

private:
	struct sPartyControls
	{
		sPartyControls();
		~sPartyControls();

		cSprite*		m_pDigimonImg;
		std::string		m_pDigimonImgFileName;
		cSprite*		m_pMasterImg;
		cSprite*		m_pDisableImg;
		cProgressBar*	m_pTamerHp;
		cProgressBar*	m_pTamerDs;
		cProgressBar*	m_pDigimonHp;
		cProgressBar*	m_pDigimonDs;

		cString::sTEXT*	m_pTamerLv;
		cString::sTEXT*	m_pDigimonLv;
		cListBoxItem *	m_BoxItem;

		void			SetFaraway(bool bValue);
	};
public:
	cPartyWindow();
	~cPartyWindow();

public:
	virtual eWINDOW_TYPE		GetWindowType(){ return WT_PARTY_WINDOW; }
	virtual void				Destroy();
	virtual void				DeleteResource();
	virtual void				Create( int nValue = 0 );

	virtual void				PlayOpenSound(){}
	virtual void				PlayCloseSound(){}

	//virtual bool				IsShowWindow();

	virtual void				Update(float const& fDeltaTime);
	virtual eMU_TYPE			Update_ForMouse();
	virtual void				Render();

	virtual void				ResetDevice();

private:
	void						_MakePartyMember();
	void						_ChangePartyMaster();
	void						_RefreshMemberUI();
	void						_AddNewMember( SMemberInfo const* pInfo );
	void						_UpdateMemberInfo( SMemberInfo const* pInfo );
	void						_ChangeMemberDigimon(SMemberInfo const* pInfo);
	void						_DeleteMemberUIList( int const& nDelSlotIdx );
	void						_SelectedMemberTarget(int const& nSlot);
	void						_ReleasedMemberTarget(int const& nSlot);
	void						_ChangeFaraway(int const& nSlot, bool bFaraway);

protected:
	virtual	bool				Construct(void);
	virtual	void				Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void						ShowMemberPopWindow(void* pSender, void* pData);
	void						ShowMemberTooltip(void* pSender, void* pData);
	void						SetTargetObject( void* pSender, void* pData );

private:
	cListBox*					m_pPartyUIList;

	typedef std::map<int, sPartyControls>			MAP_MEMBER_UI;
	typedef std::map<int, sPartyControls>::iterator	MAP_MEMBER_UI_IT;
	MAP_MEMBER_UI				m_mapMemberControls;
};