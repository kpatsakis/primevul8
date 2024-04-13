bool Cues::Find(long long time_ns, const Track* pTrack, const CuePoint*& pCP,
                const CuePoint::TrackPosition*& pTP) const {
  if (time_ns < 0 || pTrack == NULL || m_cue_points == NULL || m_count == 0)
    return false;

  CuePoint** const ii = m_cue_points;
  CuePoint** i = ii;

  CuePoint** const jj = ii + m_count;
  CuePoint** j = jj;

  pCP = *i;
  if (pCP == NULL)
    return false;

  if (time_ns <= pCP->GetTime(m_pSegment)) {
    pTP = pCP->Find(pTrack);
    return (pTP != NULL);
  }

  while (i < j) {
    // INVARIANT:
    //[ii, i) <= time_ns
    //[i, j)  ?
    //[j, jj) > time_ns

    CuePoint** const k = i + (j - i) / 2;
    if (k >= jj)
      return false;

    CuePoint* const pCP = *k;
    if (pCP == NULL)
      return false;

    const long long t = pCP->GetTime(m_pSegment);

    if (t <= time_ns)
      i = k + 1;
    else
      j = k;

    if (i > j)
      return false;
  }

  if (i != j || i > jj || i <= ii)
    return false;

  pCP = *--i;

  if (pCP == NULL || pCP->GetTime(m_pSegment) > time_ns)
    return false;

  // TODO: here and elsewhere, it's probably not correct to search
  // for the cue point with this time, and then search for a matching
  // track.  In principle, the matching track could be on some earlier
  // cue point, and with our current algorithm, we'd miss it.  To make
  // this bullet-proof, we'd need to create a secondary structure,
  // with a list of cue points that apply to a track, and then search
  // that track-based structure for a matching cue point.

  pTP = pCP->Find(pTrack);
  return (pTP != NULL);
}