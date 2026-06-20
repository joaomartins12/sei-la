
#pragma once


// 유저 계정 번호 타입선언
#ifdef VERSION_HK
typedef unsigned __int32	AccountIDX_t;
#elif defined VERSION_TW
typedef unsigned __int32	AccountIDX_t;
#elif defined VERSION_USA

#ifdef VERSION_AERIA
typedef unsigned __int32	AccountIDX_t;
#else
typedef unsigned __int32	AccountIDX_t;
#endif

#elif defined VERSION_TH
typedef unsigned __int32	AccountIDX_t;
#elif defined VERSION_KOR
typedef unsigned __int32	AccountIDX_t;
#else
typedef unsigned __int32	AccountIDX_t;
#endif