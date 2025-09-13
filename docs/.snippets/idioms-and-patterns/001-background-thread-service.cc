// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
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
  background_thread_ = std::make_unique<std::thread>(
      &MyService::BackgroundThreadEntryPoint, this);
  return Status::OK();
}

void MyService::Stop() {
  stopping_notification_.Notify();
  if (background_thread_) {
    background_thread_->join();
    background_thread_.reset();
  }
}

void MyService::BackgroundThreadEntryPoint() {
  while (!stopping_notification_.WaitWithTimeout(
      absl::Milliseconds(kLoopInterval))) {
    // ...
  }
}
// --8<-- [end:code]
