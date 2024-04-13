bool WriteEbmlDateElement(IMkvWriter* writer, uint64 type, int64 value) {
  if (!writer)
    return false;

  if (WriteID(writer, type))
    return false;

  if (WriteUInt(writer, kDateElementSize))
    return false;

  if (SerializeInt(writer, value, kDateElementSize))
    return false;

  return true;
}