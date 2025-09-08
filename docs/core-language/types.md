<!-- SPDX-License-Identifier: CC-BY-NC-ND-4.0 -->

# 类型

## 基础类型

（面向 x86_64 + Linux + GCC 的常见环境）Java 与 C++ 常见基础类型对照：

| Java 类型 | C++ 新类型 | C++ 旧类型 | 备注 |
|-----------|------------|-----------|------|
| `void` |  | `void` |  |
| `boolean` |  | `bool` |  |
| `char` | `char8_t` (C++20), `char16_t`, `char32_t` | `char`, `wchar` |  |
| `byte` | `int8_t`, `uint8_t`, `std::byte` (C++17) / `gsl::byte` | `char`, `unsigned char` | 应该用无符号类型表示 Byte，但是 Java 不支持无符号类型，所以这里对应了有符号类型 |
| `short` | `int16_t` | `short` |  |
| `int` | `int32_t` | `int` |  |
| `long` | `int64_t` | `long`, `long long` |  |
| `float` |  | `float` |  |
| `double` |  | `double` |  |

> 注意：`int`/`long` 等旧类型长度依赖 CPU / OS / 编译器，不要假设固定长度。常见的对应关系及 C++ 中基础类型的详细介绍见 <https://en.cppreference.com/w/cpp/language/types>

其他 C++ 中常见而 Java 没有的基础类型：

- `nullptr_t`：`nullptr` 的真正类型（`nullptr` 的引入是为了解决 `0` 到底是一个整数还是一个空指针的问题）。
- `size_t` / `ssize_t`（`ssize_t` 并非 C++ 标准，而是 POSIX 标准提供的，自己写程序尽量不要使用，但是调用系统 API 等时候看到了应该理解其含义）：足够大的整数类型，大到足以表示任意一个 C++ 对象大小，通常就是机器字长（即 32 位机上就是 32 位，64 位机上就是 64 位），但是标准上允许它更小或更大。
- 各整数的无符号版本：`unsigned int`, `uint64_t` 等

> 如果你的代码不符合 C++ 标准，那它可能就不是 Portable 的，比如说在 ARM 机器上可能就不能编译，或者跑起来不符合你的预期。所以对于不固定长度的数据类型，不要假设其长度是某一个特定值。

无符号类型这里单拿出来说，以示重视。简单来说，我们记住这几个原则：

1. 表示数量 / 长度优先用有符号类型（STL/ 系统 API 为历史特例）
1. 表示原始字节序列 / 内存块 / 位字段 / 明确非负语义时用无符号类型（例如用 `uint32_t` 表示 IPv4 地址）
1. 在有符号类型和无符号类型之间比较时，需要特别小心
    1. 可以将两者都转换为更大表示范围的有符号类型，然后进行比较，例如将 `int32_t` 和 `uint32_t` 都转换为 `int64_t` 然后进行比较
    1. 如果确保有符号整数不为负（使用 `DCHECK_GE(value, 0)` 进行检查，并起到提示作用），将其转换为无符号整数进行比较，因为在这种情况下无符号版本比有符号版本能表示的值的范围大
    1. 如果不能确保有符号整数不为负，先检查其是否为负，如果为负则一定小于无符号整数，否则转为上一种情况

> 原则上不要使用 C++ 的裸数组，因为裸数组在传递给函数的时候会退化成裸指针，你拿不到数组的长度。而且裸指针是很不安全的。
> 使用 `std::span<int>`（或者 `absl::Span`），`const std::array<int, kSize>&`（不常用，因为会把数组长度搞到类型里，进一步搞到了函数签名上，很恶心），来传递数组。或者干脆就使用 `std::vector` 假装是一个数组（如果你用了 abseil 库的话，可能 `absl::FixedArray` 是一个更好的选择）。

## C++ 值语意分类（选读）

这里指可能经常听过的左值（lvalue），右值（rvalue），消亡值（xvalue）等概念。个人认为 Java 转 C++ 开发的常规情况下不需要学习也可以正常使用 C++ 语言进行日常开发，但是如果需要写比较基础的类库的话，需要掌握这些知识。或者比较好奇相关问题的话，也可以看一下。这里不展开介绍了，详情见文档 <https://en.cppreference.com/w/cpp/language/value_category>。

## 关于使用有符号整型表示长度的展开讨论

使用无符号整型的一个常见错误示例：

```cpp
// Loop never end because |unsigned int| would wrap to |UINT_MAX| when counting down
// beyond 0.
for (auto i = v.size(); i >= 0; --i) {
}
```

一些讨论见 <https://stackoverflow.com/questions/51677855/is-using-an-unsigned-rather-than-signed-int-more-likely-to-cause-bugs-why>，我将来会再找一些更权威的资料。

[Google 风格指南](https://google.github.io/styleguide/cppguide.html#Integer_Types) 中提到：

> Because of historical accident, the C++ standard also uses unsigned integers to represent the size of containers - many members of the standards body believe this to be a mistake, but it is effectively impossible to fix at this point.

C++20 标准中新增了 `ssize()` 函数用来返回以有符号整型表示的容器大小 <https://en.cppreference.com/w/cpp/iterator/size>。

## 用户自定义类型

原则上，在 C++ 中，用户自定义类型享有和语言内建基础类型一样的能力。Java 与此不同，内建的数值类型永远是传值的，且分配在栈上；用户自定义类型永远是传引用的，且分配在堆上。

### class 与 struct

C++ 中支持使用 `class` 和 `struct` 来定义自定义类型。原则上自定义类型的能力和内建（builtin）类型的能力一样强。使用 `class` 和 `struct` 定义类型的唯一区别就是默认成员可见性是 private（`class`）还是 public（`struct`）。

> C++ 中没有包可见性级别。

`struct` 的概念起源于 C 语言，但是在 C++ 中只是 `class` 的另一个写法，语义上除了默认可见性没有别的区别。在 C 语言中的这种“简单”的 `struct` 相对于目前这种 C++ 的 `class` 有一个显著的优势是，我们可以通过 `memcpy` 等操作直接对一块内存区域进行操作（否则需要考虑拷贝构造函数等等）。这样的操作虽然非常危险，但是有的时候却会对性能有很大的提升。我们称类似于 C 语言中这种“简单”的 `struct` 为 [POD 类型（Plain old data-type，deprecated since C++20）](https://en.cppreference.com/w/cpp/named_req/PODType) 或 [Standard Layout Type](https://en.cppreference.com/w/cpp/named_req/StandardLayoutType)，具体概念不在此展开（可参考 <https://docs.microsoft.com/en-us/cpp/cpp/trivial-standard-layout-and-pod-types?view=msvc-160>）。

### 构造函数与析构函数（Updated 2021-07-18）

所有变量都需要被初始化后再使用，除非你能保证在使用之前先给它赋值（实际上也相当于初始化了），否则的话相当于使用了一个行为不确定的变量的值。在这种情况下，变量的内容不保证是（机器代码语义中的）0，可能是任意内容。在 Java 和 C++ 中，使用构造函数来保证一个类的成员变量都被正确初始化了。

与 Java 不同的是，在 C++ 中有 3 种方式初始化成员变量（按照推荐使用的顺序排序，同时也可以近似认为是他们的执行顺序）：

1. 成员声明处的默认初始化
1. 构造函数初始化列表
1. 构造函数函数体

如有可能，我们应当尽可能地在声明成员变量的同时指定初始化值，这样可以避免我们添加构造函数的时候漏掉一些成员变量，而且这个值和声明的地方比较近，也容易被找到。将成员变量初始化为某个常量/字面量的场景，通常使用这种方式，例如：

```cpp
class Person {
  //...
 private:
  static constexpr int32_t kUnspecifiedAge = -1;

  std::string name_{};            // Initialized with an empty string.
  int32_t age_{kUnspecifiedAge};  // Initialized with an constant.
};

// Defined in person.cc, prior to C++17.
constexpr int32_t Person::kUnspecifiedAge;
```

如果需要在构造函数参数中传入一些值用于初始化，那么我们只能选择另外 2 种方式。在这种情况下，我们也应当尽可能在构造函数的初始化列表中进行初始化。这样做性能更好一些。最后别无选择的情况下，再使用函数体。

```cpp
class Person {
 public:
  Person() = default;  // Use default keyword to generate default constructor.
                       // Provide a default constructor is useful for receiving
                       // values from output parameters.
                       //
                       // Example:
                       //   Person p;
                       //   Status s = LoadPerson(db, key, &p);
                       //   CHECK(s.ok()) << "Failed to load. reason=" << s;

  // Although |name_| & |age_| default value is specified during declaration,
  // the member initializer list would override it when using this constructor.
  Person(std::string name, int32_t age) : name_(std::move(name)), age_(age) {
    // Execute constructor body after member initializer list executed.
    CHECK(!name_.empty());  // Cannot use |name| here because it already moved.
    CHECK_GT(age_, 0);
  }

 private:
  static constexpr int32_t kUnspecifiedAge = -1;

  std::string name_{};            // Initialized with an empty string.
  int32_t age_{kUnspecifiedAge};  // Initialized with an constant.
};

// Defined in person.cc, prior to C++17.
constexpr int32_t Person::kUnspecifiedAge;
```

> 特别需要注意的是，初始化列表的顺序需要和成员变量声明的顺序保持一致。

实际上不管怎么写初始化列表，实际上在进行初始化的时候，都是按照成员变量声明的顺序执行的。所以一旦这两个顺序不一致了，就很容易造成误解。在一些特定情况下，很容易出错，例如：

```cpp
class Person {
 public:
  Person() = default;

  // !!! THIS IS AN INCORRECT PRACTICE !!!
  Person(std::string name) :
    // Expecting |name| valid here, but actually not! UNDEFINED BEHAVIOR!
    id_(absl::StrCat(name, "_", GetNextUniqueId())),
    // The next line would be executed before initializing |id_|.
    name_(std::move(name)) {}

 private:
  std::string name_;
  std::string id_;
};
```

析构函数就是 C++ 中类似于 [`AutoClosable.close()`](https://docs.oracle.com/javase/8/docs/api/java/lang/AutoCloseable.html#close--) 的方法。可以认为 C++ 中的每一个对象实例都是包含在 Java 的 [try-with-resources](https://docs.oracle.com/javase/tutorial/essential/exceptions/tryResourceClose.html) 块中的，只要超出作用域，就会立刻调用析构函数。

```cpp
class File {
 public:
  static constexpr int kInvalidFileDescriptor = -1;

  File() = default;
  File(int fd) : fd_(fd) {}

  // Close OS managed resource during destruction.
  // Example:
  //   {
  //     File file(::open(filename, O_RDONLY));
  //     PCHECK(file.valid()) << "Failed to open file '" << filename << "'.";
  //     // Read file contents...
  //   }  // Execute ~File() automatically to close the OS managed resource.
  ~File() {
    if (fd != kInvalidFileDescriptor) {
      PCHECK(::close(fd) == -1) << "Failed to close file.";
    }
  }

 private:
  int fd_{kInvalidFileDescriptor};
};

// Defined in file.cc, prior to C++17.
constexpr int File::kInvalidFileDescriptor;
```

> 在 C++ 中，构造函数可以抛出异常并被捕获处理，但是析构函数抛出异常会直接导致程序退出。如果在构造函数中可能抛出异常，那么在析构函数中一定要注意，可能有些成员变量还没有来得及被正确初始化，就抛异常出去了。

通常实践中，建议不使用异常处理（原因见 [Google C++ Style Guide 相关讨论](https://google.github.io/styleguide/cppguide.html#Exceptions)）。在不使用异常机制的情况下，通常采用返回 `Status` 类的方式来模拟抛异常和捕获异常的过程。在这种情况下，建议使用这样的模式来解决构造函数需要处理并返回错误的情况：

```cpp
class HttpClient {
 public:
  static absl::StatusOr<HttpClient> Make(HttpClientOptions options) {
    HttpClient client(std::move(options));
    absl::Status s = client.Init();
    if (!s.ok()) {
      return s;
    }

    return client;
  }

 protected:
  // Construct a |HttpClient| instance, must call |Init()| immediately after
  // creation. Use |explicit| keyword to prevent implicit cast.
  explicit HttpClient(HttpClientOptions options)
      : options_(std::move(options)) {}

  absl::Status Init() { return RefreshOauth2Token(); }

  absl::Status RefreshOauth2Token();

 private:
  HttpClientOptions options_{};
};
```

注意到这里构造函数使用了 `explicit` 关键字，这是因为该构造函数只有 1 个参数，这种情况下记住一定要用 `explicit` 关键字，否则将会发生参数到该类型的隐式类型转换。隐式类型转换总是我们需要避免的，因为这会破坏我们的类型系统。

```cpp
class ImplicitCastAllowedInt {
 public:
  ImplicitCastAllowedInt(int value);
};

class ImplicitCastDisallowedInt {
 public:
  explicit ImplicitCastDisallowedInt(int value);
};

void PassImplicitCastAllowedInt(ImplicitCastAllowedInt);
void PassImplicitCastDisallowedInt(ImplicitCastDisallowedInt);

PassImplicitCastAllowedInt(
    1);  // Implicit cast 1 to ImplicitCastAllowedInt(1)
PassImplicitCastDisallowedInt(1);  // Won't compile!
```

> 建议构造函数尽可能少做事情，只做必要的初始化工作。其他工作，例如启动后台线程，都可以用上面提到的模式挪到构造函数外面来进行。

以后台线程为例，如果在构造函数中启动，就经常会出现问题。这里不给代码了（比较繁琐），大概解释一下是怎么出问题的：

1. 父类构造函数中启动了后台线程。
1. 后台线程调用了虚函数，此时子类构造函数还未执行完成。于是被子类重载的虚函数执行时可能访问了尚未初始化的成员变量。

> 同样的道理，不要在析构函数中等待后台线程完成。此时可能子类已经析构了，再调用虚方法的话就跟上面的例子一样了。
>
> 不要在构造函数和析构函数中调用虚函数

假设有父类 `Base` 和子类 `Child`。那么构造 `Child` 对象时会先调用 `Base` 的构造函数去初始化 `Base` 的成员，然后再调用 `Child` 的构造函数去初始化 `Child` 的成员；析构 `Child` 对象时，会先析构 `Child` 的成员，再析构 `Base` 的成员。在这点上 C++ 和 Java 是一样的。

考虑这样一种情况，如果在 `Base` 的构造函数中调用了一个虚方法，而该方法被 `Child` override 了会如何呢？如果这个函数使用到了 `Child` 的成员，那么可能就会出问题，因为 `Child` 的成员还没有被正确初始化。类似的，在析构函数中调用虚方法也有类似的问题。因此请尽量避免在构造函数/析构函数中调用虚方法。

有的时候这种调用不是那么显然的。稍微不显然的例子是构造函数里面调用函数 A，然后函数 A 调用了虚函数 B。更不显然的例子是 `Start()` 里面开了个后台线程，然后 `Stop()` 去等待这个后台线程退出，为了避免最坏情况下没调用 `Stop()`，在析构函数中调用了 `Stop()`。这个后台线程调用了虚函数。

> C++ 中在构造函数中调用虚函数的话，保证只调用其在父类中定义的版本，就好像其不是一个虚方法一样。其实这样更坑（小声 BB）

### （copy-by）值语义类型/引用语义类型

Java 中自定义类型有一个比较大的缺憾，即无法定义“值类型”。在 Java 中，`int` 是一个值类型，而 `Integer` 是一个引用类型，他们之间一个显著的区别如下：

1. 删除拷贝构造/拷贝赋值
1. （可选）提供移动语义
1. 虚析构函数

```java
int a;
int b;
b = a; // Copy the value of `a` to `b`, both `a` and `b` have the same value.

Integer c;
Integer d;
d = c; // Copy the reference from `c` to `d`, both `c` and `d` reference the same value.
```

> 在 Java 中，我们只能自定义“引用类型”，无法自定义“值类型”。但是在 C++ 却与之有很大不同，我们 **by-default 定义的类型是一个“值类型”**，需要做一些额外的事情才能使其只能用于“引用类型”语义。

对于值类型，最好重载 `operator==` 和 `operator!=`，并提供 Hash 函数，这部分内容我们会在后面运算符重载小节中详细介绍。

下面给出一个在 C++ 中创建“引用类型”的例子：

```cpp
// We can copy either the value or the reference of a ValueType.
//
// This is a trivial type. The compiler would generate the default constructor,
// default copy constructor & copy assignment operator, default move constructor &
// move assignment operator, default destructor for it. As a result, it's copyable &
// moveable.
//
// Example:
//   ValueType a;
//   ValueType b = a;   // Allowed to copy the value.
//   ValueType& c = a;  // Allowed to reference the value.
struct ValueType {
  int32_t id;
  std::string name;
};

// We cannot copy the value of a RefrenceType instance.
// We can only copy the reference to a ReferenceType instance.
//
// Example:
//   ReferenceType a(2, "mock_name");
//   ReferenceType b = a;   // !!!DISALLOW!!! Won't compile!
//   ReferenceType& b = a;  // Allowed to reference a ReferenceType instance.
class ReferenceType {
 public:
  ReferenceType(int32_t id, std::string name) : id_(id), name_(std::move(name)) {}
  // Make the destructor virtual to allow children override it.
  virtual ~ReferenceType() = default;

  // Disallow copy
  ReferenceType(const ReferenceType&) = delete;
  ReferenceType& operator=(const ReferenceType&) = delete;

  // Allow move, move means steal the content of `other` instance.
  // Don't panic. Would talk about move in following chapters.
  ReferenceType(ReferenceType&& other) : id_(other.id_), name_(std::move(other.name_)) {
    other.id_ = 0;
  }
  ReferenceType& operator=(ReferenceType&& other) {
    id_ = other.id_;  // Cannot move int, it's a primitive type.
    other.id_ = 0;    // Copy then clear the field.
    name_ = std::move(other.id_);  // Move std::string
  }
};
```

简单来说，引用类型应该：

1. 禁止复制
1. 虚析构函数
1. （最好）允许 move

更多内容见 <https://isocpp.org/wiki/faq/value-vs-ref-semantics>

> 如果不允许 move 的话，或者是实现了 copy 但是暂时先不实现 move，应该显式删除 move 构造函数和赋值运算符
>
> 详细解释见 <https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all>

/// admonition | TODO
    type: todo
未转录完
///
