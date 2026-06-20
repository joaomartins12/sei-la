// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#ifndef _WIN32_WINNT		// Windows XP 이상에서만 기능을 사용할 수 있습니다.                   
#define _WIN32_WINNT 0x0501	// 다른 버전의 Windows에 맞도록 적합한 값으로 변경해 주십시오.
#endif						

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.


#include "..\CsFunc\stdafx.h"
#include "..\CsFilePack\stdafx.h"
#include "..\CsFileTable\stdafx.h"
#include "..\CsGamebryo2.3\stdafx.h"

//프로젝트 국가별 설정 chu8820
#include "_TestDefine.h"

#pragma warning( disable : 4267 )
#pragma warning( disable : 4995 )
#pragma warning( disable : 4819 )

#include <NiMain.h>
#include <NiAnimation.h>

#pragma warning( default : 4267 )
#pragma warning( default : 4995 )
#pragma warning( default : 4819 )

#include "..\CsThread\stdafx.h"

#include "00_CharDefine.h"

#include "ModelData.h"
#include "ModelDataMng.h"


#include "11_LoadSequence.h"
#include "10_LoadStruct.h"

#include "00_PropertyHeader.h"
#include "00_CharHeader.h"
#include "00_ThreadHeader.h"

#include "EffectSound.h"
#include "EffectSoundMng.h"

#include "EventCall_Base.h"
#include "EventData.h"
#include "EventSceneData.h"
#include "EventDataMng.h"
#include "EventMng.h"
#include "EventSceneMng.h"
#include "SEffectMgr.h"

