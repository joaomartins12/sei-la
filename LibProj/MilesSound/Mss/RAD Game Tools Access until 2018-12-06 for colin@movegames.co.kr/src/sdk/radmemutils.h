#ifndef __RADMEMUTILSH__
  #define __RADMEMUTILSH__

  #if defined(__RADPS3__)

    #define radstrcat strcat

    #define radstrcpy strcpy

    #define radstrlen strlen

    #define radstrcmp strcmp

    RADDEFFUNC void radmemset16(void* dest,U16 value,U32 count_w);

    #define radmemcpydb memmove
    #define radmemcpy memcpy
    #define radmemset memset

  #elif defined(__RADSPU__)

    #include <string.h>

    #define radstrcat strcat

    #define radstrcpy strcpy

    #define radstrlen strlen

    #define radstrcmp strcmp

    RADDEFFUNC void radmemset16(void* dest,U16 value,U32 count_w);

    #define radmemcpydb memmove
    #define radmemcpy memcpy
    #define radmemset memset

  #elif defined(__RADIPHONE__)

    #include <string.h>

    #define radmemcpy   memcpy

    #define radstrcat strcat

    #define radstrcpy   strcpy

    #define radstrlen   strlen

    #define radmemset   memset

    #define radmemcpydb memmove

    #define radstrcmp strcmp

    RADDEFFUNC void radmemset16(void* dest,U16 value,U32 count_w);

  #elif defined(__RADXENON__)

    #define radmemcpy   memcpy

    #define radstrcat strcat

    #define radstrcpy   strcpy

    #define radstrlen   strlen

    #define radmemset   memset

    #define radmemcpydb memmove

    #define radstrcmp strcmp

    RADDEFFUNC void radmemset16(void* dest,U16 value,U32 count_w);

  #else

    #if defined(__RADX64__)

      #define radstrcpy strcpy
      #define radstrcat strcat
      #define radmemcpy memcpy
      #define radmemcmp memcmp
      #define radmemset memset
      #define radstrlen strlen
      #define radstrchr strchr
      #define radtoupper toupper
      #define radstru32(s) ((U32)atol(s))
      #define radstru64(s) ((U64)_atoi64(s))
      #define radstruinta(s) ((U64)_atoi64(s))
      #define radstrcmp strcmp
      #define radstricmp _stricmp
      #define radstrupr _strupr
      #define radstrlwr _strlwr

      extern void *memset ( void*, int, size_t );
      extern void *memcpy ( void*, const void*, size_t );
      #pragma intrinsic(memset,memcpy)

      #define radmemcpydb memmove

      RADDEFFUNC static void __inline radmemset16(void* dest,U16 value,U32 count_w)
      {
        U32 v = value | ( ( (U32) value ) << 16 );
        U32 s = count_w >> 1;
        U32 * d = (U32*)dest;
        while ( s )
        {
          --s;
          *d++=v;
        }
        if ( count_w & 1 )
        {
          *(U16*)d = value;
        }
      }


      RADDEFFUNC static char* radstpcpy(char* p1, char* p2)
      {
        char c;
        do
        {
          c = *p2++;
          *p1++ = c;
        } while (c);
        return( p1 - 1 );
      }

    #else

      #define radstrcpy strcpy
      #define radstrcat strcat
      #define radmemcpy memcpy
      #define radmemcmp memcmp
      #define radmemset memset
      #define radstrlen strlen
      #define radstrchr strchr
      #define radtoupper toupper
      #define radstru32(s) ((U32)atol(s))
      #define radstru64(s) ((U64)_atoi64(s))
      #define radstruinta(s) ((U32)atol(s))
      #define radstrcmp strcmp
      #define radstrupr _strupr
      #define radstrlwr _strlwr

      #if defined(_MSC_VER) || defined(__RADMAC__)

        #if defined(_MSC_VER)
          #pragma warning( disable : 4035)
          extern void *memset ( void*, int, unsigned int );
          extern void *memcpy ( void*, const void*, unsigned int );
          #pragma intrinsic(memset,memcpy)
        #else
          // #pragma warning( disable : 1011 )
          #include <memory.h>
        #endif

        typedef char* RADPCHAR;

        RADDEFFUNC static void __inline radmemcpydb( void const * dest, void const * src, U32 bytes )
        {
          __asm
          {
            mov ecx,[dword ptr bytes]
            mov edi,dword ptr [dest]
            mov esi,dword ptr [src]
            std
            mov edx,ecx
            lea esi,[esi+ecx-4]
            lea edi,[edi+ecx-4]
            shr ecx,2
            rep movsd
            and edx,3
            jz dne
            add esi,3
            add edi,3
            mov ecx,edx
            rep movsb
            dne:
            cld
          }
        }

        RADDEFFUNC static int __inline radstricmp( void const * s1, void const * s2 )
        {
          int Result;
          __asm
          {
            mov edi,dword ptr [s1]
            mov esi,dword ptr [s2]

            mov eax,1 //skips the first matched check

            zc:
            cmp eax,0
            je matched

            lp:
            movzx eax,byte ptr [edi]
            movzx edx,byte ptr [esi]
            inc esi
            inc edi
            cmp eax,edx
            je zc

            cmp eax,'a'
            jb c1
            cmp eax,'z'
            ja c1
            sub eax,32
            c1:
            cmp edx,'a'
            jb c2
            cmp edx,'z'
            ja c2
            sub edx,32
            c2:

            sub eax,edx
            je lp

            cdq
            lea eax,[edx+edx+1]

            matched:

            // note: redundant, but prevents warning on OSX gcc
            mov [Result],eax
          }
          return Result;
        }

        RADDEFFUNC static RADPCHAR __inline radstpcpy(char* p1, char* p2) {
          RADPCHAR Result;
          __asm {
             mov edx,dword ptr [p1]
             mov ecx,dword ptr [p2]
             cld
            lp:
             mov al,[ecx]
             inc ecx
             mov [edx],al
             inc edx
             cmp al,0
             jne lp
             dec edx
             mov [Result],edx
          }
          return Result;
        }

        RADDEFFUNC static RADPCHAR __inline radstpcpyrs(char* p1, char* p2) {
          RADPCHAR Result;
          __asm {
            mov edx,dword ptr [p1]
            mov ecx,dword ptr [p2]
            cld
           lp:
            mov al,[ecx]
            inc ecx
            mov [edx],al
            inc edx
            cmp al,0
            jne lp
            dec ecx
            mov [Result],ecx
          }
          return Result;
        }

        RADDEFFUNC static void __inline radmemset16(void* dest,U16 value,U32 count_w) {
          __asm {
            mov edi,dword ptr [dest]
            mov ax, [word ptr value]
            mov ecx, [ dword ptr count_w]
            shl eax,16
            cld
            mov ax, [word ptr value]
            mov bl,cl
            shr ecx,1
#ifdef _MSC_VER
            rep stosd
#else
            rep stosl
#endif
            mov cl,bl
            and cl,1
            rep stosw
          }
        }

        RADDEFFUNC static void __inline radmemset32(void* dest,U32 value,U32 count_d) {
          __asm {
            mov edi,dword ptr [dest]
            mov eax,[dword ptr value]
            mov ecx,[dword ptr count_d]
            cld
            rep stosd
          }
        }

      #if defined(_MSC_VER)
        #pragma warning( default : 4035)
      #endif

      #endif

    #endif

  #endif

  #define uintneg1 ((UINTa)(SINTa)(S32)-1)
  #define RAD_align(var) var; U8 junk##var[4-(sizeof(var)&3)];
  #define RAD_align_after(var) U8 junk##var[4-(sizeof(var)&3)]={0};
  #define RAD_align_init(var,val) var=val; U8 junk##var[4-(sizeof(var)&3)]={0};
  #define RAD_align_array(var,num) var[num]; U8 junk##var[4-(sizeof(var)&3)];
  #define RAD_align_string(var,str) char var[]=str; U8 junk##var[4-(sizeof(var)&3)]={0};

  // for multi-processor machines

  #ifdef __RADNT__

    #ifdef __RADX64__

      long _InterlockedExchangeAdd(
        long volatile * Addend,
        long Value
      );

      #define LockedIncrement(var) _InterlockedExchangeAdd( (S32*) &(var), 1 )
      #define LockedDecrement(var) _InterlockedExchangeAdd( (S32*) &(var), -1 )

      #define LockedIncrementFunc(ptr) _InterlockedExchangeAdd( (S32*) (ptr), 1 )
      #define LockedDecrementFunc(ptr) _InterlockedExchangeAdd( (S32*) (ptr), -1 )

      #define LockedAddFunc(ptr,val) _InterlockedExchangeAdd( (S32*) (ptr), val )

    #else


      #define LockedIncrement(var) __asm { lock inc [var] }
      #define LockedDecrement(var) __asm { lock dec [var] }
      RADDEFFUNC static void __inline LockedIncrementFunc(void * var) {
        __asm {
          mov eax,[var]
          lock inc [eax]
        }
      }

      RADDEFFUNC static void __inline LockedDecrementFunc(void * var) {
        __asm {
           mov eax,[var]
           lock dec [eax]
        }
      }

      RADDEFFUNC static void __inline LockedAddFunc(void * var,U32 val) {
        __asm {
          mov eax,[var]
          mov edx,[val]
          lock add [eax],edx
        }
      }
    #endif

  #else

    #if defined(__RADIPHONE__)
      #include <libkern/OSAtomic.h>
      #define LockedIncrement(var) OSAtomicAdd32(  1, (volatile int32_t*) &(var) )
      #define LockedDecrement(var) OSAtomicAdd32( -1, (volatile int32_t*) &(var) )

      #define LockedIncrementFunc(ptr) OSAtomicAdd32( 1, (volatile int32_t*) (ptr) )
      #define LockedDecrementFunc(ptr) OSAtomicAdd32( -1, (volatile int32_t*) (ptr) )

      #define LockedAddFunc(ptr,val) OSAtomicAdd32( val, (volatile int32_t*) (ptr) )
    #elif defined(__RADPSP2__)
      #define LockedIncrement(var) sceAtomicIncrement((volatile int32_t*) &(var) )
      #define LockedDecrement(var) sceAtomicDecrement((volatile int32_t*) &(var) )

      #define LockedIncrementFunc(ptr) sceAtomicIncrement( 1, (volatile int32_t*) (ptr) )
      #define LockedDecrementFunc(ptr) sceAtomicDecrement( -1, (volatile int32_t*) (ptr) )

      #define LockedAddFunc(ptr,val) sceAtomicAdd((volatile int32_t*) (ptr), val )
    #elif defined(__RADMAC__)

      #if !defined(__OPENTRANSPORT__)
        extern long OTAtomicAdd32( long toAdd, long *  dest);
      #endif

      #define LockedIncrement(var) OTAtomicAdd32(  1, (long*) &(var) )
      #define LockedDecrement(var) OTAtomicAdd32( -1, (long*) &(var) )

      #define LockedIncrementFunc(ptr) OTAtomicAdd32( 1, (long*) (ptr) )
      #define LockedDecrementFunc(ptr) OTAtomicAdd32( -1, (long*) (ptr) )

      #define LockedAddFunc(ptr,val) OTAtomicAdd32( val, (long*) (ptr) )

    #elif defined(__RADSPU__)

      #define LockedIncrement(var) {++(var);}
      #define LockedDecrement(var) {--(var);}

      #define LockedIncrementFunc(ptr) {++(*((U32*)(ptr)));}
      #define LockedDecrementFunc(ptr) {--(*((U32*)(ptr)));}

      #define LockedAddFunc(ptr,val) {(*((U32*)(ptr)))+=(val);}

    #elif defined(__RADXENON__)

void __stdcall OutputDebugStringA( char const * lpOutputString );
#define OutputDebugString  OutputDebugStringA

long
__stdcall
_InterlockedIncrement(
      long volatile *lpAddend
    );


long
__stdcall
_InterlockedDecrement(
      long volatile *lpAddend
    );

long
__stdcall
_InterlockedExchangeAdd(
      long volatile *lpAddend, long amt
    );

#ifndef _XBOX_H_
unsigned int __stdcall XQueryMemoryProtect( void const * addr );
#define PAGE_WRITECOMBINE 0x400
#endif

  void __emit(unsigned int opcode);
  #define __lwsync()      __emit(0x7C2004AC)

  #define LockedIncrement(var) { _InterlockedIncrement( (long*)&var ); __lwsync(); }
  #define LockedDecrement(var) { __lwsync(); _InterlockedDecrement( (long*)&var ); }

  #define LockedIncrementFunc(ptr) { _InterlockedIncrement( (long*)(ptr) ); __lwsync(); }
  #define LockedDecrementFunc(ptr) { __lwsync(); _InterlockedDecrement( (long*)(ptr) ); }


      #define LockedAddFunc(ptr,val) _InterlockedExchangeAdd( (S32*) (ptr), val )

    #elif defined(__RADPS3__)
      #if defined(__SNC__)
        #define RAD_PS3_SNC 1
      #else
        #define RAD_PS3_GCC 1
      #endif

      #if RAD_PS3_GCC
          RADDEFFUNC static inline U32 AtomicAdd32(U32 *ea, U32 value)
          {
            U32 old, tmp;

            __asm__ volatile(
              "# AtomicAdd32(ea=%[ea],old=%[old],val=%[value],tmp=%[tmp])\n"
              ".loop%=:\n"
              "  lwarx   %[old], 0, %[ea]\n"
              "  add     %[tmp], %[value], %[old]\n"
              "  stwcx.  %[tmp], 0, %[ea]\n"
              "  bne-    .loop%=\n"
              "  lwsync\n"
              : [old]"=&r"(old), [tmp]"=&r"(tmp)
              : [ea]"b"(ea), [value]"r"(value)
              : "cc", "memory");
            return old;
          }
      #elif RAD_PS3_SNC
          static inline U32 AtomicAdd32(U32* ea, U32 value)
          {
             return __builtin_cellAtomicAdd32(ea, value);
          }
      #endif

      #define LockedIncrement(var) AtomicAdd32( (U32*) &(var),  1 )
      #define LockedDecrement(var) AtomicAdd32( (U32*) &(var), -1 )

      #define LockedIncrementFunc(ptr) AtomicAdd32( (U32*) (ptr),  1 )
      #define LockedDecrementFunc(ptr) AtomicAdd32( (U32*) (ptr), -1 )

      #define LockedAddFunc(ptr,val) AtomicAdd32( (U32*) (ptr), val )

    #elif defined(__RADLINUX__)

      #define radmemcpydb memmove

      // todo: need real interlocks here.  They're kind of hard to
      // come by on linux
      #define LockedIncrement(var) { ++var; }
      #define LockedDecrement(var) { --var; }
      RADDEFFUNC static void __inline LockedIncrementFunc(void * var) { ++(*(U32*)var); }
      RADDEFFUNC static void __inline LockedDecrementFunc(void * var) { --(*(U32*)var); }
      //static void __inline LockedAddFunc(void * var,U32 val)

    #else

      #define LockedIncrement(var) __asm { inc [var] }
      #define LockedDecrement(var) __asm { dec [var] }
      RADDEFFUNC static void __inline LockedIncrementFunc(void * var) { __asm { mov eax,[var]
                                                                  inc [eax] } }
      RADDEFFUNC static void __inline LockedDecrementFunc(void * var) { __asm { mov eax,[var]
                                                                  dec [eax] } }
      RADDEFFUNC static void __inline LockedAddFunc(void * var,U32 val) {
        __asm {
          mov eax,[var]
          mov edx,[val]
          add [eax],edx
        }
      }

    #endif

  #endif

#endif
