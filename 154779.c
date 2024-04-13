bool CuePoint::TrackPosition::Parse(IMkvReader* pReader, long long start_,
                                    long long size_) {
  const long long stop = start_ + size_;
  long long pos = start_;

  m_track = -1;
  m_pos = -1;
  m_block = 1;  // default

  while (pos < stop) {
    long len;

    const long long id = ReadID(pReader, pos, len);
    if ((id < 0) || ((pos + len) > stop)) {
      return false;
    }

    pos += len;  // consume ID

    const long long size = ReadUInt(pReader, pos, len);
    if ((size < 0) || ((pos + len) > stop)) {
      return false;
    }

    pos += len;  // consume Size field
    if ((pos + size) > stop) {
      return false;
    }

    if (id == libwebm::kMkvCueTrack)
      m_track = UnserializeUInt(pReader, pos, size);
    else if (id == libwebm::kMkvCueClusterPosition)
      m_pos = UnserializeUInt(pReader, pos, size);
    else if (id == libwebm::kMkvCueBlockNumber)
      m_block = UnserializeUInt(pReader, pos, size);

    pos += size;  // consume payload
  }

  if ((m_pos < 0) || (m_track <= 0)) {
    return false;
  }

  return true;
}