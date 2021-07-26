#ifndef APPLICATION_KERNEL_MMU_USINGS_HPP_
#define APPLICATION_KERNEL_MMU_USINGS_HPP_

#include <cstddef>
#include <list>
#include <vector>

namespace mmu {

using SizeType = std::size_t;
using VoidPtr = void*;
using ByteType = std::byte;
using PtrDif = std::ptrdiff_t;

template<class T, class Allocator>
using ListType = std::list<T, Allocator>;

template<class T, class Allocator>
using VectorType = std::vector<T, Allocator>;
}

#endif // APPLICATION_KERNEL_MMU_USINGS_HPP_