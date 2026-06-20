

#pragma once

#include "../00.Contents/CharacterCreateContents.h"

class CDigimonCreate : public cScriptUI, public CharacterCreateContents::ObserverType
{
	struct sDigimonInfo: public CUSTOMDATA
	{
		sDigimonInfo(unsigned int nIdx, bool bEnable)
			: m_nTableIDX(nIdx),m_bEnable(bEnable)
		{}
		~sDigimonInfo() {};
		unsigned int	m_nTableIDX;
		bool			m_bEnable;
	};

	struct sEvolutionInfo: public CUSTOMDATA
	{
		sEvolutionInfo(unsigned int nBaseIdx,unsigned int nIdx)
			: m_nBaseDigimonIDX(nBaseIdx),m_nTableIDX(nIdx)
		{}
		~sEvolutionInfo() {};
		unsigned int	m_nBaseDigimonIDX;
		unsigned int	m_nTableIDX;
	};

	struct sSkillInfo: public CUSTOMDATA
	{
		sSkillInfo(unsigned int nIdx )
			: m_nTableIDX(nIdx)
		{}
		~sSkillInfo() {};
		unsigned int	m_nTableIDX;
	};

public:
	CDigimonCreate();	
	virtual ~CDigimonCreate();

	bool				Init();

	void				Update(float fDeltaTime);
	void				RenderScreenUI();		// 배경 2D이미지 그리기
	void				Render3DModel();		// 3D 모델 그리기
	void				Render();				// UI 그리기

	void				ResetDevice();

	BOOL				UpdateMouse();						//2016-01-13-nova	마우스 입력처리
	BOOL				UpdateKeyboard(const MSG& p_kMsg);	//2016-01-13-nova	키보드 입력처리

	virtual	bool		Construct(void);

protected:
	virtual	void		Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void				makeMainButtonUI();
	void				makeDigimonListWindowUI();
	void				setDigimonList();

	void				Destory();
	void				UpdateSound();
	void				ChangeCharacterSound( std::string const& soundFileName );
	void				_DigimonSelectedChange(unsigned int const& nChangeSelectIdx, unsigned int const& nEvolSelectIdx);

private:
	void				SetCameraInfo();

	void				PressOkButton( void* pSender, void* pData );
	void				PressCancelButton( void* pSender, void* pData );
	void				_SelectedDigimon(void* pSender, void* pData);
	void				_SelectedDigimonEvolution(void* pSender, void* pData);
	void				_ResetEvolutionTree(CharacterCreateContents::sDigimonCreatedInfo const* pInfo);
	void				_ResetDigimonSkill(CharacterCreateContents::LIST_SKILL_INFO const& skillList);

protected:
	cScriptUI			m_MainButtonUI;
	cButton*			m_pButtonOK;
	cButton*			m_pButtonCancel;
	cEditBox*			m_pEditBox;

	cScriptUI			m_DigimonListWindowUI;
	cGridListBox*		m_pDigimonListBox;
	cText*				m_pDigimonName;
	cText*				m_pDigimonDesc;
	cText*				m_pDigimonAttributeName;
	cImage*				m_pDigimonAttributeType;
	cGridListBox*		m_pDigimonEvolutions;
	cGridListBox*		m_pDigimonSkills;

	CSound*				m_pIntroSound;
	float				m_fBackupMusic;

	bool				bIMECheck;		//2016-02-18-nova IME 상태 체크해서 ESC/RETURN 처리
};
