uint64 EbmlElementSize(uint64 type, const char* value) {
  if (!value)
    return 0;

  // Size of EBML ID
  uint64 ebml_size = GetUIntSize(type);

  // Datasize
  ebml_size += strlen(value);

  // Size of Datasize
  ebml_size += GetCodedUIntSize(strlen(value));

  return ebml_size;
}