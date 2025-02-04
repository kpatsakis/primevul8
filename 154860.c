long Cluster::CreateBlock(long long id,
                          long long pos,  // absolute pos of payload
                          long long size, long long discard_padding) {
  if (id != libwebm::kMkvBlockGroup && id != libwebm::kMkvSimpleBlock)
    return E_PARSE_FAILED;

  if (m_entries_count < 0) {  // haven't parsed anything yet
    assert(m_entries == NULL);
    assert(m_entries_size == 0);

    m_entries_size = 1024;
    m_entries = new (std::nothrow) BlockEntry*[m_entries_size];
    if (m_entries == NULL)
      return -1;

    m_entries_count = 0;
  } else {
    assert(m_entries);
    assert(m_entries_size > 0);
    assert(m_entries_count <= m_entries_size);

    if (m_entries_count >= m_entries_size) {
      const long entries_size = 2 * m_entries_size;

      BlockEntry** const entries = new (std::nothrow) BlockEntry*[entries_size];
      if (entries == NULL)
        return -1;

      BlockEntry** src = m_entries;
      BlockEntry** const src_end = src + m_entries_count;

      BlockEntry** dst = entries;

      while (src != src_end)
        *dst++ = *src++;

      delete[] m_entries;

      m_entries = entries;
      m_entries_size = entries_size;
    }
  }

  if (id == libwebm::kMkvBlockGroup)
    return CreateBlockGroup(pos, size, discard_padding);
  else
    return CreateSimpleBlock(pos, size);
}