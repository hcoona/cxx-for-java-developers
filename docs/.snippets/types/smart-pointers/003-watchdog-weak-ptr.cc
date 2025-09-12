// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
void NotifyUnhealthy();

class MyServiceImpl : public Service {
 public:
  Status Start() override;
  Status Stop() override;

  bool healthy() const override;
  absl::Time last_known_healthy_time() const override;
};

class MyServiceWatchdogServiceImpl : public Service {
 public:
  explicit MyServiceWatchdogServiceImpl(
      std::weak_ptr<MyServiceImpl> weak_my_service)
      : weak_my_service_(std::move(weak_my_service)) {}

  Status Start() override;
  Status Stop() override;

 private:
  void BackgroundTaskEntryPoint();

  std::unique_ptr<std::thread> background_thread_;
  absl::Notification stopping_notification_;

  std::weak_ptr<MyServiceImpl> weak_my_service_;
};

void MyServiceWatchdogServiceImpl::BackgroundTaskEntryPoint() {
  absl::optional<absl::Time> previous_known_healthy_time;
  while (
      !stopping_notification_.WaitForNotificationWithTimeout(kLoopInterval)) {
    std::shared_ptr<MyServiceImpl> shared_my_service = weak_my_service_.lock();
    if (!shared_my_service) {
      // The instance of my_service was destroyed.
      NotifyUnhealthy();
      break;
    }

    if (shared_my_service->healthy()) {
      if (previous_known_healthy_time.has_value() &&
          (shared_my_service->last_known_healthy_time() -
               previous_known_healthy_time.value() >
           kHealthyCheckFailureDuration)) {
        // Healthy state not updated for a while, regard it unhealthy.
        NotifyUnhealthy();
        break;
      }

      previous_known_healthy_time =
          shared_my_service->last_known_healthy_time();
    }
  }
}
// --8<-- [end:code]
