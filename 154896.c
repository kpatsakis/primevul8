long Chapters::Display::Parse(IMkvReader* pReader, long long pos,
                              long long size) {
  const long long stop = pos + size;

  while (pos < stop) {
    long long id, size;

    long status = ParseElementHeader(pReader, pos, stop, id, size);

    if (status < 0)  // error
      return status;

    if (size == 0)  // No payload.
      continue;

    if (id == libwebm::kMkvChapString) {
      status = UnserializeString(pReader, pos, size, m_string);

      if (status)
        return status;
    } else if (id == libwebm::kMkvChapLanguage) {
      status = UnserializeString(pReader, pos, size, m_language);

      if (status)
        return status;
    } else if (id == libwebm::kMkvChapCountry) {
      status = UnserializeString(pReader, pos, size, m_country);

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