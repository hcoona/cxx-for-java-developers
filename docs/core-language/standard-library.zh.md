<!--
SPDX-FileCopyrightText: 2021 Shuai Zhang

SPDX-License-Identifier: CC-BY-NC-ND-4.0
-->

# 标准库

C++ STL（Standard Template Library）是 C++ 标准规范要求提供的内置库之一，也随着语言标准一起迭代。但是编译器厂商在实现语言核心标准和 STL 库标准支持时经常会出现不统一的现象，例如宣称支持 C++14 标准的 GCC 4.9.2 并没有支持全部的 C++14 要求提供的 STL 的功能。一个大概的表格可以看 <https://en.cppreference.com/w/cpp/compiler_support>，如果需要比较精确的结果，还是需要去看编译器厂商提供的官方文档。

所以有的时候为了去补足编译器内置提供的 STL 的功能缺陷，也会出现这样的一些东西：

- <https://source.chromium.org/chromium/chromium/src/+/9d5aa289d49ad2c9068dec6f3c55a30938be01f9:base/template_util.h;drc=221e331b49dfefadbc6fa40b0c68e6f97606d0b3>
- <https://abseil.io/docs/cpp/guides/meta>
- <https://abseil.io/docs/cpp/guides/strings>

我们用的时候可以先正常用，如果发现有问题排查确认不是自己使用的错误之后，记得可以再去找一下是不是编译器厂商提供的 STL 库有问题。

## 容器

### Java 容器和 C++ 容器对应关系

大概的对应关系如下表所示，还有一份从 C++ 角度进行分类的表格见 <https://en.cppreference.com/w/cpp/container>

| Java | C++ | 备注 |
|---------|---|---|
| `Tuple` | `std::tuple` | 强烈建议只把 `tuple` 作为内部存储，不要暴露到接口上，因为别人很难理解 `tuple` 里面装的到底是什么东西，更推荐定义 `struct` 来表达含义 |
| 编译时期确定长度的定长数组 | `std::array` |  |
| 编译时期不确定长度的定长数组 | `std::unique<T[]>`, `absl::FixedArray`, 或者 `std::vector` | `std::vector` 实际上是把变长数组当定长数组使用 |
| `String` | 无（或者 `icu::UnicodeString`） | 特别注意：`std::string` 只是个 `char` 数组，没有提供 encoding 相关的功能 |
| `ArrayList` | `std::vector` | 注意：`std::vector<bool>` 不是普通的 `ArrayList`；如需位集合请使用 `std::bitset`；如需 `bool` 数组请使用 `std::unique_ptr<bool[]>` |
| `LinkedList` | `std::list` |  |
| 没有（单向链表） | `std::forward_list` |  |
| `Stack` | `std::stack` |  |
| 类似于 `Queue` 接口 | `std::queue` |  |
| `Deque` | `std::deque` |  |
| `PriorityQueue` | `std::priority_queue` |  |
| 没有（环形存储上的队列，不太清楚 Netty 里是不是有） | 无，或参考 Chromium 的 `circular_deque` | <https://source.chromium.org/chromium/chromium/src/+/master:base/containers/circular_deque.h;drc=fc269309730220a159e1d7cd96e4672a1e216eb2> |
| `HashSet` | `std::unordered_set`, `absl::flat_hash_set` |  |
| `TreeSet` | `std::set`, `absl::btree_set` |  |
| 没有（或者 Guava `Multiset`） | `std::multiset`, `absl::btree_multiset` |  |
| `HashMap` | `std::unordered_map`, `absl::flat_hash_map` |  |
| `TreeMap` | `std::map`, `absl::btree_map` |  |
| `LinkedHashMap` | 无（可用 `std::vector` + `std::unordered_map` + 自定义结构 实现） |  |
| 没有（或者 Guava `Multimap`） | `std::multimap`, `absl::btree_multimap` |  |
| `ConcurrentXXX` | 无，或 `folly::ConcurrentXXX` |  |

### 迭代器

C++ 中经常使用迭代器表示容器中的一个区间（C++ 20 之后有 Ranges 库了，但是在其大规模落地之前，还是得熟悉迭代器表示法）。通常使用一对迭代器表示一个左闭右开的区间，例如 [a, b)。基于这种假设，C++ 中的迭代器允许向后越过集合中的最后一个元素（即索引位置为 `len(c)` 的地方），但是不允许向前越过集合中的第一个元素（即索引位置为 `-1` 的地方）。

C++ 中还提供了反向迭代器用来进行反向遍历，即 `rbegin()` 和 `rend()`。可以通过调用其 `base()` 方法将其转为正向迭代器。通过调用 `std::make_reverse_iterator` 将正向迭代器转换为反向迭代器。这些不变式是成立的：

```cpp
--8<-- ".snippets/standard-library/001-iterator-reverse.cc:code"
```

C++ 中的迭代器也是有着类别的。这主要是因为不同的容器中的数据的内存排列形式不同，有的可以高效的进行随机访问（比如数组），有的则不行（比如链表）；有的可以前进后退，有的只能前进（比如单向链表）。一个详细的列表和解释见 <https://en.cppreference.com/w/cpp/iterator>。

/// admonition | 注意
类似于 `java.util.ConcurrentModificationException`，如果在使用迭代器的过程中修改了集合的内容（比如说添加/删除元素等等），可能会导致迭代器失效（即其指向的位置不再合法），但是这一行为不会导致异常，只会导致 Undefined Behavior，一定要从逻辑上避免。
///

### `std::vector` 一些需要注意的点

首先是传参的时候要用引用或者指针，避免复制整个容器，这点是跟 Java 很不一样的。

```cpp
--8<-- ".snippets/standard-library/002-vector-pass-by-ref.cc:code"
```

接下来要注意 `std::vector<bool>` 内部其实是个 bitmap 而不是一个数组，这是历史遗留问题。如果就是想要 bitmap，最好明确的使用 `std::bitset`；如果是想要使用 `bool` 数组，可以使用 `std::unique_ptr<bool[]>` 或者 `absl::FixedArray<bool>`。

最后要注意的是一个性能问题，如果有可能的话，调用 `reserve` 方法提前申请内存空间：

```cpp
--8<-- ".snippets/standard-library/003-vector-reserve.cc:code"
```

/// admonition | 注意
不要滥用 `reserve()`，特别是在循环中调用 `reserve()`。要在循环外面事先计算好需要的容量，然后调用 `reserve()`，而不是循环中每次插入前都调用 `reserve()`。
///

### Contains 判断容器内是否存在指定元素

直到 C++20 和 C++23 才给各个容器类添加了 `contains` 方法。在这之前，我们得使用这样的形式来判断容器是否包含某元素：

```cpp
--8<-- ".snippets/standard-library/004-contains-unordered-map.cc:code"
```

但是要注意的是，所有 `std::string` 类的容器（比如说还有 `std::string_view`，`std::wstring` 等等），都没有使用找不到元素时返回 `end()` 位置的约定，而是搞了一个 `npos` 常量：

```cpp
--8<-- ".snippets/standard-library/005-contains-string.cc:code"
```

这主要是因为在 `std::string` 进入标准时还没有迭代器这么个东西。

如果觉得很不爽，可以用这样的一套辅助工具方法：<https://source.chromium.org/chromium/chromium/src/+/9d5aa289d49ad2c9068dec6f3c55a30938be01f9:base/containers/contains.h>

### 安全表示一个容器中的区间（且不拷贝数据）

尽管我们可以用一对迭代器来表示一个容器中的区间，但是用起来并不那么方便，特别是我们更多的是把这个区间当作一个容器来用。在这种情况下，我们有 `absl::string_view`（即 C++17 中的 `std::string_view` ）和 `absl::Span`（即 C++20 中的 `std::span`）。

使用这两个容器的时候要特别注意，它们由于没有拷贝数据，所以其内容是否合法取决于原位置数据的合法性。

```cpp
--8<-- ".snippets/standard-library/006-string-view-lifetime.cc:code"
```

另外值得一提的是，`absl::string_view` 既可以接受 `std::string` 也可以接受 C-style string 即 `char*`，因此在有些情况下（提供字符串字面量）的时候，性能可能比 `const std::string&` 更高。所以如果比较合适的，建议在参数中使用 `absl::string_view` 而非 `const std::string&`。

但是另一个要注意的点是，有些系统调用是要求提供 C-Style string 的，例如 `::open()`。这时候不能直接给 `absl::string_view::data()`，因为它可能不是以 `'\0'` 结尾的。

```cpp
--8<-- ".snippets/standard-library/007-open-read-file.cc:code"
```

这种情况下还是用 `const std::string&` 更高效。

### Ropes: an Alternative to Strings

简单来说，使用 C 传统的方式存储一个字符串在有些（可能还比较常见的）场景下，性能是比较低的。如果使用其他数据结构存储字符串，会对性能有比较大的影响。详情见论文 *Ropes: an Alternative to Strings*（或者一个简单的介绍 <https://www.wikiwand.com/en/Rope_(data_structure)>）。如果想要使用这样的数据结构，可以考虑 abseil 库的 `cord` 容器（<https://github.com/abseil/abseil-cpp/blob/cba8cf8/absl/strings/cord.h>）。

### 字符串 StartsWith/EndsWith/Strip/...？

上面提到过了，C++ 中 `std::string` 固定了一块内存空间，而且由于 C++ 中没有 GC 机制，所以没法高效的完成 strip 之类的操作。所以如果需要 strip 这样的函数，需要使用 `std::string_view`（有 `remove_prefix` 和 `remove_suffix`）。此外 Abseil 提供了 `absl::StripPrefix` 和 `absl::StripSuffix`。

`StartsWith`/`EndsWith` 不知道为什么一直要到 C++20 才提供。可以使用 `absl::StartsWith` 和 `absl::EndsWith`（以及 Ignore Case 的版本）。

C++ 标准提供的字符串 `contains` 甚至要到 C++23。可以使用 `absl::StrContains`。

### 怎么查询一个 const referenced map？

有的同学可能遇到过这样的困惑：

```cpp
--8<-- ".snippets/standard-library/018-unordered-map-bad-lookup.cc:code"
```

会产生如下编译错误：

<!-- markdownlint-disable MD013 -->
```text
--8<-- ".snippets/standard-library/017-unordered-map-operator-error.txt:code"
```
<!-- markdownlint-enable MD013 -->

这是因为 map 的 `operator[]` 会在查不到 key 的时候自动为其插入一个 default value（详情见 <https://en.cppreference.com/w/cpp/container/unordered_map/operator_at>），因此在 `const std::unordered_map&` 中无法调用到 `operator[]`。

解决方法就是使用 `contains` 方法（C++20 以后），或者是 `find()` 方法。

```cpp
--8<-- ".snippets/standard-library/019-unordered-map-find.cc:code"
```

### 避免在严肃的用途使用 `iostream` 库提供的组件

关于 `iostream` 的一些问题见 [C++ 工程实践(7):iostream 的用途与局限 - 陈硕 - 博客园](https://www.cnblogs.com/Solstice/archive/2011/07/17/2108715.html)。

关于文件接口的替代方案是从 [LevelDB](https://github.com/google/leveldb/blob/f57513a1d6c99636fc5b710150d0b93713af4e43/include/leveldb/env.h#L74) 或 [TensorFlow](https://github.com/tensorflow/tensorflow/blob/73d09c1434aec3d1e10ff7b2e44020741bccb552/tensorflow/core/platform/file_system.h#L56) 的相关代码拿来修改一下使用。

关于字符串格式化相关的替代方案是使用 [fmt 库](https://fmt.dev/latest/index.html)或者 abseil 库提供的相关功能（`absl::StrFormat()` 系列或 `absl::Substitute()`）。

### 自定义 allocator 和 pmr 容器

STL 容器允许自定义 allocator 来改变其内存分配方式。这有时候是很有用的，比如说在查询分析场景下，可能就是一个 query 的生命周期内用一个 allocator，最后统一释放内存比较好。但是大多数情况下我们不需要去操心这个事情。

默认的 allocator 就是使用 `malloc` 和 `free` 函数进行内存分配的。glibc 默认提供的这两个函数比较慢，一般我们都建议链接 [jemalloc](https://github.com/jemalloc/jemalloc)/[gperftools tcmalloc](https://github.com/gperftools/gperftools)/[google tcmalloc](https://github.com/google/tcmalloc)/[microsoft mimalloc](https://github.com/microsoft/mimalloc) 之类的库来提高性能。注意这提高的不仅是容器的性能，还包括其他各种 `new`/`make_shared`/... 之类的性能。

如果链接了 jemalloc 还不能解决问题，确实需要自定义 allocator 的话，可以参考这个指南：<https://docs.microsoft.com/en-us/cpp/standard-library/allocators?view=msvc-160>。

在使用自定义 allocator 时，特别需要注意的问题是 `std::vector<int>` 和 `std::vector<int, MyAllocator>` 是 2 个完全不同的类型。这样用起来很不方便，C++ 后来为了解决这个问题搞了 pmr 容器（需要 C++17 以后）。简单来说，实现 `std::pmr::polymorphic_allocator`，并且使用 `std::pmr::vector` 就行了。

### Ranges 库

类似于 Java Stream API，需要等到 C++20 才有，可以提前在支持 C++17 标准的编译器上，通过这个库使用：<https://github.com/ericniebler/range-v3>

## 算法

STL 中包括了一些常见的算法。我个人觉得比较常用的有这些：

1. 二分搜索：`lower_bound`/`upper_bound`
1. 最大最小值：`min_element`/`max_element`/`minmax_element`
1. 数值计算结果限制在上下界之间：`clamp`
1. 删掉容器中所有符合条件的元素：`remove_if`

这里我就不一一详细介绍了，可以去 <https://en.cppreference.com/w/cpp/algorithm> 这里看一下。这里稍微说一下几个比较常见的 tricky 的用法。

### 找到有序容器中最后一个不大于某元素的位置

> 需要建立在全序关系之上

首先我们有这样 2 个二分算法（假设比较函数都是 `std::less<>`，容器中的元素是按照比较函数即升序排序的）：

1. `lower_bound`：找到第一个不小于给定值的位置

1. `upper_bound`：找到第一个大于给定值的位置

我们想要找到最后一个不大于给定值的位置，就得沿着容器反着找。把容器反序来看，是一个降序序列，正序找到最后一个不大于给定值的位置，即反序找到第一个不大于给定值的位置，应该使用 `lower_bound`：

```cpp
--8<-- ".snippets/standard-library/008-find-last-le-not-working.cc:code"
```

这么说起来可能有些绕，用数学语言描述可能更容易理解一些。

### 删掉容器中所有符合条件的元素

> <https://www.wikiwand.com/en/Erase%E2%80%93remove_idiom>

需要注意 `remove_if` 只是将符合条件的元素挪到容器末尾处，而没有真正删除这些元素。所以需要这样的模式调用：

```cpp
--8<-- ".snippets/standard-library/009-erase-remove-idiom.cc:code"
```

## 随机数

/// admonition | 注意
如果需要密码学安全的随机数，务必不要使用这种方法。请使用已知的密码学安全的算法去产生符合要求的随机数，例如通过 OpenSSL 库生成（见 <https://www.openssl.org/docs/man1.1.1/man3/RAND_bytes.html>）。
///

Java 中的随机数比较简单，就是调用 `Random` 类生成就行了。但是其实有些场景下需要关注并控制更多细节。简单来说，分为这样几个东西：

1. 伪随机数生成算法（更确切地，在 C++ 中是 `UniformRandomBitGenerator`）
1. 伪随机数生成算法所需的种子
1. 生成的随机数的概率分布

一个比较全面的介绍见 <https://en.cppreference.com/w/cpp/header/random>。这里简单给个例子：

```cpp
--8<-- ".snippets/standard-library/010-random-example.cc:code"
```

这里不能简单地用时间来初始化随机数生成器，安全性较差（对，说的就是你—— `srand(time(NULL))`）。一般是从系统硬件熵池中取一小段数据出来初始化随机数生成器，这样比较安全。这里也不能简单地使用 `% 6` 的方法来产生 [0, 5] 区间的整数，因为这样产生的分布不是均匀的（可以试着计算一下 [1, 8] 映射到 [1, 6]，简单取模的话概率分布是什么样子的）。

C++ 标准库中提供的 random 接口能力足够强大，但是用起来还是不太爽，Abseil 库提供的 Random 功能要更合理一些：

1. 有些算法不能简单地用 `rd()` 生成的 `unsigned int` 来初始化，需要更长的种子启动
1. 不太容易搞明白/指定随机分布的区间（左闭右开，左开右闭，等等）
1. 对测试不友好，需要自己实现一个完全符合 C++ 标准库要求的伪随机数生成器

Abseil 库提供的 Random 的介绍见 <https://abseil.io/docs/cpp/guides/random>。

如果需要 `ThreadLocalRandom` 的话，可以简单的通过添加 `thread_local` 修饰符来解决这一问题，详情见后面多线程部分关于 `thread_local` 的介绍。

## 日期和时间

/// admonition | 注意
这里假设读者了解 Joda Time 或者 Java 8 新加入的 Date API。
///

首先我们要先掰开这几个概念：

1. Clock：时钟，由时钟产生当前时间点，或者历史的某个时间点
1. Timepoint：某个确定的时间点，由时钟产生
1. Duration：两个 Timepoint 之间的距离

Java 中隐含 Clock 都是系统时钟，但是这个设定在某些场景下可能未必合理（但是大多数情况下是合理的），例如有的场景需要时钟是单向增长不会回拨的。时钟什么时候会回拨呢，比如说 NTP 发现时钟跑的太快了，进行同步的时候可能就会将时间往回拨；但是另一种选择就是不回拨时间，只把时钟变慢，然后过一段时间再恢复正常。
Timepoint 可能携带时区信息（例如 `java.time.ZonedDateTime`）或者不携带时区信息（`java.time.LocalDateTime`）。

C++ 标准库中提供了 3 种时钟：

1. `system_clock`：系统时钟，类似于 `java.time.Clock`
1. `steady_clock`：保证不会发生回拨的时钟
1. `high_resolution_clock`：高精度时钟，取决于实现

在 C++ 标准库中，将 Clock 类型放到了类型系统中，这是好的且很有意义的——在 2 个不同的时钟产生的时间之间进行比较是没有意义的。但是带来的副作用就是产生的 timepoint 的类型比较复杂，比如说可能是 `std::chrono::time_point<std::chrono::steady_clock>`。而且比较蛋疼的是如果我们要更换时钟的时候，比如说将所有的 `system_clock` 换成 `steady_clock`，还得改所有的 timepoint，特别是这些 timepoint 类型还可能出现在方法的参数上面。

在 C++ 标准库中，将时间单位也放到了 duration 的类型系统中，个人感觉在大部分常见的体系结构（特别是 64 位体系结构下）下这样做不是很有意义。

> BTW，如果想要使用其他物理量可以看一下 <https://github.com/mpusz/units>

```cpp
--8<-- ".snippets/standard-library/011-chrono-measure-time.cc:code"
```

由于我们经常需要和 epoch 时间打交道，因此特别提供了相关的转换方法：

- epoch time -> chrono time：先构造 duration，然后调用 timepoint 的构造函数 https://en.cppreference.com/w/cpp/chrono/time_point/time_point
- chrono time -> epoch time：<https://en.cppreference.com/w/cpp/chrono/time_point/time_since_epoch>
总的来说我觉得 chrono 设计考虑的比较全面，但是用起来不是很爽。比较推荐使用 Abseil 库提供的时间 API：https://abseil.io/docs/cpp/guides/time。Abseil 库的时间 API 也是假设 Clock 就是 System Clock，没有把时间单位弄到类型系统里面，所以最后搞出来的东西就比较简单，用起来比较方便。

/// admonition | 注意
在严肃的场景下考虑如何解析字符串表示的日历时间，并转换成一个系统绝对时间，应该使用 Abseil 库中的 Civil Time 或者具有类似功能的库。处理各种地区的特殊情况是非常复杂（比如说夏令时，特殊润秒规则等等）的问题，一定不要凭感觉手工去做。
///

## 多线程和并发控制

基本上可以认为 Linux 环境下 C++ 中的线程就是对 pthread 原语的一些包装，因此一些线程相关的问题最好先看看 [pthread 的 man 手册](https://linux.die.net/man/7/pthreads)。

### Sleep

```cpp
--8<-- ".snippets/standard-library/012-sleep-examples.cc:code"
```

但是睡过去容易醒过来难，如有可能，强烈建议考虑是不是应该传一个 `absl::Notification` 进来，然后使用 `notification.WaitForNotificationWithTimeout(kSleepInterval)`。具体例子见后面“常见编程 Pattern”里面的“后台线程周期性活动”。

### 线程

简单来说，创建一个 `std::thread` 实例的时候就会创建一个线程并立刻执行给定的函数。

/// admonition | 注意
这里没有机会在启动线程前执行 pthread 的一些初始化工作。如果有这方面的需要，应该手工包装一个 Thread 类出来使用，例如 <https://source.chromium.org/chromium/chromium/src/+/main:base/threading/platform_thread_posix.cc;drc=37a0e6c0a6a13a078d08f51faf276c4eb4f0ef1a>
///

但是可以在后续时间通过 `native_handle` 方法拿到其 pthread 句柄，并进行后续操作。常见的一些工作可能包括绑核（`pthread_setaffinity_np`），设置线程名（`pthread_setname_np`），设置优先级（`pthread_setschedparam`）等等。

/// admonition | 注意
`std::thread` 实例析构时，要求析构前一定调用过其 `join` 方法或者 `detach` 方法。如果不满足要求的话，程序会直接 core 掉。
///

一般来说，我们在严肃的场合下使用线程，都需要考虑程序退出时这些线程怎么处理的问题。在这里，我建议给后台线程传入一个额外的 `absl::Notification` 参数，用于通知后台线程结束。这样我们就可以 `join` 这个后台线程，等到它跑到一个合适的时机再退出整个程序了（graceful shutdown）。

### 线程池

简单来说，线程池还没有进入 C++ 标准（估摸着 C++ 23 会进，因为要统筹考虑协程和 future/promise 等机制的配合，所以得先等他们进标准）。如果要用的话，可以考虑：

1. Boost 库中的 `boost::asio::io_service`
<!-- markdownlint-disable-next-line MD033 -->
1. <del style="color:#666;">公司内 noodle 库中有一个线程池</del>
1. 自己写一个（后面这几个我都没用过）
1. <https://chriskohlhoff.github.io/executors/>
1. <https://github.com/lewissbaker/cppcoro>
1. <https://github.com/Quuxplusone/coro>

另外还可以考虑的是使用 dataflow 模式进行编程，而不直接裸用线程池，比如说：

1. 微软的 PPL：<https://docs.microsoft.com/en-us/cpp/parallel/concrt/parallel-patterns-library-ppl?view=msvc-160>（可以在 cpprestsdk 中找到一个跨平台的版本 pplx：<https://github.com/microsoft/cpprestsdk>）
1. Intel TBB：<https://software.intel.com/content/www/us/en/develop/documentation/onetbb-documentation/top.html>

### 锁

在 C++ 中不存在 `synchronized` 关键字，需要自己手工控制加锁。

| Java 类型 | C++ 类型 | 备注 |
| --- | --- | --- |
|     | `std::mutex`，`absl::Mutex` | `std::recursive_mutex` 的不可重入版本 |
|     | `std::shared_mutex`，`absl::Mutex` | `ReentrantReadWriteLock` 的不可重入版本 |
| `java.util.concurrent.locks.ReentrantLock` | `std::recursive_mutex` | |
| `java.util.concurrent.locks.ReentrantReadWriteLock` | 无 | |

在 Java 中一般使用这样的手法来确保锁被释放了：

```java
--8<-- ".snippets/standard-library/013-java-lock-example.java:code"
```

在 C++ 中一般使用这样的手法：

```cpp
--8<-- ".snippets/standard-library/014-cpp-lock-example.cc:code"
```

```cpp
--8<-- ".snippets/standard-library/015-mutable-mutex-example.cc:code"
```

### 原子访问/`volatile` (Updated 2022-02-14)

/// admonition | 注意
C++ 的 `volatile` 语义和 Java 的 `volatile` 不一样。简单来说，不要试图通过使用 `volatile` 关键字来做原子访问。
///

C++ 的原子访问比较简单，就是使用 `std::atomic<>` 泛型类包装一下你的类型就行了。但是这要求被包装的类必须满足限制条件 `TriviallyCopyable`，`CopyConstructible`，`CopyAssignable`。但是通常我们也不会用 atomic 去包装自定义的非数值类型，暂时不去管这个事情。

简单来说你可以用 `std::atomic<int>`，`std::atomic<int32_t>`，`std::atomic<double>` 之类的类型；而不必使用 `std::atomic_int` 之类的类型。他们是等价的。

/// admonition | 注意
这里特别说一下的事情是，如果对原子变量的操作在关键路径上，并且成为瓶颈了，可以通过放宽一致性要求来得到更好的性能。但是如果弄不明白就别搞这个事情了，不如 profiling 一下找找真正的瓶颈在哪儿。
///

简单的来说，对于多个原子变量的访问，如果需要按照写入的顺序读到他们的更改，需要在写的时候（`store` 方法）指定 `std::memory_order_release`，在读的时候（`load` 方法）指定 `std::memory_order_acquire`。不指定的话，默认使用 `std::memory_order_seq_cst`，是强一致性。详细介绍见 <https://en.cppreference.com/w/cpp/atomic/memory_order>，非常复杂，不建议一般程序员了解。

简单来说，用 atomic 的时候，如果在意性能，可以在所有的写操作使用 `release` 标记，在所有读操作使用 `acquire` 标记。最近看到了一个比较好的解释，见 <http://bluehawk.monmouth.edu/~rclayton/web-pages/u03-598/netmemcon.html>。

### `thread_local`

C++ 中不需要进行特殊的处理，或者是类型包裹（比如说 `java.lang.ThreadLocal`），只需要在声明变量的时候加上 `thread_local` 修饰符就可以声明一个 Thread-Local 变量了。需要注意的是：

1. 有些操作系统（比如说 Android 的一些版本）声明很多 Thread-Local 变量的时候会有问题。（<https://source.chromium.org/chromium/chromium/src/+/main:base/threading/thread_local.h;l=28;drc=7b5337170c1581e4a35399af36253f767674f581>）
1. 如果 Thread-Local 变量比较大的话，在内核比较多的机器上有可能会占用很多内存。（这是个程序设计问题，不是 C++ 自身的问题，也不是操作系统的问题）

```cpp
--8<-- ".snippets/standard-library/016-thread-local-example.cc:code"
```

### 信号量/Latch/Barrier/条件变量

信号量（`java.util.concurrent.Semaphore`）至少需要到 C++20 才能进标准库。Latch 和 Barrier 也需要等到 C++20 以后。原则上信号量在合适的使用场景下可以比条件变量（`java.util.concurrent.locks.Condition`）的性能更好一些。

条件变量是能力比较强大的一种同步模型，基本上可以用来模拟实现上面缺失的几种元素。而且如果使用场景比较复杂的话，用户应该探索一下，是不是用条件变量可以更高效地实现所需的同步功能。这里就不详细去做条件变量的教学了。

| 名称 | Java 类型 | C++ 类型 |
| --- | --- | --- |
| 信号量 | `java.util.concurrent.Semaphore` | `std::counting_semaphore`（C++20） |
| Latch | `java.util.concurrent.CountDownLatch` | `std::latch`（C++20），`absl::BlockingCounter` |
| Barrier | `java.util.concurrent.CyclicBarrier` | `std::barrier`（C++20），`absl::Barrier` |
| 条件变量 | `java.util.concurrent.locks.Condition` | `std::condition_variable`，`absl::CondVar` |

值得一提的是，`absl::Mutex` 提供了使用条件变量的一种简化形式（解锁的时候自动 Notify），具体介绍可以看 <https://abseil.io/docs/cpp/guides/synchronization#conditional-mutex-behavior>。

### `std::call_once`

在多线程并发的情况下也能保证被保护的函数只被调用一次。特别的，被保护的函数抛异常的时候，允许再次执行这个函数，直到这个函数正常执行完过。

<https://en.cppreference.com/w/cpp/thread/call_once>

但是创建一个 lazily initialized singleton 的时候不需要这样做，见后面 “常见编程 Pattern”。

### Future/Promise 模型

C++ 当前提供的 `std::future`/`std::promise` 机制比较简单，基本上干不了什么事（甚至都不能用 `then` 串起来）。已经有新的标准提案改进这个事情，但是需要等协程等改动先进标准，所以大概需要等到 C++23 才能用上。具体情况可以看一下这个文档：<https://www.modernescpp.com/index.php/the-end-of-the-detour-unified-futures>。

如果当前想要用现成的，稍微功能齐全一些的 future/promise，可以看一下 `folly::Future`/`folly::Promise`：<https://github.com/facebook/folly/blob/16a7084/folly/docs/Futures.md>。以及这个（是个标准提案的 POC，不知道性能会不会有问题）：<https://chriskohlhoff.github.io/executors/>。

> 补充说明一下，这里说的功能不全面指的是不能把多个任务串起来执行，典型的类似于 nodejs 里面的 callback 火箭。更先进的做法可能是通过 `await`/`async` 这样的关键字把多个任务通过协程之类的方式串起来，典型的类似于 C# 中的 `await`/`async` 关键字（现在 Python 好像也有了）。目前前者已经可以通过 `Folly::Future` 这样的功能补全，在新的 C++ 标准中也可以通过 `then()` 或者 `transform()` 之类的方法实现了。后者需要协程库甚至编译器的支持，可能需要等标准更明确一些才能继续讨论。
>
> 目前已经有的一些提案见 P0443 和 P1897。

新加（2021-08-03）一个扩展阅读：[浅谈 The C++ Executors](https://zhuanlan.zhihu.com/p/395250667)。

### 并发访问错误静态分析

/// admonition | 注意
简单易用，功能强大，强烈推荐！
///

这两个文档有比较详细的介绍，我就不赘述了。

- [Thread Safety  Annotations for Clang.pdf](https://llvm.org/devmtg/2011-11/Hutchins_ThreadSafety.pdf)
- [Thread Safety Analysis](https://clang.llvm.org/docs/ThreadSafetyAnalysis.html)

唯一的问题是，怎么让你的项目可以被 clang 编译。这里提供一个思路：

1. 如果你用了 Ninja，可以导出一个 `compile_commands.json` 文件，这个文件挺有用的，一般可以用来支持 clangd 提供 LSP 提示
1. 然后自己写个脚本处理一下这个文件生成对应的检查脚本（比如说可以是 makefile 或者是 build.ninja 文件，甚至是 bash 脚本）

如果你使用 `absl::Mutex`，`ABSL_GUARDED_BY` 之类的东西，就不用再自己去包装这些标注了。

### 并发访问错误动态检查

见 <https://clang.llvm.org/docs/ThreadSanitizer.html>

### 线程安全容器

C++ 标准库没有内置提供线程安全的容器，但是一些比较著名的第三方库提供了这样的功能：

- [Intel TBB Containers](https://software.intel.com/content/www/us/en/develop/documentation/tbb-documentation/top/intel-threading-building-blocks-developer-guide/containers.html)
- [Boost Lockfree](https://www.boost.org/doc/libs/1_76_0/doc/html/lockfree.html)
- [Folly ConcurrentHashMap](https://github.com/facebook/folly/blob/master/folly/concurrency/ConcurrentHashMap.h)

在用这些容器之前要三思：

1. 线程安全只能 end-to-end 去设计和实现，多个模块内部的线程安全组合起来不一定是线程安全的
1. 无锁容器未必比有锁容器快
