uint64 EbmlElementSize(uint64 type, const uint8* value, uint64 size) {
  if (!value)
    return 0;

  // Size of EBML ID
  uint64 ebml_size = GetUIntSize(type);

  // Datasize
  ebml_size += size;

  // Size of Datasize
  ebml_size += GetCodedUIntSize(size);

  return ebml_size;
}