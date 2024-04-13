uint64 WriteSimpleBlock(IMkvWriter* writer, const Frame* const frame,
                        int64 timecode) {
  if (WriteID(writer, libwebm::kMkvSimpleBlock))
    return 0;

  const int32 size = static_cast<int32>(frame->length()) + 4;
  if (WriteUInt(writer, size))
    return 0;

  if (WriteUInt(writer, static_cast<uint64>(frame->track_number())))
    return 0;

  if (SerializeInt(writer, timecode, 2))
    return 0;

  uint64 flags = 0;
  if (frame->is_key())
    flags |= 0x80;

  if (SerializeInt(writer, flags, 1))
    return 0;

  if (writer->Write(frame->frame(), static_cast<uint32>(frame->length())))
    return 0;

  return GetUIntSize(libwebm::kMkvSimpleBlock) + GetCodedUIntSize(size) + 4 +
         frame->length();
}