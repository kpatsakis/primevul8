bool WriteEbmlElement(IMkvWriter* writer, uint64 type, const char* value) {
  if (!writer || !value)
    return false;

  if (WriteID(writer, type))
    return false;

  const uint64 length = strlen(value);
  if (WriteUInt(writer, length))
    return false;

  if (writer->Write(value, static_cast<const uint32>(length)))
    return false;

  return true;
}