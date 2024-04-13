void Tags::Tag::ShallowCopy(Tag& rhs) const {
  rhs.m_simple_tags = m_simple_tags;
  rhs.m_simple_tags_size = m_simple_tags_size;
  rhs.m_simple_tags_count = m_simple_tags_count;
}