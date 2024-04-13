long long EBMLHeader::Parse(IMkvReader* pReader, long long& pos) {
  if (!pReader)
    return E_FILE_FORMAT_INVALID;

  long long total, available;

  long status = pReader->Length(&total, &available);

  if (status < 0)  // error
    return status;

  pos = 0;

  // Scan until we find what looks like the first byte of the EBML header.
  const long long kMaxScanBytes = (available >= 1024) ? 1024 : available;
  const unsigned char kEbmlByte0 = 0x1A;
  unsigned char scan_byte = 0;

  while (pos < kMaxScanBytes) {
    status = pReader->Read(pos, 1, &scan_byte);

    if (status < 0)  // error
      return status;
    else if (status > 0)
      return E_BUFFER_NOT_FULL;

    if (scan_byte == kEbmlByte0)
      break;

    ++pos;
  }

  long len = 0;
  const long long ebml_id = ReadID(pReader, pos, len);

  if (ebml_id == E_BUFFER_NOT_FULL)
    return E_BUFFER_NOT_FULL;

  if (len != 4 || ebml_id != libwebm::kMkvEBML)
    return E_FILE_FORMAT_INVALID;

  // Move read pos forward to the EBML header size field.
  pos += 4;

  // Read length of size field.
  long long result = GetUIntLength(pReader, pos, len);

  if (result < 0)  // error
    return E_FILE_FORMAT_INVALID;
  else if (result > 0)  // need more data
    return E_BUFFER_NOT_FULL;

  if (len < 1 || len > 8)
    return E_FILE_FORMAT_INVALID;

  if ((total >= 0) && ((total - pos) < len))
    return E_FILE_FORMAT_INVALID;

  if ((available - pos) < len)
    return pos + len;  // try again later

  // Read the EBML header size.
  result = ReadUInt(pReader, pos, len);

  if (result < 0)  // error
    return result;

  pos += len;  // consume size field

  // pos now designates start of payload

  if ((total >= 0) && ((total - pos) < result))
    return E_FILE_FORMAT_INVALID;

  if ((available - pos) < result)
    return pos + result;

  const long long end = pos + result;

  Init();

  while (pos < end) {
    long long id, size;

    status = ParseElementHeader(pReader, pos, end, id, size);

    if (status < 0)  // error
      return status;

    if (size == 0)
      return E_FILE_FORMAT_INVALID;

    if (id == libwebm::kMkvEBMLVersion) {
      m_version = UnserializeUInt(pReader, pos, size);

      if (m_version <= 0)
        return E_FILE_FORMAT_INVALID;
    } else if (id == libwebm::kMkvEBMLReadVersion) {
      m_readVersion = UnserializeUInt(pReader, pos, size);

      if (m_readVersion <= 0)
        return E_FILE_FORMAT_INVALID;
    } else if (id == libwebm::kMkvEBMLMaxIDLength) {
      m_maxIdLength = UnserializeUInt(pReader, pos, size);

      if (m_maxIdLength <= 0)
        return E_FILE_FORMAT_INVALID;
    } else if (id == libwebm::kMkvEBMLMaxSizeLength) {
      m_maxSizeLength = UnserializeUInt(pReader, pos, size);

      if (m_maxSizeLength <= 0)
        return E_FILE_FORMAT_INVALID;
    } else if (id == libwebm::kMkvDocType) {
      if (m_docType)
        return E_FILE_FORMAT_INVALID;

      status = UnserializeString(pReader, pos, size, m_docType);

      if (status)  // error
        return status;
    } else if (id == libwebm::kMkvDocTypeVersion) {
      m_docTypeVersion = UnserializeUInt(pReader, pos, size);

      if (m_docTypeVersion <= 0)
        return E_FILE_FORMAT_INVALID;
    } else if (id == libwebm::kMkvDocTypeReadVersion) {
      m_docTypeReadVersion = UnserializeUInt(pReader, pos, size);

      if (m_docTypeReadVersion <= 0)
        return E_FILE_FORMAT_INVALID;
    }

    pos += size;
  }

  if (pos != end)
    return E_FILE_FORMAT_INVALID;

  // Make sure DocType, DocTypeReadVersion, and DocTypeVersion are valid.
  if (m_docType == NULL || m_docTypeReadVersion <= 0 || m_docTypeVersion <= 0)
    return E_FILE_FORMAT_INVALID;

  // Make sure EBMLMaxIDLength and EBMLMaxSizeLength are valid.
  if (m_maxIdLength <= 0 || m_maxIdLength > 4 || m_maxSizeLength <= 0 ||
      m_maxSizeLength > 8)
    return E_FILE_FORMAT_INVALID;

  return 0;
}