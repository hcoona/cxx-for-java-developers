// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
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
// --8<-- [end:code]
