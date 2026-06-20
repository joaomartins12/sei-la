static void AILCALL Merge_DestMono_SrcMono_Src16_NoVolume_NoResample( // 2
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // Apply volume
  sample_l <<= 11;

  // Merge sample data loop
  merge_loop2:

  // Load the first dest value
  dest_l = *dest;

  // Merge sample data into output buffer
  dest_l += sample_l;
  *(S32*)dest = dest_l;
  dest = (S32*)dest + 1;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit2;

  // Move the source pointer
  src = (U8*)src + 2;
  if (src >= src_end) goto src_end_exit2;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // Apply volume
  sample_l <<= 11;

  // End loop
  goto      merge_loop2;

  // Jump out point if end of dest is reached
  dest_end_exit2:
  src = (U8*)src + 2;

  // Jump out point if end of src is reached
  src_end_exit2:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestStereo_SrcMono_Src16_NoVolume_NoResample( // 3
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // Apply volume
  sample_l <<= 11;

  // Merge sample data loop
  merge_loop3:

  // Load the first dest value
  dest_l = *dest;
  dest_r = *(dest + 1);

  // Merge sample data into output buffer
  dest_l += sample_l;
  dest_r += sample_l;
  *(S32*)dest = dest_l;
  *((S32*)dest + 1) = dest_r;
  dest = (S32*)dest + 2;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit3;

  // Move the source pointer
  src = (U8*)src + 2;
  if (src >= src_end) goto src_end_exit3;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // Apply volume
  sample_l <<= 11;

  // End loop
  goto      merge_loop3;

  // Jump out point if end of dest is reached
  dest_end_exit3:
  src = (U8*)src + 2;

  // Jump out point if end of src is reached
  src_end_exit3:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestMono_SrcStereo_Src16_NoVolume_NoResample( // 10
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Merge left and right channels for mono dest
  sample_l += sample_r;

  // Apply volume
  sample_l <<= 11;

  // Merge sample data loop
  merge_loop10:

  // Load the first dest value
  dest_l = *dest;

  // Merge sample data into output buffer
  dest_l += sample_l;
  *(S32*)dest = dest_l;
  dest = (S32*)dest + 1;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit10;

  // Move the source pointer
  src = (U8*)src + 4;
  if (src >= src_end) goto src_end_exit10;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Merge left and right channels for mono dest
  sample_l += sample_r;

  // Apply volume
  sample_l <<= 11;

  // End loop
  goto      merge_loop10;

  // Jump out point if end of dest is reached
  dest_end_exit10:
  src = (U8*)src + 4;

  // Jump out point if end of src is reached
  src_end_exit10:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestStereo_SrcStereo_Src16_NoVolume_NoResample( // 11
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Apply volume
  sample_l <<= 11;
  sample_r <<= 11;

  // Merge sample data loop
  merge_loop11:

  // Load the first dest value
  dest_l = *dest;
  dest_r = *(dest + 1);

  // Merge sample data into output buffer
  dest_l += sample_l;
  dest_r += sample_r;
  *(S32*)dest = dest_l;
  *((S32*)dest + 1) = dest_r;
  dest = (S32*)dest + 2;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit11;

  // Move the source pointer
  src = (U8*)src + 4;
  if (src >= src_end) goto src_end_exit11;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Apply volume
  sample_l <<= 11;
  sample_r <<= 11;

  // End loop
  goto      merge_loop11;

  // Jump out point if end of dest is reached
  dest_end_exit11:
  src = (U8*)src + 4;

  // Jump out point if end of src is reached
  src_end_exit11:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestMono_SrcMono_Src16_NoVolume_Resample( // 18
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // Apply volume
  sample_l <<= 11;

  // Merge sample data loop
  merge_loop18:

  // Load the first dest value
  dest_l = *dest;

  // Merge sample data into output buffer
  dest_l += sample_l;
  *(S32*)dest = dest_l;
  dest = (S32*)dest + 1;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit18;

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 1);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;
  if (src >= src_end) goto src_end_exit18;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // Apply volume
  sample_l <<= 11;

  // End loop
  goto      merge_loop18;

  // Jump out point if end of dest is reached
  dest_end_exit18:

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 1);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;

  // Jump out point if end of src is reached
  src_end_exit18:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestStereo_SrcMono_Src16_NoVolume_Resample( // 19
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // Apply volume
  sample_l <<= 11;

  // Merge sample data loop
  merge_loop19:

  // Load the first dest value
  dest_l = *dest;
  dest_r = *(dest + 1);

  // Merge sample data into output buffer
  dest_l += sample_l;
  dest_r += sample_l;
  *(S32*)dest = dest_l;
  *((S32*)dest + 1) = dest_r;
  dest = (S32*)dest + 2;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit19;

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 1);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;
  if (src >= src_end) goto src_end_exit19;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // Apply volume
  sample_l <<= 11;

  // End loop
  goto      merge_loop19;

  // Jump out point if end of dest is reached
  dest_end_exit19:

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 1);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;

  // Jump out point if end of src is reached
  src_end_exit19:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestMono_SrcMono_Src16_NoVolume_DownFiltered( // 22
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
 cur_l1 = *(S32*)cur_l_ptr;
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);

  // check to see if we have to call the upsampling version
  if ((playback_ratio & 0xffff0000) == 0) goto Merge_DestMono_SrcMono_Src16_NoVolume_UpFiltered;

  // build average dividers
  divider_l = 0x8000000;  // 0x100000000/32
  divider_l /= playback_ratio;
  divider_l--;

  // handle start up loop management
  r0temp = src_fract >> 30;
  src_fract &= 0x3fffffff;

  // load dest in case we jump right into the loop
  // Load the first dest value
  dest_l = *dest;
  if (r0temp >= 2) goto whole_continue22;
  if (r0temp == 1) goto last_continue22;

  // Merge sample data loop
  merge_loop22:

  // Load the first dest value
  dest_l = *dest;

  // weight the initial sample
  sample_l = cur_l1 * src_fract;
  src_fract += playback_ratio;
  sample_l >>= 16;
  cur_l1 -= sample_l;
  if (src_fract < 0x20000) goto skip_loop22;

  // loop to load all of the full sample points
  whole_loop22:
  if (src >= src_end) goto src_whole_exit22;
  whole_continue22:

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);
  cur_l1 += sample_l;
  src_fract -= 0x10000;
  src = (U8*)src + 2;
  if (src_fract >= 0x20000) goto whole_loop22;

  skip_loop22:
  src_fract &= 0xffff;
  last_continue22:
  if (src >= src_end) goto src_last_exit22;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);
  src = (U8*)src + 2;

  // weight the final sample
  r0temp = sample_l * src_fract;
  r0temp >>= 16;
  r0temp += cur_l1;
  cur_l1 = sample_l;
  sample_l = r0temp * divider_l;

  // Merge sample data into output buffer
  dest_l += sample_l;
  *(S32*)dest = dest_l;
  dest = (S32*)dest + 1;
  if ((void*)dest < (void*)dest_end) goto merge_loop22;

  // Jump out point if end of dest is reached
  dest_end_exit22:
  goto      src_save_value22;

  // jump out when src is exceed, but save our current loop position
  src_whole_exit22:
  src_fract |= 0x80000000;
  src_last_exit22:
  src_fract |= 0x40000000;
  src_save_value22:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;

  // restore upper variables

  // Routine end
  return;

Merge_DestMono_SrcMono_Src16_NoVolume_UpFiltered: // 534

  // there is no stack setup, because it has already been performed
  //   by the down filter version of this function

  // setup the resample stuff
  src_fract <<= 16;
  playback_ratio <<= 16;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;

  // Merge sample data loop
  merge_loop534:

  // Load the first dest value
  dest_l = *dest;

  // Upsample the data points
  r0temp = src_fract >> 17;
  sample_l = cur_l1 - cur_l2;
  sample_l *= r0temp;
  sample_l >>= 15;
  sample_l += cur_l2;

  // Apply volume
  sample_l <<= 11;

  // Merge sample data into output buffer
  dest_l += sample_l;
  *(S32*)dest = dest_l;
  dest = (S32*)dest + 1;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit534;

  // Add to accumulator and advance the source correctly
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract > old )  goto merge_loop534;}

  // Move the source pointer
  src = (U8*)src + 2;
  if (src >= src_end) goto src_end_exit534;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;

  // End loop
  goto      merge_loop534;

  // Jump out point if end of dest is reached
  dest_end_exit534:
  src = (U8*)src + 2;
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract <= old ) goto skip_filter_adjust534;}

  // rotate filter values
  cur_l1 = cur_l2;

  // un-increment the source to skip the early source adjustment
  src = (U8*)src - 2;

  skip_filter_adjust534:

  // Jump out point if end of src is reached
  src_end_exit534:

  // adjust fractional
  src_fract >>= 16;

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestStereo_SrcMono_Src16_NoVolume_DownFiltered( // 23
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
 cur_l1 = *(S32*)cur_l_ptr;
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);

  // check to see if we have to call the upsampling version
  if ((playback_ratio & 0xffff0000) == 0) goto Merge_DestStereo_SrcMono_Src16_NoVolume_UpFiltered;

  // build average dividers
  divider_l = 0x8000000;  // 0x100000000/32
  divider_l /= playback_ratio;
  divider_l--;

  // handle start up loop management
  r0temp = src_fract >> 30;
  src_fract &= 0x3fffffff;

  // load dest in case we jump right into the loop
  // Load the first dest value
  dest_l = *dest;
  dest_r = *(dest + 1);
  if (r0temp >= 2) goto whole_continue23;
  if (r0temp == 1) goto last_continue23;

  // Merge sample data loop
  merge_loop23:

  // Load the first dest value
  dest_l = *dest;
  dest_r = *(dest + 1);

  // weight the initial sample
  sample_l = cur_l1 * src_fract;
  src_fract += playback_ratio;
  sample_l >>= 16;
  cur_l1 -= sample_l;
  if (src_fract < 0x20000) goto skip_loop23;

  // loop to load all of the full sample points
  whole_loop23:
  if (src >= src_end) goto src_whole_exit23;
  whole_continue23:

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);
  cur_l1 += sample_l;
  src_fract -= 0x10000;
  src = (U8*)src + 2;
  if (src_fract >= 0x20000) goto whole_loop23;

  skip_loop23:
  src_fract &= 0xffff;
  last_continue23:
  if (src >= src_end) goto src_last_exit23;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);
  src = (U8*)src + 2;

  // weight the final sample
  r0temp = sample_l * src_fract;
  r0temp >>= 16;
  r0temp += cur_l1;
  cur_l1 = sample_l;
  sample_l = r0temp * divider_l;

  // Merge sample data into output buffer
  dest_l += sample_l;
  dest_r += sample_l;
  *(S32*)dest = dest_l;
  *((S32*)dest + 1) = dest_r;
  dest = (S32*)dest + 2;
  if ((void*)dest < (void*)dest_end) goto merge_loop23;

  // Jump out point if end of dest is reached
  dest_end_exit23:
  goto      src_save_value23;

  // jump out when src is exceed, but save our current loop position
  src_whole_exit23:
  src_fract |= 0x80000000;
  src_last_exit23:
  src_fract |= 0x40000000;
  src_save_value23:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;

  // restore upper variables

  // Routine end
  return;

Merge_DestStereo_SrcMono_Src16_NoVolume_UpFiltered: // 535

  // there is no stack setup, because it has already been performed
  //   by the down filter version of this function

  // setup the resample stuff
  src_fract <<= 16;
  playback_ratio <<= 16;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;

  // Merge sample data loop
  merge_loop535:

  // Load the first dest value
  dest_l = *dest;
  dest_r = *(dest + 1);

  // Upsample the data points
  r0temp = src_fract >> 17;
  sample_l = cur_l1 - cur_l2;
  sample_l *= r0temp;
  sample_l >>= 15;
  sample_l += cur_l2;

  // Apply volume
  sample_l <<= 11;

  // Merge sample data into output buffer
  dest_l += sample_l;
  dest_r += sample_l;
  *(S32*)dest = dest_l;
  *((S32*)dest + 1) = dest_r;
  dest = (S32*)dest + 2;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit535;

  // Add to accumulator and advance the source correctly
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract > old )  goto merge_loop535;}

  // Move the source pointer
  src = (U8*)src + 2;
  if (src >= src_end) goto src_end_exit535;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;

  // End loop
  goto      merge_loop535;

  // Jump out point if end of dest is reached
  dest_end_exit535:
  src = (U8*)src + 2;
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract <= old ) goto skip_filter_adjust535;}

  // rotate filter values
  cur_l1 = cur_l2;

  // un-increment the source to skip the early source adjustment
  src = (U8*)src - 2;

  skip_filter_adjust535:

  // Jump out point if end of src is reached
  src_end_exit535:

  // adjust fractional
  src_fract >>= 16;

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestMono_SrcStereo_Src16_NoVolume_Resample( // 26
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Merge left and right channels for mono dest
  sample_l += sample_r;

  // Apply volume
  sample_l <<= 11;

  // Merge sample data loop
  merge_loop26:

  // Load the first dest value
  dest_l = *dest;

  // Merge sample data into output buffer
  dest_l += sample_l;
  *(S32*)dest = dest_l;
  dest = (S32*)dest + 1;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit26;

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 2);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;
  if (src >= src_end) goto src_end_exit26;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Merge left and right channels for mono dest
  sample_l += sample_r;

  // Apply volume
  sample_l <<= 11;

  // End loop
  goto      merge_loop26;

  // Jump out point if end of dest is reached
  dest_end_exit26:

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 2);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;

  // Jump out point if end of src is reached
  src_end_exit26:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestStereo_SrcStereo_Src16_NoVolume_Resample( // 27
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Apply volume
  sample_l <<= 11;
  sample_r <<= 11;

  // Merge sample data loop
  merge_loop27:

  // Load the first dest value
  dest_l = *dest;
  dest_r = *(dest + 1);

  // Merge sample data into output buffer
  dest_l += sample_l;
  dest_r += sample_r;
  *(S32*)dest = dest_l;
  *((S32*)dest + 1) = dest_r;
  dest = (S32*)dest + 2;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit27;

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 2);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;
  if (src >= src_end) goto src_end_exit27;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Apply volume
  sample_l <<= 11;
  sample_r <<= 11;

  // End loop
  goto      merge_loop27;

  // Jump out point if end of dest is reached
  dest_end_exit27:

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 2);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;

  // Jump out point if end of src is reached
  src_end_exit27:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestMono_SrcStereo_Src16_NoVolume_DownFiltered( // 30
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
 cur_l1 = *(S32*)cur_l_ptr;
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);

  // check to see if we have to call the upsampling version
  if ((playback_ratio & 0xffff0000) == 0) goto Merge_DestMono_SrcStereo_Src16_NoVolume_UpFiltered;

  // build average dividers
  divider_l = 0x8000000;  // 0x100000000/32
  divider_l /= playback_ratio;
  divider_l--;

  // handle start up loop management
  r0temp = src_fract >> 30;
  src_fract &= 0x3fffffff;

  // load dest in case we jump right into the loop
  // Load the first dest value
  dest_l = *dest;
  if (r0temp >= 2) goto whole_continue30;
  if (r0temp == 1) goto last_continue30;

  // Merge sample data loop
  merge_loop30:

  // Load the first dest value
  dest_l = *dest;

  // weight the initial sample
  sample_l = cur_l1 * src_fract;
  src_fract += playback_ratio;
  sample_l >>= 16;
  cur_l1 -= sample_l;
  if (src_fract < 0x20000) goto skip_loop30;

  // loop to load all of the full sample points
  whole_loop30:
  if (src >= src_end) goto src_whole_exit30;
  whole_continue30:

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Merge left and right channels for mono dest
  sample_l += sample_r;
  cur_l1 += sample_l;
  src_fract -= 0x10000;
  src = (U8*)src + 4;
  if (src_fract >= 0x20000) goto whole_loop30;

  skip_loop30:
  src_fract &= 0xffff;
  last_continue30:
  if (src >= src_end) goto src_last_exit30;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Merge left and right channels for mono dest
  sample_l += sample_r;
  src = (U8*)src + 4;

  // weight the final sample
  r0temp = sample_l * src_fract;
  r0temp >>= 16;
  r0temp += cur_l1;
  cur_l1 = sample_l;
  sample_l = r0temp * divider_l;

  // Merge sample data into output buffer
  dest_l += sample_l;
  *(S32*)dest = dest_l;
  dest = (S32*)dest + 1;
  if ((void*)dest < (void*)dest_end) goto merge_loop30;

  // Jump out point if end of dest is reached
  dest_end_exit30:
  goto      src_save_value30;

  // jump out when src is exceed, but save our current loop position
  src_whole_exit30:
  src_fract |= 0x80000000;
  src_last_exit30:
  src_fract |= 0x40000000;
  src_save_value30:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;
  *(S32*)cur_r_ptr = cur_r1;

  // restore upper variables

  // Routine end
  return;

Merge_DestMono_SrcStereo_Src16_NoVolume_UpFiltered: // 542

  // there is no stack setup, because it has already been performed
  //   by the down filter version of this function

  // setup the resample stuff
  src_fract <<= 16;
  playback_ratio <<= 16;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Merge left and right channels for mono dest
  sample_l += sample_r;

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;

  // Merge sample data loop
  merge_loop542:

  // Load the first dest value
  dest_l = *dest;

  // Upsample the data points
  r0temp = src_fract >> 17;
  sample_l = cur_l1 - cur_l2;
  sample_l *= r0temp;
  sample_l >>= 15;
  sample_l += cur_l2;

  // Apply volume
  sample_l <<= 11;

  // Merge sample data into output buffer
  dest_l += sample_l;
  *(S32*)dest = dest_l;
  dest = (S32*)dest + 1;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit542;

  // Add to accumulator and advance the source correctly
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract > old )  goto merge_loop542;}

  // Move the source pointer
  src = (U8*)src + 4;
  if (src >= src_end) goto src_end_exit542;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Merge left and right channels for mono dest
  sample_l += sample_r;

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;

  // End loop
  goto      merge_loop542;

  // Jump out point if end of dest is reached
  dest_end_exit542:
  src = (U8*)src + 4;
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract <= old ) goto skip_filter_adjust542;}

  // rotate filter values
  cur_l1 = cur_l2;

  // un-increment the source to skip the early source adjustment
  src = (U8*)src - 4;

  skip_filter_adjust542:

  // Jump out point if end of src is reached
  src_end_exit542:

  // adjust fractional
  src_fract >>= 16;

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;
  *(S32*)cur_r_ptr = cur_r1;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestStereo_SrcStereo_Src16_NoVolume_DownFiltered( // 31
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
 cur_l1 = *(S32*)cur_l_ptr;
 cur_r1 = *(S32*)cur_r_ptr;
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);

  // check to see if we have to call the upsampling version
  if ((playback_ratio & 0xffff0000) == 0) goto Merge_DestStereo_SrcStereo_Src16_NoVolume_UpFiltered;

  // build average dividers
  divider_l = 0x8000000;  // 0x100000000/32
  divider_l /= playback_ratio;
  divider_l--;
  divider_r = 0x8000000;  // 0x100000000/32
  divider_r /= playback_ratio;
  divider_r--;

  // handle start up loop management
  r0temp = src_fract >> 30;
  src_fract &= 0x3fffffff;

  // load dest in case we jump right into the loop
  // Load the first dest value
  dest_l = *dest;
  dest_r = *(dest + 1);
  if (r0temp >= 2) goto whole_continue31;
  if (r0temp == 1) goto last_continue31;

  // Merge sample data loop
  merge_loop31:

  // Load the first dest value
  dest_l = *dest;
  dest_r = *(dest + 1);

  // weight the initial sample
  sample_l = cur_l1 * src_fract;
  sample_r = cur_r1 * src_fract;
  src_fract += playback_ratio;
  sample_l >>= 16;
  sample_r >>= 16;
  cur_l1 -= sample_l;
  cur_r1 -= sample_r;
  if (src_fract < 0x20000) goto skip_loop31;

  // loop to load all of the full sample points
  whole_loop31:
  if (src >= src_end) goto src_whole_exit31;
  whole_continue31:

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;
  cur_l1 += sample_l;
  cur_r1 += sample_r;
  src_fract -= 0x10000;
  src = (U8*)src + 4;
  if (src_fract >= 0x20000) goto whole_loop31;

  skip_loop31:
  src_fract &= 0xffff;
  if (src >= src_end) goto src_last_exit31;
  last_continue31:

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;
  src = (U8*)src + 4;

  // weight the final sample
  r0temp = sample_l * src_fract;
  r0temp >>= 16;
  r0temp += cur_l1;
  cur_l1 = sample_l;
  #define temp1 sample_l
  temp1 = sample_r * src_fract;
  temp1 >>= 16;
  temp1 += cur_r1;
  cur_r1 = sample_r;
  sample_r = temp1 * divider_r;
  sample_l = r0temp * divider_l;
  #undef temp1

  // Merge sample data into output buffer
  dest_l += sample_l;
  dest_r += sample_r;
  *(S32*)dest = dest_l;
  *((S32*)dest + 1) = dest_r;
  dest = (S32*)dest + 2;
  if ((void*)dest < (void*)dest_end) goto merge_loop31;

  // Jump out point if end of dest is reached
  dest_end_exit31:
  goto      src_save_value31;

  // jump out when src is exceed, but save our current loop position
  src_whole_exit31:
  src_fract |= 0x80000000;
  src_last_exit31:
  src_fract |= 0x40000000;
  src_save_value31:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;
  *(S32*)cur_r_ptr = cur_r1;

  // restore upper variables

  // Routine end
  return;

Merge_DestStereo_SrcStereo_Src16_NoVolume_UpFiltered: // 543

  // there is no stack setup, because it has already been performed
  //   by the down filter version of this function

  // setup the resample stuff
  src_fract <<= 16;
  playback_ratio <<= 16;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;
  cur_r2 = cur_r1;
  cur_r1 = sample_r;

  // Merge sample data loop
  merge_loop543:

  // Load the first dest value
  dest_l = *dest;
  dest_r = *(dest + 1);

  // Upsample the data points
  r0temp = src_fract >> 17;
  sample_l = cur_l1 - cur_l2;
  sample_r = cur_r1 - cur_r2;
  sample_l *= r0temp;
  sample_r *= r0temp;
  sample_l >>= 15;
  sample_r >>= 15;
  sample_l += cur_l2;
  sample_r += cur_r2;

  // Apply volume
  sample_l <<= 11;
  sample_r <<= 11;

  // Merge sample data into output buffer
  dest_l += sample_l;
  dest_r += sample_r;
  *(S32*)dest = dest_l;
  *((S32*)dest + 1) = dest_r;
  dest = (S32*)dest + 2;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit543;

  // Add to accumulator and advance the source correctly
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract > old )  goto merge_loop543;}

  // Move the source pointer
  src = (U8*)src + 4;
  if (src >= src_end) goto src_end_exit543;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;
  cur_r2 = cur_r1;
  cur_r1 = sample_r;

  // End loop
  goto      merge_loop543;

  // Jump out point if end of dest is reached
  dest_end_exit543:
  src = (U8*)src + 4;
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract <= old ) goto skip_filter_adjust543;}

  // rotate filter values
  cur_l1 = cur_l2;
  cur_r1 = cur_r2;

  // un-increment the source to skip the early source adjustment
  src = (U8*)src - 4;

  skip_filter_adjust543:

  // Jump out point if end of src is reached
  src_end_exit543:

  // adjust fractional
  src_fract >>= 16;

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;
  *(S32*)cur_r_ptr = cur_r1;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestMono_SrcMono_Src16_Volume_NoResample( // 34
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // Apply volume
  sample_l *= scale_left;

  // Merge sample data loop
  merge_loop34:

  // Load the first dest value
  dest_l = *dest;

  // Merge sample data into output buffer
  dest_l += sample_l;
  *(S32*)dest = dest_l;
  dest = (S32*)dest + 1;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit34;

  // Move the source pointer
  src = (U8*)src + 2;
  if (src >= src_end) goto src_end_exit34;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // Apply volume
  sample_l *= scale_left;

  // End loop
  goto      merge_loop34;

  // Jump out point if end of dest is reached
  dest_end_exit34:
  src = (U8*)src + 2;

  // Jump out point if end of src is reached
  src_end_exit34:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestStereo_SrcMono_Src16_Volume_NoResample( // 35
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // Apply volume into each sample
  sample_r = sample_l * scale_right;
  sample_l = sample_l * scale_left;

  // Merge sample data loop
  merge_loop35:

  // Load the first dest value
  dest_l = *dest;
  dest_r = *(dest + 1);

  // Merge sample data into output buffer
  dest_l += sample_l;
  dest_r += sample_r;
  *(S32*)dest = dest_l;
  *((S32*)dest + 1) = dest_r;
  dest = (S32*)dest + 2;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit35;

  // Move the source pointer
  src = (U8*)src + 2;
  if (src >= src_end) goto src_end_exit35;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // Apply volume into each sample
  sample_r = sample_l * scale_right;
  sample_l = sample_l * scale_left;

  // End loop
  goto      merge_loop35;

  // Jump out point if end of dest is reached
  dest_end_exit35:
  src = (U8*)src + 2;

  // Jump out point if end of src is reached
  src_end_exit35:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestMono_SrcStereo_Src16_Volume_NoResample( // 42
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Apply volume
  sample_l *= scale_left;
  sample_r *= scale_right;

  // Merge left and right channels for mono dest
  sample_l += sample_r;

  // Merge sample data loop
  merge_loop42:

  // Load the first dest value
  dest_l = *dest;

  // Merge sample data into output buffer
  dest_l += sample_l;
  *(S32*)dest = dest_l;
  dest = (S32*)dest + 1;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit42;

  // Move the source pointer
  src = (U8*)src + 4;
  if (src >= src_end) goto src_end_exit42;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Apply volume
  sample_l *= scale_left;
  sample_r *= scale_right;

  // Merge left and right channels for mono dest
  sample_l += sample_r;

  // End loop
  goto      merge_loop42;

  // Jump out point if end of dest is reached
  dest_end_exit42:
  src = (U8*)src + 4;

  // Jump out point if end of src is reached
  src_end_exit42:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestStereo_SrcStereo_Src16_Volume_NoResample( // 43
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Apply volume
  sample_l *= scale_left;
  sample_r *= scale_right;

  // Merge sample data loop
  merge_loop43:

  // Load the first dest value
  dest_l = *dest;
  dest_r = *(dest + 1);

  // Merge sample data into output buffer
  dest_l += sample_l;
  dest_r += sample_r;
  *(S32*)dest = dest_l;
  *((S32*)dest + 1) = dest_r;
  dest = (S32*)dest + 2;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit43;

  // Move the source pointer
  src = (U8*)src + 4;
  if (src >= src_end) goto src_end_exit43;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Apply volume
  sample_l *= scale_left;
  sample_r *= scale_right;

  // End loop
  goto      merge_loop43;

  // Jump out point if end of dest is reached
  dest_end_exit43:
  src = (U8*)src + 4;

  // Jump out point if end of src is reached
  src_end_exit43:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestMono_SrcMono_Src16_Volume_Resample( // 50
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // Apply volume
  sample_l *= scale_left;

  // Merge sample data loop
  merge_loop50:

  // Load the first dest value
  dest_l = *dest;

  // Merge sample data into output buffer
  dest_l += sample_l;
  *(S32*)dest = dest_l;
  dest = (S32*)dest + 1;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit50;

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 1);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;
  if (src >= src_end) goto src_end_exit50;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // Apply volume
  sample_l *= scale_left;

  // End loop
  goto      merge_loop50;

  // Jump out point if end of dest is reached
  dest_end_exit50:

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 1);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;

  // Jump out point if end of src is reached
  src_end_exit50:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestStereo_SrcMono_Src16_Volume_Resample( // 51
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // Apply volume into each sample
  sample_r = sample_l * scale_right;
  sample_l = sample_l * scale_left;

  // Merge sample data loop
  merge_loop51:

  // Load the first dest value
  dest_l = *dest;
  dest_r = *(dest + 1);

  // Merge sample data into output buffer
  dest_l += sample_l;
  dest_r += sample_r;
  *(S32*)dest = dest_l;
  *((S32*)dest + 1) = dest_r;
  dest = (S32*)dest + 2;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit51;

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 1);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;
  if (src >= src_end) goto src_end_exit51;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // Apply volume into each sample
  sample_r = sample_l * scale_right;
  sample_l = sample_l * scale_left;

  // End loop
  goto      merge_loop51;

  // Jump out point if end of dest is reached
  dest_end_exit51:

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 1);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;

  // Jump out point if end of src is reached
  src_end_exit51:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestMono_SrcMono_Src16_Volume_DownFiltered( // 54
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
 cur_l1 = *(S32*)cur_l_ptr;
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);

  // check to see if we have to call the upsampling version
  if ((playback_ratio & 0xffff0000) == 0) goto Merge_DestMono_SrcMono_Src16_Volume_UpFiltered;

  // build average dividers
  divider_l = 0x10000;  // 0x100000000/2048/32
  divider_l *= scale_left;
  divider_l /= playback_ratio;
  divider_l--;

  // handle start up loop management
  r0temp = src_fract >> 30;
  src_fract &= 0x3fffffff;

  // load dest in case we jump right into the loop
  // Load the first dest value
  dest_l = *dest;
  if (r0temp >= 2) goto whole_continue54;
  if (r0temp == 1) goto last_continue54;

  // Merge sample data loop
  merge_loop54:

  // Load the first dest value
  dest_l = *dest;

  // weight the initial sample
  sample_l = cur_l1 * src_fract;
  src_fract += playback_ratio;
  sample_l >>= 16;
  cur_l1 -= sample_l;
  if (src_fract < 0x20000) goto skip_loop54;

  // loop to load all of the full sample points
  whole_loop54:
  if (src >= src_end) goto src_whole_exit54;
  whole_continue54:

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);
  cur_l1 += sample_l;
  src_fract -= 0x10000;
  src = (U8*)src + 2;
  if (src_fract >= 0x20000) goto whole_loop54;

  skip_loop54:
  src_fract &= 0xffff;
  last_continue54:
  if (src >= src_end) goto src_last_exit54;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);
  src = (U8*)src + 2;

  // weight the final sample
  r0temp = sample_l * src_fract;
  r0temp >>= 16;
  r0temp += cur_l1;
  cur_l1 = sample_l;
  sample_l = r0temp * divider_l;

  // Merge sample data into output buffer
  dest_l += sample_l;
  *(S32*)dest = dest_l;
  dest = (S32*)dest + 1;
  if ((void*)dest < (void*)dest_end) goto merge_loop54;

  // Jump out point if end of dest is reached
  dest_end_exit54:
  goto      src_save_value54;

  // jump out when src is exceed, but save our current loop position
  src_whole_exit54:
  src_fract |= 0x80000000;
  src_last_exit54:
  src_fract |= 0x40000000;
  src_save_value54:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;

  // restore upper variables

  // Routine end
  return;

Merge_DestMono_SrcMono_Src16_Volume_UpFiltered: // 566

  // there is no stack setup, because it has already been performed
  //   by the down filter version of this function

  // setup the resample stuff
  src_fract <<= 16;
  playback_ratio <<= 16;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;

  // Merge sample data loop
  merge_loop566:

  // Load the first dest value
  dest_l = *dest;

  // Upsample the data points
  r0temp = src_fract >> 17;
  sample_l = cur_l1 - cur_l2;
  sample_l *= r0temp;
  sample_l >>= 15;
  sample_l += cur_l2;

  // Apply volume
  sample_l *= scale_left;

  // Merge sample data into output buffer
  dest_l += sample_l;
  *(S32*)dest = dest_l;
  dest = (S32*)dest + 1;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit566;

  // Add to accumulator and advance the source correctly
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract > old )  goto merge_loop566;}

  // Move the source pointer
  src = (U8*)src + 2;
  if (src >= src_end) goto src_end_exit566;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;

  // End loop
  goto      merge_loop566;

  // Jump out point if end of dest is reached
  dest_end_exit566:
  src = (U8*)src + 2;
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract <= old ) goto skip_filter_adjust566;}

  // rotate filter values
  cur_l1 = cur_l2;

  // un-increment the source to skip the early source adjustment
  src = (U8*)src - 2;

  skip_filter_adjust566:

  // Jump out point if end of src is reached
  src_end_exit566:

  // adjust fractional
  src_fract >>= 16;

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestStereo_SrcMono_Src16_Volume_DownFiltered( // 55
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
 cur_l1 = *(S32*)cur_l_ptr;
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);

  // check to see if we have to call the upsampling version
  if ((playback_ratio & 0xffff0000) == 0) goto Merge_DestStereo_SrcMono_Src16_Volume_UpFiltered;

  // build average dividers
  divider_l = 0x10000;  // 0x100000000/2048/32
  divider_l *= scale_left;
  divider_l /= playback_ratio;
  divider_l--;
  divider_r = 0x10000;  // 0x100000000/2048/32
  divider_r *= scale_right;
  divider_r /= playback_ratio;
  divider_r--;

  // handle start up loop management
  r0temp = src_fract >> 30;
  src_fract &= 0x3fffffff;

  // load dest in case we jump right into the loop
  // Load the first dest value
  dest_l = *dest;
  dest_r = *(dest + 1);
  if (r0temp >= 2) goto whole_continue55;
  if (r0temp == 1) goto last_continue55;

  // Merge sample data loop
  merge_loop55:

  // Load the first dest value
  dest_l = *dest;
  dest_r = *(dest + 1);

  // weight the initial sample
  sample_l = cur_l1 * src_fract;
  src_fract += playback_ratio;
  sample_l >>= 16;
  cur_l1 -= sample_l;
  if (src_fract < 0x20000) goto skip_loop55;

  // loop to load all of the full sample points
  whole_loop55:
  if (src >= src_end) goto src_whole_exit55;
  whole_continue55:

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);
  cur_l1 += sample_l;
  src_fract -= 0x10000;
  src = (U8*)src + 2;
  if (src_fract >= 0x20000) goto whole_loop55;

  skip_loop55:
  src_fract &= 0xffff;
  last_continue55:
  if (src >= src_end) goto src_last_exit55;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);
  src = (U8*)src + 2;

  // weight the final sample
  r0temp = sample_l * src_fract;
  r0temp >>= 16;
  r0temp += cur_l1;
  cur_l1 = sample_l;

  // Duplicate the left channel into the right
  sample_r = r0temp * divider_r;
  sample_l = r0temp * divider_l;

  // Merge sample data into output buffer
  dest_l += sample_l;
  dest_r += sample_r;
  *(S32*)dest = dest_l;
  *((S32*)dest + 1) = dest_r;
  dest = (S32*)dest + 2;
  if ((void*)dest < (void*)dest_end) goto merge_loop55;

  // Jump out point if end of dest is reached
  dest_end_exit55:
  goto      src_save_value55;

  // jump out when src is exceed, but save our current loop position
  src_whole_exit55:
  src_fract |= 0x80000000;
  src_last_exit55:
  src_fract |= 0x40000000;
  src_save_value55:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;
  *(S32*)cur_r_ptr = cur_r1;

  // restore upper variables

  // Routine end
  return;

Merge_DestStereo_SrcMono_Src16_Volume_UpFiltered: // 567

  // there is no stack setup, because it has already been performed
  //   by the down filter version of this function

  // setup the resample stuff
  src_fract <<= 16;
  playback_ratio <<= 16;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;

  // Merge sample data loop
  merge_loop567:

  // Load the first dest value
  dest_l = *dest;
  dest_r = *(dest + 1);

  // Upsample the data points
  r0temp = src_fract >> 17;
  sample_l = cur_l1 - cur_l2;
  sample_l *= r0temp;
  sample_l >>= 15;
  sample_l += cur_l2;

  // Apply volume into each sample
  sample_r = sample_l * scale_right;
  sample_l = sample_l * scale_left;

  // Merge sample data into output buffer
  dest_l += sample_l;
  dest_r += sample_r;
  *(S32*)dest = dest_l;
  *((S32*)dest + 1) = dest_r;
  dest = (S32*)dest + 2;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit567;

  // Add to accumulator and advance the source correctly
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract > old )  goto merge_loop567;}

  // Move the source pointer
  src = (U8*)src + 2;
  if (src >= src_end) goto src_end_exit567;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;

  // End loop
  goto      merge_loop567;

  // Jump out point if end of dest is reached
  dest_end_exit567:
  src = (U8*)src + 2;
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract <= old ) goto skip_filter_adjust567;}

  // rotate filter values
  cur_l1 = cur_l2;

  // un-increment the source to skip the early source adjustment
  src = (U8*)src - 2;

  skip_filter_adjust567:

  // Jump out point if end of src is reached
  src_end_exit567:

  // adjust fractional
  src_fract >>= 16;

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;
  *(S32*)cur_r_ptr = cur_r1;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestMono_SrcStereo_Src16_Volume_Resample( // 58
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Apply volume
  sample_l *= scale_left;
  sample_r *= scale_right;

  // Merge left and right channels for mono dest
  sample_l += sample_r;

  // Merge sample data loop
  merge_loop58:

  // Load the first dest value
  dest_l = *dest;

  // Merge sample data into output buffer
  dest_l += sample_l;
  *(S32*)dest = dest_l;
  dest = (S32*)dest + 1;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit58;

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 2);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;
  if (src >= src_end) goto src_end_exit58;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Apply volume
  sample_l *= scale_left;
  sample_r *= scale_right;

  // Merge left and right channels for mono dest
  sample_l += sample_r;

  // End loop
  goto      merge_loop58;

  // Jump out point if end of dest is reached
  dest_end_exit58:

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 2);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;

  // Jump out point if end of src is reached
  src_end_exit58:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestStereo_SrcStereo_Src16_Volume_Resample( // 59
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Apply volume
  sample_l *= scale_left;
  sample_r *= scale_right;

  // Merge sample data loop
  merge_loop59:

  // Load the first dest value
  dest_l = *dest;
  dest_r = *(dest + 1);

  // Merge sample data into output buffer
  dest_l += sample_l;
  dest_r += sample_r;
  *(S32*)dest = dest_l;
  *((S32*)dest + 1) = dest_r;
  dest = (S32*)dest + 2;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit59;

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 2);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;
  if (src >= src_end) goto src_end_exit59;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Apply volume
  sample_l *= scale_left;
  sample_r *= scale_right;

  // End loop
  goto      merge_loop59;

  // Jump out point if end of dest is reached
  dest_end_exit59:

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 2);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;

  // Jump out point if end of src is reached
  src_end_exit59:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestMono_SrcStereo_Src16_Volume_DownFiltered( // 62
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
 cur_l1 = *(S32*)cur_l_ptr;
 cur_r1 = *(S32*)cur_r_ptr;
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);

  // check to see if we have to call the upsampling version
  if ((playback_ratio & 0xffff0000) == 0) goto Merge_DestMono_SrcStereo_Src16_Volume_UpFiltered;

  // build average dividers
  divider_l = 0x10000;  // 0x100000000/2048/32
  divider_l *= scale_left;
  divider_l /= playback_ratio;
  divider_l--;
  divider_r = 0x10000;  // 0x100000000/2048/32
  divider_r *= scale_right;
  divider_r /= playback_ratio;
  divider_r--;

  // handle start up loop management
  r0temp = src_fract >> 30;
  src_fract &= 0x3fffffff;

  // load dest in case we jump right into the loop
  // Load the first dest value
  dest_l = *dest;
  if (r0temp >= 2) goto whole_continue62;
  if (r0temp == 1) goto last_continue62;

  // Merge sample data loop
  merge_loop62:

  // Load the first dest value
  dest_l = *dest;

  // weight the initial sample
  sample_l = cur_l1 * src_fract;
  sample_r = cur_r1 * src_fract;
  src_fract += playback_ratio;
  sample_l >>= 16;
  sample_r >>= 16;
  cur_l1 -= sample_l;
  cur_r1 -= sample_r;
  if (src_fract < 0x20000) goto skip_loop62;

  // loop to load all of the full sample points
  whole_loop62:
  if (src >= src_end) goto src_whole_exit62;
  whole_continue62:

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;
  cur_l1 += sample_l;
  cur_r1 += sample_r;
  src_fract -= 0x10000;
  src = (U8*)src + 4;
  if (src_fract >= 0x20000) goto whole_loop62;

  skip_loop62:
  src_fract &= 0xffff;
  if (src >= src_end) goto src_last_exit62;
  last_continue62:

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;
  src = (U8*)src + 4;

  // weight the final sample
  r0temp = sample_l * src_fract;
  r0temp >>= 16;
  r0temp += cur_l1;
  cur_l1 = sample_l;
  #define temp1 sample_l
  temp1 = sample_r * src_fract;
  temp1 >>= 16;
  temp1 += cur_r1;
  cur_r1 = sample_r;
  sample_r = temp1 * divider_r;
  sample_l = r0temp * divider_l;
  #undef temp1

  // Merge left and right channels for mono dest
  sample_l += sample_r;

  // Merge sample data into output buffer
  dest_l += sample_l;
  *(S32*)dest = dest_l;
  dest = (S32*)dest + 1;
  if ((void*)dest < (void*)dest_end) goto merge_loop62;

  // Jump out point if end of dest is reached
  dest_end_exit62:
  goto      src_save_value62;

  // jump out when src is exceed, but save our current loop position
  src_whole_exit62:
  src_fract |= 0x80000000;
  src_last_exit62:
  src_fract |= 0x40000000;
  src_save_value62:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;
  *(S32*)cur_r_ptr = cur_r1;

  // restore upper variables

  // Routine end
  return;

Merge_DestMono_SrcStereo_Src16_Volume_UpFiltered: // 574

  // there is no stack setup, because it has already been performed
  //   by the down filter version of this function

  // setup the resample stuff
  src_fract <<= 16;
  playback_ratio <<= 16;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;
  cur_r2 = cur_r1;
  cur_r1 = sample_r;

  // Merge sample data loop
  merge_loop574:

  // Load the first dest value
  dest_l = *dest;

  // Upsample the data points
  r0temp = src_fract >> 17;
  sample_l = cur_l1 - cur_l2;
  sample_r = cur_r1 - cur_r2;
  sample_l *= r0temp;
  sample_r *= r0temp;
  sample_l >>= 15;
  sample_r >>= 15;
  sample_l += cur_l2;
  sample_r += cur_r2;

  // Apply volume
  sample_l *= scale_left;
  sample_r *= scale_right;

  // Merge left and right channels for mono dest
  sample_l += sample_r;

  // Merge sample data into output buffer
  dest_l += sample_l;
  *(S32*)dest = dest_l;
  dest = (S32*)dest + 1;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit574;

  // Add to accumulator and advance the source correctly
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract > old )  goto merge_loop574;}

  // Move the source pointer
  src = (U8*)src + 4;
  if (src >= src_end) goto src_end_exit574;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;
  cur_r2 = cur_r1;
  cur_r1 = sample_r;

  // End loop
  goto      merge_loop574;

  // Jump out point if end of dest is reached
  dest_end_exit574:
  src = (U8*)src + 4;
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract <= old ) goto skip_filter_adjust574;}

  // rotate filter values
  cur_l1 = cur_l2;
  cur_r1 = cur_r2;

  // un-increment the source to skip the early source adjustment
  src = (U8*)src - 4;

  skip_filter_adjust574:

  // Jump out point if end of src is reached
  src_end_exit574:

  // adjust fractional
  src_fract >>= 16;

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;
  *(S32*)cur_r_ptr = cur_r1;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestStereo_SrcStereo_Src16_Volume_DownFiltered( // 63
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
 cur_l1 = *(S32*)cur_l_ptr;
 cur_r1 = *(S32*)cur_r_ptr;
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);

  // check to see if we have to call the upsampling version
  if ((playback_ratio & 0xffff0000) == 0) goto Merge_DestStereo_SrcStereo_Src16_Volume_UpFiltered;

  // build average dividers
  divider_l = 0x10000;  // 0x100000000/2048/32
  divider_l *= scale_left;
  divider_l /= playback_ratio;
  divider_l--;
  divider_r = 0x10000;  // 0x100000000/2048/32
  divider_r *= scale_right;
  divider_r /= playback_ratio;
  divider_r--;

  // handle start up loop management
  r0temp = src_fract >> 30;
  src_fract &= 0x3fffffff;

  // load dest in case we jump right into the loop
  // Load the first dest value
  dest_l = *dest;
  dest_r = *(dest + 1);
  if (r0temp >= 2) goto whole_continue63;
  if (r0temp == 1) goto last_continue63;

  // Merge sample data loop
  merge_loop63:

  // Load the first dest value
  dest_l = *dest;
  dest_r = *(dest + 1);

  // weight the initial sample
  sample_l = cur_l1 * src_fract;
  sample_r = cur_r1 * src_fract;
  src_fract += playback_ratio;
  sample_l >>= 16;
  sample_r >>= 16;
  cur_l1 -= sample_l;
  cur_r1 -= sample_r;
  if (src_fract < 0x20000) goto skip_loop63;

  // loop to load all of the full sample points
  whole_loop63:
  if (src >= src_end) goto src_whole_exit63;
  whole_continue63:

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;
  cur_l1 += sample_l;
  cur_r1 += sample_r;
  src_fract -= 0x10000;
  src = (U8*)src + 4;
  if (src_fract >= 0x20000) goto whole_loop63;

  skip_loop63:
  src_fract &= 0xffff;
  if (src >= src_end) goto src_last_exit63;
  last_continue63:

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;
  src = (U8*)src + 4;

  // weight the final sample
  r0temp = sample_l * src_fract;
  r0temp >>= 16;
  r0temp += cur_l1;
  cur_l1 = sample_l;
  #define temp1 sample_l
  temp1 = sample_r * src_fract;
  temp1 >>= 16;
  temp1 += cur_r1;
  cur_r1 = sample_r;
  sample_r = temp1 * divider_r;
  sample_l = r0temp * divider_l;
  #undef temp1

  // Merge sample data into output buffer
  dest_l += sample_l;
  dest_r += sample_r;
  *(S32*)dest = dest_l;
  *((S32*)dest + 1) = dest_r;
  dest = (S32*)dest + 2;
  if ((void*)dest < (void*)dest_end) goto merge_loop63;

  // Jump out point if end of dest is reached
  dest_end_exit63:
  goto      src_save_value63;

  // jump out when src is exceed, but save our current loop position
  src_whole_exit63:
  src_fract |= 0x80000000;
  src_last_exit63:
  src_fract |= 0x40000000;
  src_save_value63:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;
  *(S32*)cur_r_ptr = cur_r1;

  // restore upper variables

  // Routine end
  return;

Merge_DestStereo_SrcStereo_Src16_Volume_UpFiltered: // 575

  // there is no stack setup, because it has already been performed
  //   by the down filter version of this function

  // setup the resample stuff
  src_fract <<= 16;
  playback_ratio <<= 16;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;
  cur_r2 = cur_r1;
  cur_r1 = sample_r;

  // Merge sample data loop
  merge_loop575:

  // Load the first dest value
  dest_l = *dest;
  dest_r = *(dest + 1);

  // Upsample the data points
  r0temp = src_fract >> 17;
  sample_l = cur_l1 - cur_l2;
  sample_r = cur_r1 - cur_r2;
  sample_l *= r0temp;
  sample_r *= r0temp;
  sample_l >>= 15;
  sample_r >>= 15;
  sample_l += cur_l2;
  sample_r += cur_r2;

  // Apply volume
  sample_l *= scale_left;
  sample_r *= scale_right;

  // Merge sample data into output buffer
  dest_l += sample_l;
  dest_r += sample_r;
  *(S32*)dest = dest_l;
  *((S32*)dest + 1) = dest_r;
  dest = (S32*)dest + 2;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit575;

  // Add to accumulator and advance the source correctly
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract > old )  goto merge_loop575;}

  // Move the source pointer
  src = (U8*)src + 4;
  if (src >= src_end) goto src_end_exit575;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;
  cur_r2 = cur_r1;
  cur_r1 = sample_r;

  // End loop
  goto      merge_loop575;

  // Jump out point if end of dest is reached
  dest_end_exit575:
  src = (U8*)src + 4;
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract <= old ) goto skip_filter_adjust575;}

  // rotate filter values
  cur_l1 = cur_l2;
  cur_r1 = cur_r2;

  // un-increment the source to skip the early source adjustment
  src = (U8*)src - 4;

  skip_filter_adjust575:

  // Jump out point if end of src is reached
  src_end_exit575:

  // adjust fractional
  src_fract >>= 16;

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;
  *(S32*)cur_r_ptr = cur_r1;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestMono_SrcMono_Src16_Copy16_NoResample( // 66
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  dest_l = 2; // dest_l holds the advance value of 2 when using copy mode 

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // Merge sample data loop
  merge_loop66:


  // Copy sample data into output buffer
  STORE_LE_SWAP16_OFS(dest, sample_l, 0);
  dest = (S32*)((U8*)dest + 2);
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit66;

  // Move the source pointer
  src = (U8*)src + 2;
  if (src >= src_end) goto src_end_exit66;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // End loop
  goto      merge_loop66;

  // Jump out point if end of dest is reached
  dest_end_exit66:
  src = (U8*)src + 2;

  // Jump out point if end of src is reached
  src_end_exit66:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestStereo_SrcMono_Src16_Copy16_NoResample( // 67
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  dest_l = 2; // dest_l holds the advance value of 2 when using copy mode 

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // Merge sample data loop
  merge_loop67:


  // Copy sample data into output buffer
  STORE_LE_SWAP16_OFS(dest, sample_l, 0);
  STORE_LE_SWAP16_OFS(dest, sample_l, 2);
  dest = (S32*)dest + 1;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit67;

  // Move the source pointer
  src = (U8*)src + 2;
  if (src >= src_end) goto src_end_exit67;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // End loop
  goto      merge_loop67;

  // Jump out point if end of dest is reached
  dest_end_exit67:
  src = (U8*)src + 2;

  // Jump out point if end of src is reached
  src_end_exit67:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestMono_SrcStereo_Src16_Copy16_NoResample( // 74
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  dest_l = 2; // dest_l holds the advance value of 2 when using copy mode 

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Merge left and right channels for mono dest
  sample_l += sample_r;

  // Shift right to avoid clipping on direct copies
  sample_l >>= 1;

  // Merge sample data loop
  merge_loop74:


  // Copy sample data into output buffer
  STORE_LE_SWAP16_OFS(dest, sample_l, 0);
  dest = (S32*)((U8*)dest + 2);
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit74;

  // Move the source pointer
  src = (U8*)src + 4;
  if (src >= src_end) goto src_end_exit74;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Merge left and right channels for mono dest
  sample_l += sample_r;

  // Shift right to avoid clipping on direct copies
  sample_l >>= 1;

  // End loop
  goto      merge_loop74;

  // Jump out point if end of dest is reached
  dest_end_exit74:
  src = (U8*)src + 4;

  // Jump out point if end of src is reached
  src_end_exit74:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestStereo_SrcStereo_Src16_Copy16_NoResample( // 75
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  dest_l = 2; // dest_l holds the advance value of 2 when using copy mode 

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Merge sample data loop
  merge_loop75:


  // Copy sample data into output buffer
  STORE_LE_SWAP16_OFS(dest, sample_l, 0);
  STORE_LE_SWAP16_OFS(dest, sample_r, 2);
  dest = (S32*)dest + 1;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit75;

  // Move the source pointer
  src = (U8*)src + 4;
  if (src >= src_end) goto src_end_exit75;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // End loop
  goto      merge_loop75;

  // Jump out point if end of dest is reached
  dest_end_exit75:
  src = (U8*)src + 4;

  // Jump out point if end of src is reached
  src_end_exit75:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestMono_SrcMono_Src16_Copy16_Resample( // 82
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);
  dest_l = 2; // dest_l holds the advance value of 2 when using copy mode 

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // Merge sample data loop
  merge_loop82:


  // Copy sample data into output buffer
  STORE_LE_SWAP16_OFS(dest, sample_l, 0);
  dest = (S32*)((U8*)dest + 2);
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit82;

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 1);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;
  if (src >= src_end) goto src_end_exit82;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // End loop
  goto      merge_loop82;

  // Jump out point if end of dest is reached
  dest_end_exit82:

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 1);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;

  // Jump out point if end of src is reached
  src_end_exit82:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestStereo_SrcMono_Src16_Copy16_Resample( // 83
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);
  dest_l = 2; // dest_l holds the advance value of 2 when using copy mode 

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // Merge sample data loop
  merge_loop83:


  // Copy sample data into output buffer
  STORE_LE_SWAP16_OFS(dest, sample_l, 0);
  STORE_LE_SWAP16_OFS(dest, sample_l, 2);
  dest = (S32*)dest + 1;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit83;

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 1);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;
  if (src >= src_end) goto src_end_exit83;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // End loop
  goto      merge_loop83;

  // Jump out point if end of dest is reached
  dest_end_exit83:

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 1);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;

  // Jump out point if end of src is reached
  src_end_exit83:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestMono_SrcMono_Src16_Copy16_DownFiltered( // 86
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
 cur_l1 = *(S32*)cur_l_ptr;
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);
  dest_l = 2; // dest_l holds the advance value of 2 when using copy mode 

  // check to see if we have to call the upsampling version
  if ((playback_ratio & 0xffff0000) == 0) goto Merge_DestMono_SrcMono_Src16_Copy16_UpFiltered;

  // build average dividers
  divider_l = 0x8000000;  // 0x100000000/32
  divider_l /= playback_ratio;
  divider_l--;

  // handle start up loop management
  r0temp = src_fract >> 30;
  src_fract &= 0x3fffffff;

  // load dest in case we jump right into the loop
  if (r0temp >= 2) goto whole_continue86;
  if (r0temp == 1) goto last_continue86;

  // Merge sample data loop
  merge_loop86:


  // weight the initial sample
  sample_l = cur_l1 * src_fract;
  src_fract += playback_ratio;
  sample_l >>= 16;
  cur_l1 -= sample_l;
  if (src_fract < 0x20000) goto skip_loop86;

  // loop to load all of the full sample points
  whole_loop86:
  if (src >= src_end) goto src_whole_exit86;
  whole_continue86:

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);
  cur_l1 += sample_l;
  src_fract -= 0x10000;
  src = (U8*)src + 2;
  if (src_fract >= 0x20000) goto whole_loop86;

  skip_loop86:
  src_fract &= 0xffff;
  last_continue86:
  if (src >= src_end) goto src_last_exit86;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);
  src = (U8*)src + 2;

  // weight the final sample
  r0temp = sample_l * src_fract;
  r0temp >>= 16;
  r0temp += cur_l1;
  cur_l1 = sample_l;
  sample_l = r0temp * divider_l;

  // Scale back down after divide
  sample_l >>= 11;

  // Copy sample data into output buffer
  STORE_LE_SWAP16_OFS(dest, sample_l, 0);
  dest = (S32*)((U8*)dest + 2);
  if ((void*)dest < (void*)dest_end) goto merge_loop86;

  // Jump out point if end of dest is reached
  dest_end_exit86:
  goto      src_save_value86;

  // jump out when src is exceed, but save our current loop position
  src_whole_exit86:
  src_fract |= 0x80000000;
  src_last_exit86:
  src_fract |= 0x40000000;
  src_save_value86:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;

  // restore upper variables

  // Routine end
  return;

Merge_DestMono_SrcMono_Src16_Copy16_UpFiltered: // 598

  // there is no stack setup, because it has already been performed
  //   by the down filter version of this function

  // setup the resample stuff
  src_fract <<= 16;
  playback_ratio <<= 16;
  dest_l = 2; // dest_l holds the advance value of 2 when using copy mode 

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;

  // Merge sample data loop
  merge_loop598:


  // Upsample the data points
  r0temp = src_fract >> 17;
  sample_l = cur_l1 - cur_l2;
  sample_l *= r0temp;
  sample_l >>= 15;
  sample_l += cur_l2;

  // Copy sample data into output buffer
  STORE_LE_SWAP16_OFS(dest, sample_l, 0);
  dest = (S32*)((U8*)dest + 2);
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit598;

  // Add to accumulator and advance the source correctly
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract > old )  goto merge_loop598;}

  // Move the source pointer
  src = (U8*)src + 2;
  if (src >= src_end) goto src_end_exit598;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;

  // End loop
  goto      merge_loop598;

  // Jump out point if end of dest is reached
  dest_end_exit598:
  src = (U8*)src + 2;
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract <= old ) goto skip_filter_adjust598;}

  // rotate filter values
  cur_l1 = cur_l2;

  // un-increment the source to skip the early source adjustment
  src = (U8*)src - 2;

  skip_filter_adjust598:

  // Jump out point if end of src is reached
  src_end_exit598:

  // adjust fractional
  src_fract >>= 16;

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestStereo_SrcMono_Src16_Copy16_DownFiltered( // 87
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
 cur_l1 = *(S32*)cur_l_ptr;
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);
  dest_l = 2; // dest_l holds the advance value of 2 when using copy mode 

  // check to see if we have to call the upsampling version
  if ((playback_ratio & 0xffff0000) == 0) goto Merge_DestStereo_SrcMono_Src16_Copy16_UpFiltered;

  // build average dividers
  divider_l = 0x8000000;  // 0x100000000/32
  divider_l /= playback_ratio;
  divider_l--;

  // handle start up loop management
  r0temp = src_fract >> 30;
  src_fract &= 0x3fffffff;

  // load dest in case we jump right into the loop
  if (r0temp >= 2) goto whole_continue87;
  if (r0temp == 1) goto last_continue87;

  // Merge sample data loop
  merge_loop87:


  // weight the initial sample
  sample_l = cur_l1 * src_fract;
  src_fract += playback_ratio;
  sample_l >>= 16;
  cur_l1 -= sample_l;
  if (src_fract < 0x20000) goto skip_loop87;

  // loop to load all of the full sample points
  whole_loop87:
  if (src >= src_end) goto src_whole_exit87;
  whole_continue87:

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);
  cur_l1 += sample_l;
  src_fract -= 0x10000;
  src = (U8*)src + 2;
  if (src_fract >= 0x20000) goto whole_loop87;

  skip_loop87:
  src_fract &= 0xffff;
  last_continue87:
  if (src >= src_end) goto src_last_exit87;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);
  src = (U8*)src + 2;

  // weight the final sample
  r0temp = sample_l * src_fract;
  r0temp >>= 16;
  r0temp += cur_l1;
  cur_l1 = sample_l;
  sample_l = r0temp * divider_l;

  // Scale back down after divide
  sample_l >>= 11;

  // Copy sample data into output buffer
  STORE_LE_SWAP16_OFS(dest, sample_l, 0);
  STORE_LE_SWAP16_OFS(dest, sample_l, 2);
  dest = (S32*)dest + 1;
  if ((void*)dest < (void*)dest_end) goto merge_loop87;

  // Jump out point if end of dest is reached
  dest_end_exit87:
  goto      src_save_value87;

  // jump out when src is exceed, but save our current loop position
  src_whole_exit87:
  src_fract |= 0x80000000;
  src_last_exit87:
  src_fract |= 0x40000000;
  src_save_value87:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;

  // restore upper variables

  // Routine end
  return;

Merge_DestStereo_SrcMono_Src16_Copy16_UpFiltered: // 599

  // there is no stack setup, because it has already been performed
  //   by the down filter version of this function

  // setup the resample stuff
  src_fract <<= 16;
  playback_ratio <<= 16;
  dest_l = 2; // dest_l holds the advance value of 2 when using copy mode 

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;

  // Merge sample data loop
  merge_loop599:


  // Upsample the data points
  r0temp = src_fract >> 17;
  sample_l = cur_l1 - cur_l2;
  sample_l *= r0temp;
  sample_l >>= 15;
  sample_l += cur_l2;

  // Copy sample data into output buffer
  STORE_LE_SWAP16_OFS(dest, sample_l, 0);
  STORE_LE_SWAP16_OFS(dest, sample_l, 2);
  dest = (S32*)dest + 1;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit599;

  // Add to accumulator and advance the source correctly
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract > old )  goto merge_loop599;}

  // Move the source pointer
  src = (U8*)src + 2;
  if (src >= src_end) goto src_end_exit599;

  // Load sample data
  sample_l = (S32)(S16)LE_SWAP16(src);

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;

  // End loop
  goto      merge_loop599;

  // Jump out point if end of dest is reached
  dest_end_exit599:
  src = (U8*)src + 2;
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract <= old ) goto skip_filter_adjust599;}

  // rotate filter values
  cur_l1 = cur_l2;

  // un-increment the source to skip the early source adjustment
  src = (U8*)src - 2;

  skip_filter_adjust599:

  // Jump out point if end of src is reached
  src_end_exit599:

  // adjust fractional
  src_fract >>= 16;

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestMono_SrcStereo_Src16_Copy16_Resample( // 90
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);
  dest_l = 2; // dest_l holds the advance value of 2 when using copy mode 

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Merge left and right channels for mono dest
  sample_l += sample_r;

  // Shift right to avoid clipping on direct copies
  sample_l >>= 1;

  // Merge sample data loop
  merge_loop90:


  // Copy sample data into output buffer
  STORE_LE_SWAP16_OFS(dest, sample_l, 0);
  dest = (S32*)((U8*)dest + 2);
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit90;

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 2);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;
  if (src >= src_end) goto src_end_exit90;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Merge left and right channels for mono dest
  sample_l += sample_r;

  // Shift right to avoid clipping on direct copies
  sample_l >>= 1;

  // End loop
  goto      merge_loop90;

  // Jump out point if end of dest is reached
  dest_end_exit90:

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 2);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;

  // Jump out point if end of src is reached
  src_end_exit90:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestStereo_SrcStereo_Src16_Copy16_Resample( // 91
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);
  dest_l = 2; // dest_l holds the advance value of 2 when using copy mode 

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Merge sample data loop
  merge_loop91:


  // Copy sample data into output buffer
  STORE_LE_SWAP16_OFS(dest, sample_l, 0);
  STORE_LE_SWAP16_OFS(dest, sample_r, 2);
  dest = (S32*)dest + 1;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit91;

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 2);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;
  if (src >= src_end) goto src_end_exit91;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // End loop
  goto      merge_loop91;

  // Jump out point if end of dest is reached
  dest_end_exit91:

  // Add to accumulator and advance the source correctly
  src_fract += playback_ratio;
  r0temp = src_fract >> (16 - 2);
  src_fract &= 0xffff;
  src = (U8*)src + r0temp;

  // Jump out point if end of src is reached
  src_end_exit91:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestMono_SrcStereo_Src16_Copy16_DownFiltered( // 94
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
 cur_l1 = *(S32*)cur_l_ptr;
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);
  dest_l = 2; // dest_l holds the advance value of 2 when using copy mode 

  // check to see if we have to call the upsampling version
  if ((playback_ratio & 0xffff0000) == 0) goto Merge_DestMono_SrcStereo_Src16_Copy16_UpFiltered;

  // build average dividers
  divider_l = 0x8000000;  // 0x100000000/32
  divider_l /= playback_ratio;
  divider_l--;

  // handle start up loop management
  r0temp = src_fract >> 30;
  src_fract &= 0x3fffffff;

  // load dest in case we jump right into the loop
  if (r0temp >= 2) goto whole_continue94;
  if (r0temp == 1) goto last_continue94;

  // Merge sample data loop
  merge_loop94:


  // weight the initial sample
  sample_l = cur_l1 * src_fract;
  src_fract += playback_ratio;
  sample_l >>= 16;
  cur_l1 -= sample_l;
  if (src_fract < 0x20000) goto skip_loop94;

  // loop to load all of the full sample points
  whole_loop94:
  if (src >= src_end) goto src_whole_exit94;
  whole_continue94:

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Merge left and right channels for mono dest
  sample_l += sample_r;

  // Shift right to avoid clipping on direct copies
  sample_l >>= 1;
  cur_l1 += sample_l;
  src_fract -= 0x10000;
  src = (U8*)src + 4;
  if (src_fract >= 0x20000) goto whole_loop94;

  skip_loop94:
  src_fract &= 0xffff;
  last_continue94:
  if (src >= src_end) goto src_last_exit94;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Merge left and right channels for mono dest
  sample_l += sample_r;

  // Shift right to avoid clipping on direct copies
  sample_l >>= 1;
  src = (U8*)src + 4;

  // weight the final sample
  r0temp = sample_l * src_fract;
  r0temp >>= 16;
  r0temp += cur_l1;
  cur_l1 = sample_l;
  sample_l = r0temp * divider_l;

  // Scale back down after divide
  sample_l >>= 11;

  // Copy sample data into output buffer
  STORE_LE_SWAP16_OFS(dest, sample_l, 0);
  dest = (S32*)((U8*)dest + 2);
  if ((void*)dest < (void*)dest_end) goto merge_loop94;

  // Jump out point if end of dest is reached
  dest_end_exit94:
  goto      src_save_value94;

  // jump out when src is exceed, but save our current loop position
  src_whole_exit94:
  src_fract |= 0x80000000;
  src_last_exit94:
  src_fract |= 0x40000000;
  src_save_value94:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;
  *(S32*)cur_r_ptr = cur_r1;

  // restore upper variables

  // Routine end
  return;

Merge_DestMono_SrcStereo_Src16_Copy16_UpFiltered: // 606

  // there is no stack setup, because it has already been performed
  //   by the down filter version of this function

  // setup the resample stuff
  src_fract <<= 16;
  playback_ratio <<= 16;
  dest_l = 2; // dest_l holds the advance value of 2 when using copy mode 

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Merge left and right channels for mono dest
  sample_l += sample_r;

  // Shift right to avoid clipping on direct copies
  sample_l >>= 1;

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;

  // Merge sample data loop
  merge_loop606:


  // Upsample the data points
  r0temp = src_fract >> 17;
  sample_l = cur_l1 - cur_l2;
  sample_l *= r0temp;
  sample_l >>= 15;
  sample_l += cur_l2;

  // Copy sample data into output buffer
  STORE_LE_SWAP16_OFS(dest, sample_l, 0);
  dest = (S32*)((U8*)dest + 2);
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit606;

  // Add to accumulator and advance the source correctly
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract > old )  goto merge_loop606;}

  // Move the source pointer
  src = (U8*)src + 4;
  if (src >= src_end) goto src_end_exit606;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // Merge left and right channels for mono dest
  sample_l += sample_r;

  // Shift right to avoid clipping on direct copies
  sample_l >>= 1;

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;

  // End loop
  goto      merge_loop606;

  // Jump out point if end of dest is reached
  dest_end_exit606:
  src = (U8*)src + 4;
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract <= old ) goto skip_filter_adjust606;}

  // rotate filter values
  cur_l1 = cur_l2;

  // un-increment the source to skip the early source adjustment
  src = (U8*)src - 4;

  skip_filter_adjust606:

  // Jump out point if end of src is reached
  src_end_exit606:

  // adjust fractional
  src_fract >>= 16;

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;
  *(S32*)cur_r_ptr = cur_r1;

  // restore upper variables

  // Routine end
  return;

}


static void AILCALL Merge_DestStereo_SrcStereo_Src16_Copy16_DownFiltered( // 95
  register S32  * * destp,
  register void  * dest_end,
  register void const * * srcp,
  register U32  * src_fractp,
  register void const * src_end,
  register S32  playback_ratio,
  register S32  scale_left,
  register S32  scale_right,
  S32  * cur_l_ptr,
  S32  * cur_r_ptr
)
{
  S32* dest=0;
  void* src=0;
  U32 src_fract=0;
  S32 sample_l=0;
  S32 dest_l=0;
  S32 sample_r=0;
  S32 dest_r=0;
  S32 divider_l=0;
  S32 cur_l1=0;
  S32 cur_r1=0;
  S32 divider_r=0;
  S32 cur_l2=0;
  S32 cur_r2=0;
  S32 r0temp=0;

  // load the initial register values
 cur_l1 = *(S32*)cur_l_ptr;
 cur_r1 = *(S32*)cur_r_ptr;
  dest = *(S32**)(destp);
  src = *(void**)(srcp);
  src_fract = *(U32*)(src_fractp);
  dest_l = 2; // dest_l holds the advance value of 2 when using copy mode 

  // check to see if we have to call the upsampling version
  if ((playback_ratio & 0xffff0000) == 0) goto Merge_DestStereo_SrcStereo_Src16_Copy16_UpFiltered;

  // build average dividers
  divider_l = 0x8000000;  // 0x100000000/32
  divider_l /= playback_ratio;
  divider_l--;
  divider_r = 0x8000000;  // 0x100000000/32
  divider_r /= playback_ratio;
  divider_r--;

  // handle start up loop management
  r0temp = src_fract >> 30;
  src_fract &= 0x3fffffff;

  // load dest in case we jump right into the loop
  if (r0temp >= 2) goto whole_continue95;
  if (r0temp == 1) goto last_continue95;

  // Merge sample data loop
  merge_loop95:


  // weight the initial sample
  sample_l = cur_l1 * src_fract;
  sample_r = cur_r1 * src_fract;
  src_fract += playback_ratio;
  sample_l >>= 16;
  sample_r >>= 16;
  cur_l1 -= sample_l;
  cur_r1 -= sample_r;
  if (src_fract < 0x20000) goto skip_loop95;

  // loop to load all of the full sample points
  whole_loop95:
  if (src >= src_end) goto src_whole_exit95;
  whole_continue95:

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;
  cur_l1 += sample_l;
  cur_r1 += sample_r;
  src_fract -= 0x10000;
  src = (U8*)src + 4;
  if (src_fract >= 0x20000) goto whole_loop95;

  skip_loop95:
  src_fract &= 0xffff;
  if (src >= src_end) goto src_last_exit95;
  last_continue95:

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;
  src = (U8*)src + 4;

  // weight the final sample
  r0temp = sample_l * src_fract;
  r0temp >>= 16;
  r0temp += cur_l1;
  cur_l1 = sample_l;
  #define temp1 sample_l
  temp1 = sample_r * src_fract;
  temp1 >>= 16;
  temp1 += cur_r1;
  cur_r1 = sample_r;
  sample_r = temp1 * divider_r;
  sample_l = r0temp * divider_l;
  #undef temp1

  // Scale back down after divide
  sample_l >>= 11;
  sample_r >>= 11;

  // Copy sample data into output buffer
  STORE_LE_SWAP16_OFS(dest, sample_l, 0);
  STORE_LE_SWAP16_OFS(dest, sample_r, 2);
  dest = (S32*)dest + 1;
  if ((void*)dest < (void*)dest_end) goto merge_loop95;

  // Jump out point if end of dest is reached
  dest_end_exit95:
  goto      src_save_value95;

  // jump out when src is exceed, but save our current loop position
  src_whole_exit95:
  src_fract |= 0x80000000;
  src_last_exit95:
  src_fract |= 0x40000000;
  src_save_value95:

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;
  *(S32*)cur_r_ptr = cur_r1;

  // restore upper variables

  // Routine end
  return;

Merge_DestStereo_SrcStereo_Src16_Copy16_UpFiltered: // 607

  // there is no stack setup, because it has already been performed
  //   by the down filter version of this function

  // setup the resample stuff
  src_fract <<= 16;
  playback_ratio <<= 16;
  dest_l = 2; // dest_l holds the advance value of 2 when using copy mode 

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;
  cur_r2 = cur_r1;
  cur_r1 = sample_r;

  // Merge sample data loop
  merge_loop607:


  // Upsample the data points
  r0temp = src_fract >> 17;
  sample_l = cur_l1 - cur_l2;
  sample_r = cur_r1 - cur_r2;
  sample_l *= r0temp;
  sample_r *= r0temp;
  sample_l >>= 15;
  sample_r >>= 15;
  sample_l += cur_l2;
  sample_r += cur_r2;

  // Copy sample data into output buffer
  STORE_LE_SWAP16_OFS(dest, sample_l, 0);
  STORE_LE_SWAP16_OFS(dest, sample_r, 2);
  dest = (S32*)dest + 1;
  if ((void*)dest >= (void*)dest_end) goto dest_end_exit607;

  // Add to accumulator and advance the source correctly
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract > old )  goto merge_loop607;}

  // Move the source pointer
  src = (U8*)src + 4;
  if (src >= src_end) goto src_end_exit607;

  // Load sample data
  sample_r = LE_SWAP32(src);
  sample_l = (S32)(S16)(sample_r);
  sample_r >>= 16;

  // rotate filter values
  cur_l2 = cur_l1;
  cur_l1 = sample_l;
  cur_r2 = cur_r1;
  cur_r1 = sample_r;

  // End loop
  goto      merge_loop607;

  // Jump out point if end of dest is reached
  dest_end_exit607:
  src = (U8*)src + 4;
  { U32 old = src_fract;
    src_fract += playback_ratio;
    if ( src_fract <= old ) goto skip_filter_adjust607;}

  // rotate filter values
  cur_l1 = cur_l2;
  cur_r1 = cur_r2;

  // un-increment the source to skip the early source adjustment
  src = (U8*)src - 4;

  skip_filter_adjust607:

  // Jump out point if end of src is reached
  src_end_exit607:

  // adjust fractional
  src_fract >>= 16;

  // Save end results (use sample_l as a temporary) 
  *(S32**)destp = dest;
  *(void**)srcp = src;
  *(U32*)src_fractp = src_fract;
  *(S32*)cur_l_ptr = cur_l1;
  *(S32*)cur_r_ptr = cur_r1;

  // restore upper variables

  // Routine end
  return;

}


void AILCALL MSS_mixer_merge(
  void const * * srcp,
  U32  * src_fractp,
  void const * src_endp,
  S32  * * destp,
  void * dest_endp,
  S32  * cur_l1_ptr,
  S32  * cur_r1_ptr,
  S32  playback_ratiop,
  S32  scale_leftp,
  S32  scale_rightp,
  U32  which
#ifdef IS_X86
 ,U32             MMX_available
#endif
)
{
  if (((U8*)*destp)>=(U8*)dest_endp)
     return;

  switch( which )
  {
    case 2: Merge_DestMono_SrcMono_Src16_NoVolume_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 3: Merge_DestStereo_SrcMono_Src16_NoVolume_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 6: Merge_DestMono_SrcMono_Src16_NoVolume_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 7: Merge_DestStereo_SrcMono_Src16_NoVolume_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 10: Merge_DestMono_SrcStereo_Src16_NoVolume_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 11: Merge_DestStereo_SrcStereo_Src16_NoVolume_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 14: Merge_DestMono_SrcStereo_Src16_NoVolume_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 15: Merge_DestStereo_SrcStereo_Src16_NoVolume_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 18: Merge_DestMono_SrcMono_Src16_NoVolume_Resample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 19: Merge_DestStereo_SrcMono_Src16_NoVolume_Resample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 22: Merge_DestMono_SrcMono_Src16_NoVolume_DownFiltered( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 23: Merge_DestStereo_SrcMono_Src16_NoVolume_DownFiltered( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 26: Merge_DestMono_SrcStereo_Src16_NoVolume_Resample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 27: Merge_DestStereo_SrcStereo_Src16_NoVolume_Resample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 30: Merge_DestMono_SrcStereo_Src16_NoVolume_DownFiltered( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 31: Merge_DestStereo_SrcStereo_Src16_NoVolume_DownFiltered( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 34: Merge_DestMono_SrcMono_Src16_Volume_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 35: Merge_DestStereo_SrcMono_Src16_Volume_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 38: Merge_DestMono_SrcMono_Src16_Volume_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 39: Merge_DestStereo_SrcMono_Src16_Volume_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 42: Merge_DestMono_SrcStereo_Src16_Volume_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 43: Merge_DestStereo_SrcStereo_Src16_Volume_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 46: Merge_DestMono_SrcStereo_Src16_Volume_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 47: Merge_DestStereo_SrcStereo_Src16_Volume_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 50: Merge_DestMono_SrcMono_Src16_Volume_Resample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 51: Merge_DestStereo_SrcMono_Src16_Volume_Resample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 54: Merge_DestMono_SrcMono_Src16_Volume_DownFiltered( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 55: Merge_DestStereo_SrcMono_Src16_Volume_DownFiltered( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 58: Merge_DestMono_SrcStereo_Src16_Volume_Resample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 59: Merge_DestStereo_SrcStereo_Src16_Volume_Resample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 62: Merge_DestMono_SrcStereo_Src16_Volume_DownFiltered( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 63: Merge_DestStereo_SrcStereo_Src16_Volume_DownFiltered( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 66: Merge_DestMono_SrcMono_Src16_Copy16_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 67: Merge_DestStereo_SrcMono_Src16_Copy16_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 70: Merge_DestMono_SrcMono_Src16_Copy16_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 71: Merge_DestStereo_SrcMono_Src16_Copy16_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 74: Merge_DestMono_SrcStereo_Src16_Copy16_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 75: Merge_DestStereo_SrcStereo_Src16_Copy16_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 78: Merge_DestMono_SrcStereo_Src16_Copy16_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 79: Merge_DestStereo_SrcStereo_Src16_Copy16_NoResample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 82: Merge_DestMono_SrcMono_Src16_Copy16_Resample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 83: Merge_DestStereo_SrcMono_Src16_Copy16_Resample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 86: Merge_DestMono_SrcMono_Src16_Copy16_DownFiltered( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 87: Merge_DestStereo_SrcMono_Src16_Copy16_DownFiltered( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 90: Merge_DestMono_SrcStereo_Src16_Copy16_Resample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 91: Merge_DestStereo_SrcStereo_Src16_Copy16_Resample( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 94: Merge_DestMono_SrcStereo_Src16_Copy16_DownFiltered( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
    case 95: Merge_DestStereo_SrcStereo_Src16_Copy16_DownFiltered( destp,dest_endp,srcp,src_fractp,src_endp,playback_ratiop,scale_leftp,scale_rightp,cur_l1_ptr,cur_r1_ptr); break;
  }
}

