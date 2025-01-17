// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/third_party/quiche/src/quic/core/quic_connection_id.h"

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <string>

#include "net/third_party/quiche/src/quic/core/quic_types.h"
#include "net/third_party/quiche/src/quic/platform/api/quic_bug_tracker.h"
#include "net/third_party/quiche/src/quic/platform/api/quic_endian.h"
#include "net/third_party/quiche/src/quic/platform/api/quic_flag_utils.h"
#include "net/third_party/quiche/src/quic/platform/api/quic_flags.h"
#include "net/third_party/quiche/src/quic/platform/api/quic_logging.h"
#include "net/third_party/quiche/src/quic/platform/api/quic_text_utils.h"

namespace quic {

QuicConnectionId::QuicConnectionId() : QuicConnectionId(nullptr, 0) {}

QuicConnectionId::QuicConnectionId(const char* data, uint8_t length) {
  static_assert(
      kQuicMaxConnectionIdLength <= std::numeric_limits<uint8_t>::max(),
      "kQuicMaxConnectionIdLength too high");
  if (length > kQuicMaxConnectionIdLength) {
    QUIC_BUG << "Attempted to create connection ID of length " << length;
    length = kQuicMaxConnectionIdLength;
  }
  length_ = length;
  if (length_ == 0) {
    return;
  }
  if (!GetQuicRestartFlag(quic_use_allocated_connection_ids)) {
    memcpy(data_, data, length_);
    return;
  }
  if (length_ <= sizeof(data_short_)) {
    memcpy(data_short_, data, length_);
    return;
  }
  data_long_ = reinterpret_cast<char*>(malloc(length_));
  CHECK_NE(nullptr, data_long_);
  memcpy(data_long_, data, length_);
}

QuicConnectionId::~QuicConnectionId() {
  if (!GetQuicRestartFlag(quic_use_allocated_connection_ids)) {
    return;
  }
  if (length_ > sizeof(data_short_)) {
    free(data_long_);
    data_long_ = nullptr;
  }
}

QuicConnectionId::QuicConnectionId(const QuicConnectionId& other)
    : QuicConnectionId(other.data(), other.length()) {}

QuicConnectionId& QuicConnectionId::operator=(const QuicConnectionId& other) {
  set_length(other.length());
  memcpy(mutable_data(), other.data(), length_);
  return *this;
}

const char* QuicConnectionId::data() const {
  if (!GetQuicRestartFlag(quic_use_allocated_connection_ids)) {
    return data_;
  }
  if (length_ <= sizeof(data_short_)) {
    return data_short_;
  }
  return data_long_;
}

char* QuicConnectionId::mutable_data() {
  if (!GetQuicRestartFlag(quic_use_allocated_connection_ids)) {
    return data_;
  }
  if (length_ <= sizeof(data_short_)) {
    return data_short_;
  }
  return data_long_;
}

uint8_t QuicConnectionId::length() const {
  return length_;
}

void QuicConnectionId::set_length(uint8_t length) {
  if (GetQuicRestartFlag(quic_use_allocated_connection_ids)) {
    char temporary_data[sizeof(data_short_)];
    if (length > sizeof(data_short_)) {
      if (length_ <= sizeof(data_short_)) {
        // Copy data from data_short_ to data_long_.
        memcpy(temporary_data, data_short_, length_);
        data_long_ = reinterpret_cast<char*>(malloc(length));
        CHECK_NE(nullptr, data_long_);
        memcpy(data_long_, temporary_data, length_);
      } else {
        // Resize data_long_.
        char* realloc_result =
            reinterpret_cast<char*>(realloc(data_long_, length));
        CHECK_NE(nullptr, realloc_result);
        data_long_ = realloc_result;
      }
    } else if (length_ > sizeof(data_short_)) {
      // Copy data from data_long_ to data_short_.
      memcpy(temporary_data, data_long_, length);
      free(data_long_);
      data_long_ = nullptr;
      memcpy(data_short_, temporary_data, length);
    }
  }
  length_ = length;
}

bool QuicConnectionId::IsEmpty() const {
  return length_ == 0;
}

size_t QuicConnectionId::Hash() const {
  uint64_t data_bytes[3] = {0, 0, 0};
  static_assert(sizeof(data_bytes) >= kQuicMaxConnectionIdLength,
                "kQuicMaxConnectionIdLength changed");
  memcpy(data_bytes, data(), length_);
  // This Hash function is designed to return the same value as the host byte
  // order representation when the connection ID length is 64 bits.
  return QuicEndian::NetToHost64(kQuicDefaultConnectionIdLength ^ length_ ^
                                 data_bytes[0] ^ data_bytes[1] ^ data_bytes[2]);
}

std::string QuicConnectionId::ToString() const {
  if (IsEmpty()) {
    return std::string("0");
  }
  return QuicTextUtils::HexEncode(data(), length_);
}

std::ostream& operator<<(std::ostream& os, const QuicConnectionId& v) {
  os << v.ToString();
  return os;
}

bool QuicConnectionId::operator==(const QuicConnectionId& v) const {
  return length_ == v.length_ && memcmp(data(), v.data(), length_) == 0;
}

bool QuicConnectionId::operator!=(const QuicConnectionId& v) const {
  return !(v == *this);
}

bool QuicConnectionId::operator<(const QuicConnectionId& v) const {
  if (length_ < v.length_) {
    return true;
  }
  if (length_ > v.length_) {
    return false;
  }
  return memcmp(data(), v.data(), length_) < 0;
}

QuicConnectionId EmptyQuicConnectionId() {
  return QuicConnectionId();
}

static_assert(kQuicDefaultConnectionIdLength == sizeof(uint64_t),
              "kQuicDefaultConnectionIdLength changed");
static_assert(kQuicDefaultConnectionIdLength == PACKET_8BYTE_CONNECTION_ID,
              "kQuicDefaultConnectionIdLength changed");

}  // namespace quic
