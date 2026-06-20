

#pragma once 

class CMngCollector : public NiMemObject
{
public:
	static void ShotDown();
	static void ShotInit();

	void			ResetMap();
	void			Delete();
	void			Init();

	void			LoadTerrain( DWORD dwMapID );
	void			LoadChar( DWORD dwMapID );
	void			DeleteChar( DWORD dwMapID );
	void			LoadCompleate( DWORD dwMapID );

	void			LoadDatsCenter();
	void			EndDatsCenter();	

	void			Update( cGlobalInput::eMOUSE_INPUT mi );
	void			Update_Tutorial( cGlobalInput::eMOUSE_INPUT mi );

	void			Render( bool bClearGetterTerrain );
	void			Render_Shadow( bool bClearGetterTerrain );
	void			Render_DOF();

// 	void			Render_Tutorial( bool bClearGetterTerrain );
// 	void			Render_Shadow_Tutorial( bool bClearGetterTerrain );
	
	void			Render_PostIF();

	void			CheckNearObject();

	CsC_AvObject*	PickObject( cType& type, POINT ptMouse, bool bIncludeSelf );
	CsC_AvObject*	PickObject_Tutorial( cType& type, POINT ptMouse, bool bIncludeSelf );

	CsC_AvObject*	GetObject( cType type );
	CsC_AvObject*	GetObject( UINT nUID ){ if( nUID == 0 ){ return NULL; } cType type( nUID ); return GetObject( type ); }
	CsC_AvObject*	GetObject( eRTTI_CLIENT eRtti, TCHAR const* szName );	

	void			ResetObjectShader();

	void			RenderText();	
protected:
	//bool			m_bClientPortal;
	bool			m_bRenderText;

#ifndef VERSION_USA
	CsTimeSeq		m_LongTimeNotice;
	CsOnceTimeSeq	m_LongTimeNotice2;	
#endif

	CsTimeSeq		m_TimeNotice;
	CsOnceTimeSeq	m_TimeNotice2;

	CsTimeSeq		m_ToyNotice;
	CsOnceTimeSeq	m_ToyNotice2;
	
	CsOnceTimeSeq	m_EventNotice;
	CsTimeSeq		m_AutoSaveResistTime;	

	//CsTimeSeq		m_AutoProtect;

public:
	//void			EnableClientPortal(){ m_bClientPortal = true; }
	void			SetRenderText() { m_bRenderText = !m_bRenderText; }

	//void			ResetAutoProtect() { m_AutoProtect.OffReset(); }

public:
	enum eGAME_SCENE_STATE { eSCENE_NONE = 0, eSCENE_EVENT, eSCENE_TACTICSANI, eSCENE_DATS, eSCENE_DATA_TRADE, eSCENE_TUTORIAL_EVENT,
#ifdef CROSSWARS_SYSTEM
eSCENE_CROSS, 
#endif
};

	void				SetSceneState( eGAME_SCENE_STATE eState ) { m_eSceneState = eState; }
	void				ReleaseScene() { m_eSceneState = eSCENE_NONE; }
	eGAME_SCENE_STATE	GetSceneState() { return m_eSceneState; }
	bool				IsSceneState( eGAME_SCENE_STATE eCheckState ) { return ( eCheckState == m_eSceneState ) ? true : false; }

protected:	
	eGAME_SCENE_STATE	m_eSceneState;

public:
	struct sSearchObj
	{
		sSearchObj() : s_niCmpPos(NiPoint2::ZERO), s_fLen(0.0f), s_nFindFTID(0), s_nObjType(RTTI_NONE), s_bChecking(true)
		{}

	//	bool operator()(CsC_AvObject const* lhs)
		void operator()(const std::pair<DWORD,CsC_AvObject*>& lhs)
		{
			if( NULL == lhs.second )
				return ;
			// 숨겨진 엔피시 걸러내기
			if( eRTTI_CLIENT::RTTI_NPC == lhs.second->GetLeafRTTI() )
			{
				CNpc const* pNpc = static_cast<CNpc const*>(lhs.second);
				if( !pNpc->IsEnableUse() )
					return ;
			}

			float fTarLen = (lhs.second->GetPos2D() - s_niCmpPos).Length();
			if( fTarLen > s_fLen )
				return ;

			s_fLen = fTarLen;
			s_nFindFTID = lhs.second->GetFTID();
			s_nObjType = lhs.second->GetLeafRTTI();
			s_niSearchPos = lhs.second->GetPos2D();
		}

		bool operator==(const sSearchObj& lhs) const
		{
			if( lhs.s_nFindFTID != s_nFindFTID )
				return false;
			if( lhs.s_nObjType != s_nObjType )
				return false;

			return true;
		}

		void Reset(){ s_niCmpPos = NiPoint2::ZERO; s_fLen = 0.0f; s_nFindFTID = 0; s_nObjType = RTTI_NONE; }
		void SetTargetPos(NiPoint2 niPos) { s_niCmpPos = niPos; }
		void SetLength(float fLen) { s_fLen = fLen; }

		NiPoint2 s_niCmpPos;
		NiPoint2 s_niSearchPos;
		float s_fLen;
		UINT s_nFindFTID;
		UINT s_nObjType;
		bool s_bChecking;
	};

private:
	sSearchObj		m_sSearched;

private:
	void			_ChangeNearObject( sSearchObj const & sChangeObj );
public:
	void			ResetSearched();

private:
	cText			m_Text;
private:
	void			_RenderText( CsPoint const& pos, dm_str_t cptr szformat, ... );
};

extern CsGBTerrainMng*		g_pTerrainMng;
extern CMngCollector*		g_pMngCollector;

