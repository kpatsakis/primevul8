bool BCMVHeader::Write(PacketDataBuffer* buffer) const {
  if (buffer == nullptr) {
    std::fprintf(stderr, "Webm2Pes: nullptr for buffer in BCMV Write.\n");
    return false;
  }
  const int kBcmvSize = 4;
  for (int i = 0; i < kBcmvSize; ++i)
    buffer->push_back(bcmv[i]);

  // Note: The 4 byte length field must include the size of the BCMV header.
  const int kRemainingBytes = 6;
  const uint32_t bcmv_total_length = length + static_cast<uint32_t>(size());
  const uint8_t bcmv_buffer[kRemainingBytes] = {
      static_cast<std::uint8_t>((bcmv_total_length >> 24) & 0xff),
      static_cast<std::uint8_t>((bcmv_total_length >> 16) & 0xff),
      static_cast<std::uint8_t>((bcmv_total_length >> 8) & 0xff),
      static_cast<std::uint8_t>(bcmv_total_length & 0xff),
      0,
      0 /* 2 bytes 0 padding */};

  return CopyAndEscapeStartCodes(bcmv_buffer, kRemainingBytes, buffer);
}