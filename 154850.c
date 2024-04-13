long SegmentInfo::Parse() {
  assert(m_pMuxingAppAsUTF8 == NULL);
  assert(m_pWritingAppAsUTF8 == NULL);
  assert(m_pTitleAsUTF8 == NULL);

  IMkvReader* const pReader = m_pSegment->m_pReader;

  long long pos = m_start;
  const long long stop = m_start + m_size;

  m_timecodeScale = 1000000;
  m_duration = -1;

  while (pos < stop) {
    long long id, size;

    const long status = ParseElementHeader(pReader, pos, stop, id, size);

    if (status < 0)  // error
      return status;

    if (id == libwebm::kMkvTimecodeScale) {
      m_timecodeScale = UnserializeUInt(pReader, pos, size);

      if (m_timecodeScale <= 0)
        return E_FILE_FORMAT_INVALID;
    } else if (id == libwebm::kMkvDuration) {
      const long status = UnserializeFloat(pReader, pos, size, m_duration);

      if (status < 0)
        return status;

      if (m_duration < 0)
        return E_FILE_FORMAT_INVALID;
    } else if (id == libwebm::kMkvMuxingApp) {
      const long status =
          UnserializeString(pReader, pos, size, m_pMuxingAppAsUTF8);

      if (status)
        return status;
    } else if (id == libwebm::kMkvWritingApp) {
      const long status =
          UnserializeString(pReader, pos, size, m_pWritingAppAsUTF8);

      if (status)
        return status;
    } else if (id == libwebm::kMkvTitle) {
      const long status = UnserializeString(pReader, pos, size, m_pTitleAsUTF8);

      if (status)
        return status;
    }

    pos += size;

    if (pos > stop)
      return E_FILE_FORMAT_INVALID;
  }

  const double rollover_check = m_duration * m_timecodeScale;
  if (rollover_check > static_cast<double>(LLONG_MAX))
    return E_FILE_FORMAT_INVALID;

  if (pos != stop)
    return E_FILE_FORMAT_INVALID;

  return 0;
}