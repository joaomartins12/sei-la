
#pragma once 

class CsWeatherGroup
{
public:
	typedef	std::map< DWORD, CsWeatherGroup* >				MAP;
	typedef	std::map< DWORD, CsWeatherGroup* >::iterator	MAP_IT;

protected:
	CsWeather::LIST		m_list;	

public:
	void					Delete();
	void					Init();

	CsWeather*				Add( CsWeather::sINFO* pSrcInfo );
	void					Delete( CsWeather* pElement );
	CsWeather::LIST*		GetList(){ return &m_list; }	
	void					Clone( CsWeatherGroup* pSrcGroup );

	CsWeather::sELEMENT*	UpdateWeather( UINT nTime, int x, int y );
};

