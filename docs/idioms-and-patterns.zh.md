<!--
SPDX-FileCopyrightText: 2021 Shuai Zhang

SPDX-License-Identifier: CC-BY-NC-ND-4.0
-->

# 常见 Idioms / Design Patterns

常见资料

- <https://en.wikibooks.org/wiki/C%2B%2B_Programming/Idioms>
- <https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms>

## 后台线程周期性活动

> 自创，但是常用

```cpp
class MyService {
 public:
  Status Start();
  void Stop();

 private:
  void BackgroundThreadEntryPoint();

  std::unique_ptr<std::thread> background_thread_;
  absl::Notification stopping_notification_;
};

Status MyService::Start() {
  background_thread_ =
      std::make_unique<std::thread>(&BackgroundThreadEntryPoint, this);
}

void MyService::Stop() {
  stopping_notification_.Notify();
  if (background_thread_) {
    background_thread_->join();
    background_thread_.reset();
  }
}

void MyService::BackgroundThreadEntryPoint() {
  while (!stopping_notification_.WaitWithTimeout(absl::Millisecond(kLoopInterval))) {
    // ...
  }
}
```

## 线程安全的用时初始化的 Singleton

- <https://source.chromium.org/chromium/chromium/src/+/main:base/lazy_instance.h;l=6;drc=7b5337170c1581e4a35399af36253f767674f581>
- <https://www.modernescpp.com/index.php/thread-safe-initialization-of-a-singleton>
- [*Imperfect C++* by Matthew Wilson](http://www.imperfectcplusplus.com/)

/// admonition | 注意
除非我们就是需要一个全局变量（比如说 `GlobalBackgroundThreadPool`），我们不应该使用 Singleton 模式。我们应该考虑从构造函数，或者 setter 函数中传入需要的对象，这样做会带来更好的可组合性和可测试性。
///

```cpp
Factory& Factory::GetInstance() {
  static base::NoDestructor<Factory> instance;
  return *instance;
}
```

我们先不看 `base::NoDestructor` 是个啥，假设就是这样的：

```cpp
Factory& Factory::GetInstance() {
  static Factory instance;
  return instance;
}
```

1. 这样做只会在调用 `Factory::GetInstance()` 时才会初始化 static Factory instance。
1. 多线程并发调用 `Factory::GetInstance()` 是安全的，只会初始化 static Factory instance 一次。

/// admonition | TODO
    type: todo

1. 解释一下为什么
1. 解释一下 NoDestructor 又是干啥的
///

## 使用 `std::shared_ptr` 实现 Copy-On-Write

《Linux 多线程服务端编程：使用 muduo C++ 网络库》

```cpp
class MyQueue {
 public:
  void PushQueue(int v);
  void PrintQueue();

 private:
  std::shared_ptr<std::vector<int>> queue_ ABSL_GUARDED_BY(mutex_);
  absl::Mutex mutex_;
};

void MyQueue::PushQueue(int v) {
  absl::MutexLock lock(&mutex_);
  if (!queue_.unique()) {
    // Copy if other people reading the queue.
    queue_ = std::make_shared<std::vector<int>>(*queue_);
  }
  // Now we ensure no other people accessing the queue.
  DCHECK(queue_.unique());
  queue_.emplace_back(v);
}

void MyQueue::PrintQueue() {
  std::shared_ptr<std::vector<int>> the_queue;
  {
    absl::MutexLock lock(&mutex_);
    the_queue = queue_;
    DCHECK(!queue_.unique());
  }

  for (int v : *the_queue) {
    absl::PrintF("%d\n", v);
  }
}
```
