uint64 EbmlMasterElementSize(uint64 type, uint64 value) {
  // Size of EBML ID
  int32 ebml_size = GetUIntSize(type);

  // Datasize
  ebml_size += GetCodedUIntSize(value);

  return ebml_size;
}