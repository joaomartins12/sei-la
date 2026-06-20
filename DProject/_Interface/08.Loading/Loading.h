
#pragma once

#include "../00.Contents/LoadingContents.h"

class CLoading : public cScriptUI, public LoadingContents::ObserverType
{
public:
	CLoading(void);
	virtual ~CLoading(void);

public:
	void				Create();
	void				ResetDevice();
	virtual	bool		Construct(void);

	void				Destroy();
	void				Update(float const& fDeltaTime);
	void				Render();		


protected:
	virtual	void		Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void				_UpdateProgressBar(int const& nValue);						// 100%로를 기준으로 로딩바 다음 단계 설정
	
	void				_MakeBattleLoadUI();
	void				_PlayerLoadEnd(ContentsStream const& kStream);


	void				_MakeTip();
	void				_MakeLoadingGuageBar();
	void				_AddTeamPlayerUI( cGridListBox* pControl, BWait::sWait::sPlayerInfo const& playerList, int nCount );

private:
	cProgressBar*		m_pLoadBar;
};

