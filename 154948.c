int32 WriteUInt(IMkvWriter* writer, uint64 value) {
  if (!writer)
    return -1;

  int32 size = GetCodedUIntSize(value);

  return WriteUIntSize(writer, value, size);
}