// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
class UniqueFileDescriptor {
 public:
  static constexpr int kInvalidFileDescriptor = -1;  // From man 2 open

  UniqueFileDescriptor() = default;
  explicit UniqueFileDescriptor(int fd) : fd_(fd) {}
  ~UniqueFileDescriptor() {
    if (fd_ != UniqueFileDescriptor::kInvalidFileDescriptor) {
      ::close(fd_);  // Should LOG error if failed to close it.
    }
  }

  // omitted: copy constructor and copy assignment operator should be deleted.
  // omitted: move constructor and move assignment operator should be defined.
  // omitted: other utility methods.

 private:
  int fd_{UniqueFileDescriptor::kInvalidFileDescriptor};
};
// --8<-- [end:code]
