int32 SerializeInt(IMkvWriter* writer, int64 value, int32 size) {
  if (!writer || size < 1 || size > 8)
    return -1;

  for (int32 i = 1; i <= size; ++i) {
    const int32 byte_count = size - i;
    const int32 bit_count = byte_count * 8;

    const int64 bb = value >> bit_count;
    const uint8 b = static_cast<uint8>(bb);

    const int32 status = writer->Write(&b, 1);

    if (status < 0)
      return status;
  }

  return 0;
}