#ifndef APPLICATION_KERNEL_MMU_VIRTUALMMU_HPP_
#define APPLICATION_KERNEL_MMU_VIRTUALMMU_HPP_

#include "AlignMMU.hpp"

namespace mmu {

//********************************************************************************************************************//
// MEMORY STRUCTURE
//
// 		At the beginning the page_map_ will be placed whose size is
// related with the number of pages fitting into other
//		memory. The BlockMMU Allocator is used for memory allocations
// for the page_map_. The other part of memory is
// 		directed into the AlignMMU.
//
// MEMORY MANAGEMENT STRATEGY
//
//		Initially the VirtualMMU allocates memory through the AlignMMU
// until it throws an exception. Further 		the VirtualMMU wtires
// the least used pages into the file thereby freeing memory; thereafter it
// tries to allocate 		through the AlignMMU again.
//
//		If a user dereferences a pointer to unloaded memory, the
// VirtualMMU loads all the pages an object is on.
//
// POINTER
//
//		For inner purposes, the special pointer is required. It contains
// only SizeType ptr_
//
//
//********************************************************************************************************************//
template <SizeType kMemorySize> class VirtualMMU {
public:
  VirtualMMU(SizeType /* size*/) {}
  void *Allocate(SizeType /*count_in_bytes */) {
    return {};
  }
  void Deallocate(void * /*ptr*/, SizeType /*count_in_bytes*/) {}
};

} // namespace mmu

#endif // APPLICATION_KERNEL_MMU_VIRTUALMMU_HPP_
