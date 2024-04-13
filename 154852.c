bool Tags::ExpandTagsArray() {
  if (m_tags_size > m_tags_count)
    return true;  // nothing else to do

  const int size = (m_tags_size == 0) ? 1 : 2 * m_tags_size;

  Tag* const tags = new (std::nothrow) Tag[size];

  if (tags == NULL)
    return false;

  for (int idx = 0; idx < m_tags_count; ++idx) {
    m_tags[idx].ShallowCopy(tags[idx]);
  }

  delete[] m_tags;
  m_tags = tags;

  m_tags_size = size;
  return true;
}