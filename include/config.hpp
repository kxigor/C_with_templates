#pragma once

#include <cstdint>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace scy {

using SymT = char;
using StringView = std::basic_string_view<SymT>;
using PosT = std::uint32_t;

template <typename T>
using VectorT = std::vector<T>;

template <typename T, typename U>
using UmapT = std::unordered_map<T, U>;

}  // namespace scy