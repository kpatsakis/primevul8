uint64 EbmlElementSize(uint64 type, uint64 value, uint64 fixed_size) {
  // Size of EBML ID
  uint64 ebml_size = GetUIntSize(type);

  // Datasize
  ebml_size += (fixed_size > 0) ? fixed_size : GetUIntSize(value);

  // Size of Datasize
  ebml_size++;

  return ebml_size;
}