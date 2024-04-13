uint64 WriteVoidElement(IMkvWriter* writer, uint64 size) {
  if (!writer)
    return false;

  // Subtract one for the void ID and the coded size.
  uint64 void_entry_size = size - 1 - GetCodedUIntSize(size - 1);
  uint64 void_size = EbmlMasterElementSize(libwebm::kMkvVoid, void_entry_size) +
                     void_entry_size;

  if (void_size != size)
    return 0;

  const int64 payload_position = writer->Position();
  if (payload_position < 0)
    return 0;

  if (WriteID(writer, libwebm::kMkvVoid))
    return 0;

  if (WriteUInt(writer, void_entry_size))
    return 0;

  const uint8 value = 0;
  for (int32 i = 0; i < static_cast<int32>(void_entry_size); ++i) {
    if (writer->Write(&value, 1))
      return 0;
  }

  const int64 stop_position = writer->Position();
  if (stop_position < 0 ||
      stop_position - payload_position != static_cast<int64>(void_size))
    return 0;

  return void_size;
}