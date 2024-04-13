long ParseElementHeader(IMkvReader* pReader, long long& pos, long long stop,
                        long long& id, long long& size) {
  if (stop >= 0 && pos >= stop)
    return E_FILE_FORMAT_INVALID;

  long len;

  id = ReadID(pReader, pos, len);

  if (id < 0)
    return E_FILE_FORMAT_INVALID;

  pos += len;  // consume id

  if (stop >= 0 && pos >= stop)
    return E_FILE_FORMAT_INVALID;

  size = ReadUInt(pReader, pos, len);

  if (size < 0 || len < 1 || len > 8) {
    // Invalid: Negative payload size, negative or 0 length integer, or integer
    // larger than 64 bits (libwebm cannot handle them).
    return E_FILE_FORMAT_INVALID;
  }

  // Avoid rolling over pos when very close to LLONG_MAX.
  const unsigned long long rollover_check =
      static_cast<unsigned long long>(pos) + len;
  if (rollover_check > LLONG_MAX)
    return E_FILE_FORMAT_INVALID;

  pos += len;  // consume length of size

  // pos now designates payload

  if (stop >= 0 && pos > stop)
    return E_FILE_FORMAT_INVALID;

  return 0;  // success
}