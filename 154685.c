bool WriteEbmlElement(IMkvWriter* writer, uint64 type, uint64 value) {
  return WriteEbmlElement(writer, type, value, 0);
}