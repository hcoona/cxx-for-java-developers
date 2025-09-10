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
EXPECT_TRUE(std::make_reverse_iterator(it).base() == it);
EXPECT_TRUE(v.rbegin().base() == v.end());
EXPECT_TRUE(v.rend().base() == v.begin());
```

C++ 中的迭代器也是有着类别的。这主要是因为不同的容器中的数据的内存排列形式不同，有的可以高效的进行随机访问（比如数组），有的则不行（比如链表）；有的可以前进后退，有的只能前进（比如单向链表）。一个详细的列表和解释见 <https://en.cppreference.com/w/cpp/iterator>。

/// admonition | 注意
类似于 `java.util.ConcurrentModificationException`，如果在使用迭代器的过程中修改了集合的内容（比如说添加/删除元素等等），可能会导致迭代器失效（即其指向的位置不再合法），但是这一行为不会导致异常，只会导致 Undefined Behavior，一定要从逻辑上避免。
///

### `std::vector` 一些需要注意的点

首先是传参的时候要用引用或者指针，避免复制整个容器，这点是跟 Java 很不一样的。

```cpp
int sum(const std::vector<int>& v);

// !!! DON'T DO THIS !!!
int sum_copy_whole_vector(std::vector<int> v);
```

接下来要注意 `std::vector<bool>` 内部其实是个 bitmap 而不是一个数组，这是历史遗留问题。如果就是想要 bitmap，最好明确的使用 `std::bitset`；如果是想要使用 `bool` 数组，可以使用 `std::unique_ptr<bool[]>` 或者 `absl::FixedArray<bool>`。

最后要注意的是一个性能问题，如果有可能的话，调用 `reserve` 方法提前申请内存空间：

```cpp
absl::Status ReadFeature(ByteBuffer* buffer, std::vector<int32_t>* feature) {
  int64_t value_count;
  RETURN_IF_NOT_OK(buffer->TryReadInt64(&value_count));

  feature->reserve(static_cast<size_t>(value_count));
  for (int64_t i = 0; i < value_count; i++) {
    int32_t v;
    RETURN_IF_NOT_OK(buffer->TryReadInt32(&v));
    feature->emplace_back(v);
  }

  return absl::OkStatus();
}
```

/// admonition | 注意
不要滥用 `reserve()`，特别是在循环中调用 `reserve()`。要在循环外面事先计算好需要的容量，然后调用 `reserve()`，而不是循环中每次插入前都调用 `reserve()`。
///

### Contains 判断容器内是否存在指定元素

直到 C++20 和 C++23 才给各个容器类添加了 `contains` 方法。在这之前，我们得使用这样的形式来判断容器是否包含某元素：

```cpp
bool Contains(const std::unordered_map<int, int>& map, int target) {
  auto it = map.find(target);
  // end() pos means not found.
  return it != map.end();
}
```

但是要注意的是，所有 `std::string` 类的容器（比如说还有 `std::string_view`，`std::wstring` 等等），都没有使用找不到元素时返回 `end()` 位置的约定，而是搞了一个 `npos` 常量：

```cpp
bool Contains(const std::string& str, char target) {
  auto idx = str.find(target);
  return idx != std::string::npos;
}
```

这主要是因为在 `std::string` 进入标准时还没有迭代器这么个东西。

如果觉得很不爽，可以用这样的一套辅助工具方法：<https://source.chromium.org/chromium/chromium/src/+/9d5aa289d49ad2c9068dec6f3c55a30938be01f9:base/containers/contains.h>

### 安全表示一个容器中的区间（且不拷贝数据）

尽管我们可以用一对迭代器来表示一个容器中的区间，但是用起来并不那么方便，特别是我们更多的是把这个区间当作一个容器来用。在这种情况下，我们有 `absl::string_view`（即 C++17 中的 `std::string_view` ）和 `absl::Span`（即 C++20 中的 `std::span`）。

使用这两个容器的时候要特别注意，它们由于没有拷贝数据，所以其内容是否合法取决于原位置数据的合法性。

```cpp
absl::string_view str_view;
{
  std::string str = "Hello World!";

  str_view = str;
  LOG(INFO) << str_view;  // Valid here.

  str = "";
  LOG(INFO) << str_view;  // Invalid here!

  str_view = str;  // Valid again.
}
// Invalid again because |str| is destructed.
```

另外值得一提的是，`absl::string_view` 既可以接受 `std::string` 也可以接受 C-style string 即 `char*`，因此在有些情况下（提供字符串字面量）的时候，性能可能比 `const std::string&` 更高。所以如果比较合适的，建议在参数中使用 `absl::string_view` 而非 `const std::string&`。

但是另一个要注意的点是，有些系统调用是要求提供 C-Style string 的，例如 `::open()`。这时候不能直接给 `absl::string_view::data()`，因为它可能不是以 `'\0'` 结尾的。

```cpp
int OpenReadFile1(absl::string_view filename) {
  // Must copy it to append \0 at the end.
  std::string copied_filename(filename);
  return ::open(copied_filename.c_str());
}
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
int F(const std::unordered_map<int, int>& m, int t) {
  return m[t];
}
```

会产生如下编译错误：

<!-- markdownlint-disable MD013 -->
```text
a.cc:4:10: error: no viable overloaded operator[] for type 'const std::unordered_map<int, int>'
        return m[t];
               ~^~
/usr/bin/../lib/gcc/x86_64-linux-gnu/10/../../../../include/c++/10/bits/unordered_map.h:983:7: note: candidate function not viable: 'this' argument has type 'const std::unordered_map<int, int>', but method is not marked const
      operator[](const key_type& __k)
      ^
/usr/bin/../lib/gcc/x86_64-linux-gnu/10/../../../../include/c++/10/bits/unordered_map.h:987:7: note: candidate function not viable: 'this' argument has type 'const std::unordered_map<int, int>', but method is not marked const
      operator[](key_type&& __k)
      ^
1 error generated.
```
<!-- markdownlint-enable MD013 -->

这是因为 map 的 `operator[]` 会在查不到 key 的时候自动为其插入一个 default value（详情见 <https://en.cppreference.com/w/cpp/container/unordered_map/operator_at>），因此在 `const std::unordered_map&` 中无法调用到 `operator[]`。

解决方法就是使用 `contains` 方法（C++20 以后），或者是 `find()` 方法。

```cpp
int F(const std::unordered_map<int, int>& m, int t) {
  auto it = m.find(t);
  if (it == m.end()) {
    return -1;
  }

  return it->second;
}
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
auto it = std::lower_bound(c.rbegin(), c.rend(), std::greater<>());
```

这么说起来可能有些绕，用数学语言描述可能更容易理解一些。

### 删掉容器中所有符合条件的元素

> <https://www.wikiwand.com/en/Erase%E2%80%93remove_idiom>

需要注意 `remove_if` 只是将符合条件的元素挪到容器末尾处，而没有真正删除这些元素。所以需要这样的模式调用：

```cpp
std::vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
v.erase(std::remove_if(v.begin(), v.end(), IsOdd), v.end());
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
// Will be used to obtain a seed for the random number engine
std::random_device rd;

// Standard mersenne_twister_engine seeded with |rd()|.
std::mt19937 gen(rd());

// Produce uniform distribution for range [1, 6] (both boundary inclusive).
std::uniform_int_distribution<> distrib(1, 6);

v->reserve(n);
for (int i = 0; i < n; i++) {
  v->emplace_back(distrib(gen));
}
```

这里不能简单地用时间来初始化随机数生成器，安全性较差（对，说的就是你—— `srand(time(NULL))`）。一般是从系统硬件熵池中取一小段数据出来初始化随机数生成器，这样比较安全。这里也不能简单地使用 `% 6` 的方法来产生 [0, 5] 区间的整数，因为这样产生的分布不是均匀的（可以试着计算一下 [1, 8] 映射到 [1, 6]，简单取模的话概率分布是什么样子的）。

C++ 标准库中提供的 random 接口能力足够强大，但是用起来还是不太爽，Abseil 库提供的 Random 功能要更合理一些：

1. 有些算法不能简单地用 `rd()` 生成的 `unsigned int` 来初始化，需要更长的种子启动
1. 不太容易搞明白/指定随机分布的区间（左闭右开，左开右闭，等等）
1. 对测试不友好，需要自己实现一个完全符合 C++ 标准库要求的伪随机数生成器

Abseil 库提供的 Random 的介绍见 <https://abseil.io/docs/cpp/guides/random>。

如果需要 `ThreadLocalRandom` 的话，可以简单的通过添加 `thread_local` 修饰符来解决这一问题，详情见后面多线程部分关于 `thread_local` 的介绍。

## 日期和时间

/// admonition | TODO
    type: todo
待转录
///

## 多线程和并发控制

/// admonition | TODO
    type: todo
待转录
///
