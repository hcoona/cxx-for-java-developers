<!--
SPDX-FileCopyrightText: 2021 Shuai Zhang

SPDX-License-Identifier: CC-BY-NC-ND-4.0
-->

# Third-Party Utility Libraries

This section only introduces several libraries that complement the C++ standard library (analogous to Guava or Apache Commons in the Java ecosystem). It does NOT cover logging and other broader categories (they will be briefly touched upon later in "Common Libraries").

## Google Abseil

This is a library I strongly recommend, for several reasons:

1. All Google open-source projects are migrating shared dependencies to depend on Abseil; there is a dedicated internal team maintaining it.
2. Rich code comments, dedicated guides (<https://abseil.io/docs/cpp/guides/>), and blog posts explaining rationale (<https://abseil.io/blog/>, <https://abseil.io/tips/>).
3. Backports many C++14/17/20/... components to C++11 with APIs that closely match the standard.
4. The current feature set is pragmatic and down-to-earth—usually you can guess the implementation approach—yet highly practical:
   1. Containers: supplements to the standard containers, mainly performance oriented; also implements some features only added in later standards (`try_emplace`, `contains`, etc.).
   2. Hash: the standard library's hashing story is weak (as noted earlier). Abseil offers a better hash framework while integrating smoothly with standard containers.
   3. Meta: some older standard library implementations have incomplete or buggy `type_traits`; Abseil effectively backports/fixes a number of these utilities.
   4. Numeric: `int128` (and related utilities) can be very handy in certain domains.
   5. Random: addresses issues in the standard library's random facilities, providing more scientifically grounded implementations and interfaces.
   6. String Formatting / Utilities: the C++ standard string facilities are notoriously awkward (as noted earlier); Abseil fills many of the gaps.
   7. Status: a structured error handling mechanism—often used instead of exceptions. Even if you embrace exceptions, there are still places where returning a status object is clearer.
   8. Synchronization: more ergonomic synchronization primitives, already integrated with thread-safety annotations.
   9. Time: more user-friendly abstractions (again, as mentioned previously) plus helpers for parsing/formatting user I/O.
5. It even ships companion clang-tidy checks—extremely thoughtful.

## Boost

Boost is a large, community-maintained "grab bag" library collection that started early. Overall I personally do NOT recommend using Boost wholesale. Reasons:

1. Code quality varies significantly across components; some are quite weak.
2. To support very old C++ standards (e.g. C++98), many hacks were employed, leading to:
   1. Worse performance.
   2. Cryptic diagnostics.
   3. Complex implementations with potential hidden pitfalls.
3. There have historically been multiple breaking changes across Boost releases.
4. Standard library backports in Boost sometimes deviate from the actual standard specification.
5. Many third-party libraries depend on Boost, easily creating diamond dependency shapes. In many environments you are consuming prebuilt binaries instead of building everything from source. Since Boost does not guarantee ABI stability, diamond dependency situations become very difficult to resolve.

If you need a small, isolated Boost component, consider reading the corresponding Boost source and reimplementing the minimal subset you actually require.

### Facebook Folly

Commentary for this section relates to internal company context and is intentionally omitted here.
