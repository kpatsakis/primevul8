const Tags::SimpleTag* Tags::Tag::GetSimpleTag(int index) const {
  if (index < 0)
    return NULL;

  if (index >= m_simple_tags_count)
    return NULL;

  return m_simple_tags + index;
}