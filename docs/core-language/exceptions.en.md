<!--
SPDX-FileCopyrightText: 2021 Shuai Zhang

SPDX-License-Identifier: CC-BY-NC-ND-4.0
-->

# Exception Handling

> Internal sharing about exception handling <https://people.bytedance.net/learning/student/course-detail/614da582736bef554debbc9c>
>
> A community-contributed resource that looks pretty good: <http://baiy.cn/doc/cpp/inside_exception.htm>

## Some Discussions on Whether to Use Exceptions

Java's checked exception mechanism is often considered annoying, yet in practice it's actually a fairly proper design. I won't elaborate here; if you're interested, read [Kotlin and Checked Exception](https://www.yinwang.org/blog-cn/2017/05/23/kotlin). C++ has no such mechanism, which makes exceptions hard to "use correctly".

[The Google C++ Style Guide discusses exceptions](https://google.github.io/styleguide/cppguide.html#Exceptions) and, based on compatibility considerations with other code, disables them. The compatibility concerns are two-fold: first, whether the libraries you use can properly handle exceptions (for example, throwing an exception inside your callback might break things); second, when your code is provided as a library, whether its consumers can properly handle exceptions.

Another personal reason to add: C++ exceptions don't carry stacktrace information. If you catch an exception and log it, it's hard to locate where it was thrown.

Many people cite performance concerns. On mainstream platforms today, C++ exception handling is zero-cost on the happy path, at the price of a heavier exception path. So to use exceptions correctly, only throw for truly exceptional situations—for example, a file which is guaranteed by design to exist turns out missing. Put simply, throw only when the service simply cannot proceed normally; in such cases, even if the exception path is expensive, it's still cheaper than any successful normal processing you could do—or you might just crash outright and not handle it.

## Workaround When Not Using Exceptions

If you decide not to use exceptions, you can use `absl::Status` as a workaround. The basic idea is simple: always return something indicating success (`status.ok()`) or failure (`!status.ok()`), and use helpers to check the category of the failure (this is somewhat similar to Java, where it's recommended to catch broad categories—like `IOException`—rather than many fine-grained subclasses). When you need to return additional data besides the status, you can wrap them in a `struct` or use output parameters (Java doesn't have this mechanism; it's similar to C#'s `ref` keyword).

```cpp
--8<-- ".snippets/exceptions/001-status-example.cc:code"
```

I admit `absl::Status` still has many issues, but it's already a simple tool that satisfies most of our needs. Although it doesn't carry stacktrace information either, since it's part of the return value every call site must check whether it's ok and log accordingly, which indirectly gives you some stacktrace-like breadcrumbs. Also, `absl::Status` can carry extra payload information. See <https://github.com/abseil/abseil-cpp/blob/d96e287417766deddbff2d01b96321288c59491e/absl/status/status.h#L573> for details.

## Some Suggestions When Using Exceptions

If you really want to use exceptions, consider these aspects:

1. How to design the exception inheritance hierarchy—an old topic in Java as well.
2. Even without checked exceptions, document in every public method comment which exception types might be thrown.
3. Be careful when using exceptions in constructors; ensure destructors can handle objects whose construction failed halfway through.
4. Don't throw from destructors; they can't be caught and will directly cause a core (terminate).

/// admonition | TODO
    type: todo
Not sure how many people care about using exceptions. If you do, leave a comment for discussion; if there is enough interest I'll add more.
///
