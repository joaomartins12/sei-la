#pragma once

namespace nsDigimonTranscendenceResult
{
	const int Success						      = 0;	  // 성공
	const int Fail                                = 1;      // 기타적인 실패
	const int Not_EvoIdx					      = 20102;  // 초월이 불가능한 디지몬
	const int Not_TermsLevel				      = 20103;  // 초월하려는 디지몬의 레벨이 부족
	const int Not_TermsScale				      = 20104;  // 초월하려는 디지몬의 크기가 부족
	const int Not_TermsDigiclone			      = 20105;  // 초월하려는 디지몬의 강화 상태가 부족
	const int Not_UseAvailableDigimonMaterial     = 20106;  // 초월하려는 디지몬에게 사용가능한 재료 디지몬이 없습니다.
	const int Not_UseDigimonMaterial              = 20107;  // 초월하려는 디지몬에 사용할 수 없는 재료 디지몬입니다.
	const int ItemCountShortage                   = 20108;  // 초월 경험치를 충전하기 위한 변환칩이 부족합니다.
	const int Not_Exp						      = 20109;  // 경험치 부족
	const int Not_TransCost					      = 20110;  // 조건에 일치하는 초월 비용 없음
	const int Already_Transcendence			      = 20111;  // 이미 초월 상태
	const int Full_Exp						      = 20112;  // 풀 경험치
	const int Not_DigimonMaterialParcelOut        = 20113;  // 분양 디지몬은 초월 재료로 사용할 수 없습니다.
	const int Not_DigimonTranscendenceParcelOut   = 20114;  // 분양 디지몬은 초월을 할 수 없습니다.
	const int Not_UseDigimonTranscendenceMaterial = 20116;  // 초월 디지몬을 재료로 사용할 수 없습니다.
	const int Not_DeletePartnerDigimon            = 20018;  // 파트너 디지몬 삭제 불가능
}