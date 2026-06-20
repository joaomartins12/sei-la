#pragma once

#include "../00.Contents/DamageMeter_Contents.h"

class cDamageMeter : public cBaseWindow, public DamageMeterContents::ObserverType
{
private:
	struct sControls
	{
		sControls():pName(NULL),pValue(NULL)
		{};
		~sControls(){};

		cString::sTEXT* pName;
		cString::sTEXT* pValue;
	};

public:
	cDamageMeter();
	virtual ~cDamageMeter();

public:
	virtual	bool			Construct(void);

	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );
	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			OnMoveWindow();
	virtual void			Render();
	virtual void			ResetDevice();
	//virtual void			ResetMap();
	virtual bool			OnEscapeKey();
	//virtual void			UserDie();

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void					_ResetData();

	void					_CloseWindow(void* pSender, void* pData);
	void					_DataReset(void* pSender, void* pData);
	void					_PrevVeiwType(void* pSender, void* pData);
	void					_NextVeiwType(void* pSender, void* pData);

	void					_SetViewType( int const& nType );

	sControls*				_GetControls(int const& nIdx);	
	void					_UpdateInfo(int const& nType);

	void					_ListBoxUpdate( int const& nShowItemCount );
	bool					_makeEmptyItem(int const& nMakeCount );

private:
	cButton*				m_pCloseBtn;	// 닫기 버튼
	cButton*				m_pResetBtn;	// 데이터 초기화 버튼
	cButton*				m_pNextBtn;		// 다음 버튼
	cButton*				m_pPrevBtn;		// 이전 버튼
	cListBox*				m_pListBox;
	cText*					m_ViewTypeString;
	//cText*				m_szAvDPS;		// 파티 평균 DPS	값

	std::map<int,sControls>	m_mapControls;
};