

#include "stdafx.h"
#include "_Lan_HongKong.h"


TCHAR*	__LanGetString_HongKong( TCHAR* str )
{
	size_t nRes = CsFPS::GetHashCode( str );
	switch( nRes )
	{
	case 3574150398:
		/* 마우스를 찾을 수 없습니다. */
		return			L"搜尋不到滑鼠。";

	case 170863830:
		/* 키보드를 찾을 수 없습니다. */
		return			L"搜尋不到鍵盤。";

	case 3102853748:
		/* 지원되지 않는 모니터 색 품질입니다. eFrameFormat = %d */
		return			L"不支援的螢幕色彩品質。eFrameFormat = %d";

	case 2712967072:
		/* 하드웨어 3D가속을 받지 못합니다. 그래픽 드라이버 or DirectX or D3D가속을 확인해 주십시요. */
		return			L"無法取得硬體3D加速。請確認顯卡驅動或DirectX、D3D加速。";

	case 2170343467:
		/* DirectX 버젼이 9.0 보다 낮습니다. DirectX 버젼을 올려주세요. */
		return			L"DirectX 版本低於 9.0。請更新DirectX 版本。";

	case 2230314086:
		/* 현재의 그래픽 카드로는 사양이 낮아 지형이나 케릭터등이 정상적이지 않은 화면으로 보일 수 있습니다. */
		return			L"目前顯卡規格不足，地形或角色等的顯示畫面可能會出現異常。";

	case 2443360582:
		/* 그래픽 카드 사양이 낮아 기본 쉐이더를 사용하겠습니다.\n쉐이더 설정은 그래픽 옵션에서 조절 가능합니다. */
		return			L"因顯卡規格低，將使用基本著色器。\n著色器的設定可於貼圖選項中做調整。";

	case 192517641:
		/* 파일테이블이 잘못 되었습니다 */
		return			L"檔案表格錯誤";

	case 1242789363:
		/* 디지몬마스터즈가 이미 실행중에 있습니다.\n불법 프로그램 사용시 불이익을 당할 수 있습니다. */
		return			L"遊戲執行中。\n使用非法程式可能會受到損害。";

	case 2044459055:
		/*DirectX9.0 버전이 낮습니다. 최신 DirectX를 설치 하시겠습니까?*/
		return			L"DirectX9.0版本過舊，是否安裝最新版DirectX?";

	case 1973959437:
		/* 런쳐에서 실행하여 주십시오!! */
		return			L"請在Launcher中執行！";

		// 게임가드 관련 문자열
		// 이놈은 UI Text 테이블에 넣으면 안된다.
		// 이유 : 이 문자열을 출력할 때는 테이블 로드 전이기 때문이다.
		
		////////////XIGNCODE3 관련 문자열////////////////////////////////////////////////////////////// 
	case 1214876551:	
		/*게임핵이 발견되었습니다.*/
		return			L"發現第三方工具";
	case 2717075904:
		/*비정상적인 게임 수행으로 인해 게임을 종료합니다.\n\t\tCODE = %08X*/
		return L"因不正常的執行而終止遊戲。\n\t\tCODE = %08X";
		////////////XIGNCODE3 관련 문자열//////////////////////////////////////////////////////////////
	}
	//	assert_cs( false );
#ifndef _GIVE

	TCHAR szBuf[256];
	_stprintf_s(szBuf, _T("NotFindStr : %s / Hash : %lu"), str, nRes);
	CsMessageBox(MB_OK, szBuf);

#endif

	return L"NoString";
}
