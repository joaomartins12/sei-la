

#pragma once

class __CsStringChange
{
public:
	virtual ~__CsStringChange(){ Delete(); }

public:
	static TCHAR*	NewLineCheck( TCHAR* sz );

protected:
	struct sSTRING_CHANGE
	{
		TCHAR		s_Src[ STRING_CHANGE_LEN ];
		TCHAR		s_Dest[ STRING_CHANGE_LEN ];
	};

protected:
	std::list< sSTRING_CHANGE* >		m_List;


public:
	void			Delete();
	void			Load( nsCsFileTable::eLANGUAGE language );

	void			Change( TCHAR* sz, int nLen );


protected:	
	void		_LoadStringChange( char* cExcel, char* cSheet );
};