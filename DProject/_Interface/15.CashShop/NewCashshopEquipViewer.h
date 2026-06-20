#pragma once 
#include "../00.Contents/NewCashshopContents.h"

class NewCashshopEquipViewer : public cScriptUI, public NewCashshopContents::ObserverType
{
	struct sEquipSlotData
	{
		sEquipSlotData():s_GridItem(NULL),s_SlotIcon(NULL)
		{
		}
		~sEquipSlotData()
		{
			s_GridItem = NULL;
			s_SlotIcon = NULL;
		}

		cGridListBoxItem*	s_GridItem;
		cString::sICON*		s_SlotIcon;
	};

	struct EquipItemInfo : public CUSTOMDATA
	{
		int itemid;
		int partid;

		EquipItemInfo(int iitemid, 
			int ipartid)
			:itemid(iitemid),
			partid(ipartid)
		{}
	};

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);		
	virtual	bool			Construct(void);

public:
	NewCashshopEquipViewer();
	~NewCashshopEquipViewer();

	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create(cWindow* pkRoot, int nValue = 0 );

	virtual void			UpdateScript(float fDeltaTime);
	virtual BOOL			UpdateMouse();
	virtual void			RenderScript();
	virtual void			ResetDeviceScript();	

private:
	void					InitEquipments(void);
	void					UpdateEquipments(int const& PartId, DWORD const& ItemID);
	void					_CreateEquipSlot(void);
	cGridListBoxItem*		_EmptyEquipSlotItem( int const& gridIndx, int const& nPartsID, DWORD const& dwItemID );
	void					_UpdateEquipSlotData( int const& nParts, DWORD const& dwItemId );

	void					OnRClicked_EquipItem(void* pSender, void* pData);
	void					OnClick_Reset(void* pSender, void* pData);
	void					OnClick_TakeOffAll(void* pSender, void* pData);

	cRenderTex*				m_pRenderTex;
	CTamer*					m_pTamer;
	cGridListBox*			m_pkLeftEquipGrid;
	cGridListBox*			m_pkRightEquipGrid;
	
	cButton*				m_pkResetEquipBT;
	cButton*				m_pkTakeOffAllEquipBT;
	cButton*				m_pkTurnLeftBT;
	cButton*				m_pkTurnRightBT;
	float					m_fRot;
	std::map<int, sEquipSlotData>	m_mapEquipSlot;
};