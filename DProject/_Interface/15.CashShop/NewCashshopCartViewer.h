#pragma once 
#include "../00.Contents/NewCashshopContents.h"

class NewCashshopCartViewer : public cScriptUI, public NewCashshopContents::ObserverType
{
	struct CartItemInfo : public CUSTOMDATA
	{
		DWORD dwProductIDX;
		CartItemInfo(DWORD iid)
			:dwProductIDX(iid)
		{

		}
	};

	struct sCartControls
	{
		sCartControls():pItemControl(NULL),pkItemIcon(NULL) {}
		~sCartControls() {}
		void				ChangeItemIcon( DWORD const& dwIconID );
		void				ChangeItemData( DWORD const& dwProductIDX );
		bool				IsEmpty() const;
		void				SetEmpty();
		cGridListBoxItem*	pItemControl;
		cSprite*			pkItemIcon;
	};

public:
	NewCashshopCartViewer();
	~NewCashshopCartViewer();

	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create(cWindow* pkRoot, int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual BOOL			UpdateMouse();

public:
	void					OnSelected_CartItem(void* pSender, void* pData);
	void					OnRClicked_CartItem(void* pSender, void* pData);

protected:
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);		
	virtual	bool			Construct(void);

private:
	void					_CreateCartSlot();
	void					_InsertItem( int const& nSlotNum, DWORD const& dwProductIDX );
	void					_DeleteItem( int const& nSlotNum );

private:
	cGridListBox*					m_pkItems;
	std::map<int, sCartControls>	m_mapControls;
};