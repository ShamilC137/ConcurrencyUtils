#ifndef APPLICATION_API_CONTAINERS_STRING_HPP_
#define APPLICATION_API_CONTAINERS_STRING_HPP_

#include "../Memory/VAllocator.hpp"

#include <xstring>

namespace api {
using String = std::basic_string<char, std::char_traits<char>,
                                 /*VAllocator<char>*/ std::allocator<char>>;
}
#endif // !APPLICATION_API_CONTAINERS_STRING_HPP_
