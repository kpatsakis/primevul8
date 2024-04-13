long Chapters::Edition::Parse(IMkvReader* pReader, long long pos,
                              long long size) {
  const long long stop = pos + size;

  while (pos < stop) {
    long long id, size;

    long status = ParseElementHeader(pReader, pos, stop, id, size);

    if (status < 0)  // error
      return status;

    if (size == 0)
      continue;

    if (id == libwebm::kMkvChapterAtom) {
      status = ParseAtom(pReader, pos, size);

      if (status < 0)  // error
        return status;
    }

    pos += size;
    if (pos > stop)
      return E_FILE_FORMAT_INVALID;
  }

  if (pos != stop)
    return E_FILE_FORMAT_INVALID;
  return 0;
}