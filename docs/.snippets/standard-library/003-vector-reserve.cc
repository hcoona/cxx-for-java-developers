// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
absl::Status ReadFeature(ByteBuffer* buffer, std::vector<int32_t>* feature) {
  int64_t value_count;
  RETURN_IF_NOT_OK(buffer->TryReadInt64(&value_count));

  feature->reserve(static_cast<size_t>(value_count));
  for (int64_t i = 0; i < value_count; i++) {
    int32_t v;
    RETURN_IF_NOT_OK(buffer->TryReadInt32(&v));
    feature->emplace_back(v);
  }

  return absl::OkStatus();
}
// --8<-- [end:code]
