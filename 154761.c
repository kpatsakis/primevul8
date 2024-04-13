bool SeekHead::ParseEntry(IMkvReader* pReader, long long start, long long size_,
                          Entry* pEntry) {
  if (size_ <= 0)
    return false;

  long long pos = start;
  const long long stop = start + size_;

  long len;

  // parse the container for the level-1 element ID

  const long long seekIdId = ReadID(pReader, pos, len);
  if (seekIdId < 0)
    return false;

  if (seekIdId != libwebm::kMkvSeekID)
    return false;

  if ((pos + len) > stop)
    return false;

  pos += len;  // consume SeekID id

  const long long seekIdSize = ReadUInt(pReader, pos, len);

  if (seekIdSize <= 0)
    return false;

  if ((pos + len) > stop)
    return false;

  pos += len;  // consume size of field

  if ((pos + seekIdSize) > stop)
    return false;

  pEntry->id = ReadID(pReader, pos, len);  // payload

  if (pEntry->id <= 0)
    return false;

  if (len != seekIdSize)
    return false;

  pos += seekIdSize;  // consume SeekID payload

  const long long seekPosId = ReadID(pReader, pos, len);

  if (seekPosId != libwebm::kMkvSeekPosition)
    return false;

  if ((pos + len) > stop)
    return false;

  pos += len;  // consume id

  const long long seekPosSize = ReadUInt(pReader, pos, len);

  if (seekPosSize <= 0)
    return false;

  if ((pos + len) > stop)
    return false;

  pos += len;  // consume size

  if ((pos + seekPosSize) > stop)
    return false;

  pEntry->pos = UnserializeUInt(pReader, pos, seekPosSize);

  if (pEntry->pos < 0)
    return false;

  pos += seekPosSize;  // consume payload

  if (pos != stop)
    return false;

  return true;
}