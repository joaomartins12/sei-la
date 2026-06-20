#pragma once

#include "../00.Contents/BattleSystemContent.h"

class CBattleTypeSelectUI;
class CBattleRegistBackUI;
class CBattleModeInfoUI;
class CBattleMapInfoUI;
class CBattleDigimonListUI;

class CBattleRegister : public cScriptUI, public CBattleSystemContents::ObserverType
{
public:
	CBattleRegister();
	virtual ~CBattleRegister();

public:
	bool			Create();
	void			Update(float const& fDeltaTime);
	void			Update3DAccumtime( float const& fDeltaTime );
	BOOL			UpdateMouse();						//2016-01-13-nova	마우스 입력처리
	BOOL			UpdateKeyboard(const MSG& p_kMsg);	//2016-01-13-nova	키보드 입력처리
	void			Render3DModel();

	virtual	bool	Construct(void);
	virtual void	Render();

protected:
	virtual	void	Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void			_CreateUI();
	void			Destroy();
	void			ResetDevice();

	void			_UpdateModeChange();
	

private:
	
	CBattleTypeSelectUI*	m_BattleTypeSelectUI;	/**		배틀 타입 선택 UI		*/
	CBattleRegistBackUI*	m_pRegistBackUI;		/**		배틀 등록 UI		*/
	CBattleModeInfoUI*		m_pModeInfoUI;			/**		배틀 모드 정보 UI		*/
	CBattleMapInfoUI*		m_pMapInfoUI;			/**		배틀 맵 정보 UI			*/
	CBattleDigimonListUI*	m_pDigimonListUI;		/**		참여 가능 디지몬 목록 UI	*/
};