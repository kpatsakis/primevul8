Tags::~Tags() {
  while (m_tags_count > 0) {
    Tag& t = m_tags[--m_tags_count];
    t.Clear();
  }
  delete[] m_tags;
}