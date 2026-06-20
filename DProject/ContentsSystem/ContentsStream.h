#pragma once

class ContentsStream
{
public:
	ContentsStream(void);
	~ContentsStream(void);

	template< typename T >
	ContentsStream& operator<<(T const& rhs)
	{
		dm_size_t WriteSize = DmCS::Measure::exec( rhs );
		if( Remain() < WriteSize )
			ExtendSize( m_Wp + WriteSize );

		DmCS::Write::exec( rhs, m_cnStreamvec, m_Wp );
		return *this;
	}

	template< int T_SIZE >
	ContentsStream& operator<<(wchar_t (&wczData)[T_SIZE])
	{
		dm_size_t WriteSize = DmCS::Measure::exec( wczData );
		if( Remain() < WriteSize )
			ExtendSize( m_Wp + WriteSize );

		DmCS::Write::exec( wczData, m_cnStreamvec, m_Wp );
		return *this;
	}

	template<>
	ContentsStream& operator<<(ContentsStream const& rhs)
	{
		operator<<( rhs.m_cnStreamvec );
		return *this;
	}

	template< typename T >
	ContentsStream const& operator>>(T& rhs) const
	{
		if( IsEOS() )
			return *this;

		DmCS::Read::exec( rhs, m_cnStreamvec, m_Rp );
		return *this;
	}

	template< int T_SIZE >
	ContentsStream& operator>>(wchar_t (&wczData)[T_SIZE])
	{
		if( IsEOS() )
			return *this;

		DmCS::Read::exec( wczData, m_cnStreamvec, m_Rp );
		return *this;
	}

	template<>
	ContentsStream const& operator>>(ContentsStream& rhs) const
	{
		if( IsEOS() )
			return *this;

		this->operator>>( rhs.m_cnStreamvec );
		rhs.m_Rp = 0;
		rhs.m_Wp = rhs.m_cnStreamvec.size();

		return *this;
	}

	dm_size_t Remain(void) const;		//	남의 공간		( Write Mode )
	bool IsEOS(void) const;				//	스트림의 끝인가	( Read Mode )
	dm_size_t Size(void) const;			//	스트림의 사이즈
	void ExtendSize(dm_size_t NewSize);	//	공간 확장

	void MoveWp(dm_size_t Pos);
	void SetWp(dm_size_t Pos);

	void MoveRp(dm_size_t Pos) const;
	void SetRp(dm_size_t Pos) const;

	void operator=(ContentsStream const& rhs);

private:
	mutable	dm_size_t	m_Rp;
			dm_size_t	m_Wp;

			dm_bytevec	m_cnStreamvec;
};