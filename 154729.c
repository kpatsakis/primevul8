bool WriteEbmlElement(IMkvWriter* writer, uint64 type, uint64 value,
                      uint64 fixed_size) {
  if (!writer)
    return false;

  if (WriteID(writer, type))
    return false;

  uint64 size = GetUIntSize(value);
  if (fixed_size > 0) {
    if (size > fixed_size)
      return false;
    size = fixed_size;
  }
  if (WriteUInt(writer, size))
    return false;

  if (SerializeInt(writer, value, static_cast<int32>(size)))
    return false;

  return true;
}