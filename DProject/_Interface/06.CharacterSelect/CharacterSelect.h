#pragma once

#include "../00.Contents/CharacterSelectContents.h"

class CCharacterSelect : public cScriptUI, public CharacterSelectContents::ObserverType
{
	struct sCharUIControls
	{
		sCharUIControls();
		~sCharUIControls();

		void				SetEmptySlot(bool const& bSlotLock);
		void				SetTamerInfo(int const& nLv, std::wstring const& wsName, int const& nTamerImgIdx, bool const& bGray);
		void				SetDigimonInfo(int const& nLv, std::wstring const& wsName, bool const& bGray);

		cButton* m_pSlotBtn;
		cString::sTEXT* m_TamerName;
		cString::sTEXT* m_DigimonName;
		cString::sTEXT* m_EmptyNLockMsg;
		cString::sIMAGE* m_pTamerImage;
		cSprite* m_pLockSlotImg;
		cSprite* m_pEmptySlotImg;
	};

public:
	CCharacterSelect();
	virtual ~CCharacterSelect();

	void			Destory();
	void			Init();

	void			Update(float const& fAccumTime, float const& fDeltaTime);
	void			RenderScreenUI();
	void			Render3DModel();
	void			Render();
	void			ResetDevice();

	BOOL			UpdateMouse();
	BOOL			UpdateKeyboard(const MSG& p_kMsg);

	virtual	bool	Construct(void);

protected:
	virtual	void	Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void			ChangeCharacterInfoWindow(const int& nSelIdx);

	void			PressOkButton(void* pSender, void* pData);
	void			PressCancelButton(void* pSender, void* pData);
	void			PressCreateButton(void* pSender, void* pData);
	void			PressDeleteButton(void* pSender, void* pData);

	void			PressCharDelCloseButton(void* pSender, void* pData);
	void			PressCharDelOkButton(void* pSender, void* pData);
	void			CheckEditBoxText(void* pSender, void* pData);

	void			ChangeCharacter(void* pSender, void* pData);
	void			SeletedCharacter(void* pSender, void* pData);

	void			_ListShowUp(void* pSender, void* pData);
	void			_ListShowDown(void* pSender, void* pData);

	void			MakeBackGroundUI();
	void			MakeMainButtonUI();
	void			MakeCharListUI();
	void			MakeDelectWindow();
	void			showAndhideDeleteWindow(bool bVisible);

	void			_ResetCharacterSlot(int const& nResetSlotNum);
	void			_makeCharacterSlot(int const& nSlotNum, cListBox* pTree);

	void			SetCameraInfo();
	void			_UpdateCharacterSlotUI(int const& nSlotNum, CharacterSelectContents::sCharacterInfo const& pInfo);

private:
	bool			LoadLobbyMap();
	void			ReleaseLobbyMap();
	void			RenderLobbyMap();

private:
	cText* m_pMapName;
	cSprite* m_pMapNameBack;

	cScriptUI		m_MainButtonUI;
	cButton* m_pOk;
	cButton* m_pCancel;
	cButton* m_pCreate;
	cButton* m_pDelete;

	cScriptUI		m_CharListUI;
	cButton* m_pListUpBtn;
	cButton* m_pListDownBtn;
	cListBox* m_pCharacterList;

	// 케릭터 제거
	cScriptUI		m_DeleteWindowUI;
	cEditBox* m_pEditResistNumber;
	cButton* m_pDeleteCharCloseBtn;
	cButton* m_pDeleteCharOkBtn;

	std::map<int, sCharUIControls>	m_mapUIControls;

private:
	bool			m_bLobbyMapLoaded;
	DWORD			m_dwLobbyMapID;
};