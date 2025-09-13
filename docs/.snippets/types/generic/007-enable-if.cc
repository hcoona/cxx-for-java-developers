// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
// If the |cond| satisfied, the |std::enable_if<cond>::type| is a |void|, else
// invalid to evaluate it. So |typename std::enable_if<cond>::type*| is either
// |void*| or invalid.
//
// |template <void* ignored = nullptr>| is another usage of C++ template. We
// won't introduce it in this article. Just use it as an idiom here.
template <typename T,
          typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
std::vector<T> fromArrayToList(const T* arr, int64_t size) {
  return std::vector<T>(arr, arr + size);
}

// Another useful idiom is |absl::void_t<decltype(...your complex
// condition...)>|
//
// template <typename Container, typename Element, typename = void>
// struct HasFindWithNpos : std::false_type {};
//
// template <typename Container, typename Element>
// struct HasFindWithNpos<
//     Container, Element,
//     absl::void_t<decltype(std::declval<const Container&>().find(
//                               std::declval<const Element&>()) !=
//                           Container::npos)>> : std::true_type {};
// --8<-- [end:code]
