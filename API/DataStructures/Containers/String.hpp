#ifndef APPLICATION_API_DATASTRUCTURES_CONTAINERS_STRING_HPP_
#define APPLICATION_API_DATASTRUCTURES_CONTAINERS_STRING_HPP_

// STL
#include <string>

namespace api {
using String =
    std::basic_string<char, std::char_traits<char>, std::allocator<char>>;
} // namespace api
#endif // !APPLICATION_API_DATASTRUCTURES_CONTAINERS_STRING_HPP_
