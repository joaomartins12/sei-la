
#include "stdafx.h"
#include "SpriteAni.h"

cSpriteAni::cSpriteAni():
m_nSpriteIndex(0),
m_fCurAniTime(0.0f),
m_fSetAniTime(1.0f),
m_nSpriteMaxIndex(0),
m_nLoopCount(0)
{
}

cSpriteAni::~cSpriteAni()
{
	Delete();
}

void cSpriteAni::Delete()
{
	int nSize = m_vpSprite.Size();
	for( int i=0; i<nSize; ++i )
	{
		NiDelete m_vpSprite[ i ];
	}
	m_vpSprite.Destroy();
}

void cSpriteAni::Init( ePLAY_TYPE eType, cWindow* pParent, CsPoint pos, CsPoint size, char* cImagePath, char* cExt, int nAniCount, bool bApplyWindowSize,CsPoint textoken, eSPRITE_PLAY eSprite, int nEnterCount )
{
	cWindow::Init( pParent, pos, size, bApplyWindowSize );

	m_ePlayType = eType; 

	m_nSpriteMaxIndex = nAniCount;
	
	if( textoken != CsPoint::ZERO)
		SetTexToken(textoken);

	assert_cs( nAniCount < 100 );

	switch(eSprite)
	{
	case SPRITE_PAGE:
		{
			char sz[ MAX_PATH ];
			for( int i=1; i<=m_nSpriteMaxIndex; ++i )
			{
				sprintf_s( sz, MAX_PATH, "%s%.2d.%s", cImagePath, i, cExt );
				AddNode( sz );
			}
		}
		break;
	case SPRITE_POS:
		{
			int nCntX = 0;
			int nCntY = 0;
			for( int i = 0 ; i < m_nSpriteMaxIndex ; ++i )
			{
				if(nEnterCount != 0)
				{
					if( (i != 0) && (i >= nEnterCount) )
						nCntY = i / nEnterCount;

					nCntX = i - (nCntY * nEnterCount);
					AddNode(cImagePath, nCntX, nCntY);
				}
				else
					AddNode(cImagePath, i, 0);
			}
		}
		break;
	}

	m_bPlay = true;
}

void cSpriteAni::SetLoopCount(int nLoopCount)
{
	m_nLoopCount = nLoopCount;
}

cSprite* cSpriteAni::AddNode( char const* cImagePath )
{
	cSprite* pSprite = NiNew cSprite;
	pSprite->Init( m_pParentWindow, CsPoint::ZERO, GetSize(), cImagePath, m_bApplyWindowSize );
	m_vpSprite.PushBack( pSprite );
	return pSprite;
}

cSprite* cSpriteAni::AddNode( char const* cImagePath, int nCntX, int nCntY )
{
	cSprite* pSprite = NiNew cSprite;
	CsPoint ptSize = GetSize();
	pSprite->Init( m_pParentWindow, CsPoint::ZERO, ptSize, cImagePath, m_bApplyWindowSize );

	if(pSprite->GetScreen())
	{
		float fLeft = 0.f;
		float fTop = 0.f;
		float fRight = 1.f;
		float fBottom = 1.f;

		if(m_ptTexToken.x != 0) //해당하는 텍스쳐 크기의 X값이 0이면 --> 세로로된 스프라이트
		{
			//pSprite->GetInvTextXXX가 이미지 1/사이즈 ex 135 --> 1 / 135 
			fLeft = (m_ptTexToken.x * pSprite->GetInvTextWidth()) * nCntX;
			fRight = (m_ptTexToken.x * pSprite->GetInvTextWidth()) * (nCntX + 1);
		}

		if(m_ptTexToken.y != 0)
		{
			fTop = (m_ptTexToken.y * pSprite->GetInvTextHeight()) * nCntY;
			fBottom = (m_ptTexToken.y * pSprite->GetInvTextHeight()) * (nCntY + 1);
		} 

		pSprite->GetScreen()->SetTextures(0, 0, 
			fLeft, fTop, fRight, fBottom);	
	}

	m_vpSprite.PushBack( pSprite );
	return pSprite;
}

void cSpriteAni::Update( float const& fDeltaTime )
{
	if( m_bPlay == false )
		return;

	assert_cs( m_vpSprite.Size() > 0 );

	m_fCurAniTime -= fDeltaTime;
	while( m_fCurAniTime < 0.0f )
	{
		m_fCurAniTime += m_fSetAniTime;

		if( ++m_nSpriteIndex >= m_vpSprite.Size() )
		{
			m_nSpriteIndex = 0;

			switch( m_ePlayType )
			{
			case LOOP:									
				break;
			case ONCE:		 
				{
					m_bPlay = false; 
					m_bVisible= false;
					OnEvent( SPRITEANI_END );
				}
				break;
			case ONCE_LEAVE: 
				{
					m_bPlay = false; 
					m_nSpriteIndex = m_vpSprite.Size()-1; 
					OnEvent( SPRITEANI_END );
				}
				break;
			case LOOP_CNT:
				{
					m_nLoopCount--;
					if( 0 >= m_nLoopCount )
					{
						m_nLoopCount = 0;
						m_bPlay = false;
						m_bVisible = false;
						OnEvent( SPRITEANI_END );
					}
				}
				break;
			default:		
				assert_cs( false );
			}			
		}
	}
}

void cSpriteAni::Render()
{
	if( !m_bVisible )
		return;

	m_vpSprite[ m_nSpriteIndex ]->Render( GetDefaultPos() );
}

void cSpriteAni::Render( CsPoint pos )
{
	if( !m_bVisible )
		return;

	m_vpSprite[ m_nSpriteIndex ]->Render( pos );
}

void cSpriteAni::SetAlpha( float fAlpha )
{
	int nSize = m_vpSprite.Size();
	for( int i=0; i<nSize; ++i )
	{
		m_vpSprite[ i ]->SetAlpha( fAlpha );
	}
}

void cSpriteAni::ResetDevice()
{
	int nSize = m_vpSprite.Size();
	for( int i=0; i<nSize; ++i )
	{
		m_vpSprite[ i ]->ResetDevice();
	}
}

void cSpriteAni::PlayOn( bool bResetAni )
{
	m_bPlay = true;
	m_bVisible = true;
	if( bResetAni == true )
	{
		m_nSpriteIndex = 0;
		m_fCurAniTime = m_fSetAniTime;
	}
}

void cSpriteAni::SetTexToken( CsPoint tex )
{
	m_ptTexToken = tex;
}

void cSpriteAni::ChangeTexture(char const* pFileName, char* pExt)
{
	int nSize = m_vpSprite.Size();
	for( int i = 0; i < nSize; ++i )
	{
		char sz[ MAX_PATH ];
		sprintf_s( sz, MAX_PATH, "%s%.2d.%s", pFileName, i + 1, pExt );
		m_vpSprite[ i ]->ChangeTexture( sz );
	}
}

void cSpriteAni::SetPlayType( ePLAY_TYPE eType )
{
	m_ePlayType = eType;
}