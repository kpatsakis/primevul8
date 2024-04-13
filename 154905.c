long Cluster::Load(long long& pos, long& len) const {
  if (m_pSegment == NULL)
    return E_PARSE_FAILED;

  if (m_timecode >= 0)  // at least partially loaded
    return 0;

  if (m_pos != m_element_start || m_element_size >= 0)
    return E_PARSE_FAILED;

  IMkvReader* const pReader = m_pSegment->m_pReader;
  long long total, avail;
  const int status = pReader->Length(&total, &avail);

  if (status < 0)  // error
    return status;

  if (total >= 0 && (avail > total || m_pos > total))
    return E_FILE_FORMAT_INVALID;

  pos = m_pos;

  long long cluster_size = -1;

  if ((pos + 1) > avail) {
    len = 1;
    return E_BUFFER_NOT_FULL;
  }

  long long result = GetUIntLength(pReader, pos, len);

  if (result < 0)  // error or underflow
    return static_cast<long>(result);

  if (result > 0)
    return E_BUFFER_NOT_FULL;

  if ((pos + len) > avail)
    return E_BUFFER_NOT_FULL;

  const long long id_ = ReadID(pReader, pos, len);

  if (id_ < 0)  // error
    return static_cast<long>(id_);

  if (id_ != libwebm::kMkvCluster)
    return E_FILE_FORMAT_INVALID;

  pos += len;  // consume id

  // read cluster size

  if ((pos + 1) > avail) {
    len = 1;
    return E_BUFFER_NOT_FULL;
  }

  result = GetUIntLength(pReader, pos, len);

  if (result < 0)  // error
    return static_cast<long>(result);

  if (result > 0)
    return E_BUFFER_NOT_FULL;

  if ((pos + len) > avail)
    return E_BUFFER_NOT_FULL;

  const long long size = ReadUInt(pReader, pos, len);

  if (size < 0)  // error
    return static_cast<long>(cluster_size);

  if (size == 0)
    return E_FILE_FORMAT_INVALID;

  pos += len;  // consume length of size of element

  const long long unknown_size = (1LL << (7 * len)) - 1;

  if (size != unknown_size)
    cluster_size = size;

  // pos points to start of payload
  long long timecode = -1;
  long long new_pos = -1;
  bool bBlock = false;

  long long cluster_stop = (cluster_size < 0) ? -1 : pos + cluster_size;

  for (;;) {
    if ((cluster_stop >= 0) && (pos >= cluster_stop))
      break;

    // Parse ID

    if ((pos + 1) > avail) {
      len = 1;
      return E_BUFFER_NOT_FULL;
    }

    long long result = GetUIntLength(pReader, pos, len);

    if (result < 0)  // error
      return static_cast<long>(result);

    if (result > 0)
      return E_BUFFER_NOT_FULL;

    if ((cluster_stop >= 0) && ((pos + len) > cluster_stop))
      return E_FILE_FORMAT_INVALID;

    if ((pos + len) > avail)
      return E_BUFFER_NOT_FULL;

    const long long id = ReadID(pReader, pos, len);

    if (id < 0)  // error
      return static_cast<long>(id);

    if (id == 0)
      return E_FILE_FORMAT_INVALID;

    // This is the distinguished set of ID's we use to determine
    // that we have exhausted the sub-element's inside the cluster
    // whose ID we parsed earlier.

    if (id == libwebm::kMkvCluster)
      break;

    if (id == libwebm::kMkvCues)
      break;

    pos += len;  // consume ID field

    // Parse Size

    if ((pos + 1) > avail) {
      len = 1;
      return E_BUFFER_NOT_FULL;
    }

    result = GetUIntLength(pReader, pos, len);

    if (result < 0)  // error
      return static_cast<long>(result);

    if (result > 0)
      return E_BUFFER_NOT_FULL;

    if ((cluster_stop >= 0) && ((pos + len) > cluster_stop))
      return E_FILE_FORMAT_INVALID;

    if ((pos + len) > avail)
      return E_BUFFER_NOT_FULL;

    const long long size = ReadUInt(pReader, pos, len);

    if (size < 0)  // error
      return static_cast<long>(size);

    const long long unknown_size = (1LL << (7 * len)) - 1;

    if (size == unknown_size)
      return E_FILE_FORMAT_INVALID;

    pos += len;  // consume size field

    if ((cluster_stop >= 0) && (pos > cluster_stop))
      return E_FILE_FORMAT_INVALID;

    // pos now points to start of payload

    if (size == 0)
      continue;

    if ((cluster_stop >= 0) && ((pos + size) > cluster_stop))
      return E_FILE_FORMAT_INVALID;

    if (id == libwebm::kMkvTimecode) {
      len = static_cast<long>(size);

      if ((pos + size) > avail)
        return E_BUFFER_NOT_FULL;

      timecode = UnserializeUInt(pReader, pos, size);

      if (timecode < 0)  // error (or underflow)
        return static_cast<long>(timecode);

      new_pos = pos + size;

      if (bBlock)
        break;
    } else if (id == libwebm::kMkvBlockGroup) {
      bBlock = true;
      break;
    } else if (id == libwebm::kMkvSimpleBlock) {
      bBlock = true;
      break;
    }

    pos += size;  // consume payload
    if (cluster_stop >= 0 && pos > cluster_stop)
      return E_FILE_FORMAT_INVALID;
  }

  if (cluster_stop >= 0 && pos > cluster_stop)
    return E_FILE_FORMAT_INVALID;

  if (timecode < 0)  // no timecode found
    return E_FILE_FORMAT_INVALID;

  if (!bBlock)
    return E_FILE_FORMAT_INVALID;

  m_pos = new_pos;  // designates position just beyond timecode payload
  m_timecode = timecode;  // m_timecode >= 0 means we're partially loaded

  if (cluster_size >= 0)
    m_element_size = cluster_stop - m_element_start;

  return 0;
}