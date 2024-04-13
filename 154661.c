long ContentEncoding::ParseEncryptionEntry(long long start, long long size,
                                           IMkvReader* pReader,
                                           ContentEncryption* encryption) {
  assert(pReader);
  assert(encryption);

  long long pos = start;
  const long long stop = start + size;

  while (pos < stop) {
    long long id, size;
    const long status = ParseElementHeader(pReader, pos, stop, id, size);
    if (status < 0)  // error
      return status;

    if (id == libwebm::kMkvContentEncAlgo) {
      encryption->algo = UnserializeUInt(pReader, pos, size);
      if (encryption->algo != 5)
        return E_FILE_FORMAT_INVALID;
    } else if (id == libwebm::kMkvContentEncKeyID) {
      delete[] encryption->key_id;
      encryption->key_id = NULL;
      encryption->key_id_len = 0;

      if (size <= 0)
        return E_FILE_FORMAT_INVALID;

      const size_t buflen = static_cast<size_t>(size);
      unsigned char* buf = SafeArrayAlloc<unsigned char>(1, buflen);
      if (buf == NULL)
        return -1;

      const int read_status =
          pReader->Read(pos, static_cast<long>(buflen), buf);
      if (read_status) {
        delete[] buf;
        return status;
      }

      encryption->key_id = buf;
      encryption->key_id_len = buflen;
    } else if (id == libwebm::kMkvContentSignature) {
      delete[] encryption->signature;
      encryption->signature = NULL;
      encryption->signature_len = 0;

      if (size <= 0)
        return E_FILE_FORMAT_INVALID;

      const size_t buflen = static_cast<size_t>(size);
      unsigned char* buf = SafeArrayAlloc<unsigned char>(1, buflen);
      if (buf == NULL)
        return -1;

      const int read_status =
          pReader->Read(pos, static_cast<long>(buflen), buf);
      if (read_status) {
        delete[] buf;
        return status;
      }

      encryption->signature = buf;
      encryption->signature_len = buflen;
    } else if (id == libwebm::kMkvContentSigKeyID) {
      delete[] encryption->sig_key_id;
      encryption->sig_key_id = NULL;
      encryption->sig_key_id_len = 0;

      if (size <= 0)
        return E_FILE_FORMAT_INVALID;

      const size_t buflen = static_cast<size_t>(size);
      unsigned char* buf = SafeArrayAlloc<unsigned char>(1, buflen);
      if (buf == NULL)
        return -1;

      const int read_status =
          pReader->Read(pos, static_cast<long>(buflen), buf);
      if (read_status) {
        delete[] buf;
        return status;
      }

      encryption->sig_key_id = buf;
      encryption->sig_key_id_len = buflen;
    } else if (id == libwebm::kMkvContentSigAlgo) {
      encryption->sig_algo = UnserializeUInt(pReader, pos, size);
    } else if (id == libwebm::kMkvContentSigHashAlgo) {
      encryption->sig_hash_algo = UnserializeUInt(pReader, pos, size);
    } else if (id == libwebm::kMkvContentEncAESSettings) {
      const long status = ParseContentEncAESSettingsEntry(
          pos, size, pReader, &encryption->aes_settings);
      if (status)
        return status;
    }

    pos += size;  // consume payload
    if (pos > stop)
      return E_FILE_FORMAT_INVALID;
  }

  return 0;
}