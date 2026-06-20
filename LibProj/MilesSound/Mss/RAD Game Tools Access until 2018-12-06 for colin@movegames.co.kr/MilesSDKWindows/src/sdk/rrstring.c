// Copyright 2008-2013 RAD Game Tools
// rrstring.c 

#include "rrstring.h"

#ifdef rrstpcpy
#undef rrstpcpy
#endif
RADDEFFUNC char * rrstpcpy(char* dest,char const * src)
{
   char const * s = src;
   char * d = dest;
   for(;;)
   {
      char c = *s;
      if ( c == 0 )
         break;
      ++s;
      *d++ = c;
   }
   *d = 0;
   return d;
}

#ifdef rrmemset16
#undef rrmemset16
#endif
RADDEFFUNC void rrmemset16(void* dest,U16 value,U32 size)
{  
   U32 v=((U32)value)|(((U32)value)<<16);  
   U32* RADRESTRICT d=(U32*)dest;  
   U32 s;  

   if ( ( (UINTa) dest ) & 2 )
   {
      ((U16* RADRESTRICT )d)[0]=value;
      d = (U32* RADRESTRICT)(((UINTa)d)+2);
      --size;
   }

   s=size/2;  

   while(s--)  
   {    
      *d++=v;
   }  

   if (size&1)
      *((U16* RADRESTRICT )d)=(U16)v;
}

#ifdef rrmemset32
#undef rrmemset32
#endif
RADDEFFUNC void rrmemset32(void* dest,U32 value,U32 count_w)
{
   U32 * RADRESTRICT d = (U32 *) dest;
   U32 * RADRESTRICT end = d + count_w;
   while (d < end)
      *d++ = value;
}

RADDEFFUNC U16 *rrwcscpy(U16 *dst, const U16 *src)
{
	while (*src)
		*dst++ = *src++;
	*dst = 0;
	return dst;
}

RADDEFFUNC U32 rrwcslen(const U16 *string)
{
   U32 n=0;
   while (string[n])
      ++n;
   return n;
}

RADDEFFUNC S32 rrwcscmp(const U16 *str1, const U16 *str2)
{
   while (*str1 == *str2) {
      if (*str1 == 0) return 0;
      ++str1;
      ++str2;
   }
   return *str1 < *str2 ? -1 : *str1 > *str2;
}

RADDEFFUNC S32 rrwcsncmp(const U16 *str1, const U16 *str2, U32 n)
{
   U32 i;
   for (i=0; i < n; ++i) {
      if (str1[i] != str2[i])
         return str1[i] < str2[i] ? -1 : str1[i] > str2[i];
      if (str1[i] == 0) return 0;
   }
   return 0;
}

RADDEFFUNC U16 *rrwcschr(U16 *str, U16 chr)
{
   while (*str) {
      if (*str == chr)
         return str;
      str++;
   }
   return NULL;
}

RADDEFFUNC U16 *rrwcsrchr(U16 *str, U16 chr)
{
   U16 *match = NULL;
   while (*str) {
      if (*str == chr)
         match = str;
      str++;
   }
   return match;
}

// helper used by rrwcstol below
static U32 getwdigit(U16 chr)
{
	U16 t;
	if ((t=chr - '0') < 10)
		return t;
	else if ((t=chr - 'A') < 26 || (t=chr - 'a') < 26)
		return t + 10;
	else
		return ~0u;
}

#define S32_MAX 0x7fffffff
#define S32_MIN (-S32_MAX-1) // VC++ warns about -0x80000000 as literal constant

RADDEFFUNC S32 rrwcstol(U16 *str, U16 **endptr, int base)
{
	S32 value;
	U32 digit;
	U32 absval = 0;
	U32 base_u = (base < 2) ? 2 : (base > 36) ? 36 : base;
	U32 fast_max = (S32_MAX - (base_u - 1)) / base_u;
	rrbool neg = 0, overflow = 0;
	
	// sign?
	if (*str == '+')			str++;
	else if (*str == '-')	str++, neg = 1;
	
	// digits (read unsigned value, being careful about overflow)
	while ((digit = getwdigit(*str)) < base_u) {
		str++;
		if (absval <= fast_max || absval <= (S32_MAX-digit) / base_u)
			absval = absval*base_u + digit;
		else
			overflow = 1; // flag overflow, but we need to keep parsing
	}
	
	// handle sign and overflow
	if (!neg)
		value = overflow ? S32_MAX : absval;
	else
		value = overflow ? S32_MIN : -(S32) absval;

	if (endptr)
		*endptr = str;
	return value;
}
