
#pragma once 

class CsMapObject
{
public:
	CsMapObject();
	~CsMapObject();

	struct sFACTOR_INFO
	{
		sFACTOR_INFO():s_nOpenType(0),s_nFactor(0),s_nFactorCnt(0)
		{}
		
		int				s_nOpenType;		// 0 : 사용안함, 1 : 몬스터 Kill, 2 : 퀘스트 완료
		unsigned int	s_nFactor;			// 타입에 따른 Factor( 타입1 : 몬스터코드, 타입2 : 퀘스트 코드 )
		int				s_nFactorCnt;		// ( 타입1 : 몬스터 수 )
	};

	typedef	std::list< sFACTOR_INFO >					FACT_LIST;
	typedef	std::list< sFACTOR_INFO >::iterator			FACT_LIST_IT;
	typedef	std::list< sFACTOR_INFO >::const_iterator	FACT_LIST_CIT;

public:
	FACT_LIST const&	GetFactInfoList() const;
	void				AddFactorInfo( sFACTOR_INFO& addInfo );
	int					GetMaxFactorCount() const;
	sFACTOR_INFO const* GetFactorInfo( BYTE const& byCount ) const;
	void				DeleteFacotr( BYTE const& byCount );
	bool				IsEmpty() const;
	void				ChangeFactInfo(sFACTOR_INFO& addInfo , int nCount = 0);
	void				SetOrderNumber( int const& nNum );
	int					GetOrderNumber() const;
	

private:
	FACT_LIST			m_FactInfoList;
	mutable int			m_nOrderNumber;// 오픈 순서
};
