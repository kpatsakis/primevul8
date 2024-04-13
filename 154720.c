bool CuePoint::Load(IMkvReader* pReader) {
  // odbgstream os;
  // os << "CuePoint::Load(begin): timecode=" << m_timecode << endl;

  if (m_timecode >= 0)  // already loaded
    return true;

  assert(m_track_positions == NULL);
  assert(m_track_positions_count == 0);

  long long pos_ = -m_timecode;
  const long long element_start = pos_;

  long long stop;

  {
    long len;

    const long long id = ReadID(pReader, pos_, len);
    if (id != libwebm::kMkvCuePoint)
      return false;

    pos_ += len;  // consume ID

    const long long size = ReadUInt(pReader, pos_, len);
    assert(size >= 0);

    pos_ += len;  // consume Size field
    // pos_ now points to start of payload

    stop = pos_ + size;
  }

  const long long element_size = stop - element_start;

  long long pos = pos_;

  // First count number of track positions

  while (pos < stop) {
    long len;

    const long long id = ReadID(pReader, pos, len);
    if ((id < 0) || (pos + len > stop)) {
      return false;
    }

    pos += len;  // consume ID

    const long long size = ReadUInt(pReader, pos, len);
    if ((size < 0) || (pos + len > stop)) {
      return false;
    }

    pos += len;  // consume Size field
    if ((pos + size) > stop) {
      return false;
    }

    if (id == libwebm::kMkvCueTime)
      m_timecode = UnserializeUInt(pReader, pos, size);

    else if (id == libwebm::kMkvCueTrackPositions)
      ++m_track_positions_count;

    pos += size;  // consume payload
  }

  if (m_timecode < 0 || m_track_positions_count <= 0) {
    return false;
  }

  // os << "CuePoint::Load(cont'd): idpos=" << idpos
  //   << " timecode=" << m_timecode
  //   << endl;

  m_track_positions = new (std::nothrow) TrackPosition[m_track_positions_count];
  if (m_track_positions == NULL)
    return false;

  // Now parse track positions

  TrackPosition* p = m_track_positions;
  pos = pos_;

  while (pos < stop) {
    long len;

    const long long id = ReadID(pReader, pos, len);
    if (id < 0 || (pos + len) > stop)
      return false;

    pos += len;  // consume ID

    const long long size = ReadUInt(pReader, pos, len);
    assert(size >= 0);
    assert((pos + len) <= stop);

    pos += len;  // consume Size field
    assert((pos + size) <= stop);

    if (id == libwebm::kMkvCueTrackPositions) {
      TrackPosition& tp = *p++;
      if (!tp.Parse(pReader, pos, size)) {
        return false;
      }
    }

    pos += size;  // consume payload
    if (pos > stop)
      return false;
  }

  assert(size_t(p - m_track_positions) == m_track_positions_count);

  m_element_start = element_start;
  m_element_size = element_size;

  return true;
}