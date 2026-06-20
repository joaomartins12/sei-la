
#pragma once 

#include "Talk_Digimon.h"
#include "Talk_Tutorial.h"
#include "Talk_Event.h"
#include "Talk_Tip.h"
#include "Talk_Message.h"
#ifdef SIMPLE_TOOLTIP
#include "Talk_Tooltip.h"
#endif

class CsTalkMng
{
public:
	void			Delete();
	bool			Init( char* cPath );

protected:
	void			_LoadExcel();
	bool			_LoadBin( char* cPath );
	bool			_LoadFilePack( char* cPath );

public:
	void			SaveBin( char* cPath );

	//======================================================================================================
	// 디지몬 대화
	//======================================================================================================
protected:
	CsTalk_Digimon::MAP		m_mapDigimon;

protected:
	void					_Delete_Digimon();

	void					_LoadExcel_Digimon();
	void					_LoadBin_Digimon( FILE* fp );
	void					_LoadFilePack_Digimon( int nHandle );

	void					_SaveBin_Digimon( FILE* fp );

public:
	CsTalk_Digimon*			GetTalk_Digimon( DWORD dwID );
	bool					IsExist_Digimon( DWORD dwID );
	CsTalk_Digimon::MAP*	GetMap_Digimon(){ return &m_mapDigimon; }

	//======================================================================================================
	// 이벤트 대화
	//======================================================================================================
protected:
	CsTalk_Event::MAP		m_mapEvent;

protected:
	void					_Delete_Event();

	void					_LoadExcel_Event();
	void					_LoadBin_Event( FILE* fp );
	void					_LoadFilePack_Event( int nHandle );

	void					_SaveBin_Event( FILE* fp );

public:
	CsTalk_Event*			GetTalk_Event( DWORD dwID );
	bool					IsExist_Event( DWORD dwID ){ return ( m_mapEvent.find( dwID ) != m_mapEvent.end() ); }
	CsTalk_Event::MAP*		GetMap_Event() { return &m_mapEvent; }
	void					EventTalk_Reload() { _LoadExcel_Event(); }
#ifdef SIMPLE_TOOLTIP
	//======================================================================================================
	// 툴팁
	//======================================================================================================
protected:
	CsTalk_Tooltip::MAP		m_mapTooltip;

public:

	void					_LoadExcel_ToolTip();
	void					_LoadBin_ToolTip( FILE* fp );
	void					_LoadFilePack_ToolTip( int nHandle );

	void					_SaveBin_ToolTip( FILE* fp );

	CsTalk_Tooltip*			GetTalk_Tooltip( DWORD dwID );
	bool					IsExist_Tooltip( DWORD dwID ){ return ( m_mapTooltip.find( dwID ) != m_mapTooltip.end() ); }
	CsTalk_Tooltip::MAP*	GetMap_Tooltip() { return &m_mapTooltip; }
#endif
	//======================================================================================================
	// 메세지 대화
	//======================================================================================================
protected:
	CsTalk_Message::MAP		m_mapMessage;

protected:
	void					_Delete_Message();

	void					_LoadExcel_Message();
	void					_LoadBin_Message( FILE* fp );
	void					_LoadFilePack_Message( int nHandle );

	void					_SaveBin_Message( FILE* fp );

	void					_CheckWordOrder( TCHAR* szKor, TCHAR* szEng, DWORD dwID );

public:
	CsTalk_Message*			GetTalk_Message( DWORD dwID );
	bool					IsExist_Message( DWORD dwID ){ return ( m_mapMessage.find( dwID ) != m_mapMessage.end() ); }
	CsTalk_Message::MAP*	GetMap_Message() { return &m_mapMessage; }

	//======================================================================================================
	// 팁
	//======================================================================================================
protected:
	CsTalk_Tip::MAP			m_mapTip;

protected:
	void					_Delete_Tip();

	void					_LoadExcel_Tip();
	void					_LoadBin_Tip( FILE* fp );
	void					_LoadFilePack_Tip( int nHandle );

	void					_SaveBin_Tip( FILE* fp );

public:
	CsTalk_Tip*				GetRandom_Tip();
	CsTalk_Tip*				GetTalk_Tip( DWORD dwID ){ assert_cs( IsExist_Tip( dwID ) ); return m_mapTip[ dwID ]; }
	bool					IsExist_Tip( DWORD dwID ){ return ( m_mapTip.find( dwID ) != m_mapTip.end() ); }
	CsTalk_Tip::MAP*		GetMap_Tip() { return &m_mapTip; }

	//======================================================================================================
	// 로딩 팁
	//======================================================================================================
protected:
	CsLoading_Tip::MAP			m_mapLodingTip;

protected:
	void					_Delete_LoadingTip();

	void					_LoadExcel_LoadingTip();
	void					_LoadBin_LoadingTip( FILE* fp );
	void					_LoadFilePack_LoadingTip( int nHandle );

	void					_SaveBin_LoadingTip( FILE* fp );

public:
	CsLoading_Tip*			GetRandom_LoadingTip( int nLevel );
	CsLoading_Tip*			GetTalk_LoadingTip( DWORD dwID ){ assert_cs( IsExist_LoadingTip( dwID ) ); return m_mapLodingTip[ dwID ]; }
	bool					IsExist_LoadingTip( DWORD dwID ){ return ( m_mapLodingTip.find( dwID ) != m_mapLodingTip.end() ); }
	CsLoading_Tip::MAP*		GetMap_LoadingTip() { return &m_mapLodingTip; }



	//======================================================================================================
	// 튜토리얼
	//======================================================================================================
protected:
	CsTalk_Tutorial::MAP	m_mapTutorial;
public:
	void					Delete_Tutorial();
	bool					Load_Tutorial();

protected:
	void					_LoadExcel_Tutorial();
	bool					_LoadBin_Tutorial( char* cPath );
	void					_LoadFilePack_Tutorial( char* cPath );

	void					_SaveBin_Tutorial( char* cPath );

public:
	CsTalk_Tutorial*		GetTalk_Tutorial( DWORD dwID );
	bool					IsExist_Tutorial( DWORD dwID ){ return ( m_mapTutorial.find( dwID ) != m_mapTutorial.end() ); }

};