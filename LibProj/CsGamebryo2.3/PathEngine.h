

#pragma once

#include "..\PathEngine\i_pathengine.h"
#include "..\PathEngine\i_contentprocessing3d.h"
#include "..\PathEngine\PathEngine_DirectLinkage.h"

class CsErrorHandler : public iErrorHandler
{
    CsErrorHandler(){}
public:
    eAction handle(const char* error_type, const char* error_string, const char *const* attributes)
    {
		MessageBoxA(NULL, error_string, error_type, MB_OK);
        return CONTINUE;
    }
    static CsErrorHandler& refInstance()
    {
        static CsErrorHandler the_instance;
        return the_instance;
    }
};

inline char*
LoadBinary_AllocatedBuffer(const char* fileName, size_t& size)
{
    size = 0;
    FILE* fp = NULL;
	fopen_s( &fp, fileName, "rb" );
	assert_cs( fp != NULL );

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* buffer = csnew char[size + 1];
    fread(buffer, 1, size, fp);
    buffer[size] = 0;
    fclose(fp);

    return buffer;
}

class CsPath : public NiMemObject
{
public:
	CsPath()
	{
		m_iPath = NULL;
		m_iAgent = NULL;
		m_iApplyShape = NULL;
	}
	virtual ~CsPath()
	{
		DeleteAgent();		
	}
	void DeletePath(){
		SAFE_DELETE( m_iPath );
	}
	void DeleteAgent(){
		DeletePath();
		SAFE_DELETE( m_iAgent );
	}
public:
	iShape* m_iApplyShape;
	iAgent* m_iAgent;
	iPath*  m_iPath;
	UINT	m_nPathSize;
};

class CsPathEngine : public NiMemObject
{
public:
	CsPathEngine()
	{
		m_iPathEngine = NULL;
		m_iMesh = NULL;
		m_iShapeAgent = NULL;
		m_iContext = NULL;
	}

	virtual ~CsPathEngine()
	{
		if( m_iContext ) m_iContext->destroy();
		if( m_iShapeAgent ) m_iShapeAgent->destroy();
		//if( m_iShapeAgentNormalSize ) m_iShapeAgentNormalSize->destroy();
		if( m_iMesh ) m_iMesh->destroy();
		if( m_iPathEngine ) m_iPathEngine->deleteAllObjects();
	}

	bool init();

	virtual bool CreateMesh( const char* cFile );
	virtual void DestroyMesh();

	void ResetContext();

	bool GetCollisionSlidingPoint(CsPath* pPath, NiPoint3 const& niCurrPos,NiPoint3 const& nMove, NiPoint2& niDest);

	void AddAgent( CsPath* pPath, float sx, float sy );
	void AddAgentNormalSize( CsPath* pPath, float sx, float sy );

	bool PathTest( CsPath* pPath, float x, float y );
	bool PathTestLine( CsPath* pPath, float x, float y );
	bool GetFirstCollisionPoint( CsPath* pPath, float x, float y, CsPoint& res );

	iPathEngine*	LoadPathEngine(TCHAR* fileName, iErrorHandler* handler);
	iPath*			FindShortestPathInfo( CsPath* pPath, float x, float y );

	bool			SetPath( CsPath* pPath, float x, float y );
	bool			UpdatePath( CsPath* pPath, float speed, float& dx, float& dy, float& x, float& y, float fDeltaTime, bool bAbsoluteRotation = false );	

	virtual bool	SetShape( float px, float py, int count, long* data);
			iAgent* SetDynamicShape( float px, float py, long dataX, long dataY );
			void	DelDynamicShape( iAgent* pDel );

public:
	bool		IsLoad(){ return (m_iMesh != NULL ); }

public:
	iPathEngine* m_iPathEngine;
	
	iMesh* m_iMesh;
	iShape* m_iShapeAgent;
	iCollisionContext* m_iContext;
};

extern CsPathEngine* g_pCsPathEngine;

