
#pragma once

class CNpcMng : public NiMemObject
{
public:		
	typedef	std::map<DWORD,CNpc*>	NpcCont;
	static CsC_AvObject*  CALLBACK_GetNpcObject( DWORD dwId );
	
	CNpcMng();
	~CNpcMng();

protected:	
	//CsVectorLink< CNpc* >			m_vnNpc;
	NpcCont			m_mapNpc;

#ifndef _GIVE
	public:
		bool m_bRenderNpc;
#endif

public:
	void			Delete();
	void			Init();
	void			Reset();
	void			ResetOldMap(){ m_dwOldMapID = 0; }

	void			ResetShader();

	void			Update();
	//void			Update_Tutorial();

	void			Render();	
	void			Render_PostChar();

// 	void			Render_Tutorial();
// 	void			Render_PostChar_Tutorial();

	bool			LoadMap( DWORD dwMapID );
	void			DeleteMapNpc( DWORD const& dwMapID ); 
	void			AddNpc( uint nIDX, uint nFileTableID );
	void			DelNpc( uint const& nIDX);

	void			CheckQuestExtra();
	
	void			Tutorial_NPCEnable();
	void			SetNpcEnableFromIdx(UINT const& nNpcIdx, bool const& bVale);
	void			NPCEnable(bool const& bVal);

	CNpc*			GetNpc( uint const& nIDX );	
	bool			IsNpc( uint const& nIDX ) const;

	void			SetNpcSE_Highlight( DWORD const& dwTableID, bool const& bShow );

	/*void			DeleteItem( uint nUID );
	bool			IsItem( uint nUID ){ return m_vnItem.Get( nUID )->s_bHaveElement; }
	CItem*			GetItem( uint nUID );

	
	void			Render_Post_TR();
	void			Render_End();
	void			SetCharRender();*/

	//====================================================================================
	//
	//		public
	//
	//====================================================================================
protected:
	DWORD			m_dwOldMapID;


	//====================================================================================
	//
	//		Digivice
	//
	//====================================================================================
protected:
	CNpc*			m_pDigiviceNpc;
public:
	CNpc*			GetDigivice();

protected:
	void			_DeleteZoomDigivice();

	//====================================================================================
	//
	//		ZoomModel
	//
	//====================================================================================
protected:
	CsC_AvObject*	m_pZoomNpc;
	CNpc*			m_pZoomNpcTemp;
	float			m_fZoomNpcAlpha;
	bool			m_bIncreaseZoomAlpha;
	int				m_nZoomNpcType;

public:
	CsC_AvObject*	GetZoomNpc(){ return m_pZoomNpc; }
	int				GetZoomNpcType(){ return m_nZoomNpcType; }

	void			SetZoomNpc( CsC_AvObject* pNpc );	
	void			SetZoomNpcType( int nType ){ m_nZoomNpcType = nType; }
	void			ResetZoomNpc();

	CNpc*			LoadZoomNpcTemp( DWORD nModelID );
	
	void			RenderZoomNpc();

	NpcCont*		GetNpcVector(){ return &m_mapNpc; }

	void			EndZoomNpc();	// IsOpenNpcWindow 일 때 씰 마스터 객체 사라지지 않는 문제 수정을 위해 public으로 수정
protected:
	
	void			_DeleteZoomNpcTemp();

	//====================================================================================
	//
	//		Picking
	//
	//====================================================================================
public:
	CsC_AvObject*	PickNpc( CsGBPick* pCsGBPick, NiPoint3* vOrigin, NiPoint3* vDir, cType& type, float& fLength );

protected:
	bool			m_bRenderPos;
	bool			m_bRenderAni;	// 카드 그릴 때 동작을 1회만 하기 위한 변수
	
public:
	void			Set_RenderPos( bool bSet ) { m_bRenderPos = bSet; }
	void			Reset_RenderAni() { m_bRenderAni = false; }

	bool			Get_RenderPos() { return m_bRenderPos; }

private:
	bool			_CheckAwardNpc( DWORD const& dwFTID, CsNpc::sINFO* pFTable, std::wstring & wsGuildName );
};

extern CNpcMng* g_pNpcMng;