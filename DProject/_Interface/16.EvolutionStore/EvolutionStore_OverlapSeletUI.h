#pragma once

#include "../00.Contents/EvolutionStore_Contents.h"

class cOverlapDigimonSelectUI : public cScriptUI, public cEvolutionStoreContents::ObserverType
{
	struct sTactisData : public CUSTOMDATA
	{
		sTactisData(int nNum,DWORD dwIDX):nSlotNum(nNum),dwTbIDX(dwIDX) {};
		~sTactisData() {};

		int nSlotNum;
		DWORD dwTbIDX;
	};

public:
	cOverlapDigimonSelectUI();
	~cOverlapDigimonSelectUI();

	void						Init(cWindow* pRootWin);
	virtual	bool				Construct(void);

	bool						UpdateMouse();	
	void						SetInitData(DWORD const& dwDigimonTbIDX, int const& nLv);
	void						ResetData();

protected:
	void						Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

private:
	void						_CloseWindow(void* pSender, void* pData);
	void						_EmptyItemSlot( cGridListBox* pGridList, int const& nMakeCount );
	void						OnClicked_SelectDigimon(void* pSender, void* pData);

private:
	cButton*					m_pSelectCancel;
	cGridListBox*				m_pSelectGrid;
};