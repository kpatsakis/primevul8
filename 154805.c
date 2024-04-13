long Tags::SimpleTag::Parse(IMkvReader* pReader, long long pos,
                            long long size) {
  const long long stop = pos + size;

  while (pos < stop) {
    long long id, size;

    long status = ParseElementHeader(pReader, pos, stop, id, size);

    if (status < 0)  // error
      return status;

    if (size == 0)  // weird
      continue;

    if (id == libwebm::kMkvTagName) {
      status = UnserializeString(pReader, pos, size, m_tag_name);

      if (status)
        return status;
    } else if (id == libwebm::kMkvTagString) {
      status = UnserializeString(pReader, pos, size, m_tag_string);

      if (status)
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