#pragma once

class CsEffectSoundMng : public NiMemObject
{
public:
	CsEffectSoundMng(void);
	~CsEffectSoundMng(void);

protected:
	std::map< size_t, CsEffectSound* > m_mapEffectSound;

public:
	void Delete();
	void Init();

	void SaveFile();
	void LoadFile();
	void LoadFilePack();

	void InsertData( CsEffectSound* cs_EsData ); /**< 이펙트사운드 파일 저장 */
	std::map< size_t, CsEffectSound* >* GetMap() { return &m_mapEffectSound; }	

public:
	bool		   IsData( size_t HScode ) { return (m_mapEffectSound.find( HScode ) != m_mapEffectSound.end() ); }
	CsEffectSound* GetData( size_t HScode ) { return m_mapEffectSound[ HScode ]; }

};

extern CsEffectSoundMng* g_EffectSoundMng;
