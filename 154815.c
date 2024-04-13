bool Segment::PreloadCluster(Cluster* pCluster, ptrdiff_t idx) {
  if (pCluster == NULL || pCluster->m_index >= 0 || idx < m_clusterCount)
    return false;

  const long count = m_clusterCount + m_clusterPreloadCount;

  long& size = m_clusterSize;
  if (size < count)
    return false;

  if (count >= size) {
    const long n = (size <= 0) ? 2048 : 2 * size;

    Cluster** const qq = new (std::nothrow) Cluster*[n];
    if (qq == NULL)
      return false;
    Cluster** q = qq;

    Cluster** p = m_clusters;
    Cluster** const pp = p + count;

    while (p != pp)
      *q++ = *p++;

    delete[] m_clusters;

    m_clusters = qq;
    size = n;
  }

  if (m_clusters == NULL)
    return false;

  Cluster** const p = m_clusters + idx;

  Cluster** q = m_clusters + count;
  if (q < p || q >= (m_clusters + size))
    return false;

  while (q > p) {
    Cluster** const qq = q - 1;

    if ((*qq)->m_index >= 0)
      return false;

    *q = *qq;
    q = qq;
  }

  m_clusters[idx] = pCluster;
  ++m_clusterPreloadCount;
  return true;
}