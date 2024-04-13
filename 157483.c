bool PesHeader::Write(bool write_pts, PacketDataBuffer* buffer) const {
  if (buffer == nullptr) {
    std::fprintf(stderr, "Webm2Pes: nullptr in header writer.\n");
    return false;
  }

  // Write |start_code|.
  const int kStartCodeLength = 4;
  for (int i = 0; i < kStartCodeLength; ++i)
    buffer->push_back(start_code[i]);

  // The length field here reports number of bytes following the field. The
  // length of the optional header must be added to the payload length set by
  // the user.
  const std::size_t header_length =
      packet_length + optional_header.size_in_bytes();
  if (header_length > UINT16_MAX)
    return false;

  // Write |header_length| as big endian.
  std::uint8_t byte = (header_length >> 8) & 0xff;
  buffer->push_back(byte);
  byte = header_length & 0xff;
  buffer->push_back(byte);

  // Write the (not really) optional header.
  if (optional_header.Write(write_pts, buffer) != true) {
    std::fprintf(stderr, "Webm2Pes: PES optional header write failed.");
    return false;
  }
  return true;
}