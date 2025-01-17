// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/third_party/quiche/src/quic/test_tools/quic_session_peer.h"

#include "net/third_party/quiche/src/quic/core/quic_session.h"
#include "net/third_party/quiche/src/quic/core/quic_stream.h"
#include "net/third_party/quiche/src/quic/core/quic_utils.h"
#include "net/third_party/quiche/src/quic/platform/api/quic_map_util.h"

namespace quic {
namespace test {

// static
QuicStreamId QuicSessionPeer::GetNextOutgoingBidirectionalStreamId(
    QuicSession* session) {
  return session->GetNextOutgoingBidirectionalStreamId();
}

// static
QuicStreamId QuicSessionPeer::GetNextOutgoingUnidirectionalStreamId(
    QuicSession* session) {
  return session->GetNextOutgoingUnidirectionalStreamId();
}

// static
void QuicSessionPeer::SetNextOutgoingBidirectionalStreamId(QuicSession* session,
                                                           QuicStreamId id) {
  if (session->connection()->transport_version() == QUIC_VERSION_99) {
    session->v99_streamid_manager_.bidirectional_stream_id_manager_
        .next_outgoing_stream_id_ = id;
    return;
  }
  session->stream_id_manager_.next_outgoing_stream_id_ = id;
}

// static
void QuicSessionPeer::SetMaxOpenIncomingStreams(QuicSession* session,
                                                uint32_t max_streams) {
  if (session->connection()->transport_version() == QUIC_VERSION_99) {
    QUIC_BUG << "SetmaxOpenIncomingStreams deprecated for IETF QUIC/V99";
    session->v99_streamid_manager_.SetMaxOpenIncomingUnidirectionalStreams(
        max_streams);
    session->v99_streamid_manager_.SetMaxOpenIncomingBidirectionalStreams(
        max_streams);
    return;
  }
  session->stream_id_manager_.set_max_open_incoming_streams(max_streams);
}

// static
void QuicSessionPeer::SetMaxOpenIncomingBidirectionalStreams(
    QuicSession* session,
    uint32_t max_streams) {
  DCHECK_EQ(QUIC_VERSION_99, session->connection()->transport_version())
      << "SetmaxOpenIncomingBidirectionalStreams not supported for Google "
         "QUIC/not-V99";
  session->v99_streamid_manager_.SetMaxOpenIncomingBidirectionalStreams(
      max_streams);
}
// static
void QuicSessionPeer::SetMaxOpenIncomingUnidirectionalStreams(
    QuicSession* session,
    uint32_t max_streams) {
  DCHECK_EQ(QUIC_VERSION_99, session->connection()->transport_version())
      << "SetmaxOpenIncomingUnidirectionalStreams not supported for Google "
         "QUIC/not-V99";
  session->v99_streamid_manager_.SetMaxOpenIncomingUnidirectionalStreams(
      max_streams);
}

// static
void QuicSessionPeer::SetMaxOpenOutgoingStreams(QuicSession* session,
                                                uint32_t max_streams) {
  if (session->connection()->transport_version() == QUIC_VERSION_99) {
    QUIC_BUG << "SetmaxOpenOutgoingStreams deprecated for IETF QUIC/V99";
    session->v99_streamid_manager_.SetMaxOpenOutgoingUnidirectionalStreams(
        max_streams);
    session->v99_streamid_manager_.SetMaxOpenOutgoingBidirectionalStreams(
        max_streams);
    return;
  }
  session->stream_id_manager_.set_max_open_outgoing_streams(max_streams);
}

// static
void QuicSessionPeer::SetMaxOpenOutgoingBidirectionalStreams(
    QuicSession* session,
    uint32_t max_streams) {
  DCHECK_EQ(QUIC_VERSION_99, session->connection()->transport_version())
      << "SetmaxOpenOutgoingBidirectionalStreams not supported for Google "
         "QUIC/not-V99";
  session->v99_streamid_manager_.SetMaxOpenOutgoingBidirectionalStreams(
      max_streams);
}
// static
void QuicSessionPeer::SetMaxOpenOutgoingUnidirectionalStreams(
    QuicSession* session,
    uint32_t max_streams) {
  DCHECK_EQ(QUIC_VERSION_99, session->connection()->transport_version())
      << "SetmaxOpenOutgoingUnidirectionalStreams not supported for Google "
         "QUIC/not-V99";
  session->v99_streamid_manager_.SetMaxOpenOutgoingUnidirectionalStreams(
      max_streams);
}

// static
QuicCryptoStream* QuicSessionPeer::GetMutableCryptoStream(
    QuicSession* session) {
  return session->GetMutableCryptoStream();
}

// static
QuicWriteBlockedList* QuicSessionPeer::GetWriteBlockedStreams(
    QuicSession* session) {
  return &session->write_blocked_streams_;
}

// static
QuicStream* QuicSessionPeer::GetOrCreateDynamicStream(QuicSession* session,
                                                      QuicStreamId stream_id) {
  return session->GetOrCreateDynamicStream(stream_id);
}

// static
std::map<QuicStreamId, QuicStreamOffset>&
QuicSessionPeer::GetLocallyClosedStreamsHighestOffset(QuicSession* session) {
  return session->locally_closed_streams_highest_offset_;
}

// static
QuicSession::StaticStreamMap& QuicSessionPeer::static_streams(
    QuicSession* session) {
  return session->static_stream_map_;
}

// static
QuicSession::DynamicStreamMap& QuicSessionPeer::dynamic_streams(
    QuicSession* session) {
  return session->dynamic_streams();
}

// static
const QuicSession::ClosedStreams& QuicSessionPeer::closed_streams(
    QuicSession* session) {
  return *session->closed_streams();
}

// static
QuicSession::ZombieStreamMap& QuicSessionPeer::zombie_streams(
    QuicSession* session) {
  return session->zombie_streams_;
}

// static
QuicUnorderedSet<QuicStreamId>* QuicSessionPeer::GetDrainingStreams(
    QuicSession* session) {
  return &session->draining_streams_;
}

// static
void QuicSessionPeer::ActivateStream(QuicSession* session,
                                     std::unique_ptr<QuicStream> stream) {
  return session->ActivateStream(std::move(stream));
}

// static
void QuicSessionPeer::RegisterStaticStream(QuicSession* session,
                                           QuicStreamId id,
                                           QuicStream* stream) {
  return session->RegisterStaticStream(id, stream);
}

// static
void QuicSessionPeer::RegisterStaticStreamNew(
    QuicSession* session,
    std::unique_ptr<QuicStream> stream) {
  return session->RegisterStaticStreamNew(std::move(stream),
                                          /*stream_already_counted = */ false);
}

// static
bool QuicSessionPeer::IsStreamClosed(QuicSession* session, QuicStreamId id) {
  return session->IsClosedStream(id);
}

// static
bool QuicSessionPeer::IsStreamCreated(QuicSession* session, QuicStreamId id) {
  return QuicContainsKey(session->dynamic_streams(), id);
}

// static
bool QuicSessionPeer::IsStreamAvailable(QuicSession* session, QuicStreamId id) {
  if (session->connection()->transport_version() == QUIC_VERSION_99) {
    if (id % QuicUtils::StreamIdDelta(QUIC_VERSION_99) < 2) {
      return QuicContainsKey(
          session->v99_streamid_manager_.bidirectional_stream_id_manager_
              .available_streams_,
          id);
    }
    return QuicContainsKey(
        session->v99_streamid_manager_.unidirectional_stream_id_manager_
            .available_streams_,
        id);
  }
  return QuicContainsKey(session->stream_id_manager_.available_streams_, id);
}

// static
QuicStream* QuicSessionPeer::GetStream(QuicSession* session, QuicStreamId id) {
  return session->GetStream(id);
}

// static
bool QuicSessionPeer::IsStreamWriteBlocked(QuicSession* session,
                                           QuicStreamId id) {
  return session->write_blocked_streams_.IsStreamBlocked(id);
}

// static
QuicAlarm* QuicSessionPeer::GetCleanUpClosedStreamsAlarm(QuicSession* session) {
  return session->closed_streams_clean_up_alarm_.get();
}

// static
LegacyQuicStreamIdManager* QuicSessionPeer::GetStreamIdManager(
    QuicSession* session) {
  return &session->stream_id_manager_;
}

// static
UberQuicStreamIdManager* QuicSessionPeer::v99_streamid_manager(
    QuicSession* session) {
  return &session->v99_streamid_manager_;
}

// static
QuicStreamIdManager* QuicSessionPeer::v99_bidirectional_stream_id_manager(
    QuicSession* session) {
  return &session->v99_streamid_manager_.bidirectional_stream_id_manager_;
}

// static
QuicStreamIdManager* QuicSessionPeer::v99_unidirectional_stream_id_manager(
    QuicSession* session) {
  return &session->v99_streamid_manager_.unidirectional_stream_id_manager_;
}

// static
void QuicSessionPeer::SendRstStreamInner(QuicSession* session,
                                         QuicStreamId id,
                                         QuicRstStreamErrorCode error,
                                         QuicStreamOffset bytes_written,
                                         bool close_write_side_only) {
  session->SendRstStreamInner(id, error, bytes_written, close_write_side_only);
}

}  // namespace test
}  // namespace quic
