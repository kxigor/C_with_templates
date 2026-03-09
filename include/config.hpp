#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

namespace scy {

using SymT = char;
using StringViewT = std::basic_string_view<SymT>;
using StringT = std::basic_string<SymT>;

using PosT = std::uint32_t;

template <typename T>
using VectorT = std::vector<T>;

template <typename T, typename U>
using UmapT = std::unordered_map<T, U>;

template <typename T>
using OptionalT = std::optional<T>;

template <typename T>
using UniquePtrT = std::unique_ptr<T>;

template <typename T, typename... Args>
UniquePtrT<T> MakeUnique(Args&&... args) {
  return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename... Args>
using VariantT = std::variant<Args...>;

}  // namespace scy