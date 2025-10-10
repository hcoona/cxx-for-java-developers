/*
 * SPDX-FileCopyrightText: 2021 Shuai Zhang
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// --8<-- [start:code]
class File {
 public:
  static constexpr int kInvalidFileDescriptor = -1;

  File() = default;
  explicit File(int fd) : fd_(fd) {}

  // Disable Copy
  // ...

  // Close OS managed resource during destruction.
  // Example:
  //   {
  //     File file(::open(filename, O_RDONLY));
  //     PCHECK(file.valid()) << "Failed to open file '" << filename << "'.";
  //     // Read file contents...
  //   }  // Execute ~File() automatically to close the OS managed resource.
  ~File() {
    if (fd_ != kInvalidFileDescriptor) {
      PCHECK(::close(fd_) == 0) << "Failed to close file.";
    }
  }

 private:
  int fd_{kInvalidFileDescriptor};
};

// Defined in file.cc, prior to C++17.
constexpr int File::kInvalidFileDescriptor;
// --8<-- [end:code]
