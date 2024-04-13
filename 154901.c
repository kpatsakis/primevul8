void Tags::SimpleTag::ShallowCopy(SimpleTag& rhs) const {
  rhs.m_tag_name = m_tag_name;
  rhs.m_tag_string = m_tag_string;
}