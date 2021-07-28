#ifndef APPLICATION_API_DATASTRUCTURES_CONTAINERS_STRING_HPP_
#define APPLICATION_API_DATASTRUCTURES_CONTAINERS_STRING_HPP_
// current project
#include "../../../Config.hpp"
#include "../../Memory/VAllocator.hpp"

// STL
#include <string>

namespace api {
#if STL_ALLOCATOR_USAGE
using String =
    std::basic_string<char, std::char_traits<char>, std::allocator<char>>;
#elif VALLOCATOR_USAGE
using String =
    std::basic_string<char, std::char_traits<char>, VAllocator<char>>;
#else
static_assert(false, "Ambigious allocator");
#endif
} // namespace api
#endif // !APPLICATION_API_DATASTRUCTURES_CONTAINERS_STRING_HPP_
