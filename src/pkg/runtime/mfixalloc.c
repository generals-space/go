// Copyright 2009 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Fixed-size object allocator.  Returned memory is not zeroed.
// 固定大小对象的分配器. 其返回的内存空间未清零
// See malloc.h for overview.

#include "runtime.h"
#include "arch_GOARCH.h"
#include "malloc.h"

// Initialize f to allocate objects of the given size,
// using the allocator to obtain chunks of memory.
// 初始化f以分配指定大小size的对象(创建f就是为了给size大小的对象分配空间)
// caller: mheap.c -> runtime·MHeap_Init()
// 第3个参数void (*first)(void*, byte*)是一函数类型 
// *first为函数地址, (void*, byte*)为ta的两个参数
void
runtime·FixAlloc_Init(FixAlloc *f, uintptr size, void (*first)(void*, byte*), void *arg, uint64 *stat)
{
	f->size = size;
	f->first = first;
	f->arg = arg;
	f->list = nil;
	f->chunk = nil;
	f->nchunk = 0;
	f->inuse = 0;
	f->stat = stat;
}

void*
runtime·FixAlloc_Alloc(FixAlloc *f)
{
	void *v;
	
	if(f->size == 0) {
		runtime·printf("runtime: use of FixAlloc_Alloc before FixAlloc_Init\n");
		runtime·throw("runtime: internal error");
	}

	if(f->list) {
		v = f->list;
		f->list = *(void**)f->list;
		f->inuse += f->size;
		return v;
	}
	if(f->nchunk < f->size) {
		f->chunk = runtime·persistentalloc(FixAllocChunk, 0, f->stat);
		f->nchunk = FixAllocChunk;
	}
	v = f->chunk;
	if(f->first)
		f->first(f->arg, v);
	f->chunk += f->size;
	f->nchunk -= f->size;
	f->inuse += f->size;
	return v;
}

void
runtime·FixAlloc_Free(FixAlloc *f, void *p)
{
	f->inuse -= f->size;
	*(void**)p = f->list;
	f->list = p;
}

