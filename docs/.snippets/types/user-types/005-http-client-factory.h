// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
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
// --8<-- [end:code]
