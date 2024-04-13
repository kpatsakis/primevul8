bool Tags::Tag::ExpandSimpleTagsArray() {
  if (m_simple_tags_size > m_simple_tags_count)
    return true;  // nothing else to do

  const int size = (m_simple_tags_size == 0) ? 1 : 2 * m_simple_tags_size;

  SimpleTag* const displays = new (std::nothrow) SimpleTag[size];

  if (displays == NULL)
    return false;

  for (int idx = 0; idx < m_simple_tags_count; ++idx) {
    m_simple_tags[idx].ShallowCopy(displays[idx]);
  }

  delete[] m_simple_tags;
  m_simple_tags = displays;

  m_simple_tags_size = size;
  return true;
}