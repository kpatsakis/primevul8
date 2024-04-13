unsigned long ContentEncoding::GetEncryptionCount() const {
  const ptrdiff_t count = encryption_entries_end_ - encryption_entries_;
  assert(count >= 0);

  return static_cast<unsigned long>(count);
}