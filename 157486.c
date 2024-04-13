bool CopyAndEscapeStartCodes(const std::uint8_t* raw_input,
                             std::size_t raw_input_length,
                             PacketDataBuffer* packet_buffer) {
  if (raw_input == nullptr || raw_input_length < 1 || packet_buffer == nullptr)
    return false;

  int num_zeros = 0;
  for (std::size_t i = 0; i < raw_input_length; ++i) {
    const uint8_t byte = raw_input[i];

    if (byte == 0) {
      ++num_zeros;
    } else if (num_zeros >= 2 && (byte == 0x1 || byte == 0x3)) {
      packet_buffer->push_back(0x3);
      num_zeros = 0;
    } else {
      num_zeros = 0;
    }

    packet_buffer->push_back(byte);
  }

  return true;
}