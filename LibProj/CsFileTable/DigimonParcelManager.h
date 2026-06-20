
#pragma once 

class CsDigimonParcelMng
{
public:
	CsDigimonParcelMng();
	~CsDigimonParcelMng();

	bool					Init( char* cPath );
	void					Delete();

public:
	bool					LoadFilePack( char* cPath );
	void					SaveBin( char* cPath );

	bool					IsParcelDigimon( DWORD const& dwDigimonIDX ) const;

private:
	void					_Delete();
	bool					_LoadExcel();
	bool					_LoadBin( char* cPath );
	bool					_LoadFilePack( char* cPath );


	bool					_LoadExcel_ParcelDigimon( char* pExcelFile, char* pExcelSheetName );


private:
	std::list<DWORD>		m_listDigimon;
};
