<!--
SPDX-FileCopyrightText: 2021 Shuai Zhang

SPDX-License-Identifier: CC-BY-NC-ND-4.0
-->

# 异常处理

> 公司内关于异常处理的分享 <https://people.bytedance.net/learning/student/course-detail/614da582736bef554debbc9c>
>
> 有群友贡献的看起来比较好的资料：<http://baiy.cn/doc/cpp/inside_exception.htm>

## 是否使用异常机制的一些讨论

Java 的 Checked Exception 机制虽然经常让人觉得非常烦躁，但是实际上还是一种比较正确的做法。这里我不展开解释了，感兴趣的同学可以看一下 [Kotlin 和 Checked Exception](https://www.yinwang.org/blog-cn/2017/05/23/kotlin)。C++ 没有这样的机制，导致异常很难被“正确使用”。

[Google C++ Style Guide 中也对异常的使用进行了一些讨论](https://google.github.io/styleguide/cppguide.html#Exceptions)，然后基于和其他代码兼容性方面的考虑禁用了异常机制。这里的兼容性考虑了 2 个方面，一方面是你用的库是否能很好的处理异常（比如说你的回调函数里面抛了个异常可能就坏了），另一方面是你的代码作为一个库提供出去的时候别人是否能很好的处理。

另一方面，个人补充一点原因，就是 C++ 的异常是拿不到 stacktrace 信息的。如果你 catch 了一个异常，然后打日志，你是很难定位抛异常的地方的。

有不少人说异常性能的问题，在当前主流环境下，C++ 的异常处理在 happy path 上是 zero cost 的，但是代价就是更大的 exception path cost。所以要正确的使用异常，只在一些非常规的情况下抛异常，比如说你要读的一个按照设计逻辑上保证一定存在的文件不存在了。简单来说，就是在异常出现的情况下完全不能正常提供服务的时候抛异常，这样 exception path 就算代价大一些，也比能正常服务需要的时间更多；或者就干脆直接不处理这个异常，直接 crash 了。

## 不使用异常机制的 workaround

如果你决定不使用异常机制，可以使用 `absl::Status` 作为 workaround。基本思路很简单，就是你总是返回一个东西，代表没有异常（`status.ok()`）或者有异常（`!status.ok()`），并且可以使用一些辅助方法判断是什么类型的异常（这点倒是跟 Java 差不多，Java 也是建议 catch 之后只检查几个异常大类——比如说 `IOException`——而不是检查一些细分小异常类型）。当你除了异常还需要返回别的东西的时候，可以选择搞一个 `struct` 打包返回一些东西，或者是把这些返回值甩到 output parameter 上（这点 Java 倒是没有这样的机制，类似于 C# 的 `ref` 关键字）。

```cpp
absl::Status NewRandomAccessFile(const std::string& filename,
                                 std::unique_ptr<RandomAccessFile>* result) {
  if (filename.empty()) {
    return InvalidArgumentError("|filename| must be assigned.");
  }

  // Check `man 2 open` for further details about |open|.
  int fd = ::open(filename.c_str(), O_RDONLY);
  if (fd < 0) {
    return IOError(filename, errno);
  }

  // Return through output parameter |result|.
  *result = std::make_unique<PosixRandomAccessFile>(filename, fd);
  return absl::OkStatus();
}
```

我承认 `absl::Status` 仍然还有很多问题，但是已经是一个比较简单的能实现我们大部分要求的工具了。尽管 `absl::Status` 也不携带 stacktrace 信息，但是由于它位于返回值上，所以这个调用路径的每个地方都需要去 check 其结果是 ok 与否并打日志，这样的话变相的也算是能知道一些 stacktrace 信息了。另外就是 `absl::Status` 也是有机会附带一些额外的信息的，功能详情见 <https://github.com/abseil/abseil-cpp/blob/d96e287417766deddbff2d01b96321288c59491e/absl/status/status.h#L573>

## 使用异常机制的一些建议

如果确实想要使用异常机制的话，建议考虑这样几个方面的问题。

1. 异常继承链怎么设计，这个也是 Java 中老生常谈的一个问题了
1. 虽然没有 checked exception，但是也要在所有 public 方法注释中说明会抛出哪些类型的异常
1. 在构造函数中使用异常，需要特别小心，确保析构函数能够搞定构造到一半抛异常了的对象
1. 析构函数中不要抛异常，没法 catch，直接就 core 了

/// admonition | TODO
    type: todo
不确定有多少人关注使用异常机制的问题，有的话在后面的讨论中留言，关注的人多我再补充。
///
