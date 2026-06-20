#pragma once

#include "../00.Contents/CharacterCreateContents.h"

class CCharacterCreate : public cScriptUI, public CharacterCreateContents::ObserverType
{
	struct sSkillInfo : public CUSTOMDATA
	{
		sSkillInfo(unsigned int nIdx)
			: m_nTableIDX(nIdx)
		{
		}
		~sSkillInfo() {};
		unsigned int	m_nTableIDX;
	};

	struct sTamerInfo : public CUSTOMDATA
	{
		sTamerInfo(unsigned int nIdx, bool bEnable)
			: m_nTableIDX(nIdx), m_bEnable(bEnable)
		{
		}
		~sTamerInfo() {};
		unsigned int	m_nTableIDX;
		bool			m_bEnable;
	};

	struct sCostumeInfo : public CUSTOMDATA
	{
		sCostumeInfo(DWORD dwTamerIDX, DWORD dwCostumeID)
			: m_dwCostumeID(dwCostumeID), m_dwTamerIDXID(dwTamerIDX)
		{
		}
		~sCostumeInfo() {};
		DWORD			m_dwCostumeID;
		DWORD			m_dwTamerIDXID;
	};

	struct sCostumeName : public CUSTOMDATA
	{
		sCostumeName(std::wstring name)
			: m_sItemName(name)
		{
		}
		~sCostumeName() {};
		std::wstring	m_sItemName;
	};

public:
	CCharacterCreate();
	virtual ~CCharacterCreate();

	bool				Init();
	void				Destory();

	void				Update(float fDeltaTime);
	void				RenderScreenUI();
	void				Render3DModel();
	void				Render();

	void				ResetDevice();

	BOOL				UpdateMouse();
	BOOL				UpdateKeyboard(const MSG& p_kMsg);

	virtual	bool		Construct(void);

public:
	static bool			PreloadLobbyMapStatic();
	static bool			IsLobbyMapPreloadedStatic();
	static void			ResetLobbyMapPreloadStatic();

private:
	void				makeMainButtonUI();
	void				makeTamerWindowUI();
	void				makeCostumWindowUI();
	void				setTamerList();

protected:
	virtual	void		Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void				Update3DModel(float fDeltaTime);
	void				UpdateSound();
	void				ChangeCharacterSound(std::string const& soundFile);

	void				PressOkButton(void* pSender, void* pData);
	void				PressCancelButton(void* pSender, void* pData);
	void				_SelectedTamer(void* pSender, void* pData);
	void				_ChangeSelectedLeft(void* pSender, void* pData);
	void				_ChangeSelectedRight(void* pSender, void* pData);

	void				SetCameraInfo();
	void				CharSelectedChange(unsigned int const& nChangeSelectIdx);
	void				_ResetTamerSkill(CharacterCreateContents::sTamerCreatedInfo const* pInfo);

	void				_UpdateTamerBaseState(std::map<int, int> const& baseState);
	void				_UpdateCostumData(DWORD const& dwTamerIDX, std::list<DWORD> const& costum);
	void				_ChangeCostume(void* pSender, void* pData);

private:
	bool				LoadLobbyMap();
	void				ReleaseLobbyMap();
	void				RenderLobbyMap();

private:
	cScriptUI			m_MainButtonUI;
	cButton* m_pButtonOK;
	cButton* m_pButtonCancel;
	cEditBox* m_pEditBox;

	cScriptUI			m_CharacterListUI;
	cGridListBox* m_pTamerListBox;
	cButton* m_pLefBtn;
	cButton* m_pRightBtn;
	cText* m_pBaseStateAT;
	cText* m_pBaseStateDP;
	cText* m_pBaseStateHP;
	cText* m_pBaseStateDS;

	cText* m_pTamerName;
	cText* m_pTamerDesc;
	cListBox* m_pSkillList;

	cGridListBox* m_pTamerSkills;

	CSound* m_pIntroSound;
	float				m_fBackupMusic;

	bool				bIMECheck;

	cScriptUI			m_pCostumSelectWindow;
	cGridListBox* m_pCostumList;

	bool				m_bLobbyMapLoaded;
	DWORD				m_dwLobbyMapID;
};