void Tags::Tag::Clear() {
  while (m_simple_tags_count > 0) {
    SimpleTag& d = m_simple_tags[--m_simple_tags_count];
    d.Clear();
  }

  delete[] m_simple_tags;
  m_simple_tags = NULL;

  m_simple_tags_size = 0;
}