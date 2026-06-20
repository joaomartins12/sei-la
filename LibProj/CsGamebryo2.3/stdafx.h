// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

#include <io.h>

#include "..\CsFunc\stdafx.h"
//#include "..\CsFilePack\stdafx.h"
//#include "..\CsFilePack\CsFilePackMng.h"
#include "..\CsThread\CThreadSafeQueue.h"
#include "..\CsFileTable\stdafx.h"
//#include "..\CsSpeedTree\stdafx.h"

//프로젝트 국가별 설정 chu8820
#include "_TestDefine.h"

#pragma warning( disable : 4267 )
#pragma warning( disable : 4995 )
#pragma warning( disable : 4819 )

#include <NiMain.h>
#include <NiPick.h>
#include <NiDI8InputSystem.h>
#include <NiD3DShaderFactory.h>
#include <NiBinaryShader.h>
#include <NiParticle.h>
#include <NiCollision.h>
#include <NiTransformController.h>
#include <NiQuatTransform.h>
#include <NiD3D10Utility.h>
#include <NiActorManager.h>

#pragma warning( default : 4267 )
#pragma warning( default : 4995 )
#pragma warning( default : 4819 )

#define SAFE_NIDELETE(p)       { if(p) { NiDelete (p);     (p)=NULL; } }
#define SAFE_NIDELETE_ARRAY(p) { if(p) { NiDelete[] (p);   (p)=NULL; } }




#define THREAD_MEMPOOL_H( classname )		\
	public:\
	static	CThreadSafeQueue< ##classname* >		m_MemPool;\
	static void				DeleteInstance( ##classname* pInst ){ assert_cs( pInst ); m_MemPool.Put( pInst ); }\
	static classname*		NewInstance(){\
		##classname* pInst;\
		if( m_MemPool.Get( pInst ) == false ){ pInst = NiNew classname; }\
		return pInst;\
		}

#define THREAD_MEMPOOL_CPP( classname )			CThreadSafeQueue< ##classname* >		##classname::m_MemPool;

#define THREAD_MEMPOOL_DELETE( classname )	{\
		std::vector< ##classname* > vItems;\
		int nCount = ##classname::m_MemPool.Get( vItems );\
		for( int i=0; i<nCount; ++i ){\
			NiDelete vItems[ i ];\
		}\
		vItems.clear();\
}

#include "PathEngine.h"

#include "CsGBNiMemPool.h"
#include "CsGBMemPool.h"

#include "CsNiNode.h"
#include "CsNiGeometry.h"

#include "CsGBCamera.h"

#include "GeometryList.h"
#include "CsGeomBox.h"
#include "Sorting.h"
#include "NodeObj.h"

#include "CsGBVisible.h"

#include "CsGBFunc.h"
#include "CsGBInput.h"
#include "CsGBPick.h"

#include "CsGBTerrainHeader.h"



// Color
#define CsCOLOR2NiColor( dword )	NiColor( CsB2F( CS_GetRValue( dword ) ), CsB2F( CS_GetGValue( dword ) ), CsB2F( CS_GetBValue( dword ) ) )


