<!--
SPDX-FileCopyrightText: 2021 Shuai Zhang

SPDX-License-Identifier: CC-BY-NC-ND-4.0
-->

# Standard Library

The C++ STL (Standard Template Library) is one of the built-in libraries mandated by the C++ Standard, and evolves together with the language standard. However, compiler vendors are often inconsistent when implementing the core language and the STL portions of a given standard. For example, GCC 4.9.2 claimed C++14 support but did not implement every STL feature required by C++14. A rough overview table can be found at <https://en.cppreference.com/w/cpp/compiler_support>. For more precise information you still need to consult the official documentation provided by the compiler vendor.

Therefore, to compensate for deficiencies or missing pieces in a vendor's shipped STL implementation, you will sometimes encounter additional utility code such as:

- <https://source.chromium.org/chromium/chromium/src/+/9d5aa289d49ad2c9068dec6f3c55a30938be01f9:base/template_util.h;drc=221e331b49dfefadbc6fa40b0c68e6f97606d0b3>
- <https://abseil.io/docs/cpp/guides/meta>
- <https://abseil.io/docs/cpp/guides/strings>

In daily work just use the standard library normally first. If something misbehaves and you have confirmed it is not your own usage error, remember to also check whether the vendor's STL has a bug or missing support.

## Containers

### Mapping Between Java and C++ Containers

An approximate mapping is shown below. A classification table from the C++ perspective can be found at <https://en.cppreference.com/w/cpp/container>.

| Java | C++ | Notes |
|---------|---|---|
| `Tuple` | `std::tuple` | Strongly recommend using `tuple` only as an internal storage detail; do not expose it in interfaces because it is hard for others to understand what each element means. Prefer defining a `struct` expressing semantic meaning. |
| Fixed-size array with compile-time known length | `std::array` | |
| Fixed-size array with length only known at runtime | `std::unique_ptr<T[]>`, `absl::FixedArray`, or `std::vector` | Length decided at runtime (no further growth intended); `std::vector` here is treated as a fixed-size buffer after construction. |
| `String` | None (or `icu::UnicodeString`) | Important: `std::string` is just a `char` buffer and provides no encoding semantics. |
| `ArrayList` | `std::vector` | Warning: `std::vector<bool>` is a bitset-like specialization, not a normal dynamic array; for a bitset use `std::bitset`; for a plain bool array use `std::unique_ptr<bool[]>`. |
| `LinkedList` | `std::list` | |
| (Singly linked list) | `std::forward_list` | |
| `Stack` | `std::stack` | |
| Interface similar to `Queue` | `std::queue` | |
| `Deque` | `std::deque` | |
| `PriorityQueue` | `std::priority_queue` | |
| (Ring-buffer based queue; maybe Netty has one?) | None, or Chromium's `circular_deque` | <https://source.chromium.org/chromium/chromium/src/+/master:base/containers/circular_deque.h;drc=fc269309730220a159e1d7cd96e4672a1e216eb2> |
| `HashSet` | `std::unordered_set`, `absl::flat_hash_set` | |
| `TreeSet` | `std::set`, `absl::btree_set` | |
| (Or Guava `Multiset`) | `std::multiset`, `absl::btree_multiset` | |
| `HashMap` | `std::unordered_map`, `absl::flat_hash_map` | |
| `TreeMap` | `std::map`, `absl::btree_map` | |
| `LinkedHashMap` | None (can be emulated with `std::vector` + `std::unordered_map` + custom bookkeeping) | |
| (Or Guava `Multimap`) | `std::multimap`, `absl::btree_multimap` | |
| `ConcurrentXXX` | None, or `folly::ConcurrentXXX` | |

### Iterators

C++ frequently uses iterators to represent a range within a container (C++20 introduced Ranges, but before broad adoption you still need to be comfortable with iterator-based idioms). Typically a half-open interval [a, b) is represented by a pair of iterators. Based on this convention, iterators in C++ may legally advance one position past the last element (i.e. to index `len(c)`), but may not move before the first element (i.e. index `-1`).

C++ also provides reverse iterators for backward traversal: `rbegin()` and `rend()`. You can obtain a forward iterator from a reverse iterator via its `base()` method, and convert a forward iterator to a reverse iterator via `std::make_reverse_iterator`. These invariants hold:

```cpp
--8<-- ".snippets/standard-library/001-iterator-reverse.cc:code"
```

Iterators have categories. This reflects underlying memory layout and capabilities: some containers (like arrays) support efficient random access, others (like lists) do not; some support bidirectional movement, others (like singly linked lists) only forward movement. A detailed list and explanation is at <https://en.cppreference.com/w/cpp/iterator>.

/// admonition | Note
Similar to Java's `java.util.ConcurrentModificationException`, modifying a container (e.g., inserting/removing elements) while iterating can invalidate iterators (their referenced position becomes illegal). However, in C++ this does not raise an exception; it results in Undefined Behavior. You must prevent this logically.
///

### `std::vector`—Things to Watch Out For

First, when passing a vector, use a reference or pointer to avoid copying the entire container; this differs from Java semantics.

```cpp
--8<-- ".snippets/standard-library/002-vector-pass-by-ref.cc:code"
```

Next, note that `std::vector<bool>` internally stores bits (a bitmap) instead of an array of `bool`. This is a historical quirk. If you truly want a bitmap, explicitly use `std::bitset`; if you want a contiguous array of bool values, use `std::unique_ptr<bool[]>` or `absl::FixedArray<bool>`.

Finally, a performance tip: if possible, pre-reserve capacity using `reserve`.

```cpp
--8<-- ".snippets/standard-library/003-vector-reserve.cc:code"
```

/// admonition | Note
Do not overuse `reserve()`, especially not inside loops. Compute the needed capacity beforehand (outside the loop) and call `reserve()` once, rather than before each insertion.
///

### Contains: Determining Whether a Container Holds an Element

C++20 and C++23 gradually added a `contains` member to various container types. Before that you had to write code like this:

```cpp
--8<-- ".snippets/standard-library/004-contains-unordered-map.cc:code"
```

Be aware that string-like containers (`std::string`, `std::string_view`, `std::wstring`, etc.) do not follow the "return `end()` when not found" convention for their search functions; instead they use a special constant `npos`:

```cpp
--8<-- ".snippets/standard-library/005-contains-string.cc:code"
```

This is mainly because when `std::string` entered the standard, iterator conventions were not yet established.

If you find this awkward, you can use helper utilities like: <https://source.chromium.org/chromium/chromium/src/+/9d5aa289d49ad2c9068dec6f3c55a30938be01f9:base/containers/contains.h>

### Safely Representing a Subrange Without Copying Data

Although a pair of iterators can represent a range, it is sometimes inconvenient—especially when you conceptually want to treat the range like a lightweight view container. In such cases you have `absl::string_view` (C++17's `std::string_view`) and `absl::Span` (C++20's `std::span`).

When using these, be careful: they do not own data. The validity of their contents depends on the lifetime of the original storage.

```cpp
--8<-- ".snippets/standard-library/006-string-view-lifetime.cc:code"
```

Another point: `absl::string_view` accepts both `std::string` and C-style strings (`char*`). For string literals, performance can be better than `const std::string&`. So where appropriate, prefer `absl::string_view` over `const std::string&` as a parameter type.

However, some system calls (e.g. `::open()`) require a C-style null-terminated string. You must not pass `absl::string_view::data()` blindly because it might not be `\0` terminated.

```cpp
--8<-- ".snippets/standard-library/007-open-read-file.cc:code"
```

In that scenario using `const std::string&` is still more efficient.

### Ropes: An Alternative to Strings

In many (often common) scenarios, the traditional C representation of a string is inefficient. Using alternative data structures can yield large performance improvements. See the paper *Ropes: an Alternative to Strings* (or a brief intro at <https://www.wikiwand.com/en/Rope_(data_structure)>). If you want such a structure, consider Abseil's `cord` container (<https://github.com/abseil/abseil-cpp/blob/cba8cf8/absl/strings/cord.h>).

### String StartsWith / EndsWith / Strip / ...?

As mentioned, `std::string` owns a fixed block of memory and C++ lacks a GC, so efficient in-place operations like strip are hard. For strip-like behavior use `std::string_view` (it has `remove_prefix` / `remove_suffix`). Abseil additionally provides `absl::StripPrefix` and `absl::StripSuffix`.

`StartsWith` / `EndsWith` only arrived in C++20 for the standard string. You can use `absl::StartsWith` / `absl::EndsWith` (and case-insensitive versions) earlier.

The standard string `contains` only appears in C++23. Use `absl::StrContains` otherwise.

### How to Query a `const` Referenced Map?

You may have encountered code like:

```cpp
--8<-- ".snippets/standard-library/018-unordered-map-bad-lookup.cc:code"
```

Which produces a compile error like:

<!-- markdownlint-disable MD013 -->
```text
--8<-- ".snippets/standard-library/017-unordered-map-operator-error.txt:code"
```
<!-- markdownlint-enable MD013 -->

This is because `operator[]` inserts a default-constructed value when the key is missing (see <https://en.cppreference.com/w/cpp/container/unordered_map/operator_at>). Therefore, on a `const std::unordered_map&` you cannot call `operator[]`.

Use `contains` (since C++20) or `find()` instead.

```cpp
--8<-- ".snippets/standard-library/019-unordered-map-find.cc:code"
```

### Avoid `iostream` Components for Serious Use Cases

See this discussion: [C++ 工程实践(7):iostream 的用途与局限 - 陈硕 - 博客园](https://www.cnblogs.com/Solstice/archive/2011/07/17/2108715.html).

For file abstractions, consider adapting code from [LevelDB](https://github.com/google/leveldb/blob/f57513a1d6c99636fc5b710150d0b93713af4e43/include/leveldb/env.h#L74) or [TensorFlow](https://github.com/tensorflow/tensorflow/blob/73d09c1434aec3d1e10ff7b2e44020741bccb552/tensorflow/core/platform/file_system.h#L56).

For string formatting, use the [fmt library](https://fmt.dev/latest/index.html) or Abseil utilities (`absl::StrFormat()` family, `absl::Substitute()`).

### Custom Allocators and PMR Containers

STL containers allow custom allocators to customize memory allocation strategy. This can be useful; e.g., in a query processing scenario you might allocate everything using a single allocator and free en masse at the end of the query. In most situations you don't need to worry about this.

The default allocator uses `malloc` / `free`. The glibc defaults are relatively slow; commonly we improve performance by linking in a faster general-purpose allocator such as [jemalloc](https://github.com/jemalloc/jemalloc), [gperftools tcmalloc](https://github.com/gperftools/gperftools), [google tcmalloc](https://github.com/google/tcmalloc), or [microsoft mimalloc](https://github.com/microsoft/mimalloc). This speeds up not only containers but any `new` / `make_shared` / etc.

If switching to jemalloc is still insufficient and you truly need a custom allocator, see: <https://docs.microsoft.com/en-us/cpp/standard-library/allocators?view=msvc-160>.

Be aware: `std::vector<int>` and `std::vector<int, MyAllocator>` are different types. This is inconvenient. To alleviate that, C++ (since C++17) introduced Polymorphic Memory Resources (PMR). In short, implement / obtain a `std::pmr::polymorphic_allocator` and use `std::pmr::vector`.

### Ranges Library

Similar to Java's Stream API, Ranges arrived in C++20. For earlier (C++17) compilers you can use: <https://github.com/ericniebler/range-v3>

## Algorithms

The STL ships many common algorithms. Personally I most frequently use:

1. Binary search: `lower_bound` / `upper_bound`
1. Min / max: `min_element` / `max_element` / `minmax_element`
1. Clamp a numeric value within bounds: `clamp`
1. Remove all elements matching a predicate: `remove_if`

I will not detail each here; see <https://en.cppreference.com/w/cpp/algorithm>. Below are a few commonly tricky usages.

### Find the Last Element Not Greater Than a Value in a Sorted Container

> Requires a total ordering relation.

We have two binary search algorithms (assume comparator `std::less<>` and ascending order):

1. `lower_bound`: first position not less than (>=) the given value
1. `upper_bound`: first position greater than (>) the given value

To find the last element not greater than a value, think backwards. Viewing the container in reverse gives a descending sequence. The forward notion of "last element <= value" becomes in reverse "first element <= value", which suggests using `lower_bound` on the reversed view:

```cpp
--8<-- ".snippets/standard-library/008-find-last-le-not-working.cc:code"
```

This description can feel roundabout; a mathematical formulation may be clearer.

### Erasing All Elements Matching a Predicate

> <https://www.wikiwand.com/en/Erase%E2%80%93remove_idiom>

`remove_if` only partitions elements, moving those to be removed to the end; it does not shrink the container. Thus you must follow the erase–remove idiom:

```cpp
--8<-- ".snippets/standard-library/009-erase-remove-idiom.cc:code"
```

## Random Numbers

/// admonition | Note
If you need cryptographically secure random numbers, do NOT use the ordinary facilities here. Use a well-known cryptographically secure algorithm/library, e.g. generate bytes via OpenSSL (<https://www.openssl.org/docs/man1.1.1/man3/RAND_bytes.html>).
///

In Java you usually just instantiate `Random` and call it. In C++ you often need to be more explicit; conceptually there are three pieces:

1. The pseudo-random bit generator (in C++ terms, a `UniformRandomBitGenerator`).
1. The seed material for that generator.
1. The probability distribution used to transform raw bits into the numbers you need.

See <https://en.cppreference.com/w/cpp/header/random> for a comprehensive overview. A simple example:

```cpp
--8<-- ".snippets/standard-library/010-random-example.cc:code"
```

Do NOT seed a generator with just the current time (yes, looking at you—`srand(time(NULL))`): that has poor security properties. Typically you should pull some entropy from the system (hardware entropy pool) to seed the PRNG (pseudo-random number generator). Likewise, do NOT generate a number in `[0,5]` by doing `value % 6`; the resulting distribution is not uniform (try computing the mapping probabilities of `[1,8]` onto `[1,6]` under modulo).

The standard library `random` interfaces are powerful but a bit awkward. Abseil's Random API is often nicer:

1. Some algorithms need more seed bits than a single `rd()` `unsigned int` provides.
1. Getting the interval semantics right for distributions (closed/open endpoints) can be confusing.
1. Testing is harder—you may want a deterministic, fully standards-compliant PRNG you can swap in.

Abseil Random overview: <https://abseil.io/docs/cpp/guides/random>.

For a `ThreadLocalRandom` equivalent, just add the `thread_local` storage specifier to the generator; details are in the later `thread_local` section.

## Date and Time

/// admonition | Note
Assumes familiarity with Joda Time or Java 8's Date/Time API.
///

First disentangle these concepts:

1. Clock: produces the current (or a historical) time point.
1. Timepoint: an instant in time obtained from a clock.
1. Duration: the distance between two time points.

Java implicitly uses the system clock for almost everything. Sometimes that assumption is invalid (though usually fine): e.g. you may need a monotonically increasing clock that never goes backward. Clocks can jump backward if NTP (Network Time Protocol) notices the local clock is fast and corrects it. An alternative is to slow the clock (slew) until real time catches up, never stepping backward.
A timepoint may carry time zone information (e.g. `java.time.ZonedDateTime`) or not (`java.time.LocalDateTime`).

C++ provides three clock types:

1. `system_clock`: wall clock time, similar to `java.time.Clock`.
1. `steady_clock`: guaranteed not to go backwards.
1. `high_resolution_clock`: highest resolution available (implementation-defined alias).

In C++, clock types are part of the type system. This is valuable: comparing time points from different clocks is meaningless. The downside: verbose types like `std::chrono::time_point<std::chrono::steady_clock>`, and if you refactor from `system_clock` to `steady_clock` you must update all the dependent timepoint types—including function parameters.

Duration units are also encoded in the type system. On most modern (especially 64-bit) architectures this often delivers limited practical benefit for common cases.

> BTW: For other physical quantities see <https://github.com/mpusz/units>

```cpp
--8<-- ".snippets/standard-library/011-chrono-measure-time.cc:code"
```

Epoch conversions are common:

- epoch time -> chrono time: construct a duration, then construct a timepoint (<https://en.cppreference.com/w/cpp/chrono/time_point/time_point>)
- chrono time -> epoch time: <https://en.cppreference.com/w/cpp/chrono/time_point/time_since_epoch>

Overall chrono is thorough but not always ergonomic. Abseil's Time API (<https://abseil.io/docs/cpp/guides/time>) assumes a single system clock and keeps units out of the type system, producing a simpler, often friendlier interface.

/// admonition | Note
For robust parsing of human/calendar time strings into absolute system times, use Abseil Civil Time or a similar library. Handling locale/region-specific rules (DST transitions, leap seconds, etc.) is complex; do not attempt ad‑hoc implementations.
///

## Multithreading and Concurrency Control

On Linux, `std::thread` is essentially a wrapper around pthread primitives. For many threading questions, reading the pthread man pages first helps: <https://linux.die.net/man/7/pthreads>.

### Sleep

```cpp
--8<-- ".snippets/standard-library/012-sleep-examples.cc:code"
```

Sleeping is easy; responsive wake-up is harder. Where possible, consider passing an `absl::Notification` and using `notification.WaitForNotificationWithTimeout(kSleepInterval)` instead. See the "background thread periodic work" pattern later.

### Threads

Creating a `std::thread` object immediately spawns the thread and starts executing the provided function.

/// admonition | Note
There is no hook to run pthread initialization code before the thread function begins. If you need that, wrap your own Thread abstraction (e.g. <https://source.chromium.org/chromium/chromium/src/+/main:base/threading/platform_thread_posix.cc;drc=37a0e6c0a6a13a078d08f51faf276c4eb4f0ef1a>).
///

You can later obtain the pthread handle via `native_handle` for operations like CPU affinity (`pthread_setaffinity_np`), naming (`pthread_setname_np`), priority (`pthread_setschedparam`), etc.

/// admonition | Note
Destroying a `std::thread` object without a prior `join` or `detach` terminates the program (std::terminate). Ensure one of them is called.
///

In serious applications you must plan thread lifecycle at shutdown. Pass an `absl::Notification` to background threads so you can signal them to stop, then `join` for graceful shutdown.

### Thread Pools

Thread pools were not yet standardized (expectation was C++23 alongside co-routines & futures/executors integration). Options:

1. Boost `boost::asio::io_service`
1. (Company-internal pool—redacted in original)
1. Roll your own (several listed below I have not personally used)
1. <https://chriskohlhoff.github.io/executors/>
1. <https://github.com/lewissbaker/cppcoro>
1. <https://github.com/Quuxplusone/coro>

Also consider dataflow-style libraries instead of manually managing a pool:

1. Microsoft's PPL: <https://docs.microsoft.com/en-us/cpp/parallel/concrt/parallel-patterns-library-ppl?view=msvc-160> (Cross-platform variant `pplx` in cpprestsdk: <https://github.com/microsoft/cpprestsdk>)
1. Intel TBB: <https://software.intel.com/content/www/us/en/develop/documentation/onetbb-documentation/top.html>

### Locks

There is no `synchronized` keyword in C++; you manage locking explicitly.

| Java Type | C++ Type | Notes |
| --- | --- | --- |
|     | `std::mutex`, `absl::Mutex` | Non-reentrant equivalent of `std::recursive_mutex` |
|     | `std::shared_mutex`, `absl::Mutex` | Non-reentrant equivalent of `ReentrantReadWriteLock` |
| `java.util.concurrent.locks.ReentrantLock` | `std::recursive_mutex` | |
| `java.util.concurrent.locks.ReentrantReadWriteLock` | None | |

Typical Java pattern:

```java
--8<-- ".snippets/standard-library/013-java-lock-example.java:code"
```

Typical C++ pattern:

```cpp
--8<-- ".snippets/standard-library/014-cpp-lock-example.cc:code"
```

```cpp
--8<-- ".snippets/standard-library/015-mutable-mutex-example.cc:code"
```

### Atomic Access / `volatile` (Updated 2022-02-14)

/// admonition | Note
`volatile` in C++ is NOT the same as in Java. Do not attempt to achieve atomicity by adding `volatile`.
///

Atomic access uses `std::atomic<T>` wrapping your type. The wrapped type must meet `TriviallyCopyable`, `CopyConstructible`, `CopyAssignable` constraints. In practice you rarely wrap complex user-defined non-trivial types.

Prefer `std::atomic<int>`, `std::atomic<int32_t>`, `std::atomic<double>` etc.; these are equivalent to typedef-like forms (`std::atomic_int`).

/// admonition | Note
If atomic operations are on the hot path and profiling shows they are a bottleneck, you may relax memory ordering for performance. If you do not fully understand the implications, don't—profile and locate real bottlenecks first.
///

To observe writes to multiple atomic variables in the order they occurred, use `store(..., std::memory_order_release)` on writes and `load(std::memory_order_acquire)` on reads. The default is `std::memory_order_seq_cst` (strong ordering). Details: <https://en.cppreference.com/w/cpp/atomic/memory_order>—very intricate, not required for most developers.

Heuristic: if performance matters, mark all writes with `release` and reads with `acquire`. A helpful explanation: <http://bluehawk.monmouth.edu/~rclayton/web-pages/u03-598/netmemcon.html>.

### `thread_local`

No wrapper type (like Java's `java.lang.ThreadLocal`) is needed. Just add the `thread_local` storage specifier to a variable declaration. Caveats:

1. Some OSes (certain Android versions) have issues with many thread-local variables (<https://source.chromium.org/chromium/chromium/src/+/main:base/threading/thread_local.h;l=28;drc=7b5337170c1581e4a35399af36253f767674f581>).
1. Large thread-local objects can consume a lot of memory on machines with many cores (a design concern, not a C++ or OS defect).

```cpp
--8<-- ".snippets/standard-library/016-thread-local-example.cc:code"
```

### Semaphore / Latch / Barrier / Condition Variable

Semaphores (`java.util.concurrent.Semaphore`) only enter the C++ standard in C++20. Latches and Barriers also require C++20+. In suitable scenarios semaphores can outperform condition variables (`java.util.concurrent.locks.Condition`).

Condition variables are powerful and can emulate the other missing primitives. For complex synchronization scenarios consider whether a condition variable might be simpler or more efficient. (A full tutorial is out of scope here.)

| Name | Java Type | C++ Type |
| --- | --- | --- |
| Semaphore | `java.util.concurrent.Semaphore` | `std::counting_semaphore` (C++20) |
| Latch | `java.util.concurrent.CountDownLatch` | `std::latch` (C++20), `absl::BlockingCounter` |
| Barrier | `java.util.concurrent.CyclicBarrier` | `std::barrier` (C++20), `absl::Barrier` |
| Condition Variable | `java.util.concurrent.locks.Condition` | `std::condition_variable`, `absl::CondVar` |

`absl::Mutex` offers a simplified conditional variable pattern (auto-notify on unlock). Docs: <https://abseil.io/docs/cpp/guides/synchronization#conditional-mutex-behavior>.

### `std::call_once`

Ensures a protected function is executed exactly once even under concurrency. If it throws an exception, subsequent calls retry until it completes successfully.

<https://en.cppreference.com/w/cpp/thread/call_once>

You do NOT need this for a lazily initialized singleton; see the later patterns section.

### Future/Promise Model

Current standard `std::future` / `std::promise` are limited (e.g., no chaining with `then`). Proposals improving this depend on coroutines etc., so practical availability began around C++23. See: <https://www.modernescpp.com/index.php/the-end-of-the-detour-unified-futures>.

For a more capable implementation today consider `folly::Future` / `folly::Promise`: <https://github.com/facebook/folly/blob/16a7084/folly/docs/Futures.md>. Also: <https://chriskohlhoff.github.io/executors/> (a proposal POC; performance unknown).

> Commentary: "Limited functionality" here means inability to chain tasks (callback pyramid akin to early Node.js). A more advanced approach is using `await`/`async` keywords with coroutines—similar to C# (and now Python). Chaining via `then()` / `transform()` is addressed in newer C++ standards and libraries like Folly. True async/await style requires coroutine support in compilers and libraries; broader discussion awaits a more settled standard.
>
> Existing proposals include P0443 and P1897.

Additional reading (2021-08-03): [A Brief Look at The C++ Executors](https://zhuanlan.zhihu.com/p/395250667).

### Static Analysis for Concurrency Errors

/// admonition | Note
Simple, powerful, highly recommended!
///

Detailed introductions:

- [Thread Safety  Annotations for Clang.pdf](https://llvm.org/devmtg/2011-11/Hutchins_ThreadSafety.pdf)
- [Thread Safety Analysis](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html)

Main issue: enabling compilation with clang for your project.

One approach:

1. If you use Ninja, export a `compile_commands.json`; it's also useful for clangd LSP.
1. Write a script processing that file to generate a checking script (Makefile, build.ninja fragment, Bash script, etc.).

If you use `absl::Mutex`, `ABSL_GUARDED_BY`, etc., many annotations come for free.

### Dynamic Detection of Data Races

See <https://clang.llvm.org/docs/ThreadSanitizer.html>

### Thread-Safe Containers

The standard library does not provide thread-safe containers, but several well-known third-party libraries do:

- [Intel TBB Containers](https://software.intel.com/content/www/us/en/develop/documentation/tbb-documentation/top/intel-threading-building-blocks-developer-guide/containers.html)
- [Boost Lockfree](https://www.boost.org/doc/libs/1_76_0/doc/html/lockfree.html)
- [Folly ConcurrentHashMap](https://github.com/facebook/folly/blob/master/folly/concurrency/ConcurrentHashMap.h)

Before adopting them, consider:

1. Thread safety must be designed end-to-end; composing individually thread-safe modules does not guarantee overall thread safety.
1. Lock-free containers are not automatically faster than locked ones.
