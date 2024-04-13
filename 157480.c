void PesOptionalHeader::SetPtsBits(std::int64_t pts_90khz) {
  std::uint64_t* pts_bits = &pts.bits;
  *pts_bits = 0;

  // PTS is broken up and stored in 40 bits as shown:
  //
  //  PES PTS Only flag
  // /                  Marker              Marker              Marker
  // |                 /                   /                   /
  // |                 |                   |                   |
  // 7654  321         0  765432107654321  0  765432107654321  0
  // 0010  PTS 32-30   1  PTS 29-15        1  PTS 14-0         1
  const std::uint32_t pts1 = (pts_90khz >> 30) & 0x7;
  const std::uint32_t pts2 = (pts_90khz >> 15) & 0x7FFF;
  const std::uint32_t pts3 = pts_90khz & 0x7FFF;

  std::uint8_t buffer[5] = {0};
  // PTS only flag.
  buffer[0] |= 1 << 5;
  // Top 3 bits of PTS and 1 bit marker.
  buffer[0] |= pts1 << 1;
  // Marker.
  buffer[0] |= 1;

  // Next 15 bits of pts and 1 bit marker.
  // Top 8 bits of second PTS chunk.
  buffer[1] |= (pts2 >> 7) & 0xff;
  // bottom 7 bits of second PTS chunk.
  buffer[2] |= (pts2 << 1);
  // Marker.
  buffer[2] |= 1;

  // Last 15 bits of pts and 1 bit marker.
  // Top 8 bits of second PTS chunk.
  buffer[3] |= (pts3 >> 7) & 0xff;
  // bottom 7 bits of second PTS chunk.
  buffer[4] |= (pts3 << 1);
  // Marker.
  buffer[4] |= 1;

  // Write bits into PesHeaderField.
  std::memcpy(reinterpret_cast<std::uint8_t*>(pts_bits), buffer, 5);
}