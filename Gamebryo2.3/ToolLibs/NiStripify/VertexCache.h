// nVidia PROPRIETARY INFORMATION
//
// This source code is from the nVidia developer site,
//     http://developer.nvidia.com
// The project utilized is nvTriStrip. Slight modifications were made to
// utilize the source in Gamebryo.
//
//      Copyright (c) 2003 nVidia Corporation
//      All Rights Reserved.
//
#ifndef VERTEX_CACHE_H
#define VERTEX_CACHE_H

#include "NiStripifyLibType.h"

class NISTRIPIFY_ENTRY VertexCache 
{
    
public:
    
    VertexCache(int size);
    VertexCache();
    ~VertexCache();
    
    bool InCache(int entry);
    int AddEntry(int entry);
    void Clear();
    void Copy(VertexCache* inVcache);
    int At(int index);
    void Set(int index, int value);

private:

  int *entries;
  int numEntries;

};

#endif
