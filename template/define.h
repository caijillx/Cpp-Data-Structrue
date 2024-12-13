#include <concepts>

template <typename T>
concept Comparable = std::is_trivially_copyable_v<T> && requires(T a, T b) {
  { a < b } -> std::same_as<bool>;
  { a > b } -> std::same_as<bool>;
  { a == b } -> std::same_as<bool>;
};

enum class Color { RED, BLACK };
