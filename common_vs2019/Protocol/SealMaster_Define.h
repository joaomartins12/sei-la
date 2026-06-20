#pragma once
namespace RecvCardInfoAll
{
	struct SealInfo
	{
		u2 m_nCardCode;		// 카드 테이블 Excel의 index(몇번째 줄)
		n4 m_nSealID;		// SealID = itemID
		u2 m_nCardCount;	// 카드 개수
	};
	struct FavoriteInfo
	{
		u2 m_nCardCode;		// 카드 테이블 Excel의 index(몇번째 줄)
		u1 m_nBookmark;		// 즐찾 여부(true, false)
	};


};