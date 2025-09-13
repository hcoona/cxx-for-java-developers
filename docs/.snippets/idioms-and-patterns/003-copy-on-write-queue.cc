// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
class MyQueue {
 public:
  // Omitted: constructor, destructor, other methods.

  void Push(int v);
  void Print();

 private:
  std::shared_ptr<std::vector<int>> queue_ ABSL_GUARDED_BY(mutex_);
  absl::Mutex mutex_;
};

void MyQueue::Push(int v) {
  absl::MutexLock lock(&mutex_);
  if (!queue_.unique()) {
    // Copy if other people reading the queue.
    queue_ = std::make_shared<std::vector<int>>(*queue_);
  }
  // Now we ensure no other people accessing the queue.
  DCHECK(queue_.unique());
  queue_->emplace_back(v);
}

void MyQueue::Print() {
  std::shared_ptr<std::vector<int>> the_queue;
  {
    absl::MutexLock lock(&mutex_);
    the_queue = queue_;
  }

  for (int v : *the_queue) {
    absl::PrintF("%d\n", v);
  }
}
// --8<-- [end:code]
