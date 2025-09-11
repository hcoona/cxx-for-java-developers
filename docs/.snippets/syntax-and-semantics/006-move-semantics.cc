// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
UniqueFileDescriptor unique_fd;
{
  UniqueFileDescriptor fd(::open(filename.c_str(), "r"));
  if (fd.fd() == UniqueFileDescriptor::kInvalidFileDescriptor) {
    return absl::UnknownError("Failed to open file.");
  }

  /* do something ... */

  // Move the control to outer |unique_fd|.
  unique_fd = std::move(fd);
}
// --8<-- [end:code]
