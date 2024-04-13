uint64 EbmlDateElementSize(uint64 type) {
  // Size of EBML ID
  uint64 ebml_size = GetUIntSize(type);

  // Datasize
  ebml_size += kDateElementSize;

  // Size of Datasize
  ebml_size++;

  return ebml_size;
}