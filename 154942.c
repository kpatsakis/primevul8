unsigned long ContentEncoding::GetCompressionCount() const {
  const ptrdiff_t count = compression_entries_end_ - compression_entries_;
  assert(count >= 0);

  return static_cast<unsigned long>(count);
}