long Cluster::Parse(long long& pos, long& len) const {
  long status = Load(pos, len);

  if (status < 0)
    return status;

  if (m_pos < m_element_start || m_timecode < 0)
    return E_PARSE_FAILED;

  const long long cluster_stop =
      (m_element_size < 0) ? -1 : m_element_start + m_element_size;

  if ((cluster_stop >= 0) && (m_pos >= cluster_stop))
    return 1;  // nothing else to do

  IMkvReader* const pReader = m_pSegment->m_pReader;

  long long total, avail;

  status = pReader->Length(&total, &avail);

  if (status < 0)  // error
    return status;

  if (total >= 0 && avail > total)
    return E_FILE_FORMAT_INVALID;

  pos = m_pos;

  for (;;) {
    if ((cluster_stop >= 0) && (pos >= cluster_stop))
      break;

    if ((total >= 0) && (pos >= total)) {
      if (m_element_size < 0)
        m_element_size = pos - m_element_start;

      break;
    }

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

    if (id < 0)
      return E_FILE_FORMAT_INVALID;

    // This is the distinguished set of ID's we use to determine
    // that we have exhausted the sub-element's inside the cluster
    // whose ID we parsed earlier.

    if ((id == libwebm::kMkvCluster) || (id == libwebm::kMkvCues)) {
      if (m_element_size < 0)
        m_element_size = pos - m_element_start;

      break;
    }

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

    // const long long block_start = pos;
    const long long block_stop = pos + size;

    if (cluster_stop >= 0) {
      if (block_stop > cluster_stop) {
        if (id == libwebm::kMkvBlockGroup || id == libwebm::kMkvSimpleBlock) {
          return E_FILE_FORMAT_INVALID;
        }

        pos = cluster_stop;
        break;
      }
    } else if ((total >= 0) && (block_stop > total)) {
      m_element_size = total - m_element_start;
      pos = total;
      break;
    } else if (block_stop > avail) {
      len = static_cast<long>(size);
      return E_BUFFER_NOT_FULL;
    }

    Cluster* const this_ = const_cast<Cluster*>(this);

    if (id == libwebm::kMkvBlockGroup)
      return this_->ParseBlockGroup(size, pos, len);

    if (id == libwebm::kMkvSimpleBlock)
      return this_->ParseSimpleBlock(size, pos, len);

    pos += size;  // consume payload
    if (cluster_stop >= 0 && pos > cluster_stop)
      return E_FILE_FORMAT_INVALID;
  }

  if (m_element_size < 1)
    return E_FILE_FORMAT_INVALID;

  m_pos = pos;
  if (cluster_stop >= 0 && m_pos > cluster_stop)
    return E_FILE_FORMAT_INVALID;

  if (m_entries_count > 0) {
    const long idx = m_entries_count - 1;

    const BlockEntry* const pLast = m_entries[idx];
    if (pLast == NULL)
      return E_PARSE_FAILED;

    const Block* const pBlock = pLast->GetBlock();
    if (pBlock == NULL)
      return E_PARSE_FAILED;

    const long long start = pBlock->m_start;

    if ((total >= 0) && (start > total))
      return E_PARSE_FAILED;  // defend against trucated stream

    const long long size = pBlock->m_size;

    const long long stop = start + size;
    if (cluster_stop >= 0 && stop > cluster_stop)
      return E_FILE_FORMAT_INVALID;

    if ((total >= 0) && (stop > total))
      return E_PARSE_FAILED;  // defend against trucated stream
  }

  return 1;  // no more entries
}