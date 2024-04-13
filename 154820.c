uint64 EbmlElementSize(uint64 type, int64 value) {
  // Size of EBML ID
  int32 ebml_size = GetUIntSize(type);

  // Datasize
  ebml_size += GetIntSize(value);

  // Size of Datasize
  ebml_size++;

  return ebml_size;
}