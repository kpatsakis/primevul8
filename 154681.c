const Tags::Tag* Tags::GetTag(int idx) const {
  if (idx < 0)
    return NULL;

  if (idx >= m_tags_count)
    return NULL;

  return m_tags + idx;
}