#ifndef APPLICATION_API_DATASTRUCTURES_CONTAINERS_STRING_HPP_
#define APPLICATION_API_DATASTRUCTURES_CONTAINERS_STRING_HPP_
// current project
#include "../../../Config.hpp"

#if ALIGNED_ALLOCATOR_USAGE
#include "../../Memory/AlignedAllocator.hpp"
#endif // !ALIGNED_ALLOCATOR_USAGE

// STL
#include <string>

namespace api {
#if STL_ALLOCATOR_USAGE
using String =
    std::basic_string<char, std::char_traits<char>, std::allocator<char>>;
#elif ALIGNED_ALLOCATOR_USAGE
using String =
    std::basic_string<char, std::char_traits<char>, AlignedAllocator<char>>;
#else
static_assert(false, "Ambigious allocator");
#endif
} // namespace api
#endif // !APPLICATION_API_DATASTRUCTURES_CONTAINERS_STRING_HPP_
