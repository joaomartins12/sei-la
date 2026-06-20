#pragma once

class CEventDataMng : public NiMemObject
{
public:
	CEventDataMng();
	virtual ~CEventDataMng();

public:
	void Delete();
	void Init();

	void SaveFile(); /**< 파일 저장 */
	bool InsertData( UINT nSceneNum, CEventSceneData::sEvent_Condition* pCon, std::stack< CEventData* >* pStackEventData ); /**< 이벤트 씬 저장 */

	void				DeleteData( UINT nIdx );
	bool				IsData( UINT nIndx ) { return ( m_mapEventSceneData.find( nIndx ) != m_mapEventSceneData.end() ); }
	CEventSceneData*	GetData( UINT nIdx ){ return ( IsData( nIdx ) == true ) ? m_mapEventSceneData[ nIdx ] : NULL ; }

	CEventSceneData*	LoadData( UINT nIndx ) { assert_csm1( IsData( nIndx ) == true, _T( "씬없음 번호 = %d" ), nIndx ); return m_mapEventSceneData[ nIndx ]; }
	std::map< UINT, CEventSceneData* >* GetMap(){ return &m_mapEventSceneData; }	

protected:
	void LoadFile();
	void LoadFilePack();
	void LoadEvent( FILE* fp, UINT SceneNum );
	void LoadEventPack( int nHandle, UINT SceneNum );
	void SaveEvent( FILE* fp, CEventData* pData );	

protected:
	std::map< UINT, CEventSceneData* > m_mapEventSceneData; //이벤트 씬 정보 저장

};

extern CEventDataMng* g_pEventDataMng;