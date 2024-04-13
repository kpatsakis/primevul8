uint64 EbmlElementSize(uint64 type, float /* value */) {
  // Size of EBML ID
  uint64 ebml_size = GetUIntSize(type);

  // Datasize
  ebml_size += sizeof(float);

  // Size of Datasize
  ebml_size++;

  return ebml_size;
}