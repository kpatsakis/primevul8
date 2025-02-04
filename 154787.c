long long Block::GetTime(const Cluster* pCluster) const {
  assert(pCluster);

  const long long tc = GetTimeCode(pCluster);

  const Segment* const pSegment = pCluster->m_pSegment;
  const SegmentInfo* const pInfo = pSegment->GetInfo();
  assert(pInfo);

  const long long scale = pInfo->GetTimeCodeScale();
  assert(scale >= 1);

  // Check if tc * scale could overflow.
  if (tc != 0 && scale > LLONG_MAX / tc) {
    return -1;
  }
  const long long ns = tc * scale;

  return ns;
}