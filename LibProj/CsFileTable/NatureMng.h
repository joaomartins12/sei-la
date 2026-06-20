
#pragma once 

#include "Nature.h"
#include "BaseElement.h"

class CsNatureMng
{
public:
	void			Delete();
	bool			Init( char* cPath );

protected:
	CsNature::MAP	m_mapNatuer;
	MAP_CMP_ELEMENT m_mapCmpElement;

protected:
	void			_LoadExcel();
	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

	void			_LoadExcelBaseElement();
	void			_LoadBinBaseElement( FILE* fp );
	void			_LoadFilePackBaseElement( int nHandle );
	void			_SaveBinBaseElement( FILE* fp );

public:
	bool			IsNature( int nID ){ return ( m_mapNatuer.find( nID ) != m_mapNatuer.end() ); }
	CsNature*		GetNature( int nID );
	int				GetValue( int nOwner, int nTarget );

	bool			IsSuperiority( nsBaseElement::eCompareType eCmpType, nsBaseElement::eElementType eOrgType, nsBaseElement::eElementType eSrcType );
	bool			IsInferiority( nsBaseElement::eCompareType eCmpType, nsBaseElement::eElementType eOrgType, nsBaseElement::eElementType eSrcType );

public:
	void			SaveBin( char* cPath );	
};