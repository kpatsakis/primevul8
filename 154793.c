int32 SerializeFloat(IMkvWriter* writer, float f) {
  if (!writer)
    return -1;

  assert(sizeof(uint32) == sizeof(float));
  // This union is merely used to avoid a reinterpret_cast from float& to
  // uint32& which will result in violation of strict aliasing.
  union U32 {
    uint32 u32;
    float f;
  } value;
  value.f = f;

  for (int32 i = 1; i <= 4; ++i) {
    const int32 byte_count = 4 - i;
    const int32 bit_count = byte_count * 8;

    const uint8 byte = static_cast<uint8>(value.u32 >> bit_count);

    const int32 status = writer->Write(&byte, 1);

    if (status < 0)
      return status;
  }

  return 0;
}