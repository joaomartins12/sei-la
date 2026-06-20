
#pragma once 

class CsUI_Text
{
public:
	typedef	std::map< UINT64, std::wstring >				MAP;
	typedef	std::map< UINT64, std::wstring >::iterator		MAP_IT;
};

class CsUITextMng
{
public:
	CsUITextMng() {};
	~CsUITextMng() {};

	bool					Init( char* cPath );
	void					Delete();

public:
	bool					LoadFilePack( char* cPath );
	void					SaveBin( char* cPath );

	std::wstring			GetUIText( char* pKey );
	std::wstring			GetUIText_VA( char* pKey, ... );

private:
	bool					_LoadBin( char* cPath );
	void					_Delete_UI_Text();
	bool					_LoadFilePack( char* cPath );
	void					_LoadExcel_UI_Text();
	void					_LoadBin_UI_Text( FILE* fp );
	void					_LoadFilePack_UI_Text( int nHandle );

	void					_SaveBin_UI_Text( FILE* fp );

private:
	CsUI_Text::MAP			m_mapUI_Text;
};