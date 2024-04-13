void Tags::SimpleTag::Clear() {
  delete[] m_tag_name;
  m_tag_name = NULL;

  delete[] m_tag_string;
  m_tag_string = NULL;
}