#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <vector>

enum MessageType : uint8_t { LOGIN = 0x01, MESSAGE = 0x02, LOGOUT = 0x03 };

struct MessageHeader {
  uint8_t type;
  uint32_t length; // network byte order
};

inline std::vector<uint8_t> serialize_message(MessageType type,
                                              const std::string &payload) {
  std::vector<uint8_t> buffer(sizeof(MessageHeader) + payload.size());
  MessageHeader *header = reinterpret_cast<MessageHeader *>(buffer.data());
  header->type = static_cast<uint8_t>(type);
  header->length = htonl(payload.size()); // convert to network byte order
  std::copy(payload.begin(), payload.end(),
            buffer.data() + sizeof(MessageHeader));
  return buffer;
}

inline std::pair<MessageType, std::string>
deserialize_message(const uint8_t *data, size_t bytes_received) {
  if (bytes_received < sizeof(MessageHeader)) {
    throw std::runtime_error("Incomplete header");
  }
  const MessageHeader *header = reinterpret_cast<const MessageHeader *>(data);
  uint32_t payload_length =
      ntohl(header->length); // conver from network byte order
  if (bytes_received < sizeof(MessageHeader) + payload_length) {
    throw std::runtime_error("Incomplete payload");
  }
  return {
      static_cast<MessageType>(header->type),
      std::string(reinterpret_cast<const char *>(data + sizeof(MessageHeader)),
                  payload_length)};
}

inline std::pair<MessageType, std::string> receive_message(int sock) {

  // Read header (block until full header arrives)
  uint8_t header_buffer[sizeof(MessageHeader)];
  ssize_t bytes_received =
      recv(sock, header_buffer, sizeof(MessageHeader), MSG_WAITALL);
  if (bytes_received <= 0) {
    throw std::runtime_error("Connection closed or error");
  }

  // Parse header to get payload length
  const MessageHeader *header =
      reinterpret_cast<const MessageHeader *>(header_buffer);
  uint32_t payload_length = ntohl(header->length);

  // Read payload
  std::vector<uint8_t> payload_buffer(payload_length);
  bytes_received =
      recv(sock, payload_buffer.data(), payload_length, MSG_WAITALL);
  if (static_cast<size_t>(bytes_received) != payload_length) {
    throw std::runtime_error("Incomplete payload");
  }

  return {static_cast<MessageType>(header->type),
          std::string(reinterpret_cast<const char *>(payload_buffer.data()),
                      payload_length)};
}
