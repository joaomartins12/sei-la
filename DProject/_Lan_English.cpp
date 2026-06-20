


#include "stdafx.h"
#include "_Lan_English.h"


TCHAR*	__LanGetString_Eng( TCHAR* str )
{
	size_t nRes = CsFPS::GetHashCode( str );
	switch( nRes )
	{
	case 3574150398:
		/* 마우스를 찾을 수 없습니다. */
		return			L"Unable to detect mouse.";

	case 170863830:
		/* 키보드를 찾을 수 없습니다. */
		return			L"Unable to detect keyboard.";

	case 3102853748:
		/* 지원되지 않는 모니터 색 품질입니다. eFrameFormat = %d */
		return			L"Selected color is not supported. eFrameFormat = %d";

	case 2712967072:
		/* 하드웨어 3D가속을 받지 못합니다. 그래픽 드라이버 or DirectX or D3D가속을 확인해 주십시요. */
		return			L"Hardware 3D Acceleration is not available. Please check your graphics driver or DirectX or D3D Acceleration. ";

	case 2170343467:
		/* DirectX 버젼이 9.0 보다 낮습니다. DirectX 버젼을 올려주세요. */
		return			L"Your DirectX version is lower than 9.0. Please update DirectX.";

	case 2230314086:
		/* 현재의 그래픽 카드로는 사양이 낮아 지형이나 케릭터등이 정상적이지 않은 화면으로 보일 수 있습니다. */
		return			L"Your current graphics card may not display terrain or characters properly because it does not meet the minimum requirements. ";

	case 2443360582:
		/* 그래픽 카드 사양이 낮아 기본 쉐이더를 사용하겠습니다.\n쉐이더 설정은 그래픽 옵션에서 조절 가능합니다. */
		return			L"Due to low graphic card requirements, the basic shader was applied. \nYou can configure the Shader setting in Graphic Option.";

	case 192517641:
		/* 파일테이블이 잘못 되었습니다 */
		return			L"File Table Error";

	case 1242789363:
		/* 디지몬마스터즈가 이미 실행중에 있습니다.\n불법 프로그램 사용시 불이익을 당할 수 있습니다. */
		return			L"Digimon Masters is already running.\nYou could be in trouble if you are using an illegal copy.";

	case 2044459055:
		/*DirectX9.0 버전이 낮습니다. 최신 DirectX를 설치 하시겠습니까?*/
		return L"Your DirectX version is low. Would you install the latest version of DirectX now?";

	case 1973959437:
		/* 런쳐에서 실행하여 주십시오!! */
		return			L"Excute DMLauncher.exe.";

		// 게임가드 관련 문자열
		// 이놈은 UI Text 테이블에 넣으면 안된다.
		// 이유 : 이 문자열을 출력할 때는 테이블 로드 전이기 때문이다.
	case 3947362763:	
		/*게임가드가 실행 중 입니다. 잠시 후나 재부팅 후에 다시 실행해보시기 바랍니다.*/
		return			L"Game Guard has been activating. Please re-launch a little later, or after reboot your computer.";
	case 1378959422:	 
		/*게임이 중복 실행되었거나 게임가드가 이미 실행 중 입니다. 게임 종료후 다시 실행해보시기 바랍니다.*/
		return			L"launched Game is duplicated or Game Guard has been activating. Please re-launch after terminate the game.";
	case 3489414536:	 
		/*게임가드 초기화 에러입니다. 재부팅 후 다시 실행해보거나 충돌할 수있는 다른 프로그램들을 종료한 후 실행해 보시기 바랍니다.*/
		return			L"Game Guard Initialization Error. Please re-launch after reboot your computer or after terminate some program that crash can be occurred.";
	case 1409890449:	
		/*게임가드 파일이 없거나 변조되었습니다. 게임가드 셋업 파일을 설치해보시기 바랍니다.*/
		return			L"There are no Game Guard files or they are flasifed. Please install Game Guard setup file.";
	case 2653703415:	
		/*윈도우의 일부 시스템 파일이 손상되었습니다. 인터넷 익스플로러(IE)를\t다시 설치해보시기 바랍니다.*/		
		return			L"Some system file of Windows is damaged. Please re-install Internet Explorer(IE).";

	case 1237949768:	 
		/*게임가드 실행에 실패했습니다. 게임가드 셋업 파일을 다시 설치해보시기 바랍니다.*/
		return			L"Failed to launch Game Guard. Please re-install Game Guard setup file.";
	case 2318342878:	
		/*불법 프로그램이 발견되었습니다. 불필요한 프로그램을 종료한 후 다시 실행해보시기 바랍니다.*/
		return			L"Illegal program is detected. Please re-launch after terminate unnecessary program.";
	case 1477149656:	
		/*게임가드 업데이트를 취소하셨습니다. 접속이 계속 되지 않을 경우 인터넷 및 개인 방화벽 설정을 조정해 보시기 바랍니다.*/
		return			L"Game Guard update is canceled. If it can't be connected continually, please check your network condition and adjust settings of private firewall.";
	case 2429585766:	
		/*게임가드 업데이트 서버 접속에 실패하였습니다. 잠시 후 다시 접속하거나, 네트웍 상태를 점검해봅니다.*/
		return			L"Failed to connect with update server for Game Guard. Please re-try a little later or check your network condition.";
	case 1426417480:	
		/*게임가드 초기화 에러 또는 구버젼의 게임가드 파일입니다. 게임가드 셋업파일을 다시 설치하고 게임을 실행해봅니다.*/
		return			L"Game Guard Initialization Error or Game Guard version is old. Please re-install Game Guard setup file and re-launch the game.";
	case 1544598421:	
		/*ini 파일이 없거나 변조되었습니다. 게임가드 셋업 파일을 설치하면 해결할 수 있습니다.*/
		return			L"There is no ini file or it is flasifed. Please install Game Guard setup file to resolve the problem.";
	case 274170548:		
		/*npgmup.des 초기화 에러입니다. 게임가드폴더를 삭제후 다시 게임실행을 해봅니다.*/
		return			L"npgmup.des initialization error. Please re-launch the game after delete GameGuard folder.";
	case 890072434:		
		/*게임가드 업데이트 서버 접속에 실패하였습니다. 잠시 후 재시도 해보거나, 개인 방화벽이 있다면 설정을 조정해 보시기 바랍니다.*/
		return			L"Failed to connect with update server for Game Guard. Please re-try a little later or adjust settings of private firewall.";
	case 1913749563:	
		/*게임가드 업데이트를 완료하지 못 했습니다. 바이러스 백신을 일시 중시시킨 후 재시도 해보시거나, PC 관리 프로그램을 사용하시면 설정을 조정해 보시기 바랍니다.*/
		return			L"Game Guard update is not completed. Please re-try it after deactivate Vaccine program temporary.";
	case 3404157133:	
		/*바이러스 및 해킹툴 검사 모듈 로딩에 실패 했습니다. 메모리 부족이거나 바이러스에 의한 감염일 수 있습니다.*/
		return			L"Failed to load Virus and Hacking tool inspection module. Please check your Memory has enough capacity and there is a Virus.";
	case 2877035747:	
		/*게임가드와 충돌 프로그램이 발견되었습니다.*/
		return			L"A problem which is crashed with Game Guard is detected.";
	case 395939174:		
		/*게임가드 실행 중 에러가 발생하였습니다. 게임 폴더 안의 GameGuard폴더에 있는 *.erl 파일들을 DigimonM@inca.co.kr로 첨부하여 메일 보내주시기 바랍니다.*/
		return			L"Error is occurred while activate Game Guard. Please send a mail included files those have been tilted with *.erl in GameGuard folder of Game folder ";
	case 2935271129:	
		/*게임가드 초기화 에러 : %lu*/
		return			L"Game Guard Initialization Error : %lu";
	case 1944341339:	
		/*스피드핵이 감지되었습니다.*/
		return			L"Speed hack is detected.";
	case 1214876551:	
		/*게임핵이 발견되었습니다.*/
		return			L"Game hack is detected.";
	case 295769150:	
		/*게임이나 게임가드가 변조되었습니다.*/
		return			L"Game or Gamer Guard is falsified.";
	case 2182461964:	
		/*게임가드가 실행 중이 아닙니다.*/
		return			L"Game Guard is not running.";

		////////////게임가드 관련 문자열 여까지//////////////////////////////////////////////////////////////
		////////////XIGNCODE3 관련 문자열////////////////////////////////////////////////////////////// 
	case 2717075904:
		/*비정상적인 게임 수행으로 인해 게임을 종료합니다.\n\t\tCODE = %08X*/
		return L"Terminating game as abnormal game activity has been detected.\n\t\tCODE = %08X";
		////////////XIGNCODE3 관련 문자열//////////////////////////////////////////////////////////////
	}
// 	assert_cs( false );
//#ifndef _GIVE
//	CsMessageBoxA( MB_OK, "%s\nHashCode : %lu\n\n", str, nRes );  // disabled: pack files are English already
//#endif

	return str;  // fallback: pass the original string through so callers don't see "Translation needed"
}
