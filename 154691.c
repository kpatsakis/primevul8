bool WriteEbmlMasterElement(IMkvWriter* writer, uint64 type, uint64 size) {
  if (!writer)
    return false;

  if (WriteID(writer, type))
    return false;

  if (WriteUInt(writer, size))
    return false;

  return true;
}