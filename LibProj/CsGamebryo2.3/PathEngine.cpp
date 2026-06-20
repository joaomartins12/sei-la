#include "stdafx.h"
#include "PathEngine.h"

CsPathEngine* g_pCsPathEngine = NULL;


typedef iPathEngine* (__stdcall* tGetInterfaceFunction)(iErrorHandler*);

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
bool CsPathEngine::init(void)
{
    m_iPathEngine = LoadPathEngine( _T( "pathengine" ), &CsErrorHandler::refInstance());
    if( !m_iPathEngine ) return false;
  
	{
		int mi = 20;
		int mx = 60;
		long array[]=
		{
			 mi, mx,
			 mx, mi,
			 mx,-mi,
			 mi,-mx,
			-mi,-mx,
			-mx,-mi,
			-mx, mi,
			-mi, mx,
		};
		m_iShapeAgent = m_iPathEngine->newShape(sizeof(array)/sizeof(*array)/2,array);	
		assert_cs( m_iShapeAgent ); 
	}

	/*{
		int mi = 35;
		int mx = 80;
		long array[]=
		{
			mi, mx,
			mx, mi,
			mx,-mi,
			mi,-mx,
			-mi,-mx,
			-mx,-mi,
			-mx, mi,
			-mi, mx,
		};
		m_iShapeAgentNormalSize = m_iPathEngine->newShape(sizeof(array)/sizeof(*array)/2,array);	
		assert_cs( m_iShapeAgentNormalSize ); 
	}*/

	return true;
}

bool CsPathEngine::CreateMesh( const char* cFile )
{
	char* buffer = NULL;
	size_t size;

	if( CsFPS::CsFPSystem::IsExistOnlyPack( 0, cFile ) )
	{
		size = CsFPS::CsFPSystem::Allocate_GetFileData( 0, &buffer, cFile );
	}
	else if( _access_s( cFile, 0 ) == 0 )
		buffer = LoadBinary_AllocatedBuffer( cFile ,size );
	else
	{
		if( nsCsGBTerrain::g_SaveLoad.g_bLoadPathEngineWarning == true )
		{
			MessageBox( NULL, _T( "해당 지역의 ground가 존재 하지 않아 더미 그라운드로 대체합니다." ), NULL, MB_OK );
		}
		char sz[ 64 ] = "ToolData\\Ground_Dummy.xml";
		return CreateMesh( sz );
	}

	m_iMesh = m_iPathEngine->loadMeshFromBuffer( "xml", buffer, (ULONG)size, 0 );
	SAFE_DELETE_ARRAY(buffer);

	if( !m_iMesh )	return false;

	assert_cs( m_iShapeAgent != NULL );
	m_iContext = m_iMesh->newContext();
	m_iMesh->generateCollisionPreprocessFor(m_iShapeAgent, 0);
	m_iMesh->generatePathfindPreprocessFor(m_iShapeAgent, 0);

	
	return true;
}

void CsPathEngine::DestroyMesh()
{
	if( m_iContext )
	{
		m_iContext->destroy();
		m_iContext = NULL;
	}

	if( m_iMesh )
	{
		m_iMesh->destroy();
		m_iMesh = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
void CsPathEngine::AddAgent( CsPath* pPath, float sx, float sy )
{
	assert_cs( m_iMesh != NULL );
	assert_cs( pPath->m_iAgent == NULL );

	cPosition p;
	p.x = (long)(sx);// * 100.f);
	p.y = (long)(sy);// * 100.f);
	p.cell = 0;
    pPath->m_iAgent = m_iMesh->placeAgent(m_iShapeAgent,p);
	assert_cs( pPath->m_iAgent != NULL );

	pPath->m_iApplyShape = m_iShapeAgent;
}

void CsPathEngine::AddAgentNormalSize( CsPath* pPath, float sx, float sy )
{
	assert_cs( false );

	assert_cs( m_iMesh != NULL );
	assert_cs( pPath->m_iAgent == NULL );

	cPosition p;
	p.x = (long)(sx);// * 100.f);
	p.y = (long)(sy);// * 100.f);
	p.cell = 0;

	//pPath->m_iAgent = m_iMesh->placeAgent(m_iShapeAgentNormalSize,p);
	assert_cs( pPath->m_iAgent != NULL );

	//pPath->m_iApplyShape = m_iShapeAgentNormalSize;
}

bool CsPathEngine::SetShape( float px, float py, int count, long* data )
{
	assert_cs( m_iPathEngine != NULL );
	assert_cs( m_iMesh != NULL );
	assert_cs( m_iContext != NULL );

	cPosition p;
	p.x = (long)(px);// * 100.f);
	p.y = (long)(py);// * 100.f);
	p.cell = 0;

    iShape* shape = m_iPathEngine->newShape(count,data);
	if( !shape )
	{
		return false;
	}

    iAgent* agent = m_iMesh->placeAgent(shape,p);
	if( !agent )
	{
		SAFE_DELETE( shape );
		return false;
	}

    m_iContext->addAgent(agent);

	return true;
}

iAgent* CsPathEngine::SetDynamicShape( float px, float py, long dataX, long dataY )
{
	assert_cs( m_iPathEngine != NULL );
	assert_cs( m_iMesh != NULL );
	assert_cs( m_iContext != NULL );

	cPosition p;
	p.x = (long)(px);// * 100.f);
	p.y = (long)(py);// * 100.f);
	p.cell = 0;

	long data[ 8 ];
	data[ 0 ] = -dataX;		data[ 1 ] = -dataY;
	data[ 2 ] = -dataX;		data[ 3 ] = dataY;
	data[ 4 ] = dataX;		data[ 5 ] = dataY;
	data[ 6 ] = dataX;		data[ 7 ] = -dataY;

	iShape* shape = m_iPathEngine->newShape(4,data);
	if( !shape )
	{
		return NULL;
	}

	iAgent* agent = m_iMesh->placeAgent(shape,p);
	if( !agent )
	{
		SAFE_DELETE( shape );
		return NULL;
	}

	m_iContext->addAgent(agent);

	return agent;
}

void CsPathEngine::DelDynamicShape( iAgent* pDel )
{
	if( pDel == NULL )
		return;

	m_iContext->removeAgent( pDel );
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
iPathEngine* CsPathEngine::LoadPathEngine(TCHAR* fileName, iErrorHandler* handler)
{
    TCHAR buffer[500];
    DWORD errorValue;
    HINSTANCE hInstance;
    hInstance = LoadLibrary(fileName);
    if(!hInstance)
    {
        errorValue = GetLastError();
        MessageBox(NULL, fileName, _T( "Error: failed calling LoadLibrary() for" ), MB_OK);
        wsprintf(buffer, _T( "%d" ), errorValue);
        MessageBox(NULL, buffer, _T( "GetLastError()" ), MB_OK);
        return 0;
    }
    FARPROC procAddress;
    SetLastError(0);
    procAddress = GetProcAddress(hInstance, (LPCSTR)1);
    if(!procAddress)
    {
        errorValue = GetLastError();
        MessageBox(NULL, fileName, _T( "Error: Failed to obtain PathEngine entrypoint in" ), MB_OK);
        wsprintf(buffer, _T( "%d" ), errorValue);
        MessageBox(NULL, buffer, _T( "GetLastError()" ), MB_OK);
        return 0;
    }

    tGetInterfaceFunction getInterfaceFunction = (tGetInterfaceFunction)procAddress;
    return getInterfaceFunction(handler);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
bool CsPathEngine::SetPath( CsPath* pPath, float x, float y )
{
	assert_cs( pPath != NULL );
	assert_cs( pPath->m_iAgent != NULL );
	assert_cs( m_iContext != NULL );

	cPosition p;
	p.x = (long)(x);// * 100.f);
	p.y = (long)(y);// * 100.f);
	p.cell = 0;

	// 패스 지우기전에 갈 수 있는 길 확인, 오브젝트 충돌박스 작업하면서 같이 작업 2014.02.06 chu8820
	iPath* pPathTest = pPath->m_iAgent->findShortestPathTo(m_iContext, p);

	if( pPathTest == NULL )// 갈 수 없는 곳이면 빠져나감
		return false;

	pPath->DeletePath();
	pPath->m_iPath = pPathTest;

	// 첨에 방향성을 잡기 위해서
	if( pPath->m_iPath != NULL )
		pPath->m_nPathSize = pPath->m_iPath->size() + 1;

	return true;
}

iPath* CsPathEngine::FindShortestPathInfo( CsPath* pPath, float x, float y )
{
	if( NULL == pPath )
		return NULL;

	if( NULL == pPath->m_iAgent )
		return NULL;

	if( NULL == m_iContext )
		return NULL;

	cPosition p;
	p.x = (long)(x);// * 100.f);
	p.y = (long)(y);// * 100.f);
	p.cell = 0;

	return pPath->m_iAgent->findShortestPathTo(m_iContext, p);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
bool CsPathEngine::UpdatePath( CsPath* pPath, float speed, float& dx, float& dy, float& x, float& y, float fDeltaTime, bool bAbsoluteRotation /*=false*/ )
{
	assert_cs( pPath != NULL );
	assert_cs( pPath->m_iPath != NULL );
	assert_cs( pPath->m_iAgent != NULL );

	pPath->m_iAgent->advanceAlongPath(pPath->m_iPath,speed*fDeltaTime,m_iContext);

	cPosition p = pPath->m_iAgent->getPosition();
	x = (float)p.x;
	y = (float)p.y;

	UINT nCurSize = pPath->m_iPath->size();
	if( ( nCurSize != pPath->m_nPathSize )|| bAbsoluteRotation )
	{
		pPath->m_nPathSize = nCurSize;
		if( pPath->m_iPath->size() > 1 )
		{
			cPosition next_target = pPath->m_iPath->position(1);
			dx = (float)(next_target.x - p.x);
			dy = (float)(next_target.y - p.y);
			return true;
		}
	}	
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
bool CsPathEngine::PathTest( CsPath* pPath, float x, float y )
{
	assert_cs( m_iMesh != NULL );

	cPosition p;
	p.x = (long)(x);
	p.y = (long)(y);
	p.cell = 0;

	if( m_iMesh->testPointCollision(pPath->m_iApplyShape,m_iContext,p) )
	{
		return false;
	}

	return true;
}

bool CsPathEngine::PathTestLine( CsPath* pPath, float x, float y )
{
	assert_cs( m_iMesh != NULL );

	cPosition p;
	p.x = (long)(x);
	p.y = (long)(y);
	p.cell = 0;	

	if( pPath->m_iAgent->getMesh()->positionIsValid( p ) == false )
	{
 		assert_cs( false );
		return false;
	}

	if( pPath->m_iAgent->testCollisionTo( m_iContext, p ) )
	{
		return false;
	}
	return true;
}

bool GetIntersectPoint( CsPoint& a1, CsPoint& a2, CsPoint& b1, CsPoint& b2, CsPoint& res )
{
	double under = (b2.y - b1.y)*(a2.x-a1.x) - (b2.x-b1.x)*(a2.y-a1.y );
	if( under == 0 )
		return false;

	double _t = (b2.x-b1.x)*(a1.y-b1.y) - (b2.y-b1.y)*(a1.x-b1.x);
	double _s = (a2.x-a1.x)*(a1.y-b1.y) - (a2.y-a1.y)*(a1.x-b1.x);

	double t = _t/under;
	double s = _s/under;

	if( (t<0.0)||(t>1.0)||(s<0.0)||(s>1.0) )
		return false;

	if( ( _t == 0 )&&( _s == 0 ) )
		return false;

	res.x = (long)( a1.x + t*(double)(a2.x-a1.x) );
	res.y = (long)( a1.y + t*(double)(a2.y-a1.y) );
	return true;
}

bool CsPathEngine::GetFirstCollisionPoint( CsPath* pPath, float x, float y, CsPoint& res )
{
	long cell = 0;
	iCollisionInfo* pColInfo = pPath->m_iAgent->firstCollisionTo( m_iContext, (long)x, (long)y, cell );
	if( pColInfo == NULL )
		return false;

	long coords[ 4 ];
	pColInfo->getCollidingLine( coords );

	cPosition CurPos = pPath->m_iAgent->getPosition();
	
	CsPoint a1( CurPos.x, CurPos.y );
	CsPoint a2( (int)x, (int)y );
	CsPoint b1( coords[ 0 ], coords[ 1 ] );
	CsPoint b2( coords[ 2 ], coords[ 3 ] );

	return GetIntersectPoint( a1, a2, b1, b2, res );
}

void CsPathEngine::ResetContext()
{
	if( m_iContext )
	{
		m_iContext->destroy();
		m_iContext = NULL;
	}
	m_iContext = m_iMesh->newContext();
}
//////////////////////////////////////////////////////////////////////////
enum eSide
{
	SIDE_CENTRE,
	SIDE_LEFT,
	SIDE_RIGHT
};

inline eSide SideOfLine(const tSigned32 *line, tSigned32 x, tSigned32 y)
{
	tSigned32 axisX=line[2]-line[0];
	tSigned32 axisY=line[3]-line[1];
	x-=line[0];
	y-=line[1];
	tSigned32 lhs,rhs;
	lhs=x*axisY;
	rhs=y*axisX;
	if(lhs==rhs)
		return SIDE_CENTRE;
	if(lhs<rhs)
		return SIDE_LEFT;
	return SIDE_RIGHT;    
}

inline void PushToLeftOfLine(const tSigned32 *line, tSigned32 &x, tSigned32 &y)
{
	tSigned32 axisX=line[2]-line[0];
	tSigned32 axisY=line[3]-line[1];
	tSigned32 absolute_x,absolute_y;

	absolute_x=axisX;
	if(absolute_x<0)
		absolute_x=-axisX;
	absolute_y=axisY;
	if(absolute_y<0)
		absolute_y=-axisY;

	// force rounding in axis with smaller component
	if(absolute_y>absolute_x)
	{
		if(axisY>0)
			x--;
		else
			x++;
	}
	else
	{
		if(axisX<0)
			y--;
		else
			y++;
	}
}

void SlideAgainst(const tSigned32* collidingLine, tSigned32 currentx, tSigned32 currenty, tSigned32& dx, tSigned32& dy)
{
	float dotproduct = static_cast<float>(dx * (collidingLine[2] - collidingLine[0]) + dy * (collidingLine[3] - collidingLine[1]));
	float ratio = dotproduct;
	tSigned32 axisX=collidingLine[2]-collidingLine[0];
	tSigned32 axisY=collidingLine[3]-collidingLine[1];
	float axisLengthSquared = static_cast<float>(axisX * axisX + axisY * axisY);
	ratio/=axisLengthSquared;
	dx = static_cast<tSigned32>(static_cast<float>(axisX) * ratio);
	dy = static_cast<tSigned32>(static_cast<float>(axisY) * ratio);
	tSigned32 targetx,targety;
	targetx=currentx+dx;
	targety=currenty+dy;
	if(SideOfLine(collidingLine,targetx,targety)==SIDE_RIGHT)
	{
		PushToLeftOfLine(collidingLine,targetx,targety);
		dx=targetx-currentx;
		dy=targety-currenty;
	}
}

// void
// SlideAgainst(const tSigned32* collidingLine, tSigned32 currentX, tSigned32 currentY, double& dx, double& dy)
// {
// 	tSigned32 axisX = collidingLine[2] - collidingLine[0];
// 	tSigned32 axisY = collidingLine[3] - collidingLine[1];
// 	double dotproduct = dx * axisX + dy * axisY;
// 	double ratio = dotproduct;
// 	double axisLengthSquared = axisX * axisX + axisY * axisY;
// 	ratio /= axisLengthSquared;
// 	dx = axisX * ratio;
// 	dy = axisY * ratio;
// 	tSigned32 targetX, targetY;
// 	targetX = currentX + static_cast<tSigned32>(dx);
// 	targetY = currentY + static_cast<tSigned32>(dy);
// 	if(SideOfLine(collidingLine, targetX, targetY) == SIDE_RIGHT)
// 	{
// 		tSigned32 pushedTargetX = targetX;
// 		tSigned32 pushedTargetY = targetY;
// 		PushToLeftOfLine(collidingLine, pushedTargetX, pushedTargetY);
// 		dx += pushedTargetX - targetX;
// 		dy += pushedTargetY - targetY;
// 	}
// }

bool CsPathEngine::GetCollisionSlidingPoint(CsPath* pPath, NiPoint3 const& niCurrPos, NiPoint3 const& nMove, NiPoint2& niDest)
{
// 	SAFE_POINTER_RETVAL( pPath, false );
// 	SAFE_POINTER_RETVAL( pPath->m_iAgent, false );
// 
// 	tSigned32 dx = nMove.x;
// 	tSigned32 dy = nMove.y;
// 
// 	cPosition target;
// 	target.x = niCurrPos.x + dx;	
// 	target.y = niCurrPos.y + dx;
// 	target.cell = 0;
// 	cPosition start;
// 	start.x = niCurrPos.x;
// 	start.y = niCurrPos.y;
// 	start.cell = 0;
// 
// 	tSigned32 endCell;
// 	iCollisionInfo* pColInfo = m_iMesh->firstCollision( pPath->m_iApplyShape, m_iContext, start, target.x, target.y, endCell );
// 	if( NULL == pColInfo )
// 		return false;
// 
// 	// 충돌라인
// 	tSigned32 coords[4];
// 	pColInfo->getCollidingLine(coords);
// 	SAFE_DELETE(pColInfo);
// 
// 	CsPoint a1( niCurrPos.x, niCurrPos.y );
// 	CsPoint a2( target.x, target.y );
// 	CsPoint b1( coords[ 0 ], coords[ 1 ] );
// 	CsPoint b2( coords[ 2 ], coords[ 3 ] );
// 
// 	CsPoint res;
// 	if( !GetIntersectPoint( a1, a2, b1, b2, res ) )
// 		return false;
// 
// 	niDest.x = res.x;
// 	niDest.y = res.y;
// 	return true;
// 
// 	tSigned32 endX = res.x;
// 	tSigned32 endY = res.y;
// 
// 	// cPosition 만들기
// 	tSigned32 cell = m_iMesh->getCellForEndOfLine(start, endX, endY );
// 	if(cell != -1)
// 	{
// 		cPosition pos;
// 		pos.x = endX;
// 		pos.y = endY;
// 		pos.cell = cell;
// 		if( !m_iMesh->positionIsValid( pos ) )
// 			return false;
// 
// 		niDest.x = endX;
// 		niDest.y = endY;
// 		return true;
// 	}
// 	return false;

	SAFE_POINTER_RETVAL( pPath, false );
	SAFE_POINTER_RETVAL( pPath->m_iAgent, false );

	tSigned32 dx = nMove.x;
	tSigned32 dy = nMove.y;

	cPosition target;
	target.x = niCurrPos.x + dx;	
	target.y = niCurrPos.y + dy;
	target.cell = 0;

	cPosition startpos;
	startpos.x = niCurrPos.x;	
	startpos.y = niCurrPos.y;
	startpos.cell = 0;

	//iCollisionInfo* pColInfo = m_iMesh->firstCollision( pPath->m_iApplyShape, m_iContext, startpos, target.x, target.y,target.cell );
	iCollisionInfo* pColInfo = pPath->m_iAgent->firstCollisionTo( m_iContext, target.x,target.y,target.cell );
	if( pColInfo == NULL )
	{
		niDest.x = target.x;
		niDest.y = target.y;
		return false;
	}

	long coords[ 4 ];
	pColInfo->getCollidingLine( coords );

	CsPoint a1( niCurrPos.x, niCurrPos.y );
	CsPoint a2( target.x, target.y );
	CsPoint b1( coords[ 0 ], coords[ 1 ] );
	CsPoint b2( coords[ 2 ], coords[ 3 ] );

	CsPoint res;
	if( !GetIntersectPoint( a1, a2, b1, b2, res ) )
	{
		delete pColInfo;
		niDest.x = target.x;
		niDest.y = target.y;
		return false;
	}

	dx = res.x - a1.x;
	dy = res.y - a1.y;
	if( abs(dx) > 100 || abs(dy) > 100 )
	{
		niDest.x = res.x;
		niDest.y = res.y;
		return false;
	}

	target.x = res.x + (res.x >= 0)?100 : -100;
	target.y = res.y + (res.y >= 0)?100 : -100;

	cPosition current;
	current.x = niCurrPos.x;
	current.y = niCurrPos.y;

	int trys = 0;
	while( pColInfo && trys < 2 )
	{
		tSigned32 coords[4];
		pColInfo->getCollidingLine(coords);

		delete pColInfo;

		SlideAgainst(coords, current.x, current.y, dx, dy);
		target.x = current.x + dx;
		target.y = current.y + dy;

		//pColInfo = m_iMesh->firstCollision( pPath->m_iApplyShape, m_iContext, startpos, target.x, target.y,target.cell );	
		pColInfo = pPath->m_iAgent->firstCollisionTo( m_iContext, target.x,target.y,target.cell );
		trys++;
	}

	if( pColInfo )
		delete pColInfo;

	niDest.x = target.x;
	niDest.y = target.y;
	return true;
}
