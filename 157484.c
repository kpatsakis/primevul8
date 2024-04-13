bool PesOptionalHeader::Write(bool write_pts, PacketDataBuffer* buffer) const {
  if (buffer == nullptr) {
    std::fprintf(stderr, "Webm2Pes: nullptr in opt header writer.\n");
    return false;
  }

  const int kHeaderSize = 9;
  std::uint8_t header[kHeaderSize] = {0};
  std::uint8_t* byte = header;

  if (marker.Check() != true || scrambling.Check() != true ||
      priority.Check() != true || data_alignment.Check() != true ||
      copyright.Check() != true || original.Check() != true ||
      has_pts.Check() != true || has_dts.Check() != true ||
      pts.Check() != true || stuffing_byte.Check() != true) {
    std::fprintf(stderr, "Webm2Pes: Invalid PES Optional Header field.\n");
    return false;
  }

  // TODO(tomfinegan): As noted in above, the PesHeaderFields should be an
  // array (or some data structure) that can be iterated over.

  // First byte of header, fields: marker, scrambling, priority, alignment,
  // copyright, original.
  *byte = 0;
  *byte |= marker.bits << marker.shift;
  *byte |= scrambling.bits << scrambling.shift;
  *byte |= priority.bits << priority.shift;
  *byte |= data_alignment.bits << data_alignment.shift;
  *byte |= copyright.bits << copyright.shift;
  *byte |= original.bits << original.shift;

  // Second byte of header, fields: has_pts, has_dts, unused fields.
  *++byte = 0;
  if (write_pts == true)
    *byte |= has_pts.bits << has_pts.shift;

  *byte |= has_dts.bits << has_dts.shift;

  // Third byte of header, fields: remaining size of header.
  *++byte = remaining_size.bits & 0xff;  // Field is 8 bits wide.

  int num_stuffing_bytes =
      (pts.num_bits + 7) / 8 + 1 /* always 1 stuffing byte */;
  if (write_pts == true) {
    // Write the PTS value as big endian and adjust stuffing byte count
    // accordingly.
    *++byte = pts.bits & 0xff;
    *++byte = (pts.bits >> 8) & 0xff;
    *++byte = (pts.bits >> 16) & 0xff;
    *++byte = (pts.bits >> 24) & 0xff;
    *++byte = (pts.bits >> 32) & 0xff;
    num_stuffing_bytes = 1;
  }

  // Add the stuffing byte(s).
  for (int i = 0; i < num_stuffing_bytes; ++i)
    *++byte = stuffing_byte.bits & 0xff;

  return CopyAndEscapeStartCodes(&header[0], kHeaderSize, buffer);
}