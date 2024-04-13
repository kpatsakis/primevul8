long Segment::Load() {
  if (m_clusters != NULL || m_clusterSize != 0 || m_clusterCount != 0)
    return E_PARSE_FAILED;

  // Outermost (level 0) segment object has been constructed,
  // and pos designates start of payload.  We need to find the
  // inner (level 1) elements.

  const long long header_status = ParseHeaders();

  if (header_status < 0)  // error
    return static_cast<long>(header_status);

  if (header_status > 0)  // underflow
    return E_BUFFER_NOT_FULL;

  if (m_pInfo == NULL || m_pTracks == NULL)
    return E_FILE_FORMAT_INVALID;

  for (;;) {
    const long status = LoadCluster();

    if (status < 0)  // error
      return status;

    if (status >= 1)  // no more clusters
      return 0;
  }
}