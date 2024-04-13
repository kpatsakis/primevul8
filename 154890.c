long long GetUIntLength(IMkvReader* pReader, long long pos, long& len) {
  if (!pReader || pos < 0)
    return E_FILE_FORMAT_INVALID;

  long long total, available;

  int status = pReader->Length(&total, &available);
  if (status < 0 || (total >= 0 && available > total))
    return E_FILE_FORMAT_INVALID;

  len = 1;

  if (pos >= available)
    return pos;  // too few bytes available

  unsigned char b;

  status = pReader->Read(pos, 1, &b);

  if (status != 0)
    return status;

  if (b == 0)  // we can't handle u-int values larger than 8 bytes
    return E_FILE_FORMAT_INVALID;

  unsigned char m = 0x80;

  while (!(b & m)) {
    m >>= 1;
    ++len;
  }

  return 0;  // success
}