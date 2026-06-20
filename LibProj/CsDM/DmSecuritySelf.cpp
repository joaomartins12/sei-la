#include "stdafx.h"

#include "DmSecuritySelf.h"

#include <time.h>

#pragma region "define Util Macro"
#define DmFS_LO(X) ( X & 0x0F )
#define DmFS_HI(X) ( ( X & 0xF0 ) >> 4 )
#pragma endregion

#pragma optimize( "", off )

namespace DmFS
{
	#pragma region "Security Self Private Algorithm Function"

	typedef dm_dword_t					security_block;
	typedef std::vector< security_block >	security_keyvec;

	security_block const	max_block_langth	= USHRT_MAX;
	dm_dword_t const		max_keyvec_size		= 16;
	dm_byte_t const			xorKey				= 127;
	
	namespace Private
	{
		void ResizeByteVec(dm_bytevec& vec, dm_bytevec::size_type conref ndm_size)
		{
			dm_bytevec tmpvec( ndm_size );
			vec.swap( tmpvec );
		}

		void MakeSecurityKeyVector(security_keyvec& keyvec)
		{//	Xor ¹× Twist ¿¬»ê ¿ë
			::srand( static_cast< unsigned int >( ::_time32( NULL ) ) );

			struct Rand
			{
				union RandBlock
				{
					struct { dm_word_t b0, b1; };
					dm_dword_t v;
				};

				static dm_dword_t Generate(void)
				{
					RandBlock block;
					do
					{
						block.b0 = rand() & max_block_langth;
						block.b1 = rand() & max_block_langth;
					}
					while( 0 == block.v );
					return block.v;
				}

				static dm_dword_t Generate5n(void)
				{
					dm_dword_t tmp = Rand::Generate();
					for( int i = 1; i < 5; ++i )
						tmp ^= Rand::Generate();

					return tmp;
				}
			};

			keyvec.resize( max_keyvec_size );
			for( int i = 0; i < max_keyvec_size; ++i )
			{
				if( i < 4 )
					keyvec[ i ] = Rand::Generate5n();
				else
					keyvec[ i ] = keyvec[ i - 1 ] ^ keyvec[ i - 4 ];
			}

		}

		void TypeXor(security_keyvec conref keyvec, dm_bytevec conref i_vec, dm_bytevec& o_vec)
		{
			/*
				µ¥ÀÌÅ¸¸¦ security_blockÀÇ »çÀÌÁî¸¸Å­ Àß¶ó¼­ ±× °¹¼ö ¸¸Å­ µ¹¸é¼­
				ÀÎµ¦½º ¼øÀ¸·Î Å°¿Í µ¥ÀÌÅ¸¸¦ Xor¿¬»êÇÑ´Ù.
				> ³¡¿¡ ´Ù´Ù¸£±â Àü¿¡ Å°°¡ ³¡¿¡ ´Ù´Ù¸£¸é Å°ÀÇ ÀÎµ¦½º¸¦ Ã³À½À¸·Î µ¹¸°´Ù.
			*/

			dm_bytevec tmpvec( i_vec.size() );

			size_t dwBlockCount	= tmpvec.size() / sizeof( security_block );
			size_t dwRemain		= tmpvec.size() % sizeof( security_block );

			security_block*		pDest	= reinterpret_cast< security_block* >( &tmpvec.at( 0 ) );
			security_block cptr	pSour	= reinterpret_cast< security_block cptr >( &i_vec.at( 0 ) );

			for( size_t i = 0; i < dwBlockCount; ++i )
			{
				pDest[ i ]  = (pSour[ i ] ^ keyvec[ i % max_keyvec_size ]) ^ xorKey;
			}

			if( dwRemain )
			{
				size_t dwLackCnt = sizeof( security_block ) - dwRemain;

				dm_byte_t atmp[ sizeof( security_block ) ] = { 0, };

				::memcpy( &atmp[ dwLackCnt ], &i_vec.at( i_vec.size() - dwRemain ), dwRemain );

				pDest = reinterpret_cast< security_block* >( atmp );
				*pDest ^= keyvec[ dwBlockCount % max_keyvec_size ];
				
				::memcpy( &tmpvec.at( tmpvec.size() - dwRemain ), &atmp[ dwLackCnt ], dwRemain );
			}

			o_vec.swap( tmpvec );
		}

		void TypeTwist(dm_bytevec conref i_vec, dm_bytevec& o_vec, bool conref bEnc)
		{
			/*
				µ¥ÀÌÅ¸¸¦ security_blockÀÇ »çÀÌÁî¸¸Å­ Àß¶ó¼­ ±× °¹¼ö ¸¸Å­ µ¹¸é¼­
				ÀÎµ¦½º ¼øÀ¸·Î Å°¿Í µ¥ÀÌÅ¸¸¦ Xor¿¬»êÇÑ´Ù.
				> ³¡¿¡ ´Ù´Ù¸£¸é Å°ÀÇ ÀÎµ¦½º ÃÊ±âÈ­.
			*/

			dm_bytevec tmpvec( i_vec.size() );

			size_t dwBlockCount	= i_vec.size() / sizeof( security_block );
			size_t dwRemain		= i_vec.size() % sizeof( security_block );

			security_block*		pDest	= reinterpret_cast< security_block* >( &tmpvec.at( 0 ) );
			security_block cptr	pSour	= reinterpret_cast< security_block cptr >( &i_vec.at( 0 ) );

			size_t dwMaxBit = sizeof( security_block ) * 8;

			for( size_t i = 0, Shift = 1; i < dwBlockCount; ++i, ++Shift )
			{
				if( Shift >= dwMaxBit )
					Shift = 1;

				if( bEnc )
				{
					security_block Mask = pSour[ i ] & ( 0xFFFFFFFF << ( dwMaxBit - Shift ) );
					pDest[ i ] = pSour[ i ] << Shift;
					pDest[ i ] |= ( Mask >> ( dwMaxBit - Shift ) );
				}
				else
				{
					security_block Mask = pSour[ i ] & ( 0xFFFFFFFF >> ( dwMaxBit - Shift ) );
					pDest[ i ] = pSour[ i ] >> Shift;
					pDest[ i ] |= ( Mask << ( dwMaxBit - Shift ) );
				}
			}

			if( dwRemain )
			{// Âî²¨±â´Â ±ÍÂúÀ¸´Ï±î ÇÏÁö¸»ÀÚ.
				::memcpy( &tmpvec.at( tmpvec.size() - dwRemain ), &i_vec.at( i_vec.size() - dwRemain ), dwRemain );
			}

			o_vec.swap( tmpvec );
		}

		void TypeRvsFull(dm_bytevec conref i_vec, dm_bytevec& o_vec)
		{
			/*
				µ¥ÀÌÅ¸¸¦ security_blockÀÇ »çÀÌÁî¸¸Å­ Àß¶ó¼­ ±× °¹¼ö ¸¸Å­ µ¹¸é¼­
				µ¥ÀÌÅ¸¸¦ ¹Ý¶¥ÇØ¼­ À§Ä¡¸¦ ±³È¯ÇÑ´Ù.
			*/

			dm_bytevec tmpvec( i_vec.size() );

			size_t dwBlockCount	= i_vec.size() / sizeof( security_block );
			size_t dwRemain		= i_vec.size() % sizeof( security_block );
			bool bOdd					= ( ( dwBlockCount % 2 ) == 0 )?( false ):( true );

			security_block*		pDest	= reinterpret_cast< security_block* >( &tmpvec.at( 0 ) );
			security_block cptr	pSour	= reinterpret_cast< security_block cptr >( &i_vec.at( 0 ) );

			for( size_t i = 0; i < ( dwBlockCount / 2 ); ++i )
			{
				pDest[ i ] = ~pSour[ ( dwBlockCount - 1 ) - i ];
				pDest[ ( dwBlockCount - 1 ) - i ] = ~pSour[ i ];
			}

			if( bOdd )
			{
				pDest[ ( dwBlockCount / 2 ) ] = ~pSour[ ( dwBlockCount / 2 ) ];
			}

			if( dwRemain )
			{// Âî²¨±â´Â ±ÍÂúÀ¸´Ï±î ÇÏÁö¸»ÀÚ.
				::memcpy( &tmpvec.at( tmpvec.size() - dwRemain ), &i_vec.at( i_vec.size() - dwRemain ), dwRemain );
			}

			o_vec.swap( tmpvec );
		}

		void TypeRvsHalf(dm_bytevec conref i_vec, dm_bytevec& o_vec)
		{
			/*
				µ¥ÀÌÅ¸¸¦ security_blockÀÇ »çÀÌÁî¸¸Å­ Àß¶ó¼­ ±× °¹¼ö ¸¸Å­ µ¹¸é¼­
				security_blockÀÇ µ¥ÀÌÅ¸¸¦ ¹Ý¶¥ÇØ¼­ À§Ä¡¸¦ ±³È¯ÇÑ´Ù.
			*/

			struct SRvsHalf
			{
				static void exec(security_block& dest, security_block conref sour, security_block conref Mask)
				{
					security_block Masked = sour & Mask;
					dest = ( sour & ~Mask ) | ( ~Masked & Mask );
				}
			};

			dm_bytevec tmpvec( i_vec.size() );

			size_t dwBlockCount	= i_vec.size() / sizeof( security_block );
			size_t dwRemain		= i_vec.size() % sizeof( security_block );

			security_block*		pDest	= reinterpret_cast< security_block* >( &tmpvec.at( 0 ) );
			security_block cptr	pSour	= reinterpret_cast< security_block cptr >( &i_vec.at( 0 ) );

			for( size_t i = 0; i < dwBlockCount; ++i )
			{
				security_block Mask = 0x0000FFFF;
				if( 0 == ( i % 2 ) )
					Mask = 0xFFFF0000;

				SRvsHalf::exec( pDest[ i ], pSour[ i ], Mask );
			}

			if( dwRemain )
			{// Âî²¨±â´Â ±ÍÂúÀ¸´Ï±î ÇÏÁö¸»ÀÚ.
				::memcpy( &tmpvec.at( tmpvec.size() - dwRemain ), &i_vec.at( i_vec.size() - dwRemain ), dwRemain );
			}

			o_vec.swap( tmpvec );
		}

		void TypeShuffle(security_keyvec conref keyvec, dm_bytevec conref i_vec, dm_bytevec& o_vec, bool conref bEnc)
		{
			/*
				Å°ÀÇ »çÀÌÁî Å©±âÀÇ ºí·°À¸·Î µ¥ÀÌÅ¸¸¦ ÂÉ°µ ÈÄ
				Å° ÀÎµ¦½ºÀÇ ¼ø¼­´ë·Î Å°ÀÇ »óÀ§ 4ºñÆ®¿Í ÇÏÀ§ 4ºñÆ® °ªÀÇ ÀÎµ¦½º µ¥ÀÌÅ¸ÀÇ À§Ä¡¸¦ ¹Ù²Û´Ù.
			*/

			struct SShuffle
			{
				dm_byte_t tinyBlock[ max_keyvec_size ];
			};

			dm_bytevec tmpvec = i_vec;

			size_t dwCount		= tmpvec.size() / sizeof( SShuffle );
			size_t dwKeyCount	= keyvec.size() * sizeof( security_block );
			dm_byte_t cptr	pKey	= reinterpret_cast< dm_byte_t cptr >( &keyvec.at( 0 ) );

			for( size_t i = 0; i < dwCount; ++i )
			{
				SShuffle* pShuffle = reinterpret_cast< SShuffle* >( &tmpvec.at( i * sizeof( SShuffle ) ) );

				for( size_t j = 0; j < dwKeyCount; ++j )
				{
					if( false == bEnc )
					{
						std::swap( 
							pShuffle->tinyBlock[ DmFS_LO( pKey[ j ] ) ], 
							pShuffle->tinyBlock[ DmFS_HI( pKey[ j ] ) ] 
						);
					}
					else
					{
						std::swap( 
							pShuffle->tinyBlock[ DmFS_HI( pKey[ ( dwKeyCount - 1 ) - j ] ) ], 
							pShuffle->tinyBlock[ DmFS_LO( pKey[ ( dwKeyCount - 1 ) - j ] ) ] 
						);
					}
				}
			}

			o_vec.swap( tmpvec );
		}
	}
	#pragma endregion

	bool SecuritySelf::Encript(dm_byte_t conref flag, dm_bytevec conref i_vec, dm_bytevec& o_vec)
	{//	¾ÏÈ£È­
		dm_bytevec tmpvec = i_vec;

		Enc::Field enc( flag );
		
		if (DmFS_IS(enc.rvsfull))
			Private::TypeRvsFull(tmpvec, tmpvec);

		if (DmFS_IS(enc.twist))
			Private::TypeTwist(tmpvec, tmpvec, true);

		if( DmFS_IS( enc.xor ) )
		{
			security_keyvec keyvec;

			Private::MakeSecurityKeyVector( keyvec );
			Private::TypeXor( keyvec, tmpvec, o_vec );

			dm_size_t datasize = DmCS::Measure::exec( keyvec ) + DmCS::Measure::exec( o_vec );
			Private::ResizeByteVec( tmpvec, datasize );

			dm_size_t write_size = 0;
			if( !DmCS::Write::exec( keyvec, tmpvec, write_size ) )
				return false;

			if( !DmCS::Write::exec( o_vec, tmpvec, write_size ) )
				return false;
		}

		if (DmFS_IS(enc.rvshalf))
			Private::TypeRvsHalf(tmpvec, tmpvec);		
		
		if( DmFS_IS( enc.shuffle ) )
		{
			security_keyvec keyvec;

			Private::MakeSecurityKeyVector( keyvec );
			Private::TypeShuffle( keyvec, tmpvec, o_vec, true );

			dm_size_t datasize = DmCS::Measure::exec( keyvec ) + DmCS::Measure::exec( o_vec );
			Private::ResizeByteVec( tmpvec, datasize );

			dm_size_t write_size = 0;
			if( !DmCS::Write::exec( keyvec, tmpvec, write_size ) )
				return false;

			if( !DmCS::Write::exec( o_vec, tmpvec, write_size ) )
				return false;
		}

		o_vec.swap( tmpvec );
		return true;
	}

	bool SecuritySelf::Decript(dm_byte_t conref flag, dm_bytevec conref i_vec, dm_bytevec& o_vec)
	{//	º¹È£È­´Â ¾ÏÈ£È­ÀÇ ¿ª¼ø
		dm_bytevec tmpvec = i_vec;

		Enc::Field enc( flag );

		if( DmFS_IS( enc.shuffle ) )
		{
			security_keyvec keyvec;
			dm_bytevec datavec;
			dm_size_t read_size = 0;

			if( !DmCS::Read::exec( keyvec, tmpvec, read_size ) )
				return false;

			if( !DmCS::Read::exec( datavec, tmpvec, read_size ) )
				return false;

			if( keyvec.empty() )
				return false;

			Private::TypeShuffle( keyvec, datavec, tmpvec, false );
		}

		if( DmFS_IS( enc.rvshalf ) )
			Private::TypeRvsHalf( tmpvec, tmpvec );
		
		if( DmFS_IS( enc.xor ) )
		{
			security_keyvec keyvec;
			dm_bytevec datavec;
			dm_size_t read_size = 0;

			if( !DmCS::Read::exec( keyvec, tmpvec, read_size ) )
				return false;

			if( !DmCS::Read::exec( datavec, tmpvec, read_size ) )
				return false;

			if( keyvec.empty() )
				return false;

			Private::TypeXor( keyvec, datavec, tmpvec );
		}

		if (DmFS_IS(enc.twist))
			Private::TypeTwist(tmpvec, tmpvec, false);

		if (DmFS_IS(enc.rvsfull))
			Private::TypeRvsFull(tmpvec, tmpvec);
		
		o_vec.swap( tmpvec );
		return true;
	}

	bool SecuritySelf::Decript_V18(dm_byte_t conref flag, dm_bytevec conref i_vec, dm_bytevec& o_vec)
	{//	복호화는 암호화의 역순
		dm_bytevec tmpvec = i_vec;

		Enc::Field enc(flag);

		if (DmFS_IS(enc.twist))
			Private::TypeTwist(tmpvec, tmpvec, false);

		if (DmFS_IS(enc.xor))
		{
			security_keyvec keyvec;
			dm_bytevec datavec;
			dm_size_t read_size = 0;

			if (!DmCS::Read::exec(keyvec, tmpvec, read_size))
				return false;

			if (!DmCS::Read::exec(datavec, tmpvec, read_size))
				return false;

			if (keyvec.empty())
				return false;

			Private::TypeXor(keyvec, datavec, tmpvec);
		}

		if (DmFS_IS(enc.rvshalf))
			Private::TypeRvsHalf(tmpvec, tmpvec);

		if (DmFS_IS(enc.shuffle))
		{
			security_keyvec keyvec;
			dm_bytevec datavec;
			dm_size_t read_size = 0;

			if (!DmCS::Read::exec(keyvec, tmpvec, read_size))
				return false;

			if (!DmCS::Read::exec(datavec, tmpvec, read_size))
				return false;

			if (keyvec.empty())
				return false;

			Private::TypeShuffle(keyvec, datavec, tmpvec, false);
		}

		if (DmFS_IS(enc.rvsfull))
			Private::TypeRvsFull(tmpvec, tmpvec);

		o_vec.swap(tmpvec);
		return true;
	}

}

#pragma optimize( "", on )
