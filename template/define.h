#include <concepts>

template <typename T>
concept Comparable = requires(T a, T b) {
  { a < b } -> std::same_as<bool>;
  { a > b } -> std::same_as<bool>;
  { a <= b } -> std::same_as<bool>;
  { a >= b } -> std::same_as<bool>;
  { a == b } -> std::same_as<bool>;
  { a != b } -> std::same_as<bool>;
};

enum class Color { RED, BLACK };
