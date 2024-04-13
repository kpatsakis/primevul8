long Tags::Parse() {
  IMkvReader* const pReader = m_pSegment->m_pReader;

  long long pos = m_start;  // payload start
  const long long stop = pos + m_size;  // payload stop

  while (pos < stop) {
    long long id, size;

    long status = ParseElementHeader(pReader, pos, stop, id, size);

    if (status < 0)
      return status;

    if (size == 0)  // 0 length tag, read another
      continue;

    if (id == libwebm::kMkvTag) {
      status = ParseTag(pos, size);

      if (status < 0)
        return status;
    }

    pos += size;
    if (pos > stop)
      return E_FILE_FORMAT_INVALID;
  }

  if (pos != stop)
    return E_FILE_FORMAT_INVALID;

  return 0;
}