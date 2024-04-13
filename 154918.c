long Chapters::Atom::Parse(IMkvReader* pReader, long long pos, long long size) {
  const long long stop = pos + size;

  while (pos < stop) {
    long long id, size;

    long status = ParseElementHeader(pReader, pos, stop, id, size);

    if (status < 0)  // error
      return status;

    if (size == 0)  // 0 length payload, skip.
      continue;

    if (id == libwebm::kMkvChapterDisplay) {
      status = ParseDisplay(pReader, pos, size);

      if (status < 0)  // error
        return status;
    } else if (id == libwebm::kMkvChapterStringUID) {
      status = UnserializeString(pReader, pos, size, m_string_uid);

      if (status < 0)  // error
        return status;
    } else if (id == libwebm::kMkvChapterUID) {
      long long val;
      status = UnserializeInt(pReader, pos, size, val);

      if (status < 0)  // error
        return status;

      m_uid = static_cast<unsigned long long>(val);
    } else if (id == libwebm::kMkvChapterTimeStart) {
      const long long val = UnserializeUInt(pReader, pos, size);

      if (val < 0)  // error
        return static_cast<long>(val);

      m_start_timecode = val;
    } else if (id == libwebm::kMkvChapterTimeEnd) {
      const long long val = UnserializeUInt(pReader, pos, size);

      if (val < 0)  // error
        return static_cast<long>(val);

      m_stop_timecode = val;
    }

    pos += size;
    if (pos > stop)
      return E_FILE_FORMAT_INVALID;
  }

  if (pos != stop)
    return E_FILE_FORMAT_INVALID;
  return 0;
}