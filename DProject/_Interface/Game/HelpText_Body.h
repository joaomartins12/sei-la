

#pragma once 


class cHelpText_Body : public NiMemObject
{
public:
	cHelpText_Body();

public:
	void				Delete();
	void				DeleteBodyResource();
	void				Init();

	void				Update(float const& fDeltaTime);
	bool				Update_ForMouse( CsPoint ptLocal );
	void				Render( CsPoint ptParent );

	void				ResetDevice();

	//=================================================================================
	//		리소스
	//=================================================================================
public:
	struct sBODY_BASE
	{
		virtual ~sBODY_BASE(){}
		virtual CsHelp::eTYPE	GetType() = 0;
		virtual void			ResetDevice() = 0;
		virtual void			Render( CsPoint ptParent ){}		

		CsHelp::sELEMENT*		s_pFT;
	};
	struct sBODY_STR : public sBODY_BASE
	{
		virtual ~sBODY_STR(){ s_StringList.Delete(); }
		virtual CsHelp::eTYPE	GetType(){ return CsHelp::STRING; }
		virtual void			ResetDevice(){ s_StringList.ResetDevice(); }
		virtual void			Render( CsPoint ptParent ){ s_StringList.Render_LineHeight( s_pFT->s_ptPos + ptParent ); }
		cStringList				s_StringList;		
	};
	struct sBODY_IMAGE : public sBODY_BASE
	{
		sBODY_IMAGE(){ s_pSprite = NULL; }
		virtual ~sBODY_IMAGE(){ SAFE_NIDELETE( s_pSprite ); }
		virtual CsHelp::eTYPE	GetType(){ return CsHelp::IMAGE; }
		virtual void			ResetDevice(){ if( s_pSprite ){ s_pSprite->ResetDevice(); } }
		virtual void			Render( CsPoint ptParent );		

		cSprite*				s_pSprite;		
	};
protected:
	CsHelp::sCONTAINER*			m_pCurContainerFT;
	cSprite*					m_pStencilSprite;
	cScrollBar*					m_pScrollBarBody;
	CsVectorPB< sBODY_BASE* >	m_vpBody;
	cButton*					m_pTopBtn;

public:
	void				SetLine( int nLine );
	void				ApplyContainer( CsHelp::sCONTAINER* pConFT );
	void				ApplyImage( CsHelp::sIMAGE* pImageFT, bool bSelectBody );
	void				ApplyString( CsHelp::sSTRING* pStrFT, bool bSelectBody );
protected:
	sBODY_BASE*			_GetBody( CsHelp::sELEMENT* pEleFT );
	CsPoint				_GetScrollPos();
	void				_PlayVideo();
	void				_Pick( CsPoint ptLocal );

	//=================================================================================
	//		동영상
	//=================================================================================
protected:
	cSprite*			m_pVideoBackImage;
public:
	cSprite*			GetVideoBG(){ return m_pVideoBackImage; }
};