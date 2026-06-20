
#pragma once 
// 
// class CCardMng : public NiMemObject
// {
// protected:	
// 	typedef std::map<DWORD, CCard*>	CardCont;
// 	CardCont						m_mapCards;
// 	
// 	enum ePLAG
// 	{
// 		PLAG_BARCODE_ITEM		=	0x00000001,
// 	};
// 
// public:
// 	void			Delete();
// 	void			Init();
// 	void			Reset();
// 	void			ResetShader();
// 
// 	void			AddCard(uint nIDX, uint nFileTableID, NiPoint3 vPos, float fRot, CsC_CardObject::sCHANGE_PART_INFO* pPartCordArray );	
// 
// 	void			DeleteCard( uint const& nIDX );
// 	bool			IsCard( uint const& nIDX ) const;
// 	CCard*			GetCard( uint const& nIDX );
// 
// 	void			Update();
// 	void			Render();
// 
// 	//====================================================================================
// 	//
// 	//		Picking
// 	//
// 	//====================================================================================
// public:
// 	CsC_AvObject*	PickCard( CsGBPick* pCsGBPick, NiPoint3* vOrigin, NiPoint3* vDir, cType& type, float& fLength );
// };
// 
// extern CCardMng* g_pCardMng;
