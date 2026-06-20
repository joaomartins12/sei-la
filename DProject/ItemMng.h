
#pragma once 

class CItemMng : public NiMemObject
{
public:
	CItemMng();
	~CItemMng();

protected:	
	std::map<unsigned int, CItem*>	m_mapItem;
	enum ePLAG
	{
		PLAG_BARCODE_ITEM		=	0x00000001,
	};

public:
	void			Delete();
	void			Init();
	void			Reset();
	void			ResetShader();

	void			AddItem( uint const& nIDX, uint const& nFileTableID, nSync::Pos const& sPos, DWORD const& nOwnerUID, DWORD const& plag = 0 );	

	void			DeleteItem( uint const& nIDX );
	bool			IsItem( uint const& nIDX ) const;
	CItem*			GetItem( uint const& nIDX );

	void			Update();
	void			Render();


	CItem*			PickupItem( DWORD const& nTamerUID, NiPoint2 const& vTamerPos, float const& fRadius );


	//====================================================================================
	//
	//		Picking
	//
	//====================================================================================
public:
	CsC_AvObject*	PickItem( CsGBPick* pCsGBPick, NiPoint3* vOrigin, NiPoint3* vDir, cType& type, float& fLength );
};

extern CItemMng* g_pItemMng;