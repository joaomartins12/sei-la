
#pragma once 

class CsEffectMng
{
public:
	CsEffectMng();
	~CsEffectMng();

	bool			Init( char* cPath );
	void			Delete();

	std::string		GetEquipEffectFile(DWORD const& dwItemCode) const;

protected:
	void			_LoadExcel();
	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );


public:
	void			SaveBin( char* cPath );	

	std::map<DWORD,std::string>	m_EquipEffect;
};