unsigned long Track::GetContentEncodingCount() const {
  const ptrdiff_t count =
      content_encoding_entries_end_ - content_encoding_entries_;
  assert(count >= 0);

  return static_cast<unsigned long>(count);
}