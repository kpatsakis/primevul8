Segment::Segment(IMkvReader* pReader, long long elem_start,
                 // long long elem_size,
                 long long start, long long size)
    : m_pReader(pReader),
      m_element_start(elem_start),
      // m_element_size(elem_size),
      m_start(start),
      m_size(size),
      m_pos(start),
      m_pUnknownSize(0),
      m_pSeekHead(NULL),
      m_pInfo(NULL),
      m_pTracks(NULL),
      m_pCues(NULL),
      m_pChapters(NULL),
      m_pTags(NULL),
      m_clusters(NULL),
      m_clusterCount(0),
      m_clusterPreloadCount(0),
      m_clusterSize(0) {}