int32 WriteID(IMkvWriter* writer, uint64 type) {
  if (!writer)
    return -1;

  writer->ElementStartNotify(type, writer->Position());

  const int32 size = GetUIntSize(type);

  return SerializeInt(writer, type, size);
}