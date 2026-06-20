
#include "Channel_Define.h"

class GS2C_RECV_CURRENT_CHANNEL
{
public:
	GS2C_RECV_CURRENT_CHANNEL():channel_idx(0),m_nResult(0)
	{
	}

	int		m_nResult;

#ifdef COMPAT_487
	n2	channel_idx;
#else
	uint	channel_idx;
#endif
				// Channel number being accessed
};

class GS2C_RECV_CHANNEL_LISTINFO
{
public:
	GS2C_RECV_CHANNEL_LISTINFO():m_nResult(0)
	{
		memset( channel, 0xFF, sizeof(channel) );
	}

	int		m_nResult;
	char	channel[nLimit::Channel];	// Current channel status, -1: unused, 0: smooth, 1: many, 2: too many, 3~10: congested
};