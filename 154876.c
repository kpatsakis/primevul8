long long ReadID(IMkvReader* pReader, long long pos, long& len) {
  if (pReader == NULL || pos < 0)
    return E_FILE_FORMAT_INVALID;

  // Read the first byte. The length in bytes of the ID is determined by
  // finding the first set bit in the first byte of the ID.
  unsigned char temp_byte = 0;
  int read_status = pReader->Read(pos, 1, &temp_byte);

  if (read_status < 0)
    return E_FILE_FORMAT_INVALID;
  else if (read_status > 0)  // No data to read.
    return E_BUFFER_NOT_FULL;

  if (temp_byte == 0)  // ID length > 8 bytes; invalid file.
    return E_FILE_FORMAT_INVALID;

  int bit_pos = 0;
  const int kMaxIdLengthInBytes = 4;
  const int kCheckByte = 0x80;

  // Find the first bit that's set.
  bool found_bit = false;
  for (; bit_pos < kMaxIdLengthInBytes; ++bit_pos) {
    if ((kCheckByte >> bit_pos) & temp_byte) {
      found_bit = true;
      break;
    }
  }

  if (!found_bit) {
    // The value is too large to be a valid ID.
    return E_FILE_FORMAT_INVALID;
  }

  // Read the remaining bytes of the ID (if any).
  const int id_length = bit_pos + 1;
  long long ebml_id = temp_byte;
  for (int i = 1; i < id_length; ++i) {
    ebml_id <<= 8;
    read_status = pReader->Read(pos + i, 1, &temp_byte);

    if (read_status < 0)
      return E_FILE_FORMAT_INVALID;
    else if (read_status > 0)
      return E_BUFFER_NOT_FULL;

    ebml_id |= temp_byte;
  }

  len = id_length;
  return ebml_id;
}