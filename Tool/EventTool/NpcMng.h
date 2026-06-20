
#pragma once

#define OBJECT_MAX_COUNT					4095
#define MAX_CHAR_ID_COUNT					4096
#define NPC_FT_MASK							90000

class CNpcMng : public NiMemObject
{
public:		
	static CsC_AvObject*  CALLBACK_GetNpcObject( DWORD dwId );

protected:	
	CsVectorLink< CNpc* >			m_vnNpc;

public:
	void			Delete();
	void			DeleteChar( CsC_AvObject* pObject );
	void			Init();	
	void			ResetOldMap(){ m_dwOldMapID = 0; }

	void			ResetShader();
	void			NpcReset();

	void			Update();
	void			Render();	

	bool			LoadMap( DWORD dwMapID );
	void			AddNpc( uint nIDX, uint nFileTableID );

	CNpc*			GetNpc( uint nIDX );
	bool			IsNpc( uint nIDX ){ return m_vnNpc.IsElement( nIDX%NPC_FT_MASK ); }

	DWORD			GetMapID() { return m_dwOldMapID; }
protected:
	DWORD			m_dwOldMapID;

};

extern CNpcMng* g_pNpcMng;