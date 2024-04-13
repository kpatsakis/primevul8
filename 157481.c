std::uint16_t ReadUint16(const std::uint8_t* buf) {
  if (buf == nullptr)
    return 0;
  return ((buf[0] << 8) | buf[1]);
}