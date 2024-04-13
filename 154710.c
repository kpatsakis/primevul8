bool WriteEbmlElement(IMkvWriter* writer, uint64 type, const uint8* value,
                      uint64 size) {
  if (!writer || !value || size < 1)
    return false;

  if (WriteID(writer, type))
    return false;

  if (WriteUInt(writer, size))
    return false;

  if (writer->Write(value, static_cast<uint32>(size)))
    return false;

  return true;
}