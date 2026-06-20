
#pragma once 


#include "Tamer.h"

class CsTamerMng
{

public:
	void			Delete();
	bool			Init( char* cPath );


protected:	
	CsTamer::MAP		m_mapTamer;
	CsEmotion::MAP		m_mapEmotion;

protected:
	void			_LoadExcel();
	void			_LoadExcel_Tamer( char* cExcel );
	void			_LoadExcel_Emotion( char* cExcel );

	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

public:
	void			SaveBin( char* cPath );


public:
	CsTamer*			GetTamer( DWORD dwTamerID );
	bool				IsTamer( DWORD dwTamerID ){ return ( m_mapTamer.find( dwTamerID ) != m_mapTamer.end() ); }
	CsTamer::MAP*		GetTamerMap(){ return &m_mapTamer; }


public:
	bool				IsEmotion( int nID ){ return ( m_mapEmotion.find( nID ) != m_mapEmotion.end() ); }
	CsEmotion*			GetEmotion( int nID ){ return m_mapEmotion[ nID ]; }
	CsEmotion::MAP*		GetEmotionMap(){ return &m_mapEmotion; }
	CsEmotion::sINFO*	FindEmotion( TCHAR const* szStr, int& nFindIndex );
};

