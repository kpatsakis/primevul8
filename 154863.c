bool WriteEbmlElement(IMkvWriter* writer, uint64 type, float value) {
  if (!writer)
    return false;

  if (WriteID(writer, type))
    return false;

  if (WriteUInt(writer, 4))
    return false;

  if (SerializeFloat(writer, value))
    return false;

  return true;
}