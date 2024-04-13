bool WriteEbmlElement(IMkvWriter* writer, uint64 type, int64 value) {
  if (!writer)
    return false;

  if (WriteID(writer, type))
    return 0;

  const uint64 size = GetIntSize(value);
  if (WriteUInt(writer, size))
    return false;

  if (SerializeInt(writer, value, static_cast<int32>(size)))
    return false;

  return true;
}