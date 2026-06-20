



#pragma once 


class cHelpText_Title : public NiMemObject
{
public:
	cHelpText_Title();

public:
	void		Delete();
	void		Init();

	void		Update(float const& fDeltaTime);
	bool		Update_ForMouse( CsPoint ptLocal );

	void		Render( CsPoint ptParent );

	void		ResetDevice();

public:
	struct sINFO : public NiMemObject
	{
		sINFO(){ s_pText = NULL; s_pFT = NULL; }
		void	Delete();
		void	Init( cText::sTEXTINFO* ti, CsHelp* pHelpFT );
		void	Render( CsPoint pos );
		cText*				s_pText;
		CsHelp*				s_pFT;
	};

public:
	sINFO*		Pick( CsPoint ptLocal );
	void		SetSelectTitle( sINFO* pSelect );
	void		SetSelectTitle( int nUniqID );
	sINFO*		GetSelectTitle(){ return m_pSelectTitle; }

protected:
	CsVectorPB< sINFO* >	m_vpInfo;
	cImage*					m_pSelectMask;
	sINFO*					m_pSelectTitle;
};